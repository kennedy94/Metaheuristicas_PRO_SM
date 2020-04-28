#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <list>
#include <thread> 


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

	

	inline vector<int>
		avaliar_fo(list<job> A, list<job> B);
	

	void 
		insert(vector<int> x, int a, int b, vector<int>& newsolution);

public:
	
	inline int
		avaliar_fo(vector<int> x);

	void
		busca_local(vector<int>& BEST),
		busca_local1(vector<int>& BEST),
		busca_local2(vector<int>& BEST);

	single_machine_instance(int n, int d, vector<int> p, vector<int> a, vector<int> b, int k, double h);
	
	single_machine_instance();

	~single_machine_instance();


	vector<int> heuristica_construtiva_2();

	void escrever_resultados(string metodo, int fo, float tempo);
};

