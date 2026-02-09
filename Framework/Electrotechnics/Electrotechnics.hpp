#pragma once

using nodeID = int; // 0 will be always ground
//There's only one ground per circuit, if there were m

struct Node
{
	p2 pos;
	int id;
};

struct VoltageSource
{
	
	Node posNode;
	Node negNode;
	
	float V; //V
};

struct Resistor
{
	Node node1;
	Node node2;

	float R; //Ohms
};

struct Circuit
{
	std::vector<Resistor> resistors;
	std::vector<VoltageSource> vSources;
};