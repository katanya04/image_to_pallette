#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include "commons.h"
#include "raylib.h"
#include <stdbool.h>

typedef struct button button;
typedef struct menu menu;
typedef struct scroll_menu scroll_menu;
typedef struct vector_2i vector_2i;

struct button {
    Rectangle __area;
    Color text_color;
    Color disabled_text_color;
    Color background_color;
    Color hover_color;
    Color disabled_background_color;
    Color border_color;
    int border_size;
    char* text;
    void (*on_click)(button*);
    unsigned int (*padding_x)(button*);
    unsigned int (*padding_y)(button*);
    bool opens_menu;
    bool is_disabled;
    bool is_hidden;
    struct menu *menu_to_open;
};

typedef enum menu_direction {
    TOP_TO_BOTTOM,
    BOTTOM_TO_TOP,
} menu_direction;

struct menu {
    Rectangle __area;
    button* buttons;
    int num_buttons;
    unsigned int (*spacing)(menu*);
    bool is_open;
    menu_direction direction;
};

void draw_button(button*, int x, int y, unsigned int fontsize);
void draw_menu(menu*, int x, int y, unsigned int fontsize);
void toggle_menu(menu*);
unsigned int get_padding_width();
unsigned int get_padding_height();
bool mouse_hovering_button(button*);
button* mouse_hovering_buttons(button** buttons, unsigned int num_elements);
vector_2i get_button_size(button*, unsigned int fontsize);

#endif