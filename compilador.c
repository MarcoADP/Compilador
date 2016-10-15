#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


char listaNaoTerminais[50];
char listaTerminais[50];
char listaProducoes[10][50]; //elemento 0 = elemento à esquerda
int contNaoTerminais = 0;
int contTerminais = 0;
int contProducoes = 0;

void ajuda();
FILE *abre_arquivo(char * nome);
void leitura(FILE *arquivo);
void parse_linha(char *linha);
void add_terminal(char term);
void add_nao_terminal(char nterm);
void add_producao(char *prod, int prod_tam);
int convertCharToInt(char ch);
char convertIntToChar(int num);
int verificaExistencia(char lista[50], int cont, char num);
void mostraLista(char lista[50], int cont);
void mostraProducoes();
void first();
void follow();
void constroiTabela();

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

  FILE * arquivo = abre_arquivo(nome_arquivo);

  leitura(arquivo);

  first();
  follow();
  constroiTabela();

  printf("\n\n");
  fclose(arquivo);
  return 0;
}

void ajuda() {
  printf("TO DO\n");
}

FILE *abre_arquivo(char * nome) {
  FILE *arquivo = fopen(nome, "r");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo \"%s\"\n", nome);
    exit(1);
  }
  return arquivo;
}


void leitura(FILE *arquivo) {
  int linha_tam = 256;
  char linha[linha_tam];
  while (fgets(linha, linha_tam, arquivo) != NULL) {
    parse_linha(linha);
  }
  mostraLista(listaTerminais, contTerminais);
  mostraLista(listaNaoTerminais, contNaoTerminais);
  mostraProducoes();
}

void parse_linha(char *linha) {
  int producao_tam = 0;
  char producao[50];
  char *ch = linha;
  while (!isalpha(*ch)) {
    ch++;
  }
  printf(" NAO TERMINAL(%c) - ", *ch);
  producao[producao_tam++] = *ch;
  add_nao_terminal(*ch);
  ch++;
  while (!isalpha(*ch)) {
    ch++;
  }
  do {
    while (isalpha(*ch) || *ch == '$') {
      if (islower(*ch)) {
        if (*ch == 'e') {
          printf (" TERMINAL(VAZIO) ");
        } else {
          printf (" TERMINAL(%c) ", *ch);
        }
        add_terminal(*ch);
      } else if (*ch == '$') {
        printf(" TERMINAL($) ");
        add_terminal(*ch);
      } else {
        printf (" NÃO TERMINAL(%c) ", *ch);
        add_nao_terminal(*ch);
      }
      producao[producao_tam++] = *ch;
      ch++;
    }
    if (*ch == '|') {
      add_producao(producao, producao_tam);
      producao_tam = 1; // reseta o buffer mantendo o nao terminal na primeira posicao
      printf("|");
    }
    ch++;
  } while (*ch != '\0');
  add_producao(producao, producao_tam);
  printf("\n");
}

void add_nao_terminal(char nterm) {
  if (!verificaExistencia(listaNaoTerminais, contNaoTerminais, nterm)) {
    listaNaoTerminais[contNaoTerminais++] = nterm;
  }
}

void add_terminal(char term) {
  if (!verificaExistencia(listaTerminais, contTerminais, term)) {
    listaTerminais[contTerminais++] = term;
  }
}

void add_producao(char *prod, int prod_tam) {
  prod[prod_tam] = '\0';
  printf("producao(%s) ", prod);
  strcpy(listaProducoes[contProducoes++], prod);
}

int convertCharToInt(char ch) {
  return ch;
}

char convertIntToChar(int num) {
  return num;
}

int verificaExistencia(char lista[50], int cont, char num) {
  int i;
  for (i = 0; i < cont; i++) {
    if (lista[i] == num) {
      return 1;
    }
  }
  return 0;
}

void mostraLista(char lista[50], int cont) {
  int i;
  printf("\n");
  for (i = 0; i < cont; i++) {
    printf("%c - ", lista[i]);
  }
  printf("\n");
}

void mostraProducoes() {
  int i, j;
  for (i = 0; i < contProducoes; i++) {
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
  for (i = (contProducoes - 1); i >= 0; i--) {
    //printf("\n");
    if (verificaExistencia(listaTerminais, contTerminais, listaProducoes[i][1])) {
        printf("\n%c -- TERMINAL", listaProducoes[i][1]);
        //adicionar listaproducao[i][1] em first(listaproducao[i][1])
    } else if (verificaExistencia(listaNaoTerminais, contNaoTerminais, listaProducoes[i][1])) {
        printf("\n%c -- NAO TERMINAL", listaProducoes[i][1]);
      //adicionar o first(listaproducao[i][1]) em first(listaproducao[i][1]) EXCETO e
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

void constroiTabela(){
    int tabela[contNaoTerminais+1][contTerminais+1];
    int i, j;
    for(i = 0; i <= contNaoTerminais; i++){
        for(j = 0; j <= contTerminais; j++){
            tabela[i][j] = 35;
        }
    }

    for(i = 1; i <= contNaoTerminais; i++){
        //printf("\n%c", listaNaoTerminais[i-1]);
        tabela[i][0] = convertCharToInt(listaNaoTerminais[i-1]);
    }

    for(i = 1; i <= contTerminais; i++){
        //printf("\n%c", listaNaoTerminais[i-1]);
        tabela[0][i] = convertCharToInt(listaTerminais[i-1]);
    }

    //PREENCHER A TABELA!!!

    printf("\n");
    for(i = 0; i <= contNaoTerminais; i++){
        printf("\n");
        for(j = 0; j <= contTerminais; j++){
            printf("%c  ", convertIntToChar(tabela[i][j]));
        }
    }

    //retornar a tabela
}
