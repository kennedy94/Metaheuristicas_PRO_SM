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


void single_machine_instance::escrever_resultados(string metodo, int fo, float tempo) {
	ofstream saida("resultados.csv", ofstream::app);
	saida << metodo << "," << n << "," << k << "," << h << "," << fo << "," << tempo << endl;
	saida.close();
}

single_machine_instance::single_machine_instance()
{
}


single_machine_instance::~single_machine_instance()
{
}

/*Heuristica dispensada*/
int single_machine_instance::heuristica_construtiva_1()
{
	vector<int>
		x(n, 0); // declarar vetor x com zeros e dimensão n
	int fo,
		fo_antiga;

	list<job>
		A, A_aux,
		B, B_aux,
		JOBS;

	for (int i = 0; i < n; i++) {
		x[i] = i;
		JOBS.push_back(job(i, p[i], a[i], b[i]));
	}

	int i = 0;
	while (!JOBS.empty()) {

		int soma = 0;
		for (auto a : A) {
			soma += a.p;
		}
		A_aux = A;
		B_aux = B;


		list<job>::iterator j = max_element(JOBS.begin(), JOBS.end(), taxa());

		B_aux.push_back(*j);
		int fo1 = avaliar_fo(A_aux, B_aux);
		B = B_aux;
		B_aux.pop_back();

		A_aux.push_back(*j);
		if (d - soma - (*j).p >= 0 && fo1 > avaliar_fo(A_aux, B_aux)) {
			B = B_aux;
			A = A_aux;
		}
		else {
			A_aux.pop_back();
			A = A_aux;
		}
		cout << "A: ";
		for (auto a :A){
			cout << a.id + 1<< " ";
		}
		cout << "; B: ";
		for (auto b : B) {
			cout << b.id + 1<< " ";
		}
		cout << "\t" << avaliar_fo(A, B) << " " << endl;
		JOBS.erase(j);
	}


	return avaliar_fo(A, B);
}


/*Heuristica a ser considerada*/
int single_machine_instance::heuristica_construtiva_2()
{
	vector<int>
		x(n, 0); // declarar vetor x com zeros e dimensão n
	int fo,
		fo_best;

	list<job>
		A, A_aux, A_best,
		B, B_aux, B_best,
		JOBS;

	for (int i = 0; i < n; i++) {
		x[i] = i;
		JOBS.push_back(job(i, p[i], a[i], b[i]));
	}

	int i = 0;
	while (!JOBS.empty()) {

		int soma = 0;
		for (auto a : A) {
			soma += a.p;
		}

		job j_best = *JOBS.begin();

		if (i == 0) {
			j_best = *min_element(JOBS.begin(), JOBS.end(), comp_job_adiantado());
			A_best.push_back(j_best);

		}
		else {
			fo_best = INT_MAX;
			for (auto j : JOBS) {

				A_aux = A;
				B_aux = B;

				B_aux.push_back(j);
				int fo1 = avaliar_fo(A, B_aux);
				if (fo1 <= fo_best) {
					fo_best = fo1;
					A_best = A;
					B_best = B_aux;
					j_best = j;
				}
				if (d - soma - (j).p >= 0) {
					A_aux.push_back(j);
					int fo2 = avaliar_fo(A_aux, B);

					if (fo2 <= fo_best) {
						fo_best = fo2;
						B_best = B;
						A_best = A_aux;
						j_best = j;
					}
				}

				
			}
		}
		i++;
		JOBS.remove(j_best);
		A = A_best;
		B = B_best;
	}


	return avaliar_fo(A, B);
}


inline int single_machine_instance::avaliar_fo(list<job> A, list<job> B) {

	A.sort(comp_job_adiantado());
	B.sort(comp_job_atrasado());

	int
		C_time = d,
		fo = 0, soma = 0;

	for (auto i: B){
		C_time += i.p;
		fo += i.b * max(0, C_time - d);
	}

	C_time = d;

	for (auto i : A) {
		soma += i.p;
		fo += i.a * max(0, d - C_time);
		C_time -= i.p;
	}

#ifdef DEBUG
	if (C_time < 0) {
		cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ERRO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		exit(0);
	}
#endif // DEBUG
	
	A.reverse();

	int fo2;


	fo2 = 0;
	C_time = 0;


	for (auto i : A) {
		C_time += i.p;
		fo2 += i.a * (max(0, d - C_time));
	}
	if (!B.empty() || !A.empty()) {
		if (C_time + (*B.begin()).p <= d) {
			fo2 = INT_MAX; // o atrasado não está mais atrasado
		}
		else {
			//continue calculando fo
			for (auto i : B) {
				C_time += i.p;
				fo2 += i.b * (max(0, C_time - d));
			}

		}
	}
	else {
		return fo;
	}

	return min(fo,fo2);
}



/*
A partir daqui é preparação para metaheurística
*/
inline int single_machine_instance::avaliar_fo(vector<int> x) {
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
			C_time = 0;

		j++;
	} while (j < n);
	//O(n^2) no pior caso 

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