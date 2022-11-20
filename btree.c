#include "btree.h"

//aloca memoria e inicializa variaveis para o cabecalho
BTHeader* create_btheader(){
    BTHeader* header = malloc(sizeof(BTHeader));
    header->status = '0';
    header->noRaiz = -1;
    header->nroChavesTotal = 0;
    header->alturaArvore = 0;
    header->RRNproxNo = -1;

    return header;
}

//le os campos do cabecalho
void read_btheader( FILE* file, BTHeader* header){
    fseek(file, 0, SEEK_SET);
    
    fread(&header->status, sizeof(char), 1, file);
    fread(&header->noRaiz, sizeof(int), 1, file);
    fread(&header->nroChavesTotal, sizeof(int), 1, file);
    fread(&header->alturaArvore, sizeof(int), 1, file);
    fread(&header->RRNproxNo, sizeof(int), 1, file);

    fseek(file, 49, SEEK_CUR); //pula o lixo

}

//escreve o cabecalho no arquivo indicado
void write_btheader(FILE* file, BTHeader* header) {
    fseek(file, 0, SEEK_SET);

    fwrite(&header->status, sizeof(char), 1, file);
    fwrite(&header->noRaiz, sizeof(int), 1, file);
    fwrite(&header->nroChavesTotal, sizeof(int), 1, file);
    fwrite(&header->alturaArvore, sizeof(int), 1, file);
    fwrite(&header->RRNproxNo, sizeof(int), 1, file);
    char garbage = GARBAGE;
    for(size_t i = 0; i < 48; i++){
        fwrite(&garbage, sizeof(char), 1, file);
    }
}

void update_btheader(FILE* file, BTHeader* header) {
    fseek(file, 0, SEEK_SET);

    fwrite(&header->status, sizeof(char), 1, file);
    fwrite(&header->noRaiz, sizeof(int), 1, file);
    fwrite(&header->nroChavesTotal, sizeof(int), 1, file);
    fwrite(&header->alturaArvore, sizeof(int), 1, file);
    fwrite(&header->RRNproxNo, sizeof(int), 1, file);
}

void read_node(FILE* file, Node* node) {
    fread(&node->folha, LEN_FOLHA, 1, file);
    fread(&node->nroChavesNo, LEN_NROCHAVESNO, 1, file);
    fread(&node->alturaNo, LEN_ALTURANO, 1, file);
    fread(&node->RRNdoNo, LEN_RRNDONO, 1, file);
    for (size_t i = 0; i < 5; i++)
    {
        fread(&node->ponteiro[i], LEN_PONTEIRO, 1, file);
        if (i != 4) {
            fread(&node->key[i].search_key, LEN_SEARCHKEY, 1, file);
            fread(&node->key[i].RRN_key, LEN_RRN_KEY, 1, file);
        }
    }
}

void write_node(FILE* file, Node* node) {
    fwrite(&node->folha, LEN_FOLHA, 1, file);
    fwrite(&node->nroChavesNo, LEN_NROCHAVESNO, 1, file);
    fwrite(&node->alturaNo, LEN_ALTURANO, 1, file);
    fwrite(&node->RRNdoNo, LEN_RRNDONO, 1, file);
    for (size_t i = 0; i < 5; i++)
    {
        fwrite(&node->ponteiro[i], LEN_PONTEIRO, 1, file);
        if (i != 4) {
            fwrite(&node->key[i].search_key, LEN_SEARCHKEY, 1, file);
            fwrite(&node->key[i].RRN_key, LEN_RRN_KEY, 1, file);
        }
    }
    
}

Key* create_key(){

    Key* key = malloc(sizeof(Key));
    key->search_key = -1;
    key->RRN_key = -1;
    
    return key;
}


Node* create_node(){
    Node* node= malloc(sizeof(Node));
    node->nroChavesNo = 0;
    node->alturaNo = 0;
    // folha
    // RRn do no

    for (size_t i = 0; i < 5; i++)
    {
        node->ponteiro[i] = -1;
        if (i != 4) {
            node->key[i].search_key = -1;
            node->key[i].RRN_key = -1;
        }
    }

    return node;
}

void delete_keys(Node* node) {

    node->nroChavesNo = 0;
    for (size_t i = 0; i < 5; i++)
    {
        node->ponteiro[i] = -1;
        if (i != 4) {
            node->key[i].search_key = -1;
            node->key[i].RRN_key = -1;
        }
    }
}

void print_nodes(Node* node) {

    printf("Folha %c\n", node->folha);
    printf("Altura %d\n", node->alturaNo);
    printf("Nro de chaves: %d\n", node->nroChavesNo);
    printf("RRN do node %d\n\n", node->RRNdoNo);
    for (size_t i = 0; i < 5; i++)
    {
        printf("Ponteiro %ld - %d\n", i, node->ponteiro[i]);
        if (i != 4) {
            printf("Chave %ld -    %d\n", i, node->key[i].search_key);
        }
    }
    printf("\n\n");
    
}
