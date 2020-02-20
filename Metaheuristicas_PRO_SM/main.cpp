

#include "single_machine_instance.h"

using namespace std;

int main() {
	/*
	Declarando variáveis
	*/
	const char*
		nome_arquivo = "sch10.txt"; //Nome do arquivo a ser lido
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

	
	p = { 7, 1, 18, 6, 13, 14, 5, 6 };
	a = { 2, 8, 4, 9, 5, 5, 7, 4 };
	b = { 14, 7, 8, 9, 7, 9, 5, 14 };
	n = 8;
	single_machine_instance EXEMPLO(n, 14, p, a, b, 1, 0.2);
	EXEMPLO.heuristica_construtiva_1();

	/*
	Lendo arquivo
	*/
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
		for (double h = 0.2; h < 1; h += 0.2){

			//definir a data de entrega
			int sum_p = accumulate(p.begin(), p.end(), 0);
			d = ceil((float)sum_p * h);

			//criar instância e adicionar no vetor;
			INSTANCIA.push_back(single_machine_instance(n, d, p, a, b, k, h));
		}
		
	}
	arquivo.close(); //fechar arquivo

	cout << "ARQUIVO LIDO COM SUCESSO" << endl;

	//INSTANCIA[0].heuristica_construtiva_1();

	return 0;
}