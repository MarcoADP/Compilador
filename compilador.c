#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include "set.h"
#include "producao.h"
#include "err.h"


struct set terminais;
struct set nao_terminais;
struct producao producoes;
extern char *buffer_err;

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
void new_first();
void first();
void follow();

int main(int argc, char *argv[]) {


  int opcao;
  char nome_arquivo[60] = "null";

  // if (argc == 1) {
  //   errexit("É necessário indicar ao menos o nome do arquivo de entrada!\n\n"
  //           "Para mais detalhe, indique a diretiva -h para acessar a Seção Ajuda\n\n");
  // }
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
  producoes_init(&producoes);
  set_init(&nao_terminais);
  set_init(&terminais);
  leitura(arquivo);

  new_first();
  follow();

  printf("\n\n");
  fclose(arquivo);
  return 0;
}

void ajuda() {
  errexit("TO DO\n");
}

FILE *abre_arquivo(char * nome) {
  FILE *arquivo = fopen(nome, "r");
  if (arquivo == NULL) {
    sprintf(buffer_err, "Erro ao abrir o arquivo \"%s\"\n", nome);
    errexit(buffer_err);
  }
  return arquivo;
}


void leitura(FILE *arquivo) {
  int linha_tam = 256;
  char linha[linha_tam];
  while (fgets(linha, linha_tam, arquivo) != NULL) {
    parse_linha(linha);
  }
  set_print(&terminais);
  set_print(&nao_terminais);
  producoes_print(&producoes);
  // mostraLista(listaTerminais, contTerminais);
  // mostraLista(listaNaoTerminais, contNaoTerminais);
  // mostraProducoes();
}

void parse_linha(char *linha) {

  struct set producao;
  // int producao_tam = 0;
  // char producao[REGRA_TAMANHO_MAX];
  char *ch = linha;
  while (!isalpha(*ch)) {
    ch++;
  }
  printf(" NAO TERMINAL(%c) - ", *ch);
  set_add(&producao, *ch);
  // producao[producao_tam++] = *ch;
  set_add(&nao_terminais, *ch);
  ch++;
  while (!isalpha(*ch)) {
    ch++;
  }
  do {
    while (isalpha(*ch) || *ch == '$') {
      if (islower(*ch)) {
        if (*ch == 'e') {
          printf(" TERMINAL(VAZIO) ");
        } else {
          printf(" TERMINAL(%c) ", *ch);
        }
        if (!set_add(&terminais, *ch)) {
          sprintf(buffer_err, "Erro ao adicionar %c no conjunto de terminais.\n", *ch);
          errexit(buffer_err);
        }
      } else if (*ch == '$') {
        printf(" TERMINAL($) ");
        if (!set_add(&terminais, *ch)) {
          sprintf(buffer_err, "Erro ao adicionar %c no conjunto de terminais.\n", *ch);
          errexit(buffer_err);
        }
      } else {
        printf (" NÃO TERMINAL(%c) ", *ch);
        if (!set_add(&nao_terminais, *ch)) {
          sprintf(buffer_err, "Erro ao adicionar %c no conjunto de nao terminais.\n", *ch);
          errexit(buffer_err);
        }
      }
      set_add(&producao, *ch);
      // producao[producao_tam++] = *ch;
      ch++;
    }
    if (*ch == '|') {
      producoes_add(&producoes, &producao);
      // add_producao(producao, producao_tam);
      producao.tamanho = 1; // reseta o buffer mantendo o nao terminal na primeira posicao
      printf("|");
    }
    ch++;
  } while (*ch != '\0');
  producoes_add(&producoes, &producao);
  // add_producao(producao, producao_tam);
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
void new_first() {
  //FIRST(A) = { t | A =>* tw for some w }
  bool mudou;
  struct set *producao;
  char *elemento;
  do {
    mudou = false;
    for (int i = 0; i < producoes.tamanho; i++) {
      producao = &producoes.regras[i];
      elemento = &producao->elementos[1];
      if (set_contains(&terminais, *elemento)) {
        printf("first(%c) = %c\n", producao->elementos[0], *elemento);
      }
    }
  } while (mudou);
}

void first() {
  int i, j;
  printf("\n\n");
  //VERIFICAR LISTAPRODUCAO[0] e LISTAPRODUCAO[1] pode ser o mesmo
  for (i = 0; i < contProducoes; i++) {
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
