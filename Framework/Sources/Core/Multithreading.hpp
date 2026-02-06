//Try-catch


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
//Only to be used by the main thread
//The idea is that the usage of the struct is only high level, you submit functions to it that do not need an inmediate result
// and you recover its result when ready
//A threadPool initializes all cores at the start and manages their use 
struct ThreadPool
{
	//Internal parameters to track the thread
	struct ThreadData
	{
		std::thread t; //the actual thread object
		std::function<void()> task; //function object that the thread will run in worker //can hold a std::bind result (or the lambda equivlent)
		std::any valueCopy; //copies from the submitted value

		//ptr is here because atomic is not movable and vector needs it to be movable, working with the ptr is the only
		// valid workaround I've found (atomic_ref is not very compatible with a threadPool)
		std::unique_ptr<std::atomic<uint8_t>> state; // 0 idle, 1 ready, 2 busy, 3 terminate

		ThreadData() : state(std::make_unique<std::atomic<uint8_t>>(0)) {}

		void reset()
		{
			task = nullptr;
			valueCopy.reset();

			// makes it idle 
			state->store(0, std::memory_order_release);
			state->notify_one();
		}
	};
	vector<ThreadData> threadsData; //vector needs to work with moveable atocmic_ref instead of atomic

	//links the value reference you submit/recover to a thread id for the vector
	//only the main thread does submit and recover
	//MAP BREAKS IF THE VARIABLE GETS OUT OF SCOPE
	std::unordered_map<const void*, size_t> linker;


	ThreadPool(unsigned int cores = std::numeric_limits<unsigned int>::max())
	{
		//main thread already exists, so I will have availableThreads+1
		unsigned int availableThreads = std::max(1u, std::thread::hardware_concurrency());
		availableThreads /= 2;
		if (cores == 0 || cores > availableThreads)
			threadsData.resize(availableThreads);
		else threadsData.resize(cores);

		//initializing all threads in an idle mode
		for (auto& thread : threadsData)
		{
			//relaxed because we don't care about any other worker variable here
			thread.state->store(0, std::memory_order_relaxed); // idle
			thread.t = std::thread(worker, &thread);
		}
		cout << "Cores: " << threadsData.size() << endl;

	}

	//analogous to int main. alway running, but only doing something if the thread has assigned a task
	static void worker(ThreadData* thread)
	{
		while (true)
		{


			//keep sleeping until the state isn't 0 (the thread might wake even when its state hasn't changed from 0)
			while (thread->state->load(std::memory_order_relaxed) == 0)
				thread->state->wait(0, std::memory_order_acquire); //acquire forces the worker thread to update its values when it wakes

			//we can't use s before, as we need to calculate it in real time if it does indeed wake up on its own
			uint8_t s = thread->state->load(std::memory_order_relaxed);

			//terminates
			if (s == 3)
				break;

			//is a task has been assigned
			if (s == 2 && thread->task)
			{
				//performs the function
				try
				{
					thread->task();
				}
				catch (...)
				{
					cout << "Worker thread unknown exception" << endl;
					thread->state->store(3, std::memory_order_release); // terminate this worker
					break;
				}

				//ensuring that the program terminates if the order was given while running the function
				if (thread->state->load(std::memory_order_relaxed) == 3)
					break;

				//ready after performing it
				thread->state->store(1, std::memory_order_release);
			}

		}
	}

	//true when the submit has succeded
	//value the reference, f the submited function, args its possible function calls
	template<typename T, typename... Args>
	bool submit(const T& value, void (*f)(T&, Args...), Args... args)
	{
		// prevents from submitting twice if the value reference is still there
		if (linker.find(&value) != linker.end()) return false;

		//finds the first idle thread
		size_t i = 0;
		for (; i < threadsData.size(); ++i)
		{
			//relaxed because it doesn't care about other variables, it's just looking for the state
			if (threadsData[i].state->load(std::memory_order_relaxed) == 0)
				break;
		}
		if (i == threadsData.size())
			return false; //all threads are occupied

		//the idle thread is the one we will be using
		ThreadData& thread = threadsData[i];
		linker[&value] = i;
		thread.valueCopy = value;

		//bind links all elements of the function to task (the function object we will actually call later)
		//ref because we need the reference of valueCopy, forward isn't necessary, but is more efficient than passing the elements directly 
		thread.task = std::bind(
			f,
			std::ref(std::any_cast<T&>(thread.valueCopy)),
			std::forward<Args>(args)...
		);

		//release to force the thread to see the recently set valueCopy and task instead of old values before updating it to busy
		thread.state->store(2, std::memory_order_release);// task prepared and ready to run
		thread.state->notify_one(); //stops the wait state from the worker
		return true;
	}



	//we look for the thread that contains our value reference, if the function is ready, we swap the values and reset the thread
	template<typename T>
	bool recover(T& value)
	{
		auto it = linker.find(&value);
		//if value isn't here
		if (it == linker.end())
			return false;

		ThreadData& thread = threadsData[it->second];
		//if state isn't ready
		if (thread.state->load(std::memory_order_acquire) != 1)
			return false;

		//sets new value and reset thread
		value = std::any_cast<T>(thread.valueCopy);
		thread.reset();
		linker.erase(it);

		return true;
	}

	//Meant to be used only at the end to properly terminate all the threads
	~ThreadPool()
	{
		for (size_t i = 0; i < threadsData.size(); i++)
		{
			cout << "Ending thread " << i << endl;
			threadsData[i].state->store(3, std::memory_order_release); // terminate
			threadsData[i].state->notify_one();

			threadsData[i].task = nullptr;

			//join prevents detached thread from accesing destroyed data
			//it waits for the functions to end
			if (threadsData[i].t.joinable())
				threadsData[i].t.join();

		}
		linker.clear();
	}
};





void workerFunction(int& value, int var)
{
	value += var;
}

//Dummy example, The while loop imitates a program
/*int main()
{
	int value = 0;

	int workerVar = 10; //to see that we can enter extra variables to the function


	ThreadPool tp;


	tp.submit(value, workerFunction, workerVar);

	while (true) //represents the application
	{
		tp.recover(value);
		cout << value << endl;
		if (value != 0)  break;
	}

}*/


