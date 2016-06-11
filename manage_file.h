/*
Bernardo Bacellos de Castro Cunha    9293380
Eduardo Santos Carlos de Souza       9293481
Fabricio Guedes Faria                9293522
Piero Lima Capelo                    9293115
*/

#ifndef MANAGE_FILE_H
#define MANAGE_FILE_H


typedef struct fields fields;


//Aloca um registro
fields * mount_registry(unsigned int, char *, char *, char *, unsigned int, unsigned int, char *);
//Cria em RAM o bloco de dados a ser escrito no arquivo,
//retornando esse bloco e armazenando na variavel size_t * o tamanho desse bloco
void * mount_file_registry(fields *, size_t *);
//Libera a memoria alocada em um registro
void free_registry(fields * registry);

//Le uma string do arquivo binario, parando ao encontrar o caracter "|"
char * fread_string(FILE *);
//Le um unico registro do arquivo binario
fields * fread_registry(FILE *);
//Le um unico registro do arquivo de texto e retorna ele alocado na heap
fields * fread_text(FILE *, size_t);

//Retorna um vetor com numeros de 0 ate N em ordem aleatoria sem repeticao
unsigned int * shuffle (size_t, size_t);
//Percorre o arquivo de texto e retorna os offsets do inicio de cada serie
size_t * getTextsOffsets(FILE *, size_t);
//Escreve o arquivo binario baseado no arquivo textual
void write_bin_file(const char *, const char *, size_t);

//Faz a busca sequencial dentro do arquivo binario pelo ID especificado
fields * sequencial_search(FILE *, int);

//Imprime todos os IDs e os titulos
void print_all_ids(FILE *, size_t);
//Imprime um registro
void print_registry(fields *);



#endif