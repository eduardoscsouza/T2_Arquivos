#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arvB.h"

#define ORDER 4
#define N_ELEMENTS 3
#define MIN_CHILD 2
#define MIN_KEYS 1
#define SIZEOF_ID 4
#define SIZEOF_OFFSET_POINTER 8
#define SIZEOF_CHILDREN_POINTER 4
#define SIZEOF_PAGE 64


struct BNodeElement{
	size_t datafile_offset;
	int id;
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
	BNode * new_bnode = (BNode *) malloc(10 * SIZEOF_PAGE);
	new_bnode->ocup = 0;
	new_bnode->itself_offset = itself_offset;
	new_bnode->father_offset = father_offset;

	memset(new_bnode->elements, 0, sizeof(BNodeElement) * N_ELEMENTS);
	memset(new_bnode->children_offset, 0, sizeof(int) * ORDER);

	return new_bnode;
}

BNodeElement * new_bnode_element(int id, size_t datafile_offset)
{
	BNodeElement * new_bnode_element = (BNodeElement *) malloc (sizeof(BNodeElement));
	new_bnode_element->id = id;
	new_bnode_element->datafile_offset = datafile_offset;

	return new_bnode_element;
}


//Funcao para ler o arquivo do cabecalho
int read_header(const char * header_file_name)
{
	int root_offset;
	FILE * header_file = fopen(header_file_name, "r+");
	if (header_file==NULL){
		header_file = fopen(header_file_name, "w+");
		root_offset = -1;
		fwrite(&root_offset, sizeof(root_offset), 1, header_file);		
	}
	else fread(&root_offset, sizeof(root_offset), 1, header_file);

	fclose(header_file);
	return root_offset;
}


//Funcao para inserir no arquivo .idx
void insert_on_file(const char * arvb_file_name, BNode * bnode, int offset)
{
	FILE * tree_file = fopen(arvb_file_name, "w+");
	fseek(tree_file, offset, SEEK_SET);
	fwrite(bnode, SIZEOF_PAGE, 1, tree_file);
	fclose(tree_file);
}

//Insecao em no que possui espaco livre
void ordered_insert(BNode * bnode, BNodeElement * ins_elem)
{
	int i, j;
	for (i=0; i<bnode->ocup && ins_elem->id > bnode->elements[i].id; i++);
	for (j=N_ELEMENTS-1; j>i; j--) bnode->elements[j] = bnode->elements[j-1];

	bnode->elements[i] = (*ins_elem);
	//memcpy(bnode->elements + i, ins_elem, sizeof(*ins_elem));
}

//Funcao que faz a insercao no nó em disco e faz as correcoes
void bnode_insert_element(const char * arvb_file_name, BNode * bnode, BNodeElement * ins_elem)
{
	if (bnode->father_offset==-1){	//insercao na raiz
		if (bnode->ocup==N_ELEMENTS){	//raiz cheia

		}
		else {	//raiz nao cheia
			ordered_insert(bnode, ins_elem);
			insert_on_file(arvb_file_name, bnode, bnode->itself_offset);
		}

	}
	else{	//insercao fora da raiz

	}
}

//Funcao que faz a busca e chama outra para a insercao
void tree_insert_element(const char * header_file_name, const char * arvb_file_name, BNodeElement * ins_elem)
{
	int root_offset = read_header(header_file_name);
	if (root_offset==-1){
		BNode * new_root = new_bnode(0, -1);
		bnode_insert_element(arvb_file_name, new_root, ins_elem);
		free(new_root);
	}
	else{

	}
}


int main(int argv, char * argc[])
{
	printf("%ld %ld %ld \n", sizeof(size_t) + sizeof(int), sizeof(BNodeElement), sizeof(BNode));
	/*
	BNodeElement * ins_elem = new_bnode_element(0, 0);
	tree_insert_element("header.hea", "idx.avb", ins_elem);
	free(ins_elem);*/

	return 0;
}