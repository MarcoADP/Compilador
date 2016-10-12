#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE * arquivo;
int listaNaoTerminais[50];
int listaTerminais[50];
char listaProducoes[10][50]; //elemento 0 = elemento à esquerda
int contNaoTerminais = 0;
int contTerminais = 0;
int contProducoes = 0;

void ajuda();
void leitura(char * nome);
int convertCharToInt(char ch);
char convertIntToChar(int num);
int verificaExistencia(int lista[50], int cont, int num);
void mostraLista(int lista[50], int cont);
void mostraProducoes();
void first();
void follow();

int main(int argc, char *argv[]) {


  int opcao;
  char nome_arquivo[60] = "null";

  if (argc == 1) {
    printf("É necessário indicar ao menos o nome do arquivo de entrada!\n\n");
    printf("Para mais detalhe, indique a diretiva -h para acessar a Seção Ajuda\n\n");
    return 0;
  }
  //printf("a\n");
  while ((opcao = getopt(argc,argv,"hi:")) != -1) {
    switch (opcao) {
      case 'h':
        ajuda();
        return 0;
      case 'i':
        strcpy(nome_arquivo, optarg);
        break;
      case '?':
      case ':':
      default:
        ajuda();
        return 0;
    }
  }

  printf("%s\n", nome_arquivo);
  if (!strcmp(nome_arquivo, "null")) {
    printf("erro no nome do arquivo\n");
    return 0;
  } else {
    leitura(nome_arquivo);
  }

  first();
  follow();

  printf("\n\n");
  fclose(arquivo);

  return 0;
}

void ajuda() {
  printf("TO DO\n");
}

void leitura(char * nome) {

  arquivo = fopen(nome, "r");

  if(arquivo == NULL) {
    printf("Erro ao abrir o arquivo\n");
    return;
  }

  char ch = fgetc(arquivo);
  int ascii = ch;
  printf("\n");
  int contAtual = 0;
  char chEsquerda;
  int flag = 0;
  while (ch != EOF) {
    if (ch == '\n') {
      //printf(" ENTER ");
      contProducoes++;
      contAtual = 0;
      flag = 0;
      printf("\n");
    }
    else if (ch == '\t') {
      //printf(" TAB ");
    }
    else if (ch == ' ') {
      //printf(" ESPAÇO ");
    } else if (ascii > 64 && ascii < 91) {
      printf(" NAO TERMINAL ");
      if (flag == 0) {
        chEsquerda = ch;
        //listaProducoes[contProducoes][contAtual] = ch;
        //contAtual++;
      }
      listaProducoes[contProducoes][contAtual] = ch;
      contAtual++;
      if (!verificaExistencia(listaNaoTerminais, contNaoTerminais, ch)) {
        listaNaoTerminais[contNaoTerminais] = ch;
        contNaoTerminais++;
      }
      //printf(" %c ", ch);
    } else if (ascii > 47 && ascii < 58) {
      printf(" NUMERO ");
    } else if (ascii > 60 && ascii < 123) {
      printf(" TERMINAL ");
      flag = 1;
      if (contAtual == 0) {
        listaProducoes[contProducoes][contAtual] = chEsquerda;
        contAtual++;
      }
      listaProducoes[contProducoes][contAtual] = ch;
      contAtual++;
      if (!verificaExistencia(listaTerminais, contTerminais, ch)) {
        listaTerminais[contTerminais] = ch;
        contTerminais++;
      }
    } else if(ascii == 124) {
      printf(" %c ", ch);
      contProducoes++;
      contAtual = 0;
      flag = 1;
    } else if(ascii == 45) {
      printf(" %c ", ch);
    } else {
      printf(" TERMINAL ");
      flag = 1;
      if (contAtual == 0) {
        listaProducoes[contProducoes][contAtual] = chEsquerda;
        contAtual++;
      }
      listaProducoes[contProducoes][contAtual] = ch;
      contAtual++;
      if (!verificaExistencia(listaTerminais, contTerminais, ch)) {
        listaTerminais[contTerminais] = ch;
        contTerminais++;
      }
    }
    ch = fgetc(arquivo);
    ascii = ch;
  }
  printf("\n");
  mostraLista(listaNaoTerminais, contNaoTerminais);
  mostraLista(listaTerminais, contTerminais);
  mostraProducoes();
}

int convertCharToInt(char ch) {
  return ch;
}

char convertIntToChar(int num) {
  return num;
}

int verificaExistencia(int lista[50], int cont, int num) {
  int i;
  for (i = 0; i < cont; i++) {
    if (lista[i] == num) {
      return 1;
    }
  }
  return 0;
}

void mostraLista(int lista[50], int cont) {
  int i;
  printf("\n");
  for (i = 0; i < cont; i++) {
    printf("%c - ", lista[i]);
  }
  printf("\n");
}

void mostraProducoes() {
  int i, j;
  for (i = 0; i <= contProducoes; i++) {
    printf("\n");
    for (j = 0; j < 50; j++) {
      printf("%c", listaProducoes[i][j]);
      if (j == 0) {
        printf(" - ");
      }
    }
  }
}

void first() {
  int i, j;
  printf("\n\n");
  //VERIFICAR LISTAPRODUCAO[0] e LISTAPRODUCAO[1] pode ser o mesmo
  for (i = 0; i <= contProducoes; i++) {
    printf("\n");
    if (verificaExistencia(listaTerminais, contTerminais, listaProducoes[i][j])) {
      //adicionar listaproducao[i][1] em first(listaproducao[i][0])
    } else if (verificaExistencia(listaNaoTerminais, contNaoTerminais, listaProducoes[i][j])) {
      //adicionar o first(listaproducao[i][1]) em first(listaproducao[i][0]) EXCETO e
    }

    for (j = 2; j < 50; j++) {
      //if(algo que nao entendi){
      //first(listaproducao[i][0]) += first(Xi) EXCETO e
      //}
    }
    //if(listaproducao[i][0]) deriva e{
    //  first(listaproducao[i][0]) += e
    //}
  }
}

void follow() {
  //if()
}
