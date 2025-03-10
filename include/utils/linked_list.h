#ifndef LINKED_LIST_H_INCLUDED
#define LINKED_LIST_H_INCLUDED
#include <stdbool.h>

typedef struct node {
   void *data;
   struct node *next;
} node;

typedef struct linked_list {
    node *head;
    unsigned long num_elements;
} linked_list;

node* insert_node(linked_list *, void *);
node* insert_node_at(linked_list *, void *, unsigned long position);
bool delete_node(linked_list *, node *);
bool delete_node_at(linked_list *, unsigned long position);
node* get_node_at(linked_list *, unsigned long position);
unsigned long get_node_position(linked_list *, void *);
void move_node(linked_list *, void *, unsigned long new_position);

#endif