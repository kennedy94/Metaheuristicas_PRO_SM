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
inline int single_machine_instance::avaliar_fo(vector<int> x){
	if (x.size() != n) {
		cerr << "ERRO, DIMENSAO DE X DIFERENTE DE N" << endl;
		getchar();
		exit(0);
	}

	int
		C_time = d, // completion time acumulado
		fo = 0,		// valor de função objetivo
		fo_antiga = INT_MAX,	//valor de fo da ultima iteracao
		delta,	// melhora do valor de fo
		j = 0,	//iterador auxiliar para "mover uma posição para trás"
		sum = 0;

	do { 
		fo = 0;

		for (int i = 0; i < n; i++) {
			C_time += p[x[i]];
			fo += a[x[i]] * max(0, d - C_time) + b[x[i]] * max(0, C_time - d);
		}
		delta = fo - fo_antiga;

		fo_antiga = fo;
		sum += p[x[j]];
		//sem straddling jobs
		C_time = d - sum;

		if (C_time < 0)
			break;

		j++;
	} while (delta < 0);
	//O(n^2) no pior caso 

	return fo;
}


/*
começo de uma heurística construtiva
*/
vector<int> single_machine_instance::heuristica_construtiva_1()
{
	vector<int>
		x(n, 0); // declarar vetor x com zeros e dimensão n
	int fo,
		fo_antiga;

	for (int i = 0; i < n; i++)
		x[i] = i;


	list<job>
		A, A_aux,
		B, B_aux;

	for (int i = 0; i < n; i++){
		A.push_back(job(i, p[i], a[i], b[i]));
	}

	A.sort(comp_job_atrasado());
	fo_antiga = avaliar_fo(A, B);


	list<job>::iterator it = A.begin();
	while (it != A.end()) {
		int soma = 0;
		for (auto bit : B)
			soma += bit.p;

		if (d - soma >= (*it).p) {
			A_aux = A;
			B_aux = B;

			//inserir ordenado
			B.insert(lower_bound(B.begin(), B.end(), *it, comp_job_adiantado()), *it);

			list<job>::iterator it_aux = it;
			it++;
			A.erase(it_aux);

			fo = avaliar_fo(A, B);
			if (fo > fo_antiga) {
				A = A_aux;
				B = B_aux;
				break;
			}
			fo_antiga = fo;
			for (auto bit : B)
				soma += bit.p;
		}
		else {
			break;
		}

		
	}

	cout << avaliar_fo(A, B) << endl;

	//Passar solução dos conjuntos para o vetor x
	int j = 0;
	B.reverse();
	for (auto it : B){
		x[j] = it.id;
		j++;
	}
	for (auto it : A) {
		x[j] = it.id;
		j++;
	}

	cout << avaliar_fo(x) << endl;
	return x;
}

inline int single_machine_instance::avaliar_fo(list<job> A, list<job> B) {

	int
		C_time = d,
		fo = 0;

	for (auto i: A){
		C_time += i.p;
		fo += i.b * max(0, C_time - d);
	}

	C_time = d;

	for (auto i : B) {
		fo += i.a * max(0, d - C_time);
		C_time -= i.p;
	}

	return fo;
}


void single_machine_instance::busca_local(vector<int> x, vector<int> &BEST) {
	vector<int> newsolution(n);

	newsolution = x;

	int fo = avaliar_fo(x),
		fo_n;

	for (int it1 = 0; it1 < n - 1; it1++)
		for (int it2 = 1; it2 < n; it2++) {
			if (it1 != it2) {
				insert(x, it1, it2, newsolution);
				fo_n = avaliar_fo(newsolution);

				if (fo_n < fo) {
					BEST = newsolution;
					fo = fo_n;
				}
			}
		}
}

void single_machine_instance::insert(vector<int> x, int a, int b, vector<int> &newsolution) {
	newsolution[b] = x[a];

	for (int c = a; c < b; c++)
		newsolution[c] = x[c + 1];

	for (int c = 0; c < a; c++)
		newsolution[c] = x[c];

	for (int c = b + 1; c < n; c++)
		newsolution[c] = x[c];
}