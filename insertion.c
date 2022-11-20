#include "insertion.h"


void shift_keys_node(Node* node, int pos) {

    for (int i = 3; i >= pos; i--) {

        if (i != pos) {
            node->ponteiro[i + 1] = node->ponteiro[i];
        }

        if(i != 3) {
            node->key[i + 1] = node->key[i];
        }
    }  
}


void split(Key* local_key, int* local_right_rrn, Node* node, Key* promo_key, int* right_rrn, Node* new_node) {
    
    Key_list* keys = malloc(sizeof(Key_list));
    for (size_t i = 0; i < 5; i++)
    {
        keys->ponteiro[i] = node->ponteiro[i];
        if (i != 4) {
            keys->key[i]= node->key[i];
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
            }
            
            keys->key[j] = *(local_key);
            keys->ponteiro[j+1] = *(local_right_rrn);
            break;

        }
    }

    *promo_key = keys->key[2]; // elemento central do array

    new_node->alturaNo = node->alturaNo;
    new_node->folha = node->folha;
    new_node->nroChavesNo = 2;

    // coloca os valores dps do centro para a nova pagina
    for (size_t i = 0; i < 3; i++)
    {
        new_node->ponteiro[i] = keys->ponteiro[i+3];
        if (i != 2) {
            new_node->key[i]  = keys->key[i+3];
        }
    }


    //deleta os valores e coloca os novos valores na pagina antiga
    delete_keys(node);
    node->nroChavesNo = 2;
    for (size_t i = 0; i < 3; i++)
    {
        node->ponteiro[i] = keys->ponteiro[i];
        if (i != 2) {
            node->key[i] = keys->key[i];
        }
    }
    
        free(keys);
}


int insert(FILE* file, int current_rrn, Key* key, Key* promo_key, int* right_rrn) {

    // como esse nó nao existe, a chave deverá ficar no nó anterior
    if (current_rrn == -1) {
        *promo_key = *key;
        *right_rrn = -1;
        return DO_PROMOTION;
        
    } else {
        Node* node = create_node();
        fseek(file, LEN_PAGDISC + (current_rrn*LEN_PAGDISC), SEEK_SET);
        read_node(file, node);

        int pos;
        for (size_t i = 0; i < 4; i++)
        {
            if (key->search_key <= node->key[i].search_key || node->key[i].search_key == -1) {
                if (node->key[i].search_key == key->search_key) {
                    printf("Chave já inserida\n");
                    free(node);

                    return EXIT;
                }
                pos = i;
                break;
            }
            pos = 4;
        }

        Key* local_promo = malloc(sizeof(Key));
        int* local_right_rrn = malloc(sizeof(int));
        int return_value = insert(file, node->ponteiro[pos], key, local_promo, local_right_rrn);

        if (return_value == EXIT || return_value == NO_PROMOTION) {
            free(node);
            free(local_promo);
            free(local_right_rrn);

            return return_value;

        } else if (node->nroChavesNo < 4) {

            if (node->key[pos].search_key == -1) {
                node->key[pos] = *local_promo;
                node->ponteiro[pos+1] = *local_right_rrn;
            } else {
                shift_keys_node(node, pos);
                node->key[pos] = *local_promo;
                node->ponteiro[pos+1] = *local_right_rrn;
            }            
            
            node->nroChavesNo++;
            fseek(file, LEN_PAGDISC + (current_rrn*LEN_PAGDISC), SEEK_SET);
            write_node(file, node);
            //print_nodes(node);

            free(node);
            free(local_promo);
            free(local_right_rrn);

            return NO_PROMOTION;

        } else {
            Node* new_node = create_node();
            //print_nodes(node);
            split(local_promo, local_right_rrn, node, promo_key, right_rrn, new_node);
            
            // escrevendo o node
            fseek(file, LEN_PAGDISC + (current_rrn*LEN_PAGDISC), SEEK_SET);
            write_node(file, node);
            //print_nodes(node);

            // verifica qual o proximo rrn livre
            BTHeader* header = create_btheader();
            read_btheader(file, header);

            // escreve o novo no
            new_node->RRNdoNo = header->RRNproxNo;
            fseek(file, LEN_PAGDISC + (header->RRNproxNo*LEN_PAGDISC), SEEK_SET);
            write_node(file, new_node);
            //print_nodes(new_node);

            // update no header com o novo proximo rrn
            *right_rrn = header->RRNproxNo;
            header->RRNproxNo++;
            update_btheader(file, header);

            free(node);
            free(local_promo);
            free(local_right_rrn);
            free(new_node);
            free(header);

            return DO_PROMOTION;
        }
        
    }
}


int command7(char* data_name, char* index_name) {

    // abrindo arquivo de dados
    FILE* data_file = fopen(data_name, "rb"); // problema na funçao de abrir arquivo TO DO
    if (data_file == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    Header_reg* reg_header = malloc(sizeof(Header_reg));
    read_header(reg_header, data_file);
    if (reg_header->status == '0') {
        printf("Falha no processamento do arquivo.\n");
        free(reg_header);
        fclose(data_file);
        return 0; 
    }

    // abrindo arquivo de indices
    FILE* index_file = fopen(index_name, "wb+");
    if (index_file == NULL) {
        printf("Falha no processamento do arquivo.\n");
        free(reg_header);
        fclose(data_file);
        return 0;
    }    

    // se o arquivo de dados n tiver regsitros, escrever apenas o header no arquivo de indexes
    if (reg_header->proxRRN == 0) {
        BTHeader* ind_header = create_btheader();
        write_btheader(index_file, ind_header);
        fclose(data_file);
        fclose(index_file);
        free(ind_header);
        return 1;
    }


    // lendo o primeiro registro dos dados
    Data_reg* data = malloc(sizeof(Data_reg));
    Key* data_key = create_key();
    fseek(data_file, LEN_DISC_PAG, SEEK_SET);
    read_register(data_file, data);

    // colocando esse primeiro registro lido como raiz
    Node* node = create_node();
    node->folha = '1'; // nao tem filhos
    node->nroChavesNo = 1; // vai ter uma chave
    node->alturaNo = 1; // ta na parte mais baixo da arvore
    node->RRNdoNo = 0; // primeiro RRN do arquivo de indexes
    node->key[0].search_key = data->idConecta; // chave raiz no momento
    node->key[0].RRN_key = 0; // RRN dessa chave no arquivo de dados

    // criando e escrevendo o header do arquivo de index
    BTHeader* ind_header = create_btheader();
    ind_header->noRaiz = 0;
    ind_header->nroChavesTotal = 1;
    ind_header->RRNproxNo = 1;
    ind_header->alturaArvore = 1;
    write_btheader(index_file, ind_header);
    write_node(index_file, node);
    //print_nodes(node);

    for (size_t i = 1; i < reg_header->proxRRN; i++) {
        fseek(data_file, LEN_DISC_PAG+(LEN_REG*i), SEEK_SET); 
        if(!read_register(data_file, data)) { // se o registro tiver deletado
            continue;
        }
        
        data_key->search_key = data->idConecta;
        data_key->RRN_key = i;

        Key* promo_key = malloc(sizeof(Key));
        int* right_rrn = malloc(sizeof(int));

        if (insert(index_file, ind_header->noRaiz, data_key, promo_key, right_rrn) == DO_PROMOTION) {    
            read_btheader(index_file, ind_header);
            Node* new_root = create_node();
            new_root->folha = '0';
            new_root->nroChavesNo = 1; // vai ter uma chave
            new_root->alturaNo = ind_header->alturaArvore + 1; // ta 1 de altura acima do ultimo nó
            new_root->ponteiro[0] = ind_header->noRaiz;
            new_root->key[0] = *promo_key;
            new_root->ponteiro[1] = *right_rrn;

            // escreve a nova raiz
            new_root->RRNdoNo = ind_header->RRNproxNo;
            fseek(index_file, LEN_PAGDISC + (ind_header->RRNproxNo*LEN_PAGDISC), SEEK_SET);
            write_node(index_file, new_root);
            //print_nodes(new_root);

            // update no header com as infos atualizadas
            read_btheader(index_file, ind_header);
            ind_header->noRaiz = ind_header->RRNproxNo;
            ind_header->RRNproxNo++;
            ind_header->alturaArvore = new_root->alturaNo;
            ind_header->nroChavesTotal++;

            free(new_root);

        } else {
            // le o header e atualiza
            read_btheader(index_file, ind_header);
            ind_header->nroChavesTotal++;
        }

        update_btheader(index_file, ind_header);
        free(promo_key);
        free(right_rrn);

    }

    // colocando 1 no status e fechando o arquivo
    read_btheader(index_file, ind_header);
    ind_header->status = '1';
    update_btheader(index_file, ind_header);

    fclose(data_file);
    fclose(index_file);

    free(reg_header);
    free(data);
    free(data_key);
    free(node);
    free(ind_header);

    return 1;
}

int command9(char* data_name, char* index_name) {

    // abre o arquivo de dados, verifica o status e coloca 0 no status
    FILE* data_file = fopen(data_name, "rb+");
    if (data_file == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    Header_reg* reg_header = malloc(sizeof(Header_reg));
    read_header(reg_header, data_file);
    if (reg_header->status == '0') {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    reg_header->status = '0';
    write_header(data_file,reg_header);

    // abre o arquivo de indexes, verifica o status e coloca 0 no status
    FILE* index_file = fopen(index_name, "rb+");
    if (index_file == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    BTHeader* ind_header = malloc(sizeof(BTHeader));
    read_btheader(index_file, ind_header);
    if (ind_header->status == '0') {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    ind_header->status = '0';
    update_btheader(index_file, ind_header);

    // verifica o numero de inserções
    int n;
    scanf("%d", &n);

    Data_reg* new_reg = create_reg();
    Key* new_key = create_key();
    char str[50];
    for (size_t i = 0; i < n; i++) {

        // le os headers
        read_header(reg_header, data_file);
        read_btheader(index_file, ind_header);

        // lendo os inputs do teclado
        scanf("%s ", str);
        if(strcmp(str, "NULO") != 0) {
            new_reg->idConecta = atoi(str);
        } else {
            new_reg->idConecta = -1;
        }
        scan_quote_string(new_reg->nomePoPs);
        scan_quote_string(new_reg->nomePais);   
        scan_quote_string(new_reg->siglaPais);

        scanf(" %s", str);
        if(strcmp(str, "NULO") != 0) {
            new_reg->idPoPsConectado = atoi(str);
        } else {
            new_reg->idPoPsConectado = -1;
        }

        scan_quote_string(&new_reg->unidadeMedida);

        scanf(" %s", str);
        if(strcmp(str, "NULO") != 0) {
            new_reg->velocidade = atoi(str);
        } else {
            new_reg->velocidade = -1;
        }
        
        // criando a chave para o arquivo de indexes
        new_key->search_key = new_reg->idConecta;
        new_key->RRN_key = reg_header->proxRRN;

        // pula para o fim do arquivo de dados e escreve o novo registro e atualiza o header  
        fseek(data_file, LEN_DISC_PAG+(reg_header->proxRRN*LEN_REG), SEEK_SET);
        write_register(data_file, new_reg);
        reg_header->proxRRN++;
        write_header(data_file, reg_header);


        // faz a inserção da chave no arquivo de indexes
        Key* promo_key = create_key();
        int* right_rrn = malloc(sizeof(int));
        if (insert(index_file, ind_header->noRaiz, new_key, promo_key, right_rrn) == DO_PROMOTION) {
            Node* new_root = create_node();
            new_root->folha = '0';
            new_root->nroChavesNo = 1; // vai ter uma chave
            new_root->alturaNo = ind_header->alturaArvore; // ta 1 de altura acima do ultimo nó
            new_root->ponteiro[0] = ind_header->noRaiz;
            new_root->key[0] = *promo_key;
            new_root->ponteiro[1] = *right_rrn;

            // verifica qual o proximo rrn livre
            read_btheader(index_file, ind_header);

            // escreve a nova raiz
            new_root->RRNdoNo = ind_header->RRNproxNo;
            fseek(index_file, LEN_PAGDISC + (ind_header->RRNproxNo*LEN_PAGDISC), SEEK_SET);
            write_node(index_file, new_root);

            // update no header com o novo proximo rrn
            read_btheader(index_file, ind_header);
            ind_header->noRaiz = ind_header->RRNproxNo;
            ind_header->RRNproxNo++;
            ind_header->alturaArvore = new_root->alturaNo;
            ind_header->nroChavesTotal++;

            free(new_root);
        } else {
            read_btheader(index_file, ind_header);
            ind_header->nroChavesTotal++;
        }

        update_btheader(index_file, ind_header);
        free(promo_key);
        free(right_rrn);
    }

    // atualiza o status dos arquivos  
    reg_header->status = '1';
    write_header(data_file, reg_header);
    ind_header->status = '1';
    update_btheader(index_file, ind_header);

    free(reg_header);
    free(ind_header);
    free(new_reg);
    free(new_key);
    fclose(data_file);
    fclose(index_file);

    return 1;
}