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

int search_node(const char *, int, int);
BNodeElement * search_element(const char *, int, int);

void ordered_insert(BNode *, BNodeElement *);
void remove_element(BNode *, BNodeElement *);

int which_child(const char *, BNode *);
int possible_redistribution(const char *, BNode *);
void redistribution(const char *, int, int, int, BNodeElement *);

void fix_children(const char *, BNode *);
void split_1_to_2(const char *, const char *, BNode *, BNodeElement *, int);
void split_2_to_3(const char *, const char *, BNode *, BNodeElement *, int);

void bnode_insert_element(const char *, const char *, BNode *, BNodeElement *, int);
void tree_insert_element(const char *, const char *, BNodeElement *);


#endif