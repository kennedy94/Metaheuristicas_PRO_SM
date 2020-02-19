#include "single_machine_instance.h"



single_machine_instance::single_machine_instance(int n, int d,
	vector<int> p, vector<int> a, vector<int> b, int k, double h) {
	
	this->n = n;
	this->d = d;
	this->p = p;
	this->a = a;
	this->b = b;
	this->k = k;
	this->h = h;
}

single_machine_instance::single_machine_instance()
{
}


single_machine_instance::~single_machine_instance()
{
}


/*
procedimento para avaliar o valor de função objetivo de uma permutação x
*/
inline int single_machine_instance::avaliar_solucao(vector<int> x){
	if (x.size() != n) {
		cerr << "ERRO, DIMENSAO DE X DIFERENTE DE N" << endl;
		getchar();
		exit(0);
	}

	int
		C_time = 0, // completion time acumulado
		fo = 0;		// valor de função objetivo

	for (int i = 0; i < n; i++){
		C_time += p[x[i]];
		fo += a[x[i]] * (max(0, d - C_time), b[x[i]] * max(0, C_time - d));
	}

	return fo;
}


/*
começo de uma heurística construtiva
*/
vector<int> single_machine_instance::heuristica_construtiva_1()
{
	vector<int>
		x(n, 0); // declarar vetor x com zeros e dimensão n

	for (int i = 0; i < n; i++)
		x[i] = i;

	cout << avaliar_solucao(x) << endl;

	return x;
}
