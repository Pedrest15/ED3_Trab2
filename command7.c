// A ideia dessa comando é pegar o arquivo .bin do trabalho 1 e criar uma árvore de indices para ele

// cada no(pagina de disco) possui 5 descendentes
// cada no, menos a raiz, possui no minimo 3 descendentes
// a raiz possui pelo menos 2 descentes
// todas as folhas aparecem no mesmo nível
#include "command7.h"

typedef struct
{
    int ponteiro[6]; // RRN dos nos filhos
    Key key[5]; // chaves
}key_list;



int recursive_next_node(FILE* index_file, Node* node, Key* data, int node_pai) {

    if (node->folha == '1') { // né ???
        return node_pai;
    }

    for (size_t i = 0; i < 4; i++) {

        if (data->search_key > node->key[i]->search_key && i != 3) {
            continue;
        }
        if (node->ponteiro[i] != -1) {
            node_pai = node->RRNdoNo;
            fseek(index_file, ((node->ponteiro[i]*LEN_PAGDISC) + LEN_PAGDISC), SEEK_SET);
            read_node(index_file, node);
            node_pai = recursive_next_node(index_file, node, data, node_pai);
            break;
        }
    }
    return node_pai;
}

void shift_keys_node(Node* node, int j) {

    for (int i = 3; i >= j; i--) {

        if (i != j) {
            node->ponteiro[i + 1] = node->ponteiro[i];
        }

        if(i != 3) {
            *(node->key[i + 1]) = *(node->key[i]);
        }
    }  
}

void split(Key* local_key, int* local_right_rrn, Node* node, Key* promo_key, int* right_rrn, Node* new_node) {
    
    printf("entrei split\n");
    key_list* keys = malloc(sizeof(key_list));
    for (size_t i = 0; i < 5; i++)
    {
        keys->ponteiro[i] = node->ponteiro[i];
        if (i != 4) {
            keys->key[i].search_key = node->key[i]->search_key;
            keys->key[i].RRN_key = node->key[i]->RRN_key;
        }
    }
    keys->key[4] = *(local_key);
    keys->ponteiro[5] = *(local_right_rrn);
    

    for (int j = 0; j < 6; j++)
    {
        if (local_key->search_key <= keys->key[j].search_key) {
            
            for (int i = 4; i >= j; i--) {
                if (i != j) {
                    keys->ponteiro[i+1] = keys->ponteiro[i];
                }
                if (i != 4) {
                    keys->key[i+1] = keys->key[i];
                }
                keys[j + 1] = keys[j];
            }
            
            keys->key[j] = *(local_key);
            keys->ponteiro[j+1] = *(local_right_rrn);
            break;

        }
    }
    

    *promo_key = keys->key[2]; // elemento central do array
    //*right_rrn = keys->ponteiro[3];

    printf("RAIZES\n");
    for (size_t i = 0; i < 6; i++)
    {
        printf("Ponteiro %ld - %d\n", i, keys->ponteiro[i]);
        if (i != 5) {
            printf("Chave %ld -    %d\n", i, keys->key[i].search_key);
        }
    }
    printf("\n");

    new_node->alturaNo = node->alturaNo;
    new_node->folha = node->folha;
    new_node->nroChavesNo = 2;

    // coloca os valores dps do centro para a nova pagina
    for (size_t i = 0; i < 3; i++)
    {
        new_node->ponteiro[i] = keys->ponteiro[i+3];
        if (i != 2) {
            *(new_node->key[i]) = keys->key[i+3];
        }
    }


    //deleta os valores e coloca os novos valores na pagina antiga
    delete_keys(node);
    printf("to aqui\n");
    node->nroChavesNo = 2;
    for (size_t i = 0; i < 2; i++)
    {
        node->ponteiro[i] = keys->ponteiro[i];
        if (i != 4) {
            node->key[i]->search_key = keys->key[i].search_key;
            node->key[i]->RRN_key = keys->key[i].RRN_key;
        }
    }
    
        free(keys);
        printf("sai do split\n");
}

int insert(FILE* file, int current_rrn, Key* key, Key* promo_key, int* right_rrn) {

    //printf("entre insert\n");
    //printf("entrei insert, %d\n", current_rrn);
    // como esse nó nao existe, a chave deverá ficar no nó anterior
    if (current_rrn == -1) {
        *promo_key = *key;
        *right_rrn = -1;
        //printf("-> atribuindo a promo_key: %d\n", (*promo_key)->search_key);
        return DO_PROMOTION;
        
    } else {
        Node* node = create_node();
        fseek(file, LEN_PAGDISC + (current_rrn*LEN_PAGDISC), SEEK_SET);
        read_node(file, node);

        int pos;
        for (size_t i = 0; i < 4; i++)
        {
            if (key->search_key <= node->key[i]->search_key || node->key[i]->search_key == -1) {
                if (node->key[i]->search_key == key->search_key) {
                    printf("Chave já inserida\n");
                    printf("linha 155\n");
                    release_node(node);

                    return EXIT;
                }
                pos = i;
                break;
            }
            pos = 3;
        }

        Key* local_promo = malloc(sizeof(Key));
        int* local_right_rrn = malloc(sizeof(int));
        int return_value = insert(file, node->ponteiro[pos], key, local_promo, local_right_rrn);

        if (return_value == EXIT, return_value == NO_PROMOTION) {
            printf("linha 170\n");
            release_node(node);
            free(local_promo);
            free(local_right_rrn);

            return return_value;

        } else if (node->nroChavesNo < 4) {
            // insert P_B_KEY and P_B_RRN in PAGE
            if (node->key[pos]->search_key == -1) {
                *(node->key[pos]) = *local_promo;
                node->ponteiro[pos+1] = *local_right_rrn;
            } else {
                shift_keys_node(node, pos);
                node->key[pos]->search_key = local_promo->search_key;
                node->key[pos]->RRN_key = local_promo->RRN_key;
                node->ponteiro[pos+1] = *local_right_rrn;
            }            
            
            node->nroChavesNo++;
            fseek(file, LEN_PAGDISC + (current_rrn*LEN_PAGDISC), SEEK_SET);
            write_node(file, node);
            print_nodes(node);

            printf("linha 194\n");
            release_node(node);
            free(local_promo);
            free(local_right_rrn);

            return NO_PROMOTION;
        } else {
            Node* new_node = create_node();
            print_nodes(node);
            split(local_promo, local_right_rrn, node, promo_key, right_rrn, new_node);
            
            // escrevendo o node
            fseek(file, LEN_PAGDISC + (current_rrn*LEN_PAGDISC), SEEK_SET);
            write_node(file, node);
            print_nodes(node);

            // verifica qual o proximo rrn livre
            BTHeader* header = create_btheader();
            read_btheader(file, header);

            // escreve o novo no
            new_node->RRNdoNo = header->RRNproxNo;
            fseek(file, LEN_PAGDISC + (header->RRNproxNo*LEN_PAGDISC), SEEK_SET);
            write_node(file, new_node);
            print_nodes(new_node);

            // update no header com o novo proximo rrn
            *right_rrn = header->RRNproxNo;
            header->RRNproxNo++;
            update_btheader(file, header);

            printf("linha 225\n");
            free(node);
            printf("linha 230\n");
            free(local_promo);
            printf("linha 232\n");
            free(local_right_rrn);
            printf("linha 229\n");
            release_node(new_node);
            free(header);

            return DO_PROMOTION;
        }
        
    }
}

// colocar 3 /// aparece
/// @brief 
/// @param data_name 
/// @param index_name 
/// @return 
int command7(char* data_name, char* index_name) {

    // abrindo arquivo de dados
    FILE* data_file = fopen(data_name, "rb"); // problema na funçao de abrir arquivo TO DO
    if (data_file == NULL) {
        return 0;
    }

    // abrindo arquivo de indices
    FILE* index_file = fopen(index_name, "wb+");
    if (index_file == NULL) {
       return 0;
    }    

    // lendo header do arquivo de dados
    Header_reg* reg_header = malloc(sizeof(Header_reg));
    read_header(reg_header, data_file);
    

    // TO DO POSSÍVEIS CASOS QUE O PROGRAMA ACABA NA HORA
    /**
     * STATUS = 0
     * PROX_RRN = 0
    */


    // lendo o primeiro registro dos dados
    Data_reg* data = malloc(sizeof(Data_reg));
    Key* data_key = create_key();
    char aux;
    fseek(data_file, LEN_DISC_PAG, SEEK_SET);
    read_register(data_file, data);

    // colocando esse primeiro registro lido como raiz
    Node* node = create_node();
    node->folha = '1'; // nao tem filhos
    node->nroChavesNo = 1; // vai ter uma chave
    node->alturaNo = 0; // ta na parte mais baixo da arvore
    node->RRNdoNo = 0; // ????
    node->key[0]->search_key = data->idConecta; // chave raiz no momento
    node->key[0]->RRN_key = 0; // RRN dessa chave no arquivo de dados

    // criando e escrevendo o header do arquivo de index
    BTHeader* ind_header = create_btheader();
    ind_header->noRaiz = 0; // ignorando o header
    ind_header->nroChavesTotal++;
    ind_header->RRNproxNo = 1;
    write_btheader(index_file, ind_header);
    write_node(index_file, node);
    print_nodes(node);

    // for (size_t i = 1; i < reg_header->proxRRN; i++) {
    for (size_t i = 1; i < reg_header->proxRRN; i++) {
        fseek(data_file, LEN_DISC_PAG+(LEN_REG*i), SEEK_SET); 
        if(!read_register(data_file, data)) { // se o registro tiver deletado
            continue;
        }
        data_key->search_key = data->idConecta;
        data_key->RRN_key = i;
        Key* promo_key = malloc(sizeof(Key));
        int* right_rrn = malloc(sizeof(int));

        printf("add o %d - %ld\n", data_key->search_key, i);
        if (insert(index_file, ind_header->noRaiz, data_key, promo_key, right_rrn) == DO_PROMOTION) {
            printf("Valor que sobe: %d\n", promo_key->search_key);
            Node* new_root = create_node();
            new_root->folha = '0';
            new_root->nroChavesNo = 1; // vai ter uma chave
            new_root->alturaNo = node->alturaNo++; // ta 1 de altura acima do ultimo nó
            new_root->ponteiro[0] = ind_header->noRaiz;
            new_root->key[0] = promo_key;
            new_root->ponteiro[1] = *right_rrn;

            // verifica qual o proximo rrn livre
            read_btheader(index_file, ind_header);

            // escreve a nova raiz
            new_root->RRNdoNo = ind_header->RRNproxNo;
            fseek(index_file, LEN_PAGDISC + (ind_header->RRNproxNo*LEN_PAGDISC), SEEK_SET);
            write_node(index_file, new_root);
            print_nodes(new_root);

            // update no header com o novo proximo rrn
            ind_header->noRaiz = ind_header->RRNproxNo;
            ind_header->RRNproxNo++;
            update_btheader(index_file, ind_header);
            free(new_root);

        }

    }

    // colocando 1 no status e fechando o arquivo
    ind_header->status = '1';
    update_btheader(index_file, ind_header);

    fclose(data_file);
    fclose(index_file);

    free(reg_header);
    free(data);
    printf("linha 345\n");
    release_key(data_key);
    printf("linha 347\n");
    release_node(node);
    release_btheader(ind_header);

    return 1;
}