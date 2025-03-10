#include "utils/linked_list.h"
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

node* insert_node_at(linked_list *linked_list, void *data, unsigned long position) { // if position > num_elements, will be inserted at num_elements
    node* new_node = malloc(sizeof(node));
    if (new_node == NULL)
        return NULL;
    *new_node = (node){data, NULL};
    if (linked_list->head == NULL)          // Empty list
        linked_list->head = new_node;
    else if (position == 0) {               // Insert before head
        new_node->next = linked_list->head;
        linked_list->head = new_node;
    } else {                                // Insert after head
        node* current = linked_list->head;
        while (current->next != NULL && --position != 0) {current = current->next;}
        node* next_node = current->next;
        current->next = new_node;
        new_node->next = next_node;
    }
    linked_list->num_elements++;
    return new_node;
}

node* insert_node(linked_list *linked_list, void *data) {
    return insert_node_at(linked_list, data, ULONG_MAX);
}

bool delete_node(linked_list *linked_list, node *node_to_delete) {
    if (node_to_delete == NULL)                 // Empty node to be deleted
        return false;
    if (linked_list->head == NULL)              // Empty list
        return false;
    if (linked_list->head == node_to_delete) {  // Delete head
        node *new_head = linked_list->head->next;
        free(linked_list->head);
        linked_list->head = new_head;
        linked_list->num_elements--;
        return true;
    }                                           // Delete after head
    node* current = linked_list->head;
    while (current->next != NULL) {
        if (current->next == node_to_delete) {
            node *next = current->next->next;
            free(current->next);
            current->next = next;
            linked_list->num_elements--;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool delete_node_at(linked_list * linked_list, unsigned long position) {
    return delete_node(linked_list, get_node_at(linked_list, position));
}

node* get_node_at(linked_list *linked_list, unsigned long position) {
    node* current = linked_list->head;
    while (current != NULL && position-- != 0) {current = current->next;}
    return current;
}

unsigned long get_node_position(linked_list *linked_list, void *data) {                                   // Is after head
    node* current = linked_list->head;
    long long position = 0;
    while (current != NULL && current->data != data) {
        current = current->next;
        position++;
    }
    if (current->data == data)
        return position;
    return (unsigned long) -1;
}

void move_node(linked_list *linked_list, void *data, unsigned long new_position) {
    delete_node(linked_list, data);
    insert_node_at(linked_list, data, new_position);
}