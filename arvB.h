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

int search(const char *, int, int);

void ordered_insert(BNode *, BNodeElement *);
void remove_element(BNode *, BNodeElement *);

void bnode_insert_element(const char *, const char *, BNode *, BNodeElement *);
void tree_insert_element(const char *, const char *, BNodeElement *);


#endif