#ifndef GUI_ELEMENTS_H_INCLUDED
#define GUI_ELEMENTS_H_INCLUDED

#include <stdbool.h>
#include "raylib.h"

typedef struct button button;
typedef struct menu menu;

struct button {
    Rectangle area;
    const char* text;
    void (*on_click)(void);
    bool opens_menu;
    struct menu *menu_to_open;
};

struct menu {
    Rectangle area;
    button* buttons;
    int num_buttons;
    bool is_open;
};

void draw_button(button*, int x, int y, int fontsize);
void draw_menu(menu*, int x, int y, int fontsize);
void toggle_menu(menu*);
int get_border_width();
int get_border_height();
bool mouse_hovering_button(button*);

#endif