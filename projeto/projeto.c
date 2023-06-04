//mudar tudo para mesmo tipo, pois todos vao se apontar entre si
//esse é um rascunho de um site ("https://www.geeksforgeeks.org/thresholding-based-image-segmentation/") que tentei implementar só de teoria num downtime que tive
//a variável "x" que não está definida significa uma margem de mudança entre o limiar i e i+1, a ideia daqueles laços é chegar no vale que haviamos dito
//só não tenho noção do que seria um delta bom para usarmos, podemos implementar um dia desses
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Celula Celula;
typedef struct Cabeca Cabeca;

struct Celula {

	int valor;
	int indiceLinha, indiceColuna;
	Celula* direita;
	Celula* abaixo;
};

struct Cabeca {

	Celula* primeiraLinha;
	Celula* primeiraColuna;
	Celula* ultimaLinha;
	Celula* ultimaColuna;	
};

Cabeca* criaCabeca();
void insereLinha(Cabeca* cabeca, int linha);
void insereColuna(Cabeca* cabeca, int coluna);
void carregarImagem(char arquivo[], Cabeca* cabeca, int* nPixels, int* histograma);
float calcularLimiar(int* histograma, int nPixels);

int main() {

	//histograma
	//limiar - identificar o vale 
	int histograma[256];
	int nPixels;
	Cabeca* matriz = criaCabeca();
	carregarImagem("teste.pgm", matriz, &nPixels, histograma);
	float limiar = calcularLimiar(histograma, nPixels);
	printf("%f\n", limiar);

	return 0;
}

Cabeca* criaCabeca() {
	
	Cabeca* cabeca = (Cabeca*) malloc(sizeof(Cabeca));

	if(cabeca) {

		cabeca->ultimaLinha = NULL;
		cabeca->ultimaColuna = NULL;
		cabeca->primeiraColuna = NULL;
		cabeca->primeiraLinha = NULL;
	}

	return cabeca;
}

void insereLinha(Cabeca* cabeca, int linha) {
	
	Celula* novaLinha = (Celula*) malloc(sizeof(Celula));

	if(novaLinha) {

		novaLinha->valor = -1;
		novaLinha->indiceColuna = -1;
		novaLinha->indiceLinha = linha;

		if(cabeca->ultimaLinha) {
		
			cabeca->ultimaLinha->abaixo = novaLinha;	

		} else {

			cabeca->primeiraLinha = novaLinha;
		}

		cabeca->ultimaLinha = novaLinha;
		novaLinha->direita = novaLinha;
		novaLinha->abaixo = cabeca->primeiraLinha;
	}

}

void insereColuna(Cabeca* cabeca, int coluna) {
	
	Celula* novaColuna = (Celula*) malloc(sizeof(Celula));

	if(novaColuna) {

		novaColuna->valor = -1;
		novaColuna->indiceColuna = coluna;
		novaColuna->indiceLinha = -1;
		

		if(cabeca->primeiraColuna) {
		
			cabeca->ultimaColuna->direita = novaColuna;		

		} else {
		
			cabeca->primeiraColuna = novaColuna;
		}

		cabeca->ultimaColuna = novaColuna;
		novaColuna->direita = cabeca->primeiraColuna;
		novaColuna->abaixo = novaColuna;
	}
}

Celula* insereCelula(int valor, int linha, int coluna, int nlinhas, int ncolunas, Cabeca* cabeca) {

	Celula* nova = (Celula*) malloc(sizeof(Celula));

	if(nova) {

		nova->valor = valor;
		nova->indiceColuna = coluna;
		nova->indiceLinha = linha;
		
		Celula* linhaAchada = cabeca->primeiraLinha;
		Celula* colunaAchada = cabeca->primeiraColuna;

		do {

			linhaAchada = linhaAchada->abaixo;
		
		} while(linhaAchada != cabeca->primeiraLinha && linhaAchada->indiceLinha != linha);	

		do {

			colunaAchada = colunaAchada->direita;
		
		} while(colunaAchada != cabeca->primeiraColuna && colunaAchada->indiceColuna != coluna);

		if(linhaAchada->direita == linhaAchada) {

			linhaAchada->direita = nova;
			nova->direita = linhaAchada;
		
		} else {

			Celula* pauxlinha = linhaAchada->direita;

			do {
				pauxlinha = pauxlinha->direita;
			} while (pauxlinha->direita != linhaAchada);

			pauxlinha->direita = nova;
			nova->direita = linhaAchada;

		}
		
		if(colunaAchada->abaixo == colunaAchada) {

			colunaAchada->abaixo = nova;
			nova->abaixo = colunaAchada;
		
		} else {

			Celula* pauxcoluna = colunaAchada->abaixo;

			do {
				pauxcoluna = pauxcoluna->abaixo;
			} while (pauxcoluna->abaixo != colunaAchada);

			pauxcoluna->abaixo = nova;
			nova->abaixo = colunaAchada;
		}

	}

	return nova;
}

void carregarImagem(char arquivo[], Cabeca* cabeca, int* nPixels, int* histograma) {

	FILE* imagem = fopen(arquivo, "r");

	char formato[2];
	int nlinhas, ncolunas;
	

	fscanf(imagem, "%s %d %d", formato, &nlinhas, &ncolunas);

    *nPixels = nlinhas * ncolunas;
    int pixel = 0;

    Celula* celula = NULL;

	for (int i = 0; i < ncolunas; ++i) {

		insereColuna(cabeca, i);
	}

	for (int i = 0; i < nlinhas; ++i) {

		insereLinha(cabeca, i);	
	}

	for (int i = 0; i < nlinhas; ++i) {

		for (int j = 0; j < ncolunas; ++j){
			
			fscanf(imagem, "%d", &pixel);
			histograma[pixel]++;
			printf("testeeee %d\n", histograma[pixel]);
			if(pixel != 255) {

				celula = insereCelula(pixel, i, j, nlinhas, ncolunas, cabeca);
				printf("[%d %d %d] ", celula->valor, celula->indiceLinha, celula->indiceColuna);
			}
		}
	}
}

float calcularLimiar(int* histograma, int nPixels) {

	float limiar = 0,auxlim1 = 0,auxlim2 = 0,limiarNovo;
	int count1,count2;

	for (int i = 0; i < 256; ++i) {
		
		limiar = (histograma[i]*i) + limiar;	
	}

while ((limiarNovo - limiar) > x) 
{
	limiar = limiar / nPixels;
	count1 = 0;
	for (int i = 0; i < ceil(limiar); ++i) {
		auxlim1 = histograma[i]*i + auxlim1;
		count1++;
	}
	auxlim1 = auxlim1/count1;

	count2 = 0;
	for (int i = floor(limiar); i < 256; i++){
		auxlim2 = histograma[i]*i + auxlim2;
		count2++;
	}
	auxlim2 = auxlim2/count2;
	
	limiarNovo = (auxlim2 + auxlim1)/2;
}

	return limiar;
}
