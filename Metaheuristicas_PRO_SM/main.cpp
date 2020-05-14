

#include "single_machine_instance.h"
#include <chrono>

using namespace std;

int main() {
	/*
	Declarando variáveis
	*/

	vector<const char*>
		arquivos =
		{ "sch10.txt" ,"sch20.txt",
		"sch50.txt",  "sch100.txt",  "sch200.txt",
		"sch500.txt",
		"sch1000.txt" };

	int
		n, //variável auxiliar para numero de jobs
		d, //variável auxiliar para data de entrega
		n_problema; // Número de problemas no arquivo;
	vector<int>
		p, //vetor auxiliar de tempo de processamento
		a, //vetor auxiliar de penalidade de antecedência
		b; //vetor auxiliar de penalidade de atraso


	p = { 7,1,18,6,13,14,5,6 };
	a = { 2,8,4,9,5,5,7,4 };
	b = { 14,7,8,9,7,9,5,14 };
	single_machine_instance EXEMPLO(8, 14, p, a, b, 1, 14);

	//EXEMPLO.heuristica_construtiva_2();

	


	for (auto nome_arquivo : arquivos) {
		/*
		Lendo arquivo
		*/
		ifstream
			arquivo(nome_arquivo); //Variável para ler arquivo
		vector<single_machine_instance>
			INSTANCIA; //Vetor com as instâncias que estão no arquivo

		arquivo >> n_problema;

		//cada problema
		for (int k = 1; k <= n_problema; k++) {

			//ler dados
			arquivo >> n;
			p = vector<int>(n);
			a = vector<int>(n);
			b = vector<int>(n);
			for (int j = 0; j < n; j++) {
				arquivo >> p[j] >> a[j] >> b[j];
			}

			//criar uma instância para cada problema com um 'h' diferente
			for (double h = 0.2; h < 1; h += 0.2) {

				//definir a data de entrega
				int sum_p = accumulate(p.begin(), p.end(), 0);
				d = floor((float)sum_p * h);

				//criar instância e adicionar no vetor;
				INSTANCIA.push_back(single_machine_instance(n, d, p, a, b, k, h));
			}

		}
		arquivo.close(); //fechar arquivo

		cout << "ARQUIVO LIDO COM SUCESSO" << endl;

		double time2;
		
		
		
		for (auto inst : INSTANCIA) {
			auto comeco_tudo = chrono::high_resolution_clock::now();

			single_machine_instance::solucao_const_busca solucao_inicial = inst.PSO();

			int
				fo = inst.avaliar_fo(solucao_inicial.A, solucao_inicial.B)[0],
				fo_antiga;


			chrono::duration<double> elapsed;
			double episilon = 1.0e-5;

			//busca local
			auto comeco = chrono::high_resolution_clock::now();
			int n_iter = 0;
			do
			{
				fo_antiga = fo;

				inst.busca_local_com_conjunto(solucao_inicial.A, solucao_inicial.B);

				fo = inst.avaliar_fo(solucao_inicial.A, solucao_inicial.B)[0];

				auto fim = chrono::high_resolution_clock::now();
				elapsed = fim - comeco;
			} while ( elapsed.count() <= 400 &&  (double)(fo_antiga - fo)/ fo_antiga > episilon);
			//cout << "n_inter = " << n_iter << endl;


			elapsed = chrono::high_resolution_clock::now() - comeco_tudo;

			time2 = elapsed.count();
			inst.escrever_resultados("busca local", fo, time2);

			//break;
		}	
		break;
	}




	return 0;
}