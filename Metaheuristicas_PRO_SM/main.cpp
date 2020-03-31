

#include "single_machine_instance.h"
#include <chrono>

using namespace std;

int main() {
	/*
	Declarando variáveis
	*/

	vector<const char*>
		arquivos = { "sch10.txt" ,"sch20.txt",
		"sch50.txt",  "sch100.txt",  "sch200.txt",
		"sch500.txt",  "sch1000.txt" };


	/*
	Lendo arquivo
	*/
	for (auto nome_arquivo : arquivos) {

		int
			n, //variável auxiliar para numero de jobs
			d, //variável auxiliar para data de entrega
			n_problema; // Número de problemas no arquivo;
		vector<int>
			p, //vetor auxiliar de tempo de processamento
			a, //vetor auxiliar de penalidade de antecedência
			b; //vetor auxiliar de penalidade de atraso

		vector<single_machine_instance>
			INSTANCIA; //Vetor com as instâncias que estão no arquivo

		ifstream
			arquivo(nome_arquivo); //Variável para ler arquivo

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
			auto comeco = chrono::high_resolution_clock::now();
			int fo = inst.heuristica_construtiva_2();
			auto fim = chrono::high_resolution_clock::now();

			chrono::duration<double> elapsed = fim - comeco;
			time2 = elapsed.count();
			inst.escrever_resultados("construtiva", fo, time2);
		}
		auto fim = chrono::high_resolution_clock::now();

		

		ofstream saida("resultados.csv", ofstream::app);
		saida << endl << endl;
		saida.close();
	}

	return 0;
}