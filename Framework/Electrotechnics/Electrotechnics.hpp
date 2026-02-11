#pragma once
#include <armadillo>
struct Circuit
{
    struct Node
    {
        int id;
        p2 pos;
        Node(int id_, p2 pos_) :id(id_), pos(pos_) {}
    };
    //Node 0 is always ground if more than one element connects to the ground
    //, they make a connection between them as if they were a cable

    struct Resistor
    {
        int nodeA;
        int nodeB;

        float R; // ohms
    };

    struct VoltageSource
    {
        int nodePos; //terminals
        int nodeNeg;

        float V; // volts
    };

    std::vector<Node> nodes;

    std::vector<Resistor> resistors;
    std::vector<VoltageSource> sources;

    int addNode(p2 pos = p2{0,0})
    {
        int id = nodes.size();
        nodes.push_back({ id, pos });

        return id;
    }

    void addResistor(int nodeA, int nodeB, float R)
    {
        resistors.push_back({ nodeA, nodeB, R });
    }

    void addVoltageSource(int nodePos, int nodeNeg, float V)
    {
        sources.push_back({ nodePos, nodeNeg, V });
    }

    Circuit()
    {

    }
};