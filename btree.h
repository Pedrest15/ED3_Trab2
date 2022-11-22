#ifndef __BTREE_H__
#define __BTREE_H__

#include <stdio.h>
#include <stdlib.h>

#include "register.h"

#define DO_PROMOTION 2
#define NO_PROMOTION 1
#define EXIT 0


//defines de tamanhos bastante usados
#define LEN_PAGDISC 65
#define LEN_FOLHA 1
#define LEN_NROCHAVESNO 4
#define LEN_ALTURANO 4
#define LEN_RRNDONO 4
#define LEN_PONTEIRO 4
#define LEN_SEARCHKEY 4
#define LEN_RRN_KEY 4

//define a estrutura do cabecalho
typedef struct{
    char status;
    int noRaiz;
    int nroChavesTotal;
    int alturaArvore;
    int RRNproxNo;
} BTHeader;

// define estrutura de uma chave e o ponteiro referente
typedef struct {
    int search_key; // IdConecta
    int RRN_key; // RRN no arquivo de dados que essa chave esta localizada
} Key;

//define estutura de um no
typedef struct {
    int ponteiro[5]; // RRN dos nos filhos
    Key key[4]; // chaves

    char folha; 
    int nroChavesNo;
    int alturaNo;
    int RRNdoNo;
} Node;

typedef struct {
    int ponteiro[6]; // RRN dos nos filhos
    Key key[5]; // chaves
} key_list;

// criação do header
BTHeader* create_btheader();
void release_btheader(BTHeader* header);

// leitura e escrita do header
int read_btheader(FILE* file, BTHeader* header);
void write_btheader(FILE* file, BTHeader* header);
void update_btheader(FILE* file, BTHeader* header);

// leitura e escrita do no
int read_node(FILE* file, Node* node);
void write_node(FILE* file, Node* node);

// criação da chave
Key* create_key();

// criação e manipulação do nó
Node* create_node();
void release_node(Node* node);
void delete_keys(Node* node);
void print_nodes(Node* node);

#endif