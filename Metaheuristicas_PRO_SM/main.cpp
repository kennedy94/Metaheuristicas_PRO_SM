

#include "single_machine_instance.h"

using namespace std;

int main() {
	/*
	Declarando vari�veis
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
			n, //vari�vel auxiliar para numero de jobs
			d, //vari�vel auxiliar para data de entrega
			n_problema; // N�mero de problemas no arquivo;
		vector<int>
			p, //vetor auxiliar de tempo de processamento
			a, //vetor auxiliar de penalidade de anteced�ncia
			b; //vetor auxiliar de penalidade de atraso

		vector<single_machine_instance>
			INSTANCIA; //Vetor com as inst�ncias que est�o no arquivo

		ifstream
			arquivo(nome_arquivo); //Vari�vel para ler arquivo

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

			//criar uma inst�ncia para cada problema com um 'h' diferente
			for (double h = 0.2; h < 1; h += 0.2) {

				//definir a data de entrega
				int sum_p = accumulate(p.begin(), p.end(), 0);
				d = ceil((float)sum_p * h);

				//criar inst�ncia e adicionar no vetor;
				INSTANCIA.push_back(single_machine_instance(n, d, p, a, b, k, h));
			}

		}
		arquivo.close(); //fechar arquivo

		cout << "ARQUIVO LIDO COM SUCESSO" << endl;

		for (auto inst : INSTANCIA) {
			inst.heuristica_construtiva_2();
		}
		break;
	}

	return 0;
}