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

struct follow {
  char chave;
  struct set elementos;  
};

struct first *first_set;
struct follow *follow_set;
struct set terminais;
struct set nao_terminais;
struct producoes producoes;
int **tabela_set;
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
void first();
bool first_add(char nao_terminal, char terminal);
struct first *get_first(char nao_terminal);
void print_first();
void follow();
bool follow_add(char nao_terminal, char terminal);
struct follow *get_follow(char nao_terminal);
void print_follow();
void constroiTabela();
int** criaMatriz(int l, int c);
void add_matriz(int** matriz, int l, int c, int conteudo);
int calcula_posicao_naoterminais(char c);
int calcula_posicao_terminais(char c);
void mostra_matriz(int** matriz);

int main(int argc, char *argv[]) {
  lerArgumentos(argc, argv);

  producoes_init(&producoes);
  set_init(&nao_terminais);
  set_init(&terminais);

  FILE * arquivo = abre_arquivo(nome_arquivo);
  parse_arquivo(arquivo);
  fclose(arquivo);
  
  first_set = malloc(nao_terminais.tamanho * sizeof(struct first));
  follow_set = malloc(nao_terminais.tamanho * sizeof(struct follow));
  tabela_set = criaMatriz(nao_terminais.tamanho, terminais.tamanho+1);

  first();
  follow();
  constroiTabela();
  
  print_all();
  mostra_matriz(tabela_set);

  free(first_set);
  exit(EXIT_SUCCESS);
}

void print_all() {
  printf("\nTerminais:\n");
  set_print(&terminais);
  printf("\n");

  printf("Não terminais:\n");
  set_print(&nao_terminais);
  printf("\n");

  printf("Regras de Produção:\n");
  producoes_print(&producoes);
  printf("\n");

  print_first();
  printf("\n\n");
  
  print_follow();
  printf("\n\n");
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
  //inicializando first_set[i]
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
        // adiciona o elemento se for um terminal
        if (set_contains(&terminais, *elemento)) {
          mudou |= first_add(chave, *elemento);
          break;
        }

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
  struct first *f = get_first(nao_terminal);
  int tamanho_old = f->elementos.tamanho;
  set_add(&f->elementos, terminal);

  return tamanho_old != f->elementos.tamanho;
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
  printf("FIRST SET: \n{\n");
  for (size_t i = 0; i < nao_terminais.tamanho; i++) {
    printf("  {\n    Chave: %c, ", first_set[i].chave);
    set_print(&first_set[i].elementos);
    printf("  },\n");
  }
  printf("}\n");
}

void follow() {
  for(size_t i = 0; i < nao_terminais.tamanho; i++){
    follow_set[i].chave = nao_terminais.elementos[i];
    set_init(&follow_set[i].elementos);
  }

  struct regra *producao;
  int tam;
  char *elemento;
  char chave;
  char *anterior;
  
  //RULE 1
  //ADD $ em S (final de cadeia no estado inicial)
  follow_add('S', '$');


  for(int i = 0; i < producoes.tamanho; i++){
    producao = &producoes.regras[i];
    tam = producao->tamanho-1;
    chave = producao->elementos[0];
    
    //RULE 2
    //A -> alpha B betha 
    //O que estão em First de Betha são follow de B ou seja Follow(B) <- First(betha)
    for(int j = tam; j > 1; j--){
      elemento = &producao->elementos[j];
      anterior = &producao->elementos[j-1];
      
      if(set_contains(&nao_terminais, *anterior)){
        if(set_contains(&nao_terminais, *elemento)){
          for (int k = 0; k < nao_terminais.tamanho; k++) {
            if (*elemento == first_set[k].chave) {
              //printf("First de %c => ", *elemento);
              for(int l = 0; l < first_set[k].elementos.tamanho; l++){
                if('e' != first_set[k].elementos.elementos[l]){
                  //printf(" %c ", first_set[k].elementos.elementos[l]);
                  follow_add(*anterior, first_set[k].elementos.elementos[l]);  
                }                
              }
              break;
            } 
          }
        } else {
          follow_add(*anterior, *elemento);
        }
      }
    } 
  }

  bool mudou = false;

  for(int i = 0; i < producoes.tamanho; i++){
    producao = &producoes.regras[i];
    tam = producao->tamanho-1;
    chave = producao->elementos[0];

    //RULE 3
    //A -> alpha B
    //O que estão em Follow A estão em Follow B ou seja Follow(B) <- Follow(A)
    elemento = &producao->elementos[tam];
    if(set_contains(&nao_terminais, *elemento)){
      for(int k = 0; k < nao_terminais.tamanho; k++){
        if(chave == follow_set[k].chave){
          for(int l = 0; l < follow_set[k].elementos.tamanho; l++){
            mudou |= follow_add(*elemento, follow_set[k].elementos.elementos[l]);
          }
          break;
        }
      }
    }

    //RULE 4
    //A -> alpha B betha
    //Se First(B) contem vazio, entao Follow A em Follow B, ou seja Follow(B) <- Follow(A) se betha tem vazio
    for(int j = tam; j > 1; j--){
      elemento = &producao->elementos[j];
      anterior = &producao->elementos[j-1];
      if(set_contains(&nao_terminais, *elemento)){
        for(int k = 0; k < nao_terminais.tamanho; k++){
          if(*elemento == first_set[k].chave){
            if(set_contains(&first_set[k].elementos, 'e')){
              for(int kk = 0; kk < nao_terminais.tamanho; kk++){
                if(chave == follow_set[kk].chave){
                  for(int l = 0; l < follow_set[kk].elementos.tamanho; l++){
                    mudou |= follow_add(*anterior, follow_set[kk].elementos.elementos[l]);
                  }
                  break;
                }
              }
            }
            break;
          }
        }
      }
    }
  } 
}

bool follow_add(char nao_terminal, char terminal){
  int tamanho_old;
  int i;
  for (i = 0; i < nao_terminais.tamanho; i++) {
    if (nao_terminal == follow_set[i].chave) {
      tamanho_old = follow_set[i].elementos.tamanho;
      set_add(&follow_set[i].elementos, terminal);
      break;
    }
  }
  return tamanho_old != follow_set[i].elementos.tamanho;
}

struct follow *get_follow(char nao_terminal) {
  for (size_t i = 0; i < nao_terminais.tamanho; i++) {
    if (nao_terminal == follow_set[i].chave) {
      return &follow_set[i];
    }
  }
  return NULL;
}

void print_follow() {
  printf("FOLLOW SET: \n{");
  for (size_t i = 0; i < nao_terminais.tamanho; i++) {
    printf("{Chave: %c,\n", follow_set[i].chave);
    set_print(&follow_set[i].elementos);
    printf("},\n");
  }
  printf("}\n");
}

void constroiTabela(){
  struct regra *producao;
  //int tam;
  char *elemento;
  char chave;
  //int linha, coluna;
  /*for(int i = 0; i < producoes.tamanho; i++){
    producao = &producoes.regras[i];
    chave = producao->elementos[0];
    linha = calcula_posicao_naoterminais(chave);
    //percorrendo k 0-|NT| ate achar o first do elemento
    for (int k = 0; k < nao_terminais.tamanho; k++) {

      //achou o first do elemento
      if (chave == first_set[k].chave) {
        for(int j = 0; j < first_set[k].elementos.tamanho; j++){
          coluna = calcula_posicao_terminais(first_set[k].elementos.elementos[j]);
          tabela_set[linha][coluna] = i;
        }



        if(set_contains(&first_set[k].elementos, 'e')){

        }
        break;
      }
    }
  }*/

  for(int i = 0; i < producoes.tamanho;i++){
    producao = &producoes.regras[i];
    //tam = producao->tamanho-1;
    chave = producao->elementos[0];
    int linha, coluna;
    //A -> alpha - correndo em cada elemento da producao
    for(int j = 1; j <= 1; j++){
      elemento = &producao->elementos[j];
      
      //se elemento é NT
      if(set_contains(&nao_terminais, *elemento)){
        
        //percorrendo k 0-|NT| ate achar o first do elemento
        for (int k = 0; k < nao_terminais.tamanho; k++) {

          //achou o first do elemento
          if (*elemento == first_set[k].chave) {
            
            //Regra 1
            for(int l = 0; l < first_set[k].elementos.tamanho; l++){
              linha = calcula_posicao_naoterminais(chave);
              coluna = calcula_posicao_terminais(first_set[k].elementos.elementos[l]); 
              tabela_set[linha][coluna] = i;
              //add producao em M[chave, first_set[k].elementos.elementos[l]]
            }

            //Regra 2
            if(set_contains(&first_set[k].elementos, 'e')){
              
              //percorrendo n 0-|NT| ate achar o follow da chave
              for(int n = 0; n < nao_terminais.tamanho; n++){

                //achou o follow da chave
                if(chave == follow_set[n].chave){

                  //percorrendo os elementos do follow(chave)
                  for(int b = 0; b < follow_set[n].elementos.tamanho; b++){
                    linha = calcula_posicao_naoterminais(chave);
                    coluna = calcula_posicao_terminais(follow_set[n].elementos.elementos[b]);
                    tabela_set[linha][coluna] = i;
                    //add producao em M[chave, follow_set[n].elementos.elementos[l]]

                  }

                  //Regra 3
                  if(set_contains(&follow_set[k].elementos, '$')){
                    linha = calcula_posicao_naoterminais(chave);
                    //coluna = calcula_posicao_terminais('$');
                    tabela_set[linha][terminais.tamanho] = i;
                    //add producao em M[chave, $]
                  }

                  break;
                }
              }
            }
            break;
          }
        }
      } else {
        linha = calcula_posicao_naoterminais(chave);
        coluna = calcula_posicao_terminais(*elemento);
        tabela_set[linha][coluna] = i;
        if(*elemento == 'e'){
          for(int n = 0; n < nao_terminais.tamanho; n++){

          //achou o follow da chave
            if(chave == follow_set[n].chave){
              for(int b = 0; b < follow_set[n].elementos.tamanho; b++){
                printf("CHAVE => %c\n", chave);
                linha = calcula_posicao_naoterminais(chave);
                coluna = calcula_posicao_terminais(follow_set[n].elementos.elementos[b]);
                if(follow_set[n].elementos.elementos[b] != '$'){
                  tabela_set[linha][coluna] = i;
                }
                //add producao em M[chave, follow_set[n].elementos.elementos[l]]

              }

              if(set_contains(&follow_set[n].elementos, '$')){
                linha = calcula_posicao_naoterminais(chave);
                coluna = terminais.tamanho;
                tabela_set[linha][coluna] = i;
              }
              break;
            }
          }
        }
        //percorrendo n 0-|NT| ate achar o follow da chave
        /*for(int n = 0; n < nao_terminais.tamanho; n++){

          //achou o follow da chave
          if(chave == follow_set[n].chave){
            //coluna = calcula_posicao_terminais('$');
            //tabela_set[linha][terminais.tamanho] = i;
            break;
          }
        }*/
      }
    
      

    }
  


  }
}

int **criaMatriz(int l, int c){
    int **new_matrix = malloc(l * sizeof(int *));
    new_matrix[0] = calloc(l * c, sizeof(int));
    for (int i = 0; i < l; i++)
        new_matrix[i] = (*new_matrix + i * c);

    for(int i = 0; i < l; i++){
      for(int j = 0; j < c; j++){
        new_matrix[i][j] = -1;
      }
    }
    return new_matrix;
}

int calcula_posicao_terminais(char c){
  for(int i = 0; i < terminais.tamanho; i++){
    if(c == terminais.elementos[i]){
      return i;
    }
  }
  return -1;
}

int calcula_posicao_naoterminais(char c){
  for(int i = 0; i < nao_terminais.tamanho; i++){
    //printf("%c -- %c\n", c, nao_terminais.elementos[i]);
    if(c == nao_terminais.elementos[i]){
      return i;
    }
  }
  return -1;
}

void mostra_matriz(int** matriz){
  printf("\nTabela Preditiva\n");
  printf("\t ");
  int a;
  for(int i = 0; i < terminais.tamanho; i++){
    if(terminais.elementos[i] != 'e'){
      printf("%c\t", terminais.elementos[i]);
    } else {
      a = i;
    }
  }
  printf("$\t");
  for(int i = 0; i < nao_terminais.tamanho; i++){
    printf("\n");
    printf("%c\t", nao_terminais.elementos[i]);
    for(int j = 0; j <= terminais.tamanho; j++){
      if(j == a){
        continue;
      }
      if(matriz[i][j] != -1){
        printf("%s\t", producoes.regras[matriz[i][j]].elementos);
      } else {
        printf("%d\t", matriz[i][j]);
      }
      
    }
  }
  printf("\n\n");
}