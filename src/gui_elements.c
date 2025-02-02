#include "gui_elements.h"
#include "raylib.h"

int get_border_width() {
    return GetScreenWidth() * 0.01;
}

int get_border_height() {
    return GetScreenHeight() * 0.01;
}

bool mouse_hovering_button(button* self) {
    Vector2 mouse_pos = GetMousePosition();
    return (mouse_pos.x > self->area.x && mouse_pos.x < self->area.x + self->area.width &&
            mouse_pos.y > self->area.y && mouse_pos.y < self->area.y + self->area.height);
}

void draw_button_given_width(button* self, int x, int y, int fontsize, int width) {
    int height = fontsize + get_border_height() * 2;
    DrawRectangle(x, y, width, height, GRAY);

    Vector2 mouse_pos = GetMousePosition();
    if (mouse_pos.x >= x && mouse_pos.x <= x + width && mouse_pos.y >= y && mouse_pos.y <= y + height) {
        DrawRectangle(x + 1, y + 1, width - 2, height - 2, SKYBLUE);
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        DrawRectangle(x + 1, y + 1, width - 2, height - 2, RAYWHITE);
    }
    DrawText(self->text, x + get_border_width(), y + get_border_height(), fontsize, BLACK);
    self->area = (Rectangle){x, y, width, height};
}

void draw_button(button* self, int x, int y, int fontsize) {
    int text_width = MeasureText(self->text, fontsize);
    int width = text_width + get_border_width() * 2;
    draw_button_given_width(self, x, y, fontsize, width);
}

void draw_menu(menu* self, int x, int y, int fontsize) {
    button* buttons = self->buttons;
    int max_width = 0;
    int height = fontsize + get_border_height();
    for (int i = 0; i < self->num_buttons; i++) {
        int width = MeasureText((buttons + i)->text, fontsize);
        if (width > max_width)
            max_width = width;
    }
    for (int i = 0; i < self->num_buttons; i++) {
        draw_button_given_width(buttons + i, x, y + (height * i), fontsize, max_width + get_border_width() * 2);
    }
    self->area = (Rectangle){x, y, max_width, height * self->num_buttons};
}

void toggle_menu(struct menu* self) {
    self->is_open ^= 1;
}