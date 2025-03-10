#ifndef GUI_ELEMENTS_H_INCLUDED
#define GUI_ELEMENTS_H_INCLUDED

#include "utils/button.h"

void check_click_gui();
void draw_gui();
void resize_font();
void draw_zoom_and_pos();
void draw_layer_menu();
void update_layer_menu();
void debug_value(float value, unsigned int index);
bool mouse_inside_layer_menu();
void scroll_upwards();
void scroll_downwards();

extern button file_buttons[3];
extern menu file_menu;
extern button file_bt;
extern int title_1_font_size;
extern int title_2_font_size;
extern int toolbar_height;

#endif