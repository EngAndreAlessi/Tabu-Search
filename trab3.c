#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define filename "ulysses16_50_10.dat"


void inicializa_vet(int tam, int *v)
{
	int i;
    for(i=0;i<tam;i++)
    {
        v[i]=0;
    }
}


void print_vet(int *vet, int tam)
{
	int i;
	for(i=0;i<tam;i++)
		printf("%d ",vet[i]);
	printf("\n");
}

int calcula_custo_rota(int *rota, int *draft, int *demanda, int peso , int tamanho, int linha, int coluna, int m[linha][coluna])
{
	int i;
	int ret=0;
	int visitados[tamanho-1];
	for(i=1;i<tamanho-1;i++)
	{
		visitados[i]=0;
	}
	i=0;
	visitados[0] = 1;
	for(i=1;i<tamanho-1;i++)
	{
		if((m[rota[i-1]][rota[i]]>1) && (visitados[rota[i]]==0) && (draft[rota[i]]>=peso))
		{
			ret += m[rota[i-1]][rota[i]];
			peso--;
			visitados[rota[i]]=1;
		}
		else
		{
			return 1000000;
		}	
	}
	ret+=m[rota[i-1]][0];
	return ret;
} 

void copia_vet(int *vet1, int *vet2, int tam)
{
	int i;
	for(i=0;i<tam;i++)
	{
		vet1[i]=vet2[i];
	}
}


void printa_matriz(int tam, int m[tam][tam])
{
    int i,j;
    for(i=0;i<tam;i++)
    {
        for(j=0;j<tam;j++)
        {
            printf("%d ",m[i][j]);
        }
        printf("\n");
    }
}



int main(void)
{
    //==============================================================================================
    //=============Entrada de dados=================================================================
    FILE *fp;

    srand(time(NULL));

    fp = fopen(filename,"r");

    printf(filename"\n");

    int nCidades,max;

    fscanf(fp,"%d",&nCidades);

    int m[nCidades][nCidades];
    int demanda_vet[nCidades];
    int draft_vet[nCidades];
    int peso=0,pesoT;
    int rota[nCidades+1];
    int i,j;
    max=-1;
    for(i=0;i<nCidades;i++)
    {
        for(j=0;j<nCidades;j++)
        {
            fscanf(fp,"%d",&m[i][j]);
            if(m[i][j]>max)
                max=m[i][j];
        }
    }

    for(i=0;i<nCidades;i++)
    {
        fscanf(fp,"%d",&demanda_vet[i]);
        peso+=demanda_vet[i];
    }
    pesoT=peso;
    for(i=0;i<nCidades;i++)
    {
        fscanf(fp,"%d",&draft_vet[i]);
    }

    //=================================================================================================
    //=================================Método guloso===================================================

    // Vetor de visitados
    int distTotal=0 , menorDist;
    int visitados[nCidades];
    for(i=0;i<nCidades;i++)
    {
        visitados[i] = 0;
    }
    visitados[0]=1;
    int k; //contador
    int anterior = 0;
    int prox;
    //Método guloso

    //printf("Cidade: %d \t Dist acumulada: 0\n",anterior);
    rota[0] = 0;
    for(k=1;k<nCidades;k++)
    {
        menorDist = max;
        for(j=0;j<nCidades;j++)
        {
            if( (visitados[j]==0) && (draft_vet[j]>=peso) && (m[anterior][j]<=menorDist) && (anterior!=j) )
            {
                prox=j;
                menorDist = m[anterior][j];
            }
        }
        distTotal+=menorDist;
        anterior=prox;
        rota[k] = anterior;
        visitados[anterior]=1;
        peso-=demanda_vet[anterior];
        //printf("Cidade: %d \t Dist acumulada: %d\n",anterior,distTotal);
    }
    distTotal+=m[anterior][0];
    rota[k] = 0;
    //printf("Cidade: 0 \t Total: %d\n",distTotal);
    printf("Rota gulosa: \n");
    print_vet(rota,nCidades+1);
    printf("Distancia gulosa: %d\n",distTotal);
    //print_vet(visitados,nCidades);
    //print_vet(draft_vet,nCidades);

    //=================================================================================================
    //======================Busca Tabu=================================================================

    int n = 100; // numero de vizinhos a serem gerados
    int S[nCidades+1]; // solucao candidata
    copia_vet(S,rota,nCidades+1); // solucao candidata recebe a solucao gulosa
    int melhor[nCidades+1]; // melhor solucao encontrada
    copia_vet(melhor,S,nCidades+1); // melhor solucao ate entao = gulosa
    int R[nCidades+1],W[nCidades+1]; // vizinhos de S
    int LT[nCidades]; // lista tabu
    for(i=0;i<nCidades;i++)
    {
        LT[i]=0;
    }
    int iter=0; // contador do numero de iteracoes
    int melhorIter=0; // iteracao mais recente que forneceu a melhor solucao ate o momento
    int BTmax = 10000; // numero maximo de iteracoes sem melhora
    int cid1, cid2; // cidades escolhidas para swap
    int cidT1,cidT2; // cidades que irao pra tabu
    int aux; // variavel auxiliar para troca de cidades
    int distR,distW; // distancia total percorrida por R e por W
    int melhorDist = distTotal; // melhor distancia, inicializada com a distancia do metodo guloso
    int distS = distTotal; // distancia da solucao candidata, inicialmente a da gulosa
    //int freqMem[nCidades];
    //inicializa_vet(nCidades,freqMem);
    //int div1,div2; // cidades a serem escolhidas no processo de diversificacao
    
    while(iter-melhorIter<=BTmax)
    {
        iter++;
        copia_vet(R,S,nCidades+1);
        copia_vet(W,S,nCidades+1);
        distR = distS;
        distW = distS;
        for(i=0;i<n;i++)
        {
            cid1 = 1 + rand()%(nCidades-1);
            cid2 = 1 + rand()%(nCidades-1);
            aux = W[cid1];
            W[cid1] = W[cid2];
            W[cid2] = aux;
            //print_vet(W,nCidades+1);
            distW = calcula_custo_rota(W,draft_vet,demanda_vet,pesoT,nCidades+1,nCidades,nCidades,m);
            //printf("DistW: %d\n",distW);
            //sleep(1);
            if( (LT[cid1]==0 && LT[cid2]==0 && distW < distR) || distW < melhorDist )
            {
                copia_vet(R,W,nCidades+1);
                distR = distW;
                cidT1 = cid1;
                cidT2 = cid2;
            }
            copia_vet(W,S,nCidades+1);
        }
        if(distR < distS)
        {
            copia_vet(S,R,nCidades+1);
            distS = distR;
            for(i=0;i<nCidades;i++)
            {
                if(LT[i]>0)
                    LT[i]--;
            }
            LT[cidT1]+=3;
            LT[cidT2]+=3;
        }
        if(distS < melhorDist)
        {
            copia_vet(melhor,S,nCidades+1);
            melhorDist = distS;
            melhorIter = iter;
        }
    }
    printf("Lista tabu:\n");
    print_vet(LT,nCidades);
    printf("Rota apos busca tabu: \n");
    print_vet(melhor,nCidades+1);
    printf("Distancia total: %d\n",melhorDist);

	return 0;
}
