#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "btree.h"
#include "funcoesFornecidas.h"

/**
 * @brief Essa função serve para fazer as chaves e os ponteiros pularem uma casa para a direita
 * a partir de uma posição especificada
 * 
 * @param node no nas quais as chaves iram dar o shift
 * @param j posição especificada
 */
void shift_keys_node(Node* node, int pos);


/**
 * @brief Nessa função é feito o split que acontece quando é necessario adicionar uma nova chave
 * no nó, porém ele já esta cheio, então é criado um nó "irmão" aonde ficará os 2 maiores
 * valores desse nó e o valor mediano será promovida para o nó pai desses irmãos
 * 
 * @param local_key chave a ser inserida
 * @param local_right_rrn ponteiro a direita da chave a ser inserida
 * @param node nó que será dividido
 * @param promo_key chave que será promovida para o nó pai
 * @param right_rrn ponteiro a direita dessa chave
 * @param new_node nó irmão que será criado no split
 */
void split(Key* local_key, int* local_right_rrn, Node* node, Key* promo_key, int* right_rrn, Node* new_node);


/**
 * @brief Nessa função acontece o insert onde nele é encontrado o local ideal para a chave ser inserida
 * a partir de uma recurssão. Se for possível, a chave é inserida, se não, essa função chama a função
 * split o que torna possível a inserção dessa chave
 * 
 * @param file arquivo index_file para ler os nós
 * @param current_rrn rrn do nó atual
 * @param key chave a ser inserida
 * @param promo_key chave q será promovida, se necessário
 * @param right_rrn ponteiro a direita dessa chave
 * @return int retorna se é ou não necessário fazer a promoção no nó pai
 */
int insert(FILE* file, int current_rrn, Key* key, Key* promo_key, int* right_rrn);

/**
 * @brief Aqui é onde funciona a função driver que inicializa a árvore, criando a sua primeira raiz e
 * inicilizando a primeira chave. Após isso é feito um for pelo arquivo de data inteiro, pegando todas
 * as chaves e adicionando na arvore B. Aqui tambem é feita a criação de uma nova raiz se for feita o
 * split da raiz em algum momento
 * 
 * @param data_name nome do arquivo de dados
 * @param index_name nome do arquivo de indexes
 * @return int retorna se a função foi concluida com sucesso ou nao
 */
int command7(char data_name[], char index_name[]);

/**
 * @brief Nesse função ocorre a inserção de dados a partir de inputs dados pelo teclado
 * 
 * @param data_name nome do aquivo de dados
 * @param index_name nome do aquivo de indexes
 * @return int retorna se a função foi concluida com sucesso ou nao
 */
int command9(char* data_name, char* index_name);

