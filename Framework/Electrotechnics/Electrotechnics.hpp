#pragma once
#include <armadillo>


//struct Circuit
//{
//    //2 resistances and one voltage source
//    //2 nodes, hardcoded from start
//    //pos terminal of the source and one terminal to each resistor goes into node 1
//    //neg source terminal and the other terminal of each resistance go to to node 0, which is also ground
//
//    struct Terminal
//    {
//        int node = -1; //floating/unconnected
//
//        //node 0 is always ground if more than one element connects to the ground
//        //, they make a connection between them as if they were a cable
//    };
//
//    struct Resistor
//    {
//        Terminal a;
//        Terminal b;
//
//        float R; // ohms
//
//        Resistor(float R_) :R(R_) {}
//    };
//
//    struct VoltageSource
//    {
//        Terminal pos;
//        Terminal neg;
//
//        float V; // volts
//
//        VoltageSource(float V_) :V(V_) {}
//    };
//
//    std::vector<Resistor> resistors;
//    std::vector<VoltageSource> sources;
//    std::vector<int> nodes = { 0,1 }; //manual for this case
//
//
//
//    void addResistor(float R)
//    {
//        Resistor res(R);
//        resistors.push_back(res);
//    }
//
//    void addVoltageSource(float V)
//    {
//        VoltageSource source(V);
//        sources.push_back(source);
//    }
//
//    //sets a terminal in a node
//    void connect(Terminal& t, int node)
//    {
//        t.node = node;
//    }
//
//    static int nodeRowIndex(int node)  // maps node -> equation row for voltage unknowns
//    {
//        // ground has no voltage unknown
//        return (node == 0) ? -1 : (node - 1);
//    }
//
//    // outputs (filled in constructor)
//    std::vector<double> nodeVoltage;     // size = nodeCount, nodeVoltage[0]=0
//    std::vector<double> sourceCurrent;   // size = sourceCount, sign per MNA variable
//
//    Circuit()
//    {
//        addResistor(5);
//        addResistor(10);
//        addVoltageSource(10);
//        connect(sources[0].neg, 0);
//        connect(sources[0].pos, 1);
//        connect(resistors[0].a, 0);
//        connect(resistors[0].b, 1);
//        connect(resistors[1].a, 1);
//        connect(resistors[1].b, 0);
//
//        // --------- MNA solve (generic for any counts, using current data) ---------
//        const int nodeCount = (int)nodes.size();        // assumes nodes are 0..N-1
//        const int sourceCount = (int)sources.size();
//        const int dim = (nodeCount - 1) + sourceCount;
//
//        arma::mat A(dim, dim, arma::fill::zeros);
//        arma::vec b(dim, arma::fill::zeros);
//
//        // Stamp resistors
//        for (size_t ri = 0; ri < resistors.size(); ++ri)
//        {
//            const Resistor& r = resistors[ri];
//
//            if (r.a.node < 0 || r.b.node < 0)
//                throw std::runtime_error("Unconnected resistor terminal.");
//
//            const int na = r.a.node;
//            const int nb = r.b.node;
//
//            const double g = 1.0 / (double)r.R; // conductance
//
//            const int ia = nodeRowIndex(na);
//            const int ib = nodeRowIndex(nb);
//
//            if (ia >= 0) A(ia, ia) += g;
//            if (ib >= 0) A(ib, ib) += g;
//
//            if (ia >= 0 && ib >= 0)
//            {
//                A(ia, ib) -= g;
//                A(ib, ia) -= g;
//            }
//        }
//
//        // Stamp voltage sources
//        for (size_t si = 0; si < sources.size(); ++si)
//        {
//            const VoltageSource& s = sources[si];
//
//            if (s.pos.node < 0 || s.neg.node < 0)
//                throw std::runtime_error("Unconnected voltage source terminal.");
//
//            const int np = s.pos.node;
//            const int nn = s.neg.node;
//
//            const int ip = nodeRowIndex(np);
//            const int in = nodeRowIndex(nn);
//
//            const int k = (nodeCount - 1) + (int)si; // row/col for source current variable
//
//            // KCL coupling (B and C = B^T)
//            if (ip >= 0) { A(ip, k) += 1.0; A(k, ip) += 1.0; }
//            if (in >= 0) { A(in, k) -= 1.0; A(k, in) -= 1.0; }
//
//            // Voltage constraint: Vpos - Vneg = Vs
//            b(k) = (double)s.V;
//        }
//
//        arma::vec x;
//        if (!arma::solve(x, A, b))
//            throw std::runtime_error("MNA solve failed (singular system).");
//
//        // Unpack results
//        nodeVoltage.assign(nodeCount, 0.0); // ground = 0
//        for (int n = 1; n < nodeCount; ++n)
//            nodeVoltage[n] = x(n - 1);
//
//        sourceCurrent.assign(sourceCount, 0.0);
//        for (int si = 0; si < sourceCount; ++si)
//            sourceCurrent[si] = x((nodeCount - 1) + si);
//
//        std::cout << "=== Solution ===\n";
//
//        for (int n = 0; n < nodeCount; ++n)
//        {
//            std::cout << "Node " << n << " voltage = "
//                << nodeVoltage[n] << " V\n";
//        }
//
//        for (int si = 0; si < sourceCount; ++si)
//        {
//            std::cout << "Voltage source " << si << " current = "
//                << sourceCurrent[si] << " A\n";
//        }
//
//        for (size_t ri = 0; ri < resistors.size(); ++ri)
//        {
//            const Resistor& r = resistors[ri];
//
//            double Va = nodeVoltage[r.a.node];
//            double Vb = nodeVoltage[r.b.node];
//
//            double I = (Va - Vb) / r.R;
//
//            std::cout << "Resistor " << ri
//                << " (" << r.R << " ohm) current = "
//                << I << " A\n";
//        }
//    }
//};

struct Circuit
{
    //2 resistances and one voltage source
    //2 nodes, hardcoded from start
    //pos terminal of the source and one terminal to each resistor goes into node 1
    //neg source terminal and the other terminal of each resistance go to to node 0, which is also ground

    struct Terminal
    {
        int node = -1; //floating/unconnected

        //node 0 is always ground if more than one element connects to the ground
        //, they make a connection between them as if they were a cable
    };

    struct Resistor
    {
        Terminal a;
        Terminal b;

        float R; // ohms

        Resistor(float R_) :R(R_) {}
    };

    struct VoltageSource
    {
        Terminal pos;
        Terminal neg;

        float V; // volts

        VoltageSource(float V_) :V(V_) {}
    };

    std::vector<Resistor> resistors;
    std::vector<VoltageSource> sources;
    std::vector<int> nodes = { 0,1,2 }; //manual for this case



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
    void connect(Terminal& t, int node)
    {
        t.node = node;
    }

    static int nodeRowIndex(int node)  // maps node -> equation row for voltage unknowns
    {
        // ground has no voltage unknown
        return (node == 0) ? -1 : (node - 1);
    }

    // outputs (filled in constructor)
    std::vector<double> nodeVoltage;     // size = nodeCount, nodeVoltage[0]=0
    std::vector<double> sourceCurrent;   // size = sourceCount, sign per MNA variable

    Circuit()
    {
        addResistor(5);
        addResistor(10);
        addResistor(20);
        addVoltageSource(10);
        connect(sources[0].neg, 0);
        connect(sources[0].pos, 1);
        connect(resistors[0].a, 0);
        connect(resistors[0].b, 2);
        connect(resistors[1].a, 1);
        connect(resistors[1].b, 0);
        connect(resistors[2].a, 1);
        connect(resistors[2].b, 2);

        // MNA SOLVER
        const int nodeCount = (int)nodes.size();        // assumes nodes are 0..N-1
        const int sourceCount = (int)sources.size();
        const int dim = (nodeCount - 1) + sourceCount;

        arma::mat A(dim, dim, arma::fill::zeros);
        arma::vec b(dim, arma::fill::zeros);

        // Stamp resistors
        for (size_t ri = 0; ri < resistors.size(); ++ri)
        {
            const Resistor& r = resistors[ri];

            if (r.a.node < 0 || r.b.node < 0)
                throw std::runtime_error("Unconnected resistor terminal.");

            const int na = r.a.node;
            const int nb = r.b.node;

            const double g = 1.0 / (double)r.R; // conductance

            const int ia = nodeRowIndex(na);
            const int ib = nodeRowIndex(nb);

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

            if (s.pos.node < 0 || s.neg.node < 0)
                throw std::runtime_error("Unconnected voltage source terminal.");

            const int np = s.pos.node;
            const int nn = s.neg.node;

            const int ip = nodeRowIndex(np);
            const int in = nodeRowIndex(nn);

            const int k = (nodeCount - 1) + (int)si; // row/col for source current variable

            // KCL coupling (B and C = B^T)
            if (ip >= 0) { A(ip, k) += 1.0; A(k, ip) += 1.0; }
            if (in >= 0) { A(in, k) -= 1.0; A(k, in) -= 1.0; }

            // Voltage constraint: Vpos - Vneg = Vs
            b(k) = (double)s.V;
        }

        arma::vec x;
        if (!arma::solve(x, A, b))
            throw std::runtime_error("MNA solve failed (singular system).");

        // Unpack results
        nodeVoltage.assign(nodeCount, 0.0); // ground = 0
        for (int n = 1; n < nodeCount; ++n)
            nodeVoltage[n] = x(n - 1);

        sourceCurrent.assign(sourceCount, 0.0);
        for (int si = 0; si < sourceCount; ++si)
            sourceCurrent[si] = x((nodeCount - 1) + si);

        std::cout << "=== Solution ===\n";

        for (int n = 0; n < nodeCount; ++n)
        {
            std::cout << "Node " << n << " voltage = "
                << nodeVoltage[n] << " V\n";
        }

        for (int si = 0; si < sourceCount; ++si)
        {
            std::cout << "Voltage source " << si << " current = "
                << sourceCurrent[si] << " A\n";
        }

        for (size_t ri = 0; ri < resistors.size(); ++ri)
        {
            const Resistor& r = resistors[ri];

            double Va = nodeVoltage[r.a.node];
            double Vb = nodeVoltage[r.b.node];

            double I = (Va - Vb) / r.R;

            std::cout << "Resistor " << ri
                << " (" << r.R << " ohm) current = "
                << I << " A\n";
        }
    }
};