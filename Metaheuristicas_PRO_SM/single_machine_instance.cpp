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

/*Heuristica a ser considerada*/
single_machine_instance::solucao_const_busca single_machine_instance::heuristica_construtiva_2()
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
			//A_best.insert(std::lower_bound(A_best.begin(), A_best.end(), j_best, comp_job_adiantado()), j_best);
			A_best.push_back(j_best);

		}
		else {
			fo_best = INT_MAX;
			for (auto j : JOBS) {

				A_aux = A;
				B_aux = B;

				B_aux.insert(std::lower_bound(B_aux.begin(), B_aux.end(), j, comp_job_atrasado()), j);
				//B_aux.push_back(j);
				int fo1 = avaliar_fo(A, B_aux)[0];
				if (fo1 <= fo_best) {
					fo_best = fo1;
					A_best = A;
					B_best = B_aux;
					j_best = j;
				}
				if (d - soma - (j).p >= 0) {
					A_aux.insert(std::lower_bound(A_aux.begin(), A_aux.end(), j, comp_job_adiantado()), j);
					//A_aux.push_back(j);
					int fo2 = avaliar_fo(A_aux, B)[0];

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

	solucao_const_busca solucao_f;
	solucao_f.A = A;
	solucao_f.B = B;
	solucao_f.st = avaliar_fo(A, B)[1];


	return solucao_f;
}


inline vector<int> single_machine_instance::avaliar_fo(list<job> A, list<job> B) {
	int tempo_init;

	//A.sort(comp_job_adiantado());
	//B.sort(comp_job_atrasado());

	int
		C_time = d,
		fo = 0, soma = 0;

	if (!B.empty()) {
		for (auto i : B) {
			C_time += i.p;
			fo += i.b * max(0, C_time - d);
		}
	}

	C_time = d;
	if (!A.empty()) {
		for (auto i : A) {
			soma += i.p;
			fo += i.a * max(0, d - C_time);
			C_time -= i.p;
		}
	}

	
	A.reverse();

	int fo2;


	fo2 = 0;
	C_time = 0;

	if (!A.empty()) {
		for (auto i : A) {
			C_time += i.p;
			fo2 += i.a * (max(0, d - C_time));
		}
	}
	if (!B.empty() || !A.empty()) {
		if (!B.empty()) {
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
	}
	else {
		return vector<int> {fo, d - soma};
	}

	if (fo < fo2) {
		return vector<int> {fo, d - soma};
	}
	else {
		return vector<int> {fo2, 0};
	}
}



/*
A partir daqui é preparação para metaheurística
*/
inline int single_machine_instance::avaliar_fo(vector<int> x) {
	if (x.size() != n + 1) {
		cerr << "ERRO, DIMENSAO DE X DIFERENTE DE N+1" << endl;
		getchar();
		exit(0);
	}

	int
		starting_time = x[n],
		obj = 0;


	for (int i = 0; i < n; i++)
	{
		starting_time += p[x[i]];
		if (starting_time <= d) {
			obj += a[x[i]] * (d - starting_time);
		}
		else
		{
			obj += b[x[i]] * (starting_time - d);
		}
	}
		

	return obj;
}



//it1 < it2
//void single_machine_instance::busca_local1(vector<int>& BEST) {
//	vector<int>
//		newsolution = BEST,
//		x = BEST;
//
//	int fo = avaliar_fo(x),
//		fo_n;
//
//	for (int it1 = 0; it1 < n; it1++) {
//		for (int it2 = it1 + 1; it2 < n; it2++) {
//			if (it1 != it2) {
//				insert(x, it1, it2, newsolution);
//				fo_n = avaliar_fo(newsolution);
//
//				if (fo_n < fo) {
//					BEST = newsolution;
//					fo = fo_n;
//				}
//			}
//		}
//	}
//}
//
//void single_machine_instance::busca_local2(vector<int>& BEST) {
//	vector<int>
//		newsolution = BEST,
//		x = BEST;
//
//	int fo = avaliar_fo(x),
//		fo_n;
//
//	for (int it1 = 0; it1 < n; it1++) {
//		for (int it2 = it1 + 1; it2 < n; it2++) {
//			if (it1 != it2) {
//				insert(x, it2, it1, newsolution);
//				fo_n = avaliar_fo(newsolution);
//
//				if (fo_n < fo) {
//					BEST = newsolution;
//					fo = fo_n;
//				}
//			}
//		}
//	}
//}
//
//void single_machine_instance::busca_local( vector<int> &BEST) {
//	vector<int>
//		BEST1 = BEST,
//		BEST2 = BEST;
//
//	thread
//		t1(&single_machine_instance::busca_local1, this, std::ref(BEST1)),
//		t2(&single_machine_instance::busca_local2, this, std::ref(BEST2));
//
//	t1.join();
//	t2.join();
//
//
//	int fo1 = avaliar_fo(BEST1),
//		fo2 = avaliar_fo(BEST2);
//
//	if (fo1 <= fo2) {
//		BEST = BEST1;
//	}
//	else {
//		BEST = BEST2;
//	}
//
//
//}


void single_machine_instance::insert(vector<int> x, int a, int b, vector<int>& newsolution) {
	newsolution = x;
	int c__;
	if (a < b) {
		newsolution[b] = x[a];

		for (int c = a; c < b; c++) {
			c__ = c + 1;
			newsolution[c] = x[c__];
		}
	}
	else {

		newsolution[b] = x[a];

		for (int c = b + 1; c <= a; c++) {
			c__ = c - 1;
			newsolution[c] = x[c__];
		}
	}
}




void single_machine_instance::insert_A_pro_B(list<job>& A_best, list<job>& B_best) {
	list<job>
		A_aux = A_best,	//Guardam solução de entrada
		B_aux = B_best,
		A = A_best,		//São usados para os movimentos
		B = B_best;

	int i = 0;

	int
		fo_melhor = avaliar_fo(A, B)[0],
		fo_atual;

	job tarefa_sai_de_B;

	while (i < A_aux.size()) {
		A = A_aux;
		B = B_aux;

		auto it = next(A.begin(), i);
		//inserir mantendo V-shape
		B.insert(std::lower_bound(B.begin(), B.end(), *it, comp_job_atrasado()), *it);

		A.erase(it);
		
		//para cada "job" em B

		list<job> B2 = B;

		for (int j = 0; j < B2.size(); j++) {
			B = B2;
			auto it_B = next(B.begin(), j);
			tarefa_sai_de_B = *it_B;

			int sum = 0;
			if (!A.empty()) {
				for (auto a : A) {
					sum += a.p;
				}

				if (d - sum >= tarefa_sai_de_B.p) {
					B.erase(it_B);
					A.insert(std::lower_bound(A.begin(), A.end(), tarefa_sai_de_B, comp_job_adiantado()), tarefa_sai_de_B);
				}
			}

			fo_atual = avaliar_fo(A, B)[0];
			if (fo_melhor >= fo_atual) {
				A_best = A;
				B_best = B;
			}
		}
		i++;

	}
}

void single_machine_instance::insert_B_pro_A(list<job>& A_best, list<job>& B_best) {
	list<job>
		A_aux = A_best,
		B_aux = B_best,
		A = A_best,
		B = B_best;

	int i = 0;

	int
		fo_melhor = avaliar_fo(A, B)[0],
		fo_atual;

	job tarefa_sai_de_A;
	

	for (int i = 0; i < B_aux.size(); i++) {
		
		A = A_aux;
		B = B_aux;

		auto it = next(B.begin(), i);
		//inserir mantendo V-shape
		int sum = 0;
		if (!A.empty()) {
			for (auto a : A) {
				sum += a.p;
			}

			if (d - sum >= (*it).p) {
				A.insert(std::lower_bound(A.begin(), A.end(), *it, comp_job_adiantado()), *it);
				B.erase(it);
			}
		}
		
		list<job> A2 = A;
		for (int j = 0; j < A2.size(); j++)
		{
			A = A2;
			auto it_A = next(A.begin(), j);
			tarefa_sai_de_A = *it_A;
			A.erase(it_A);
			B.insert(std::lower_bound(B.begin(), B.end(), tarefa_sai_de_A, comp_job_atrasado()), tarefa_sai_de_A);


			fo_atual = avaliar_fo(A, B)[0];
			if (fo_melhor >= fo_atual) {
				A_best = A;
				B_best = B;
			}
		}

	}
	//cout << endl;
}

void single_machine_instance::insert_B_pro_A_2(list<job>& A_best, list<job>& B_best)
{

	list<job>
		A_aux = A_best,
		B_aux = B_best,
		A = A_best,
		B = B_best;

	int i = 0;

	int
		fo_melhor = avaliar_fo(A, B)[0],
		fo_atual;

	job tarefa_sai_de_A;




	for (int i = 0; i < B_aux.size(); i++) {

		A = A_aux;
		B = B_aux;

		auto it = next(B.begin(), i);
		//inserir mantendo V-shape
		int sum = 0;
		int id_job = (*it).id;
		if (!A.empty()) {
			for (auto a : A) {
				sum += a.p;
			}

			if (d - sum >= (*it).p) {
				A.insert(std::lower_bound(A.begin(), A.end(), *it, comp_job_adiantado()), *it);
				B.erase(it);
			}
		}

		fo_atual = avaliar_fo(A, B)[0];
		if (fo_melhor >= fo_atual) {
			A_best = A;
			B_best = B;
		}
		

		auto it_A = A.begin();
		while(!A.empty() && it_A != A.end())
		{
			if ((*it_A).id != id_job) {
				auto it_aux = next(it_A, 1);
				tarefa_sai_de_A = *it_A;
				A.erase(it_A);
				B.insert(std::lower_bound(B.begin(), B.end(), tarefa_sai_de_A, comp_job_atrasado()), tarefa_sai_de_A);
				it_A = it_aux;

				fo_atual = avaliar_fo(A, B)[0];
				if (fo_melhor >= fo_atual) {
					A_best = A;
					B_best = B;
				}
			}
			else
			{
				it_A++;
			}
		}

	}
}

void single_machine_instance::insert_A_pro_B_2(list<job>& A_best, list<job>& B_best){
	list<job>
		A_aux = A_best,	//Guardam solução de entrada
		B_aux = B_best,
		A = A_best,		//São usados para os movimentos
		B = B_best;

	int i = 0;

	int
		fo_melhor = avaliar_fo(A, B)[0],
		fo_atual;

	job tarefa_sai_de_B;

	while (i < A_aux.size()) {
		A = A_aux;
		B = B_aux;

		auto it = next(A.begin(), i);
		//inserir mantendo V-shape
		B.insert(std::lower_bound(B.begin(), B.end(), *it, comp_job_atrasado()), *it);
		int it_id = (*it).id;
		A.erase(it);


		fo_atual = avaliar_fo(A, B)[0];
		if (fo_melhor >= fo_atual) {
			A_best = A;
			B_best = B;
		}

		//para cada "job" em B

		while (B.size() > 1) {
			auto it_B = B.begin();
			if ((*it_B).id == it_id) {
				it_B++;
			}

			
			tarefa_sai_de_B = *it_B;

			int sum = 0;
			if (!A.empty()) {
				for (auto a : A) {
					sum += a.p;
				}

				if (d - sum >= tarefa_sai_de_B.p) {
					B.erase(it_B);
					A.insert(std::lower_bound(A.begin(), A.end(), tarefa_sai_de_B, comp_job_adiantado()), tarefa_sai_de_B);
				}
				else
				{
					break;
				}
			}

			fo_atual = avaliar_fo(A, B)[0];
			if (fo_melhor >= fo_atual) {
				A_best = A;
				B_best = B;
			}
		}
		i++;

	}
}


void single_machine_instance::busca_local_com_conjunto(list<job>& A, list<job>& B) {

	solucao_const_busca
		BEST,
		BEST1,
		BEST2;

	BEST.A = A;
	BEST.B = B;
	BEST.st = avaliar_fo(A, B)[1];
	int fo = avaliar_fo(A, B)[0];

	BEST1 = BEST;
	BEST2 = BEST;

	thread
		t1(&single_machine_instance::insert_A_pro_B_2, this, std::ref(BEST1.A), std::ref(BEST1.B)),
		t2(&single_machine_instance::insert_B_pro_A_2, this, std::ref(BEST2.A), std::ref(BEST2.B));

	t1.join();
	t2.join();

	thread
		t3(&single_machine_instance::insert_A_pro_B_2, this, std::ref(BEST2.A), std::ref(BEST2.B)),
		t4(&single_machine_instance::insert_B_pro_A_2, this, std::ref(BEST1.A), std::ref(BEST1.B));

	t3.join();
	t4.join();



	int fo1 = avaliar_fo(BEST1.A, BEST1.B)[0],
		fo2 = avaliar_fo(BEST2.A, BEST2.B)[0];

	if (fo1 <= fo2) {
		BEST = BEST1;
	}
	else {
		BEST = BEST2;
	}

	A = BEST.A;
	B = BEST.B;
}


/*PSO*/
/*transforma o 'chromossomo' numa sequencia viável*/
vector<int> single_machine_instance::x_para_sequencia_SPV(vector<float> x)
{
	//vector<float> x_aux = x;
	//x_aux.pop_back();
	vector<int> indices(n);

	iota(indices.begin(), indices.end(), 0);

	stable_sort(indices.begin(), indices.end(),
		[&x](int i, int j) {return x[i] < x[j];});

	indices.push_back(max(0, (int)round(x.back())));

	return indices;
}

vector<float> single_machine_instance::sequencia_para_x(vector<int> sequencia){

	vector<float> x(sequencia.size());

	for (int i = 0; i < n+1; i++){
		x[sequencia[i]] = (float)i;
	}
	x[n] = (float)sequencia[n];

	return x;
}

vector<int> single_machine_instance::conjuntos_para_sequencia(solucao_const_busca solu)
{
	vector<int> sequencia(n + 1);

	int i = 0;
	solu.A.reverse();


	for (auto a : solu.A) {
		sequencia[i] = a.id;
		i++;
	}

	for (auto b : solu.B) {
		sequencia[i] = b.id;
		i++;
	}

	sequencia[i] = solu.st;

	return sequencia;
}