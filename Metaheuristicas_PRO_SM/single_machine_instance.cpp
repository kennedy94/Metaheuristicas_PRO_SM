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
	vector<int> fo_st = avaliar_fo(A, B);
	solucao_f.fo = fo_st[0];
	solucao_f.st = fo_st[1];


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

	if(C_time == 0)
		return vector<int> {fo, 0};
	
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


void single_machine_instance::busca_local_com_conjunto(solucao_const_busca &BEST) {

	solucao_const_busca
		BEST1,
		BEST2;

	vector<int> fo_st;

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


	fo_st = avaliar_fo(BEST1.A, BEST1.B);
	BEST1.fo = fo_st[0];
	BEST1.st = fo_st[1];

	fo_st = avaliar_fo(BEST2.A, BEST2.B);
	BEST2.fo = fo_st[0];
	BEST2.st = fo_st[1];

	int fo1 = avaliar_fo(BEST1.A, BEST1.B)[0],
		fo2 = avaliar_fo(BEST2.A, BEST2.B)[0];

	if (BEST1.fo <= BEST2.fo) {
		BEST = BEST1;
	}
	else {
		BEST = BEST2;
	}
}


/*PSO*/
/*transforma o 'chromossomo' numa sequencia viável*/
vector<float> single_machine_instance::solucao_para_x(single_machine_instance::solucao_const_busca solu)
{
	vector<float> saida(n);
	for (auto a: solu.A){
		saida[a.id] = -1.0;
	}

	for (auto b : solu.B) {
		saida[b.id] = 1.0;
	}
	return saida;
}

single_machine_instance::solucao_const_busca single_machine_instance::x_para_solucao(vector<float> x)
{
	solucao_const_busca solu;
	
	for (int i = 0; i < x.size(); i++){
		job tarefa;
		tarefa.a = a[i];
		tarefa.b = b[i];
		tarefa.id = i;
		tarefa.p = p[i];
		if (x[i] <= 0){
			solu.A.insert(lower_bound(solu.A.begin(), solu.A.end(), tarefa, comp_job_adiantado()),
				tarefa);
		}
		else {
			solu.B.insert(lower_bound(solu.B.begin(), solu.B.end(), tarefa, comp_job_atrasado()),
				tarefa);
		}
	}

	int sum = 0;
	for (auto a: solu.A){
		sum += a.p;
	}
	if (sum <= d) {
		vector<int> fo_st = avaliar_fo(solu.A, solu.B);
		solu.fo = fo_st[0];
		solu.st = fo_st[1];
	}
	else {
		//inviável
		solu.fo = INT_MAX;
		solu.st = 0;
	}

	return solu;
}




single_machine_instance::solucao_const_busca single_machine_instance::PSO() {
	auto comeco = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed;

	std::mt19937 gen(time(NULL));
	std::uniform_real_distribution<>
		dis(-1.0, 1.0);

	float
		w = 0.9,		//inércia, não pode ficar menor que 0.4
		alpha = 0.975,	//fator de decréscimo de inércia
		c1 = 2,			//parametros sociais
		r1 = 0.5,
		c2 = 2,			//parametros cognitivos
		r2 = 0.5;
	int m = 2 * n,
		f_GB = INT_MAX;			//f de global best

	vector< vector<float> >
		P(m),	//população
		PB(m),	//personal best
		V(m);	//velocidades

	vector<float>
		GB(n);	//global best

	vector<int>
		f_PB(m),
		f_AB(m);	//actual best

	for (int i = 0; i < m; i++) {
		P[i] = vector<float>(n);
		PB[i] = vector<float>(n);
		V[i] = vector<float>(n);
	}

	//gerar população inicial
	P = gerar_populacao(m);
	for (int i = 0; i < m; i++) {
		//posição
		for (int j = 0; j < n; j++) {
			//gene
			//P[i][j] = dis(gen);
			PB[i][j] = P[i][j];
			V[i][j] = 4*dis(gen);
		}
	}

	//iniciar valores de f de personal best e global best
	for (int i = 0; i < m; i++) {
		vector<float> x(n);
		for (int j = 0; j < n; j++) {
			x[j] = P[i][j];
		}
		solucao_const_busca solu = x_para_solucao(x);

		f_PB[i] = solu.fo;
		if (f_PB[i] < f_GB) {
			f_GB = f_PB[i];
			GB = x;
		}
	}

	int n_restart = 0,
		qtde_nao_melhoria = 0;
	
	//loop principal
	int k = 0;
	do
	{
		bool melhorou = false;

		//atualização da velocidade
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				V[i][j] = w * V[i][j] + c1 * r1 * (PB[i][j] - P[i][j]) + c2 * r2 * (GB[j] - P[i][j]);
			}
		}
		//atualização inércia
		w = max((float)0.4, w * alpha);

		//atualização posição
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				P[i][j] = P[i][j] + V[i][j];
			}
		}

		//atualizar personal e global best
		for (int i = 0; i < m; i++) {
			vector<float> x(n);
			for (int j = 0; j < n; j++) {
				x[j] = P[i][j];
			}
			solucao_const_busca solu = x_para_solucao(x);	//avaliação e decoder

			if (solu.fo < f_PB[i]) {
				f_PB[i] = solu.fo;

				PB[i] = x;

				if (f_PB[i] < f_GB) {
					f_GB = f_PB[i];
					GB = x;
					melhorou = true;
				}

			}

		}
		auto fim = chrono::high_resolution_clock::now();
		elapsed = fim - comeco;

		if (!melhorou) {
			qtde_nao_melhoria++;
			if (qtde_nao_melhoria == 10) {
				P = restart_populacao(m);
				qtde_nao_melhoria = 0;
				n_restart++;
			}
		}
		else {
			qtde_nao_melhoria = 0;
		}
	} while (k++ != 5*n && elapsed.count() <= 600 && n_restart <= 10);


	return x_para_solucao(GB);
}

vector<vector<float>> single_machine_instance::gerar_populacao(int m)
{
	std::mt19937 gen(time(NULL));
	std::uniform_real_distribution<>
		dis(-1.0, 1.0);

	solucao_const_busca solucao_inicial = heuristica_construtiva_2();
	auto comeco = chrono::high_resolution_clock::now();
	int 
		n_iter = 0,
		fo_antiga;
	chrono::duration<float> elapsed;
	float episilon = 1.0e-5;
	do
	{
		fo_antiga = solucao_inicial.fo;

		busca_local_com_conjunto(solucao_inicial);

		auto fim = chrono::high_resolution_clock::now();
		elapsed = fim - comeco;
	} while (elapsed.count() <= 400 && (float)(fo_antiga - solucao_inicial.fo) / fo_antiga > episilon);
	cout << "\tPrimeira Busca Local" << endl;
	vector<float> x = solucao_para_x(solucao_inicial);
	vector<vector<float>> P(m);

	P[0] = x;
	int j = 0;
	for (int i = 1; i < m; i++) {
		if (j >= n) {
			//populãção aleatória
			P[i] = vector<float>(n);
			for (int j = 0; j < n; j++) {
				P[i][j] = dis(gen);
			}
		}
		else {
			//solução inicial perturbada
			if (x[j] <= 0) {
				x[j] = 1;
			}
			else {
				x[j] = -1;
			}
			P[i] = x;
			x = P[0];
			j++;
		}
	}
	return P;

}

vector<vector<float>> single_machine_instance::restart_populacao(int m)
{
	std::mt19937 gen(time(NULL));
	std::uniform_real_distribution<>
		dis(-1.0, 1.0);

	vector<vector<float>> P(m);


	for (int i = 0; i < m; i++) {
		//populãção aleatória
		P[i] = vector<float>(n);
		for (int j = 0; j < n; j++) {
			P[i][j] = dis(gen);
		}
	}
	return P;
}