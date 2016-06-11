#ifndef ARVB_H
#define ARVB_H


typedef struct BNodeElement BNodeElement;
typedef struct BNode BNode;

BNode * new_bnode(int, int);
BNodeElement * new_bnode_element(int, int);

void write_header(const char *, int);
int read_header(const char *);

int get_file_size(const char *);

void insert_on_file(const char *, BNode *, int);
BNode * read_from_file(const char *, int);

void print_node(BNode * bnode);
void print_tree(const char *, int);
void view_tree(const char * , const char *);

//Busca o BNode que contem ou conteria o ID buscado
//argumentos -> arvb_filename, id, bnode_offset
int search_node(const char *, int, int)
//Busca dentro de um Bnode o elemento
//argumentos -> bnode, id
BNodeElement * search_element(BNode *, int);

void ordered_insert(BNode *, BNodeElement *);
void remove_element(BNode *, BNodeElement *);

void bnode_insert_element(const char *, const char *, BNode *, BNodeElement *);
void tree_insert_element(const char *, const char *, BNodeElement *);


#endif