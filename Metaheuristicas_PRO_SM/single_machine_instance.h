#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <list>


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

		job(int id, int p, int a, int b) {
			this->id = id;
			this->p = p;
			this->a = a;
			this->b = b;
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

	inline int
		avaliar_fo(vector<int> x),
		avaliar_fo(list<job> A, list<job> B);

	void 
		busca_local(vector<int> x, vector<int>& best),
		insert(vector<int> solution, int a, int b, vector<int>& newsolution);

public:

	single_machine_instance(int n, int d, vector<int> p, vector<int> a, vector<int> b, int k, double h);
	
	single_machine_instance();

	~single_machine_instance();

	vector<int> heuristica_construtiva_1();

	vector<int> heuristica_construtiva_2();

	void escrever_resultados(string metodo, int fo, float tempo);
};

