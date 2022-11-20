/*
Fabricio Sampaio [runcodes] - 12547423 - participacao: 100%
Pedro Lucas Castro de Andrade - 11212289 - participacao: 100%
*/

#include <stdio.h>

#include "insertion.h"
//#include "command8.h"
//#include "command9.h"
//#include "command10.h"
#include "funcoesFornecidas.h"

int main(void) {
    //variaveis p/ entrada do teclado
    int command;
    char data_file1[30];
    // char data_file2[30];
    char index_file[30];

    //le do teclado qual o comando sera acionado
    scanf("%d ", &command);

    //aciona a execucao do comando lido
    switch (command){
        case 7:
            scanf("%s %s", data_file1, index_file);
            if(command7(data_file1, index_file)){
                binarioNaTela(index_file);
            }
            break;
        case 8:
            scanf("%s %s", data_file1, index_file);
            //command8(file1, file2);
            break;
        case 9:
            scanf("%s %s", data_file1, index_file);
            if(command9(data_file1, index_file)){

                binarioNaTela(data_file1);
                binarioNaTela(index_file);
            }
            break;
        case 10:
            
            break;
    }
}