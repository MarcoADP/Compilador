#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE * arquivo;

void leitura(char * nome);
void ajuda();

int main(int argc, char *argv[]){


	int opcao;
	char nome_arquivo[60] = "null";

	if(argc == 1){
	  printf("É necessário indicar ao menos o nome do arquivo de entrada!\n\n");
	  printf("Para mais detalhe, indique a diretiva -h para acessar a Seção Ajuda\n\n");
	  return 0;
	}
	//printf("a\n");
	while((opcao = getopt(argc,argv,"hi:")) != -1){
      	switch (opcao) {
         	case 'h':
         	{
            	ajuda();
            	return 0;
         	}
	        case 'i':
	        {
	        	strcpy(nome_arquivo, optarg);
	            break;
	        }
    	}  
   	}

	printf("%s\n", nome_arquivo);
   	if(!strcmp(nome_arquivo, "null")){

   		printf("erro no nome do arquivo\n");
   		return 0;	
   	} else {
   		leitura(nome_arquivo);	
   	}
	
	printf("\n\n");

	fclose(arquivo);

	return 0;
}

void ajuda(){
	printf("TO DO\n");
}

void leitura(char * nome){
	
	arquivo = fopen(nome, "r");

	if(arquivo == NULL){
		printf("Erro ao abrir o arquivo\n");
		return;
	}

	char ch = fgetc(arquivo);
	int ascii = ch;
	printf("\n");
	while(ch != EOF){
		if(ch == '\n'){
			//printf(" ENTER ");
			printf("\n");
		}
		else if(ch == '\t'){
			//printf(" TAB ");
		}
		else if(ch == ' '){
			//printf(" ESPAÇO ");
		} else if (ascii > 64 && ascii < 91){
			printf(" NAO TERMINAL ");
			//printf(" %c ", ch);	
		} else if (ascii > 47 && ascii < 58){
			printf(" NUMERO ");
		} else if (ascii > 60 && ascii < 123){
			printf(" TERMINAL ");
		} else {
			printf(" %c ", ch);	
		}
		
		ch = fgetc(arquivo);
		ascii = ch;
	}

}