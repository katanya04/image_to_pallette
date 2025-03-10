#include "utils/button.h"
#include "commons.h"
#include "raylib.h"
#include <stdlib.h>

unsigned int get_padding_width() {
    return GetScreenWidth() * 0.01;
}

unsigned int get_padding_height() {
    return GetScreenHeight() * 0.01;
}

bool mouse_hovering_button(button* self) {
    return mouse_hovering_buttons(&self, 1) != NULL;
}

button* mouse_hovering_buttons(button** buttons, unsigned int num_elements) {
    Vector2 mouse_pos = GetMousePosition();
    if (!IsCursorOnScreen())
        return NULL;
    for (unsigned int i = 0; i < num_elements; i++) {
        button *current = buttons[i];
        if (mouse_pos.x > current->__area.x && mouse_pos.x < current->__area.x + current->__area.width &&
            mouse_pos.y > current->__area.y && mouse_pos.y < current->__area.y + current->__area.height)
            return current;
    }
    return NULL;
}

void draw_button_extended(button* self, int x, int y, int fontsize, int padding_x, int padding_y) {
    if (self->is_hidden)
        return;
    unsigned int width = MeasureText(self->text, fontsize) + padding_x;
    unsigned int height = fontsize + padding_y;
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, self->border_size, self->border_color);

    Vector2 mouse_pos = GetMousePosition();
    if (!self->is_disabled && mouse_hovering_button(self)) {
        DrawRectangle(x + self->border_size, y + self->border_size, width - self->border_size * 2, height - self->border_size * 2, self->hover_color);
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        DrawRectangle(x + self->border_size, y + self->border_size, width - self->border_size * 2, height - self->border_size * 2, self->is_disabled ? self->disabled_background_color : self->background_color);
    }
    DrawText(self->text, x + self->border_size + padding_x / 2, y + height / 2 - fontsize / 2 , fontsize, self->is_disabled ? self->disabled_text_color : self->text_color);
    self->__area = (Rectangle){x, y, width, height};
}

void draw_button(button* self, int x, int y, unsigned int fontsize) {
    unsigned int padding_x = self->padding_x == NULL ? get_padding_width() * 2 : self->padding_x(self);
    unsigned int padding_y = self->padding_y == NULL ? padding_y = get_padding_height() * 2 : self->padding_y(self);
    draw_button_extended(self, x, y, fontsize, padding_x, padding_y);
}

void draw_menu(menu* self, int x, int y, unsigned int fontsize) {
    button* buttons = self->buttons;
    unsigned int max_width = 0;
    unsigned int spacing = 0;
    if (self->spacing != NULL)
        spacing = self->spacing(self);
    for (int i = 0; i < self->num_buttons; i++) {
        unsigned int width = MeasureText((buttons + i)->text, fontsize);
        unsigned int padding_x = (buttons + i)->padding_x == NULL ? get_padding_width() * 2 : (buttons + i)->padding_x((buttons + i));
        if (width + padding_x > max_width)
            max_width = width + padding_x;
    }
    unsigned int acc_height = 0;
    for (int i = 0; i < self->num_buttons; i++) {
        button* current_button;
        switch (self->direction) {
        case BOTTOM_TO_TOP:
            current_button = buttons + (self->num_buttons - i - 1);
            break;
        default: // TOP_TO_BOTTOM:
            current_button = buttons + i;
            break;
        }
        unsigned int width = MeasureText(current_button->text, fontsize);
        unsigned int padding_x = current_button->padding_x == NULL ? get_padding_width() * 2 : current_button->padding_x(current_button);
        padding_x += max_width - (width + padding_x);
        unsigned int padding_y = current_button->padding_y == NULL ? padding_y = get_padding_height() * 2 : current_button->padding_y(current_button);
        draw_button_extended(current_button, x, y + acc_height, fontsize, padding_x, padding_y);
        acc_height += fontsize + padding_y + spacing;
    }
    self->__area = (Rectangle){x, y, max_width, acc_height};
}

void toggle_menu(struct menu* self) {
    self->is_open ^= 1;
}

vector_2i get_button_size(button* self, unsigned int fontsize) {
    unsigned int padding_x = self->padding_x == NULL ? get_padding_width() * 2 : self->padding_x(self);
    unsigned int padding_y = self->padding_y == NULL ? padding_y = get_padding_height() * 2 : self->padding_y(self);
    unsigned int width = MeasureText(self->text, fontsize) + padding_x;
    unsigned int height = fontsize + padding_y;
    return (vector_2i){width, height};
}