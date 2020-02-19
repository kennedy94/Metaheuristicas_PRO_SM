#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>


using namespace std;

class single_machine_instance
{
private:
	int n, d, k;
	double h;
	vector<int> p, a, b;
	inline int avaliar_solucao(vector<int> x);
	
public:
	single_machine_instance(int n, int d, vector<int> p, vector<int> a, vector<int> b, int k, double h);
	single_machine_instance();
	~single_machine_instance();

	

	vector<int> heuristica_construtiva_1();
};

