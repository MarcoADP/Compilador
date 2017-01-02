#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include "includes/set.h"
#include "includes/producao.h"
#include "includes/err.h"


struct set terminais;
struct set nao_terminais;
struct producoes producoes;
struct producoes producoes_novas;
extern char buffer_err[];

char nome_arquivo[60];


void print_all();
void lerArgumentos(int argc, char *argv[]);
void ajuda();
FILE *abre_arquivo(char * nome);
void parse_arquivo(FILE *arquivo);
void parse_linha(char *linha);
void parse_left(char *left, char *middle, struct regra *producao);
void parse_right(char *right, struct regra *producao);
void parse_terminais();
int convertCharToInt(char ch);
char convertIntToChar(int num);
void mostra_matriz(int** matriz);
bool temRecursaoDireta(char NT);
void eliminarRecursao();
void substituirRecursaoIndireta(char Ai, char Aj, struct regra *producao);
void eliminarRecursaoImediata(int num);
char proximoNT();


int main(int argc, char *argv[]) {
  lerArgumentos(argc, argv);

  producoes_init(&producoes);
  set_init(&nao_terminais);
  set_init(&terminais);

  FILE * arquivo = abre_arquivo(nome_arquivo);
  parse_arquivo(arquivo);
  fclose(arquivo);

  eliminarRecursao();

  print_all();

  return 0;
}

bool temRecursaoDireta(char NT) {
  struct regra *producao;

  for(int i = 0; i < producoes_novas.tamanho; i++) {
    producao = &producoes_novas.regras[i];
    if(producao->elementos[0] == producao->elementos[1] && producao->elementos[0] == NT) {
      return true;
    }
  }
  return false;
}

void eliminarRecursao() {
  int n = nao_terminais.tamanho;
  struct regra *producao;

  producoes_init(&producoes_novas);
  producoes_copy(&producoes, &producoes_novas);

  for(int i = 0; i < n; i++) {
    for(int j = 0; j < i; j++) {
      for(int k = producoes_novas.tamanho - 1; k >= 0; k--) {
        producao = &producoes_novas.regras[k];

        //SE Alguma producao P for A(i) -> A(j)...
        if((producao->elementos[0] == nao_terminais.elementos[i]) && (producao->elementos[1] == nao_terminais.elementos[j])) {
          //Substituir A(j) por betha, onde A(j) -> betha, para todas as producoes que corresponda 
          substituirRecursaoIndireta(nao_terminais.elementos[i], nao_terminais.elementos[j], producao);
        }
      } 
    }
    eliminarRecursaoImediata(i);
  }
}

void substituirRecursaoIndireta(char Ai, char Aj, struct regra *p) {
  char cadeiaY[50];
  strcpy(cadeiaY, &p->elementos[2]);

  // Remover Ai -> Aj cadeiaY
  producoes_remove(&producoes_novas, p);

  for (int i = 0; i < nao_terminais.tamanho; ++i) {
    struct regra *producao = &producoes_novas.regras[i];
    // Aj -> delta1 | delta2 | .. | deltak
    if (producao->elementos[0] == Aj) {
      //Criar nova regra Ai -> delta(i)cadeiaY
      struct regra producao_nova;
      regra_init(&producao_nova);
      regra_add(&producao_nova, Ai);

      for (int j = 1; j < producao->tamanho; ++j) {
        regra_add(&producao_nova, producao->elementos[j]);
      }
      for (char *ch = cadeiaY; *ch != '\0'; ch++) {
        regra_add(&producao_nova, *ch);
      }

      producoes_add(&producoes_novas, &producao_nova);
    }
  }
}

void eliminarRecursaoImediata(int num) {
  char NT = nao_terminais.elementos[num];
  struct regra *producao;
  char ch;

  if (temRecursaoDireta(NT)) {
    ch = proximoNT();
    set_add(&nao_terminais, ch);

    struct producoes producoesNT;
    producoes_init(&producoesNT);
    producoes_get(NT, &producoes_novas, &producoesNT);


    //ADD A -> betha A'
    for(int i = 0; i < producoesNT.tamanho; i++) {
      producao = &producoesNT.regras[i];
      if(producao->elementos[1] != NT) {
        //ADD A -> bethaA'
        struct regra producao_nova;

        if(producao->elementos[1] == 'e') {
          regra_init(&producao_nova);
          regra_add(&producao_nova, producao->elementos[0]);
        } else {
          producao_nova = *producao;
        }
        regra_add(&producao_nova, ch);
        producoes_add2(&producoes_novas, &producao_nova, NT);
      }
    }
    bool primeiro = true;
    //ADD A' -> alphaA'  
    for(int i = 0; i < producoesNT.tamanho; i++) {
      producao = &producoesNT.regras[i];
      if(producao->elementos[1] == NT) {
        //Criando A' -> (...)A'
        struct regra producao_nova;
        regra_init(&producao_nova);
        regra_add(&producao_nova, ch);
        for(int i = 2; i < producao->tamanho; i++) {
          regra_add(&producao_nova, producao->elementos[i]);
        }
        regra_add(&producao_nova, ch);
        if (primeiro)
          producoes_add2(&producoes_novas, &producao_nova, NT);
        else 
          producoes_add2(&producoes_novas, &producao_nova, ch);
      }
    }
    struct regra producao_nova;
    regra_init(&producao_nova);
    regra_add(&producao_nova, ch);
    regra_add(&producao_nova, 'e');
    producoes_add2(&producoes_novas, &producao_nova, ch);

    // Remover producoes antigas de A (NT) 
    for (int i = 0; i < producoesNT.tamanho; i++) {
      producao = &producoesNT.regras[i];
      producoes_remove(&producoes_novas, producao);
    }
  }
}

char proximoNT() {
  for(char ch = 'A'; ch <= 'Z'; ch++) {
    if (!set_contains(&nao_terminais, ch)) {
      return ch;
    }
  }
  return 'Z';
}

void print_all() {
  printf("Gramatica original:\n");
  producoes_print_formatado(&producoes);
  printf("\n");

  printf("Gramatica sem Recursao a Esquerda:\n");
  producoes_print_formatado(&producoes_novas);
  printf("\n");
}

void lerArgumentos(int argc, char *argv[]) {
  int opcao;
  bool i_flag = false;

  while ((opcao = getopt(argc,argv,"hi:")) != -1) {
    switch (opcao) {
      case 'i':
        strcpy(nome_arquivo, optarg);
        i_flag = true;
        break;
      case 'h':
        ajuda(argv[0]);
        break;
      case '?':
      case ':':
      default:
        ajuda(argv[0]);
    }
  }

  if (!i_flag) {
    ajuda(argv[0]);
  }
}

void ajuda(char *nome_programa) {
  sprintf(buffer_err, "\n Usage: %s -i arquivo_entrada <opcoes>\n\
    \n  OPCOES:\n\
    -h,                      mostra tela de ajuda.\n", nome_programa);
  errexit(buffer_err);
}

FILE *abre_arquivo(char * nome) {
  FILE *arquivo = fopen(nome, "r");
  if (arquivo == NULL) {
    sprintf(buffer_err, "Erro ao abrir o arquivo \"%s\"\n", nome);
    errexit(buffer_err);
  }
  return arquivo;
}

void parse_arquivo(FILE *arquivo) {
  int linha_tam = 256;
  char linha[linha_tam];

  while (fgets(linha, linha_tam, arquivo) != NULL) {
    parse_linha(linha);
  }

  parse_terminais();
}

void parse_linha(char *linha) {
  char *left, *right, *middle;
  struct regra producao;

  regra_init(&producao);

  left = linha;
  middle = strchr(linha, '-');
  right = middle + 1;

  parse_left(left, middle, &producao);
  parse_right(right, &producao);
  producoes_add(&producoes, &producao);
}

void parse_left(char *left, char *middle, struct regra *producao) {
  // avança ponteiro até o símbolo não terminal
  while (isspace(*left)) {
    left++;
  }

  if (!isupper(*left)) {
    sprintf(buffer_err, "ERRO: O Simbolo nao terminal '%c' precisa ser uppercase.", *left);
    errexit(buffer_err);
  }

  set_add(&nao_terminais, *left);
  regra_add(producao, *left);
}

void parse_right(char *right, struct regra *producao) {
  // avança ponteiro até o primeiro símbolo
  while (isspace(*right)) {
    right++;
  }

  do {
    while (!isspace(*right) && *right != '\0' && *right != '|') {
      regra_add(producao, *right);
      right++;
    }

    if (*right == '|') {
      producoes_add(&producoes, producao);
      producao->tamanho = 1;
    }

    if (*right != '\0') {
      right++;
    }
  } while (*right != '\0');
}

void parse_terminais() {
  struct regra *producao;
  for (int i = 0; i < producoes.tamanho; i++) {
    producao = &producoes.regras[i];
    for (size_t j = 1; j < producao->tamanho; j++) {
      if (!set_contains(&nao_terminais, producao->elementos[j])) {
        set_add(&terminais, producao->elementos[j]);
      }
    }
  }
}

int convertCharToInt(char ch) {
  return ch;
}

char convertIntToChar(int num) {
  return num;
}