#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <list>
#include <thread> 
#include <random>


using namespace std;


class single_machine_instance
{
private:
	struct job {
		int
			id,
			p,
			a,
			b;
		job() {};
		job(int id, int p, int a, int b) {
			this->id = id;
			this->p = p;
			this->a = a;
			this->b = b;
		}
		bool operator ==(const job& j) const {                           
			return this->id == j.id; // return the result by reference
		}

		bool operator !=(const job& j) const {
			return !operator==(j); // return the result by reference
		}
	};


	struct comp_job_atrasado{
		bool operator ()(const job j1, const job j2) {
			return (float)j1.p / j1.b < (float)j2.p / j2.b;
		}
	};
	struct comp_job_adiantado {
		bool operator ()(const job j1, const job j2) {
			return (float)j1.p / j1.a < (float)j2.p / j2.a;
		}
	};

	struct comp_processamento {
		bool operator ()(const job j1, const job j2) {
			return (float)j1.p < (float)j2.p;
		}
	};

	struct taxa {
		bool operator ()(const job j1, const job j2) {
			return (float) j1.b/ j1.a < (float)j2.b / j2.a;
		}
	};

	

	int n,
		d,
		k;

	double
		h;

	vector<int>
		p,
		a,
		b;
	vector<vector<float>>
		gerar_populacao(int m),
		restart_populacao(int m);


public:
	struct solucao_const_busca {
		list<job>
			A,
			B;
		int
			st,
			fo;

		solucao_const_busca() {
			A = list<job>();
			B = list<job>();
			st = int();
			fo = int();
		};
	};

	vector<float>
		solucao_para_x(solucao_const_busca solu);

	single_machine_instance::single_machine_instance::solucao_const_busca
		x_para_solucao(vector<float> x);
	
	inline int
		avaliar_fo(vector<int> x);
	inline vector<int>
		avaliar_fo(list<job> A, list<job> B);

	void
		insert_A_pro_B_2(list<job>& A_best, list<job>& B_best),	//faz movimento de A pro B
		insert_B_pro_A_2(list<job>& A_best, list<job>& B_best),	//faz movimento de B pro A
		busca_local_com_conjunto(solucao_const_busca& BEST);	//busca local geral paralelizada usando conjuntos A e B e V-shape
	
	solucao_const_busca PSO(); //para testar as funções auxiliares;

	single_machine_instance(int n, int d, vector<int> p, vector<int> a, vector<int> b, int k, double h);

	solucao_const_busca heuristica_construtiva_2();


	void escrever_resultados(string metodo, int fo, float tempo);
};

