#pragma once
#include <armadillo>

//for large dimensions use sparse matices
//if elements are unconnected, the solver doesn't work properly

//Modified Nodal Analysis (MNA)
struct Circuit
{
	struct Resistor;
	struct VoltageSource;

	std::vector<Resistor> resistors;
	std::vector<VoltageSource> sources;
	std::vector<int> nodes = { 0,1,2 }; //manual for this case
	//node 0 is always ground if more than one element connects to the ground
	//, they make a connection between them as if they were a cable


	// outputs (filled in constructor)
	std::vector<float> nodeVoltage;     // size = nodesSize, nodeVoltage[0]=0
	std::vector<float> sourceCurrent;   // size = sourcesSize, sign per MNA vaiable

	int nodesSize = 0;
	int sourcesSize = 0;
	int dim = 0;

	struct Resistor
	{
		//Terminals
		int a = -1;//floating/unconnected
		int b = -1;

		float R; // ohms

		Resistor(float R_) :R(R_) {}
	};

	struct VoltageSource
	{
		//Terminals
		int pos = -1;
		int neg = -1;

		float V; // volts

		VoltageSource(float V_) :V(V_) {}
	};





	void addResistor(float R)
	{
		Resistor res(R);
		resistors.push_back(res);
	}

	void addVoltageSource(float V)
	{
		VoltageSource source(V);
		sources.push_back(source);
	}

	//sets a terminal in a node
	void connect(int& t, int node)
	{
		t = node;
	}

	void connect(int& t1, int node1, int& t2, int node2)
	{
		t1 = node1;
		t2 = node2;
	}

	void connect(Resistor& resistor, int node0, int node1)
	{
		resistor.a = node0;
		resistor.b = node1;
	}



	//MNA [A]·[x]=[B]
	//[x]T = [V1,...,VnodeN,IVS0,...,IVSn], //x is our solution
	// Contains all voltages in nodes excluding 0 and the current through each voltage source
	void solve()
	{
		nodesSize = (int)nodes.size();
		sourcesSize = (int)sources.size();
		dim = (nodesSize - 1) + sourcesSize; //nS-1 because node 0 is known

		arma::mat A(dim, dim, arma::fill::zeros);
		arma::vec b(dim, arma::fill::zeros);

		// Stamp resistors
		for (size_t i = 0; i < resistors.size(); ++i)
		{
			const Resistor& r = resistors[i];

			if (r.a < 0 || r.b < 0)
				continue; //skips element due to open circuit for now

			const double g = 1.0f / r.R; // conductance //because I=g·V

			//matrix indices
			//we are only interested if the terminals that aren't in node 0
			//if they are in 0, they are set to -1 so it isn't calculated
			//else we decrease 1 so the matrix uses 0
			const int ia = (r.a == 0) ? -1 : (r.a - 1);
			const int ib = (r.b == 0) ? -1 : (r.b - 1);

			if (ia >= 0) A(ia, ia) += g;
			if (ib >= 0) A(ib, ib) += g;

			if (ia >= 0 && ib >= 0)
			{
				A(ia, ib) -= g;
				A(ib, ia) -= g;
			}
		}

		// Stamp voltage sources
		for (size_t si = 0; si < sources.size(); ++si)
		{
			const VoltageSource& s = sources[si];

			if (s.pos < 0 || s.neg < 0)
				continue;

			const int np = s.pos;
			const int nn = s.neg;

			const int ip = (np == 0) ? -1 : (np - 1);
			const int in = (nn == 0) ? -1 : (nn - 1);


			const int k = (nodesSize - 1) + (int)si; // row/col for source current vaiable

			// KCL coupling (B and C = B^T)
			if (ip >= 0) { A(ip, k) += 1.0; A(k, ip) += 1.0; }
			if (in >= 0) { A(in, k) -= 1.0; A(k, in) -= 1.0; }

			// Voltage constraint: Vpos - Vneg = Vs
			b(k) = s.V;
		}

		arma::vec x;
		if (!arma::solve(x, A, b))
			throw std::runtime_error("MNA solve failed (singular system).");

		// Unpack results
		nodeVoltage.assign(nodesSize, 0.0); // ground = 0
		for (int n = 1; n < nodesSize; ++n)
			nodeVoltage[n] = x(n - 1);

		sourceCurrent.assign(sourcesSize, 0.0);
		for (int si = 0; si < sourcesSize; ++si)
			sourceCurrent[si] = x((nodesSize - 1) + si);

		print(A);
		print(b);
	}

	Circuit()
	{
		addResistor(5);
		addResistor(10);
		addVoltageSource(10);
		connect(sources[0].neg, 0, sources[0].pos, 1);
		connect(resistors[0], 0, 1);
		connect(resistors[1], 1, 0);


		/*addResistor(5);
		addResistor(10);
		addResistor(20);
		addVoltageSource(10);
		connect(sources[0].neg, 0);
		connect(sources[0].pos, 1);
		connect(resistors[0], 0, 2);
		connect(resistors[1], 1, 0);
		connect(resistors[2], 1, 2);*/

		solve();
		printCircuit();
	}

	void printCircuit()
	{
		std::cout << "=== Solution ===\n";

		for (int n = 0; n < nodesSize; ++n)
		{
			std::cout << "Node " << n << " voltage = "
				<< nodeVoltage[n] << " V\n";
		}

		for (int si = 0; si < sourcesSize; ++si)
		{
			std::cout << "Voltage source " << si << " current = "
				<< sourceCurrent[si] << " A\n";
		}

		for (size_t i = 0; i < resistors.size(); ++i)
		{
			const Resistor& r = resistors[i];

			double Va = nodeVoltage[r.a];
			double Vb = nodeVoltage[r.b];

			double I = (Va - Vb) / r.R;

			std::cout << "Resistor " << i
				<< " (" << r.R << " ohm) current = "
				<< I << " A\n";
		}
	}
};