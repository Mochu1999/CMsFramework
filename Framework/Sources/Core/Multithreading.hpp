








#include <iostream>
using namespace std;
#include <vector>

#include <thread>
#include <atomic>
#include <functional>
#include <any>

//in atomic operation other threads will see the state of the system before the operation started 
// or after it finished, but cannot see any intermediate state

//Only to be used withvalues that do not go out of scope

struct ThreadPool
{
	struct ThreadData
	{
		uint8_t state = 0; // 0 idle, 1 ready, 2 busy
		std::any valueCopy;
		std::function<void()> task;
		std::thread t;
	};

	//possible because only the main thread will do submit and tryTakeResult, otherwise there's a data race accesing the pointer
	std::unordered_map<const void*, size_t> valueToSlot;
	vector<ThreadData> tData;

	ThreadPool()
	{
		//main thread althreadStates exists, so I will have availableThreads+1
		unsigned int availableThreads = std::max(1u, std::thread::hardware_concurrency());

		tData.resize(availableThreads);
	}

	//true when the submit has succeded
	template<typename T, typename F, typename... Args>
	bool submit(const T& value, F&& f, Args&&... args)
	{
		// making impossible to submit the same value twice
		if(valueToSlot.find(&value) != valueToSlot.end()) return false;

		size_t i = 0;
		for (; i < tData.size(); ++i)
		{
			if (std::atomic_ref<uint8_t>(tData[i].state).load(std::memory_order_relaxed) == 0)
				break;
		}
		if (i == tData.size())
			return false; //No submit if all threads are occupied

		auto& slot = tData[i];

		slot.valueCopy = value;
		valueToSlot[&value] = i;

		slot.task = [&, func = std::forward<F>(f),
			... boundArgs = std::forward<Args>(args)]() mutable
			{
				func(std::any_cast<T&>(slot.valueCopy), boundArgs...);
			};

		std::atomic_ref<uint8_t>(slot.state).store(2, std::memory_order_relaxed);// busy

		slot.t = std::thread([&slot]()
			{
				slot.task();
				std::atomic_ref<uint8_t>(slot.state).store(1, std::memory_order_release); // ready
			});
		return true;
	}

	template<typename T>
	bool tryTakeResult(T& value)
	{
		auto it = valueToSlot.find(&value);

		if (it == valueToSlot.end())
			return false;

		auto& slot = tData[it->second];

		if (std::atomic_ref<uint8_t>(slot.state)
			.load(std::memory_order_acquire) != 1)
			return false;

		value = std::any_cast<T>(slot.valueCopy);

		if (slot.t.joinable())
			slot.t.join();
		slot.task = nullptr;
		slot.valueCopy.reset();

		std::atomic_ref<uint8_t>(slot.state)
			.store(0, std::memory_order_relaxed); // idle

		valueToSlot.erase(it); // key point

		return true;
	}

	//Meant to be used only at the end to properly terminate all the threads
	~ThreadPool()
	{
		for (auto& slot : tData)
		{
			if (slot.t.joinable())
				slot.t.join();
		}
		valueToSlot.clear();
	}
};





void worker(int& value, int var)
{
	value += var;
}

//int main()
//{
//	int value = 0;
//	int valueW = value; //only to be accesed by the thread
//	int var = 10;
//
//	
//
//	ThreadPool pool;
//
//
//	pool.submit(value, worker, var);
//
//
//	while (true) //represents the application
//	{
//		pool.tryTakeResult(value);
//		cout << value << endl;
//		if (value != 0)  break;
//	}
//
//
//}


