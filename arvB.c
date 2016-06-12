#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arvB.h"
#include "manage_file.h"

#define ORDER 4
#define N_ELEMENTS 3
#define MIN_CHILD 2
#define MIN_KEYS 1
#define SIZEOF_ID 4
#define SIZEOF_OFFSET_POINTER 8
#define SIZEOF_CHILDREN_POINTER 4
#define SIZEOF_PAGE 64


struct BNodeElement {
	int id;
	int datafile_offset;
};

struct BNode{
	int ocup;
	int father_offset;
	int itself_offset;
	BNodeElement elements [N_ELEMENTS];
	int children_offset[ORDER];
};


BNode * new_bnode(int itself_offset, int father_offset)
{
	BNode * new_bnode = (BNode *) malloc(SIZEOF_PAGE);
	memset(new_bnode, -1, SIZEOF_PAGE);

	new_bnode->ocup = 0;
	new_bnode->itself_offset = itself_offset;
	new_bnode->father_offset = father_offset;

	return new_bnode;
}

BNodeElement * new_bnode_element(int id, int datafile_offset)
{
	BNodeElement * new_bnode_element = (BNodeElement *) malloc (sizeof(BNodeElement));
	new_bnode_element->id = id;
	new_bnode_element->datafile_offset = datafile_offset;

	return new_bnode_element;
}


//Funcao para escrever o arquivo do cabecalho
void write_header(const char * header_filename, int root_offset)
{
	FILE * header_file = fopen(header_filename, "w+");
	if (fwrite(&root_offset, sizeof(root_offset), 1, header_file)!=1) printf("HEADER WRITTING ERROR!\n");
	fclose(header_file);
}

//Funcao para ler o arquivo do cabecalho
int read_header(const char * header_filename)
{
	int root_offset;
	FILE * header_file = fopen(header_filename, "r+");
	if (header_file==NULL) {
		root_offset = -1;
		write_header(header_filename, root_offset);
	}
	else {
		if (fread(&root_offset, sizeof(root_offset), 1, header_file)!=1) printf("HEADER READING ERROR!\n");
		fclose(header_file);
	}

	return root_offset;
}


//Funcao que retorna o tamanho do arquivo
int get_file_size(const char * filename)
{
	FILE * file = fopen(filename, "r");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fclose(file);

	return size;
}


//Funcao para inserir no arquivo .idx
void insert_on_file(const char * arvb_filename, BNode * bnode, int offset)
{
	FILE * tree_file = fopen(arvb_filename, "r+");
	if (tree_file!=NULL){
		fseek(tree_file, offset, SEEK_SET);
		if (fwrite(bnode, SIZEOF_PAGE, 1, tree_file)!=1) printf("BNODE WRITTING ERROR!\n");
		fclose(tree_file);
	}
	else printf("OPENING ERROR!\n");
}

//Funcao para ler no arquivo .idx
BNode * read_from_file(const char * arvb_filename, int offset)
{
	BNode * bnode = (BNode*) malloc(SIZEOF_PAGE);
	memset(bnode, -1, SIZEOF_PAGE);

	FILE * tree_file = fopen(arvb_filename, "r+");
	if (tree_file!=NULL){
		fseek(tree_file, offset, SEEK_SET);
		if (fread(bnode, SIZEOF_PAGE, 1, tree_file)!=1) printf("BNODE READING ERROR!\nOFFSET->%d\nFILE SIZE->%d\n%s\n", offset, get_file_size(arvb_filename), arvb_filename);
		fclose(tree_file);
	}
	else printf("OPENING ERROR!\n");

	return bnode;
}


//Impressao dos elementos de apenas 1 no
void print_node(BNode * bnode)
{
    printf("[");

    int i;
    for(i=0; i<bnode->ocup; i++){
        printf("%d", bnode->elements[i].id);
        if(i<(bnode->ocup - 1)) printf(", ");
    }

    printf("]");
}

//Impressao da arvore inteira recursivamente
void print_tree(const char * arvb_filename, int offset)
{
    BNode * bnode = read_from_file(arvb_filename, offset);
    print_node(bnode);
    
    int i;
   	for (i=0; i<=bnode->ocup && bnode->children_offset[i]!=-1; i++){
        printf("(");
        print_tree(arvb_filename, bnode->children_offset[i]);
        printf(")");
    }

    free(bnode);
}

//Funcao base da chamada da impressao
void view_tree(const char * header_filename, const char * arvb_filename)
{
    int root_offset = read_header(header_filename);
    if (root_offset!=-1){
	    print_tree(arvb_filename, root_offset);
	    printf("\n");
	}
}


//Busca o no que contem ou conteria o ID buscado
int search_node(const char * arvb_filename, int bnode_offset, int key)
{
    BNode * bnode = read_from_file(arvb_filename, bnode_offset);

    int i, offset = -1;
    for(i=0; i<bnode->ocup && offset==-1; i++){
        if(bnode->elements[i].id > key && bnode->children_offset[i]!=-1) offset = search_node(arvb_filename, bnode->children_offset[i], key);  
        else if(bnode->elements[i].id == key) offset = bnode->itself_offset;  
    }
    if(offset == -1 && bnode->children_offset[i]!=-1) offset = search_node(arvb_filename, bnode->children_offset[i], key);

    free(bnode);
    if (offset==-1) return bnode_offset;
    else return offset;
}

//Busca dentro de um no o elemento e retorna alocado na RAM
BNodeElement * search_element(const char * arvb_filename, int bnode_offset, int key)
{
    BNode * bnode = read_from_file(arvb_filename, bnode_offset);
    BNodeElement * bnode_element = NULL;

    int i, found;
    for(i=0, found=0; i<bnode->ocup && !found; i++){
        if(bnode->elements[i].id == key){
        	bnode_element = (BNodeElement *) malloc(sizeof(BNodeElement));
            memcpy(bnode_element, bnode->elements + i, sizeof(BNodeElement));
            found = 1;
        }
    }

    free(bnode);
    return bnode_element;
}


//Insecao em no que possui espaco livre
void ordered_insert(BNode * bnode, BNodeElement * ins_elem)
{
	int i, j;
	for (i=0; (i<bnode->ocup) && (ins_elem->id > bnode->elements[i].id); i++);
	for (j=N_ELEMENTS-1; j>i; j--) bnode->elements[j] = bnode->elements[j-1];

	bnode->elements[i] = (*ins_elem);
	bnode->ocup++;
}

//Remocao do no
void remove_element(BNode * bnode, BNodeElement * rem_elem)
{
	int i, j;
	for (i=0; i<bnode->ocup && (bnode->elements[i].id != rem_elem->id); i++);

	if (i!=bnode->ocup){
		for (j=i; j < bnode->ocup-1; j++) bnode->elements[j] = bnode->elements[j+1];
		bnode->ocup--;
	}
}


//Funcao que retorna qual a posicao relativa do no entre os ponteiros do pai
int which_child(const char * arvb_filename, BNode * bnode)
{
    int i;
    int relative_idx = -1;
    BNode * parent = read_from_file(arvb_filename, bnode->father_offset);
    for(i=0; i<(parent->ocup+1) && relative_idx==-1; i++)
    	if(bnode->itself_offset == parent->children_offset[i]) relative_idx = i;
    
    free(parent);
    return relative_idx;
}

//Funcao que retorna qual irmao para se fazer redistribuicao, e -1 se nao houver
int possible_redistribution(const char * arvb_filename, Bnode * bnode)
{
	BNode * brother;
	int relative_idx = which_child(arvb_filename, bnode), ocup = 0;
	if (relative_idx < N_ELEMENTS){
		brother = read_from_file(bnode->father_offset->children_offset[relative_idx+1]);
		ocup = brother->ocup;
		free(brother);
		if (ocup!=N_ELEMENTS) return (relative_idx + 1);
	}

	if (relative_idx > 0){
		brother = read_from_file(bnode->father_offset->children_offset[relative_idx-1]);
		ocup = brother->ocup;
		free(brother);
		if (ocup!=N_ELEMENTS) return (relative_idx - 1);
	}

	return -1;
}


void redistribution(const char * arvb_filename, int father_offset, int origin_relative_child, int destiny_relative_child, BNodeElement * ins_elem)
{
    BNode * father = read_from_file(arvb_filename, father_offset);
    BNode * origin = read_from_file(arvb_filename, father->children_offset[origin_relative_child])
    BNode * destiny = read_from_file(arvb_filename, father->children_offset[destiny_relative_child]);
    BNodeElement * father_element = (BNodeElement*) malloc(sizeof(BNodeElement))
    BNodeElement * origin_element = (BNodeElement*) malloc(sizeof(BNodeElement));

    if(origin_relative_child < destiny_relative_child){ //destino > origem ----> esquerda para a direita
        memcpy(father_element, father->elements + origin_relative_child, sizeof(BNodeElement));
        memcpy(origin_element, origin->elements + N_ELEMENTS-1, sizeof(BNodeElement));
    }
    else{ //origem > destino -----> direita para a esquerda
        memcpy(father_element, father->elements + destiny_relative_child, sizeof(BNodeElement));
        memcpy(origin_element, origin->elements, sizeof(BNodeElement));
    }

    ordered_insert(destiny, father_element);
    remove_element(father, father_element);
    ordered_insert(father, origin_element);
    remove_element(origin, origin_element);
    ordered_insert(origin, ins_elem);

    insert_on_file(arvb_filename, father, father->itself_offset);
    insert_on_file(arvb_filename, origin, origin->itself_offset);
    insert_on_file(arvb_filename, destiny, destiny->itself_offset);

    free(father);
    free(origin);
    free(destiny);
    free(father_element);
    free(origin_element);
}


//Funcao que faz a insercao no nó em disco e faz as correcoes
void bnode_insert_element(const char * header_filename, const char * arvb_filename, BNode * bnode, BNodeElement * ins_elem)
{
	if (bnode->ocup==N_ELEMENTS) { //no cheio
		if (bnode->father_offset==-1){ //raiz cheia 
			//Criacao do vetor ordenado com N_ELEments + 1 elemento
			//(Pagina antiga + elemento a ser inserido)
			BNodeElement elements [N_ELEMENTS + 1];
			memcpy(elements, bnode->elements, sizeof(BNodeElement) * N_ELEMENTS);
			int i, j;
			for (i=0; i<N_ELEMENTS && ins_elem->id > elements[i].id; i++);
			for (j=N_ELEMENTS-1; j>i; j--) elements[j] = elements[j-1];
			elements[i] = (*ins_elem);

			//Criacao dos novos nos
			BNode * new_root = new_bnode(get_file_size(arvb_filename), -1);
			BNode * new_brother = new_bnode(get_file_size(arvb_filename) + SIZEOF_PAGE, new_root->itself_offset);

			//Definicao dos filhos dos novos nos
			new_root->children_offset[0] = bnode->itself_offset;
			new_root->children_offset[1] = new_brother->itself_offset;
			new_brother->children_offset[0] = bnode->children_offset[ORDER - 2];
			new_brother->children_offset[1] = bnode->children_offset[ORDER - 1];

			//Insercao dos elementos nos novos nos
			ordered_insert(new_root, elements + N_ELEMENTS - 1);
			ordered_insert(new_brother, elements + N_ELEMENTS);
			remove_element(bnode, elements + N_ELEMENTS - 1);
			
			//Mudar o pai de bnode children_offset

			//Correcao dos filhos do atual atual
			memset(bnode->children_offset + (ORDER - 2), -1, sizeof(int) * 2);

			//Correcao do pai e da raiz
			bnode->father_offset = new_root->itself_offset;
			write_header(header_filename, new_root->itself_offset);

			//Insercao no arquivo
			insert_on_file(arvb_filename, new_root, new_root->itself_offset);
			insert_on_file(arvb_filename, new_brother, new_brother->itself_offset);
			insert_on_file(arvb_filename, bnode, bnode->itself_offset);

			//Liberacao da RAM
			free(new_root);
			free(new_brother);
		}
		else {
			//int brother = possible_redistribution(arvb_filename, bnode);
			//if (brother!=-1) {
				//funcao_do_bernardo(arb_filename, bnode->father_offset, which_children(bnode), brother); 
			//se nao, split 2-to-3
		}
	}
	else { //no com espaco
		ordered_insert(bnode, ins_elem);
		insert_on_file(arvb_filename, bnode, bnode->itself_offset);
	}
}

//Funcao que faz a busca e chama outra para a insercao
void tree_insert_element(const char * header_filename, const char * arvb_filename, BNodeElement * ins_elem)
{
	int root_offset = read_header(header_filename);
	if (root_offset==-1){
		BNode * new_root = new_bnode(0, -1);
		write_header(header_filename, new_root->itself_offset);

		FILE * tree_file = fopen(arvb_filename, "w+");
		insert_on_file(arvb_filename, new_root, new_root->itself_offset);
		fclose(tree_file);

		bnode_insert_element(header_filename, arvb_filename, new_root, ins_elem);
		free(new_root);
	}
	else {
		BNode * bnode = read_from_file(arvb_filename, search_node(arvb_filename, read_header(header_filename), ins_elem->id));
		bnode_insert_element(header_filename, arvb_filename, bnode, ins_elem);
		free(bnode);
	}
}


#define SIZE 7


int main(int argv, char * argc[])
{
	int i;
	int * vect = shuffle(SIZE, time(NULL));
	for(i=0; i<SIZE; i++) printf("%d ", vect[i]);
	printf("\n");

	for(i=0; i<SIZE; i++){
		BNodeElement * b = new_bnode_element(vect[i], 0);
		tree_insert_element("header.hea", "idx.avb", b);
		free(b);
	}
	free(vect);

	view_tree("header.hea", "idx.avb");
	return 0;
}