#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include "set.h"
#include "producao.h"
#include "err.h"

struct first {
  char chave;
  struct set elementos;
};

struct first *first_set;
struct set terminais;
struct set nao_terminais;
struct producoes producoes;
extern char buffer_err[];

char nome_arquivo[60];

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
void first();
bool first_add(char nao_terminal, char terminal);
struct first *get_first(char nao_terminal);
void print_first();
void follow();
void constroiTabela();

int main(int argc, char *argv[]) {
  lerArgumentos(argc, argv);

  FILE * arquivo = abre_arquivo(nome_arquivo);
  producoes_init(&producoes);
  set_init(&nao_terminais);
  set_init(&terminais);
  parse_arquivo(arquivo);
  fclose(arquivo);
  printf("\nTerminais:\n");
  set_print(&terminais);
  printf("\n");
  printf("Não terminais:\n");
  set_print(&nao_terminais);
  printf("\n");
  printf("Regras de Produção:\n");
  producoes_print(&producoes);
  printf("\n");

  first_set = malloc(nao_terminais.tamanho * sizeof(struct first));
  first();
  print_first();
  follow();
  constroiTabela();
  free(first_set);
  printf("\n\n");
  exit(EXIT_SUCCESS);
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

void first() {
  for (size_t i = 0; i < nao_terminais.tamanho; i++) {
    first_set[i].chave = nao_terminais.elementos[i];
    set_init(&first_set[i].elementos);
  }
  struct regra *producao;
  char *elemento;
  char chave;
  //FIRST(A) = { t | A =>* tw for some w }
  for (int i = 0; i < producoes.tamanho; i++) {
    producao = &producoes.regras[i];
    chave = producao->elementos[0];
    elemento = &producao->elementos[1];
    if (set_contains(&terminais, *elemento)) {
      // printf("first(%c) = %c\n", chave, *elemento);
      first_add(chave, *elemento);
    }
  }

  bool mudou;
  do {
    mudou = false;
    for (int i = 0; i < producoes.tamanho; i++) {
      producao = &producoes.regras[i];
      chave = producao->elementos[0];
      elemento = &producao->elementos[1];
      // comeca com nao terminal
      if (set_contains(&terminais, *elemento)) {
        continue;
      }
      do {
        // comeca com nao terminal
        // adiciona todos os first deste nao terminal exceto vazio
        struct first *f = get_first(*elemento);
        for (size_t i = 0; i < f->elementos.tamanho; i++) {
          if (f->elementos.elementos[i] != 'e') {
            mudou |= first_add(chave, f->elementos.elementos[i]);
          }
        }
        // se nao terminal nao deriva vazio, passa a proxima producao
        if (!set_contains(&f->elementos, 'e')) {
          // nao terminal e nao deriva VAZIO
          break;
        }
        elemento++;
      } while (*elemento != '\0');
      // todos os elementos da producao sao nao terminais e derivam vazio
      if (elemento == '\0') {
        mudou |= first_add(chave, 'e');
      }
    }
  } while (mudou);
}

bool first_add(char nao_terminal, char terminal) {
  int tamanho_old;
  int i;
  for (i = 0; i < nao_terminais.tamanho; i++) {
    if (nao_terminal == first_set[i].chave) {
      tamanho_old = first_set[i].elementos.tamanho;
      set_add(&first_set[i].elementos, terminal);
      break;
    }
  }
  return tamanho_old != first_set[i].elementos.tamanho;
}

struct first *get_first(char nao_terminal) {
  for (size_t i = 0; i < nao_terminais.tamanho; i++) {
    if (nao_terminal == first_set[i].chave) {
      return &first_set[i];
    }
  }
  return NULL;
}

void print_first() {
  printf("FIRST SET: \n{");
  for (size_t i = 0; i < nao_terminais.tamanho; i++) {
    printf("{Chave: %c,\n", first_set[i].chave);
    set_print(&first_set[i].elementos);
    printf("},\n");
  }
  printf("}\n");
}

void follow() {
  //if()
}

void constroiTabela(){
    // int tabela[contNaoTerminais+1][contTerminais+1];
    // int i, j;
    // for(i = 0; i <= contNaoTerminais; i++){
    //     for(j = 0; j <= contTerminais; j++){
    //         tabela[i][j] = 35;
    //     }
    // }
    //
    // for(i = 1; i <= contNaoTerminais; i++){
    //     //printf("\n%c", listaNaoTerminais[i-1]);
    //     tabela[i][0] = convertCharToInt(listaNaoTerminais[i-1]);
    // }
    //
    // for(i = 1; i <= contTerminais; i++){
    //     //printf("\n%c", listaNaoTerminais[i-1]);
    //     tabela[0][i] = convertCharToInt(listaTerminais[i-1]);
    // }
    //
    // //PREENCHER A TABELA!!!
    //
    // printf("\n");
    // for(i = 0; i <= contNaoTerminais; i++){
    //     printf("\n");
    //     for(j = 0; j <= contTerminais; j++){
    //         printf("%c  ", convertIntToChar(tabela[i][j]));
    //     }
    // }
    //
    // //retornar a tabela
}
