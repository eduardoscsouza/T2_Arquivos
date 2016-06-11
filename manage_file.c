/*
Bernardo Bacellos de Castro Cunha    9293380
Eduardo Santos Carlos de Souza       9293481
Fabricio Guedes Faria                9293522
Piero Lima Capelo                    9293115
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "manage_file.h"

struct fields{
    unsigned int id;
    char * title;
    char * sinopsis;
    char * origin_country;
    unsigned int release_date;
    unsigned int season;
    char * genre;
};


//Aloca um registro
fields * mount_registry(unsigned int id, char * title, char * sinopsis, char * origin_country, unsigned int release_date, unsigned int season, char *genre)
{
    fields * newborn = (fields*) malloc(sizeof(fields));
    newborn->id = id;
    newborn->title = title;
    newborn->sinopsis = sinopsis;
    newborn->origin_country = origin_country;
    newborn->release_date = release_date;
    newborn->season = season;
    newborn->genre = genre;

    return newborn;
}


//Cria em RAM o bloco de dados a ser escrito no arquivo,
//retornando esse bloco e armazenando na variavel size_t * o tamanho desse bloco
void * mount_file_registry(fields * reg, size_t * size)
{
    (*size) = strlen(reg->title) + strlen(reg->sinopsis) + strlen(reg->origin_country) + strlen(reg->genre)
    + sizeof(reg->release_date) + sizeof(reg->season) + sizeof(reg->id) + (12 * sizeof(char));

    //12 -> Quatro "+1" das strings + 8 dos delimitadores;

    void * file_registry = malloc((*size));
    char end_register = '#';
    char delimiter = '|';

    //Copia das partes do registro e dos delimitadores para 
    //o bloco que sera escrito no disco
    size_t cur_offset = 0;
    memcpy(file_registry+cur_offset, &(reg->id), sizeof(reg->id));
    cur_offset += sizeof(reg->id);
    memcpy(file_registry+cur_offset, &delimiter, sizeof(delimiter));
    cur_offset += sizeof(delimiter);
    strcpy(file_registry+cur_offset, (reg->title));
    cur_offset += strlen(reg->title)*sizeof(char) + 1;
    memcpy(file_registry+cur_offset, &delimiter, sizeof(delimiter));
    cur_offset += sizeof(delimiter);
    strcpy(file_registry+cur_offset, (reg->sinopsis));
    cur_offset += strlen(reg->sinopsis)*sizeof(char) + 1;
    memcpy(file_registry+cur_offset, &delimiter, sizeof(delimiter));
    cur_offset += sizeof(delimiter);
    strcpy(file_registry+cur_offset, (reg->origin_country));
    cur_offset += strlen(reg->origin_country)*sizeof(char) + 1;
    memcpy(file_registry+cur_offset, &delimiter, sizeof(delimiter));
    cur_offset += sizeof(delimiter);
    memcpy(file_registry+cur_offset, &(reg->release_date), sizeof(reg->release_date));
    cur_offset += sizeof(reg->release_date);
    memcpy(file_registry+cur_offset, &delimiter, sizeof(delimiter));
    cur_offset += sizeof(delimiter);
    memcpy(file_registry+cur_offset, &(reg->season), sizeof(reg->season));
    cur_offset += sizeof(reg->season);
    memcpy(file_registry+cur_offset, &delimiter, sizeof(delimiter));
    cur_offset += sizeof(delimiter);
    strcpy(file_registry+cur_offset, (reg->genre));
    cur_offset += strlen(reg->genre)*sizeof(char) + 1;
    memcpy(file_registry+cur_offset, &delimiter, sizeof(delimiter));
    cur_offset += sizeof(delimiter);
    memcpy(file_registry+cur_offset, &end_register, sizeof(end_register));

    return file_registry;
}


//Libera a memoria alocada em um registro
void free_registry(fields * registry)
{
    free(registry->title);
    free(registry->sinopsis);
    free(registry->origin_country);
    free(registry->genre);
    free(registry);
}



//Le uma string do arquivo binario, parando ao encontrar o caracter "|"
char * fread_string(FILE * fp)
{
    char * str = NULL;
    char c = 0;
    int str_size = 0;
    do{
        fread(&c, sizeof(char), 1, fp);
        str = (char*) realloc(str, ++str_size);
        if (c != '|') str[str_size-1] = c;
    }while(c != '|'); //tratar loop
    
    str[str_size-1] = '\0';

    return str;
}


//Le um unico registro do arquivo binario
fields * fread_registry(FILE * in_file)
{    
    int id;
    char *title, *sinopsis, *origin_country, *genre;
    unsigned int release_date, season;
    char delimiter;

    fread(&id, sizeof(id), 1, in_file);
    fread(&delimiter, sizeof(delimiter), 1, in_file);
    title = fread_string(in_file);
    sinopsis = fread_string(in_file);
    origin_country = fread_string(in_file);
    fread(&release_date, sizeof(release_date), 1, in_file);
    fread(&delimiter, sizeof(delimiter), 1, in_file);
    fread(&season, sizeof(season), 1, in_file);
    fread(&delimiter, sizeof(delimiter), 1, in_file);
    genre = fread_string(in_file);
    fread(&delimiter, sizeof(delimiter), 1, in_file);

    return mount_registry(id, title, sinopsis, origin_country, release_date, season, genre);
}


//Le um unico registro do arquivo de texto e retorna ele alocado na heap
fields * fread_text(FILE * in_file, size_t offset)
{
    //coloca a posicao atual do arquivo na posicao desejada
    fseek(in_file, offset, SEEK_SET);

    char *title, *sinopsis, *origin_country, *genre;
    unsigned int release_date, season;

    title = fread_string(in_file);
    fgetc(in_file);     //leitura do \n
    sinopsis = fread_string(in_file);
    fgetc(in_file);
    origin_country = fread_string(in_file);
    fgetc(in_file); 
    fscanf(in_file, "%d", &release_date);
    fgetc(in_file);
    fscanf(in_file, "%d", &season);
    fgetc(in_file);
    genre = fread_string(in_file);
    fgetc(in_file);

    return mount_registry(0, title, sinopsis, origin_country, release_date, season, genre);
}



//Retorna um vetor com numeros de 0 ate N em ordem aleatoria sem repeticao
unsigned int * shuffle (size_t n, size_t seed)
{
    srand(seed);

    size_t i;
    unsigned int * vect = (unsigned int *) malloc(n * sizeof(unsigned int));
    for (i=0; i<n; i++) vect[i] = i;
    
    for (i=0; i<n; i++){
        //escolha de uma posicao aleatoria entre i e n;
        size_t pos = (rand () % (n-i)) + i;

        //swap entre vect[i] e vect[pos];
        size_t aux = vect[pos]; 
        vect[pos] = vect[i];
        vect[i] = aux;
    }

    return vect;
}


//Percorre o arquivo de texto e retorna os offsets do inicio de cada serie
size_t * getTextsOffsets(FILE * in_file, size_t n_registry)
{
    fseek(in_file, 0, SEEK_SET);
    size_t * offsets = (size_t *) malloc(sizeof(size_t) * n_registry);

    int i = 0;
    while(!feof(in_file) && i<n_registry){
        offsets[i] = ftell(in_file);
        //le a proxima serie e reposiciona a posicao atual do arquivo no inicio da proxima serie
        free_registry(fread_text(in_file, offsets[i++]));
    }

    fseek(in_file, 0, SEEK_SET);
    return offsets;
}


//Escreve o arquivo binario baseado no arquivo textual
void write_bin_file(const char * in_file_name, const char * out_file_name, size_t reg_count)
{
    FILE * in_file = fopen(in_file_name, "r");
    FILE * out_file = fopen(out_file_name, "w");
    
    //Pega os offsets do arquivo de texto para poder aleatorizar
    //a posicao das series no arquivo binario
    size_t * offsets = getTextsOffsets(in_file, reg_count);

    //Vetores de inteiros em ordem aleatoria sem repeticao
    unsigned int * ids = shuffle(reg_count, clock());
    unsigned int * pos = shuffle(reg_count, clock());
    
    int i;
    for (i=0; i<reg_count; i++) {
        //le um registro de uma posicao aleatoria do arquivo de texto
        fields * registry = fread_text(in_file, offsets[pos[i]]);
        //define nesse registro um id aleatorio
        registry->id = ids[i];

        size_t reg_size;
        void * file_registry = mount_file_registry(registry, &reg_size);

        //escreve no arquivo binario o registro
        fwrite(file_registry, reg_size, 1, out_file);
        free_registry(registry);
        free(file_registry);
    }

    fclose(in_file);
    fclose(out_file);
    free(offsets);
    free(ids);
    free(pos);
}



//Faz a busca sequencial dentro do arquivo binario pelo ID especificado
fields * sequencial_search(FILE * in_file, int id)
{
    fseek(in_file, 0, SEEK_SET);

    int found = 0;
    fields * registry = NULL;
    while(found==0){
        registry = fread_registry(in_file);
        if (registry->id!=id) free_registry(registry);
        else found = 1;
    }

    return registry;
}



//Imprime todos os IDs e os titulos
void print_all_ids(FILE * in_file, size_t id_count)
{
    int i;
    for (i=0; i<id_count; i++){
        fields * registry = sequencial_search(in_file, i);
        printf("%d: %s\n", registry->id, registry->title);
        free_registry(registry);
    }
}


//Imprime um registro
void print_registry(fields * registry)
{
    printf("ID: %d\n", registry->id);
    printf("Titulo: %s\n", registry->title);
    printf("Sinopse: %s\n", registry->sinopsis);
    printf("Pais de Origem: %s\n", registry->origin_country);
    printf("Data de Lancamento: %d\n", registry->release_date);
    printf("Temporada: %d\n", registry->season);
    printf("Genero: %s\n", registry->genre);
}



#define N_REG 100

int main()
{
    //Inicializacao que le o arquicod e texto e escreve o binario
    write_bin_file("series.txt", "series.bin", N_REG);

    int opcao, indice, i;
    FILE * bin_in_file;
    fields * found_registry;

    //Impressao na tela dos ids das series para facilitar o usuario na busca
    //IMPORTANTE: Apesar de ser impresso com os IDs em ordem crescenre,
    //nenhum dos dois arquivos em si esta ordenado 
    bin_in_file = fopen("series.bin", "r");
    printf("Lista de Series por ID:\n\n");
    print_all_ids(bin_in_file, N_REG);
    printf("\n");
    fclose(bin_in_file);

    do {
        printf("Entre com o comando respectivo para realizar a operação:\n");
        printf("\t1-Imprimir um registro do arquivo.\n");
        printf("\t2-Imprimir todos os registros do arquivo.\n");
        printf("\t3-Sair do programa.\n");

        do{
            scanf("%d", &opcao);
            if (opcao < 1 || opcao > 3){
                printf("Esse nao eh um comando valido. Por favor entre com um dos numeros dos comandos abaixo:\n");
                printf("\t1-Imprimir um registro do arquivo.\n");
                printf("\t2-Imprimir todos os registros do arquivo.\n");
                printf("\t3-Sair do programa.\n");
            }
        }while(opcao < 1 || opcao > 3);

        switch(opcao){
            case 1:
                //Impressao de um ID de uma serie selecionada
                bin_in_file = fopen("series.bin", "r");
                //Impressao novamente da lista dos IDs com os titulos para o usuario
                printf("Lista de Series por ID:\n\n");
                print_all_ids(bin_in_file, N_REG);
                printf("\n");

                printf("Entre com o indice (0-99) da serie que deseja saber mais informacoes sobre:\n");
                do{
                    scanf("%d", &indice);
                    if (indice < 0 || indice > 99) printf("Indice invalido, por favor digite um indice de 0 a 99:");
                }while(indice < 0 || indice > 99);

                printf("\n");
                found_registry = sequencial_search(bin_in_file, indice);
                print_registry(found_registry);
                free_registry(found_registry);
                fclose(bin_in_file);
                printf("\n");
                break;

            case 2:
                //Impressao de todas as series na ordem que estao no arquivo binario
                fgetc(stdin); //Remocao do \n que o scanf nao le
                bin_in_file = fopen("series.bin", "r");
                for(i=0; !feof(bin_in_file) && i<N_REG; i++){
                    fields * registry = fread_registry(bin_in_file);
                    print_registry(registry);
                    free_registry(registry);
                    printf("\n");
                    printf("Pressione Enter para o proximo\n");
                    fgetc(stdin);
                }

                fclose(bin_in_file);
                break;

            default:
                break;
        }
    }while(opcao != 3);
    
    return 0;
}
