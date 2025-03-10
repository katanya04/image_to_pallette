#ifndef COMMONS_H_INCLUDED
#define COMMONS_H_INCLUDED

#include "raylib.h"
#include "utils/button.h"
#include "utils/linked_list.h"
#include "corecrt.h"
#include <stdio.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

typedef struct layer {
    Image image;
    Texture2D texture;
} layer;

typedef struct vector_2i {
    int x;
    int y;
} vector_2i;

extern float zoom;
extern Vector2 draw_pos;
#define self(node) ((layer*) node->data)
extern linked_list layers;
extern size_t current_layer;
extern FILE *fptr;

void add_layer();
void remove_layer(size_t index);
void export_pallette();
void export_raylib_header();
void save_canvas(const char* path);
unsigned int num_places(unsigned long n);

#endif