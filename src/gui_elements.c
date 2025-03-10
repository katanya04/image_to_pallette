#include "raylib.h"
#include "gui_elements.h"
#include "utils/button.h"
#include "stddef.h"
#include "commons.h"
#include "utils/window.h"
#include <stdlib.h>

void debug_value(float value, unsigned int index);
void click_delete_layer(button* button);
unsigned int layer_buttons_y_padding();
unsigned int layer_delete_menu_spacing();

#define GENERIC_BUTTON(button_text, button_on_click, disabled) {.text = button_text, .on_click = button_on_click, .opens_menu = false, .is_disabled = disabled, .text_color = BLACK, .disabled_text_color = GRAY, .background_color = RAYWHITE, .disabled_background_color = LIGHTGRAY, .hover_color = SKYBLUE, .border_color = GRAY, .border_size = 1}
#define LAYER_DELETE_BUTTON {.text = "x", .on_click = click_delete_layer, .opens_menu = false, .is_disabled = false, .text_color = RED, .background_color = BLANK, .hover_color = (Color){102, 191, 255, 128}, .border_color = GRAY, .border_size = 1}

int title_1_font_size = 450 / 20;
int title_2_font_size = 450 / 30;
int toolbar_height;
float scroll_bar_state = 0;

void export_pallette() {
    const char* output_path = save_file_menu();
    if (output_path)
        save_canvas(output_path);
}

void export_raylib_header() {
    const char* output_path = save_file_menu();
    if (output_path)
        ExportImageAsCode(self(get_node_at(&layers, current_layer))->image, output_path);
}

void scroll_upwards() {
    unsigned long first_to_show = (unsigned long) max(layers.num_elements - 3, 0) * scroll_bar_state;
    int to_be_scrolled = min(3, layers.num_elements - first_to_show - 3);
    if (to_be_scrolled > 0) {
        scroll_bar_state = (float) (first_to_show + to_be_scrolled) / max(layers.num_elements - 3, 0);
    }
    update_layer_menu();
}

void scroll_downwards() {
    unsigned long first_to_show = (unsigned long) max(layers.num_elements - 3, 0) * scroll_bar_state;
    int to_be_scrolled = max(-3, -first_to_show);
    if (to_be_scrolled < 0) {
        scroll_bar_state = (float) (first_to_show + to_be_scrolled) / max(layers.num_elements - 3, 0);
    }
    update_layer_menu();
}

button file_buttons[3] = {GENERIC_BUTTON("Add layer", add_layer, false), GENERIC_BUTTON("Export pallette file", export_pallette, true), GENERIC_BUTTON("Export raylib header file", export_raylib_header, true)};
menu file_menu = (menu){.buttons = file_buttons, .num_buttons = 3, .is_open = false};
button file_bt = (button){.text = "File", .on_click = NULL, .opens_menu = true, .menu_to_open = &file_menu, .is_disabled = false, .text_color = BLACK, .disabled_text_color = GRAY, .background_color = RAYWHITE, .disabled_background_color = LIGHTGRAY, .hover_color = SKYBLUE, .border_color = GRAY, .border_size = 1};

button layer_buttons[3];
button layer_delete_buttons[3] = {LAYER_DELETE_BUTTON, LAYER_DELETE_BUTTON, LAYER_DELETE_BUTTON};
menu layer_menu = (menu){.is_open = true, .num_buttons = 3, .buttons = layer_buttons, .direction = BOTTOM_TO_TOP};
menu layer_delete_menu = (menu){.is_open = true, .num_buttons = 3, .buttons = layer_delete_buttons, .direction = BOTTOM_TO_TOP, .spacing = layer_delete_menu_spacing};

button scroll_upwards_button = (button){.text = "▲", .background_color = BLANK, .hover_color = (Color){102, 191, 255, 128},
                                        .border_color = GRAY, .text_color = BLANK, .border_size = 1, .on_click = scroll_upwards,
                                        .disabled_background_color = LIGHTGRAY, .disabled_text_color = BLANK};
button scroll_downwards_button=(button){.text = "▼", .background_color = BLANK, .hover_color = (Color){102, 191, 255, 128},
                                        .border_color = GRAY, .text_color = BLANK, .border_size = 1, .on_click = scroll_downwards,
                                        .disabled_background_color = LIGHTGRAY, .disabled_text_color = BLANK};

menu *all_menus[] = {&file_menu, &layer_menu, &layer_delete_menu};
button *all_buttons[] = {   &file_bt, &scroll_upwards_button, &scroll_downwards_button, &layer_delete_buttons[0], &layer_delete_buttons[1],
                            &layer_delete_buttons[2], &layer_buttons[0], &layer_buttons[1], &layer_buttons[2]};
button *file_menu_buttons[] = {&file_buttons[0], &file_buttons[1], &file_buttons[2]};

void check_click_gui() {
    if (!IsCursorOnScreen()) {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        return;
    }
    button *hovering = mouse_hovering_buttons(all_buttons, 9);
    if (hovering == NULL && file_menu.is_open) {
        hovering = mouse_hovering_buttons(file_menu_buttons, 3);
    }
    if (hovering == NULL || hovering->is_disabled || hovering->is_hidden) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            file_menu.is_open = false;
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        return;
    } else {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (hovering->opens_menu) {
                menu *to_open = hovering->menu_to_open;
                hovering->menu_to_open->is_open = true;
                for (int i = 0; i < 3; i++) {
                    menu *current_menu = all_menus[i];
                    if (current_menu == to_open)
                        continue;
                    current_menu->is_open = false;
                }
            } else {
                hovering->on_click(hovering);
                for (int i = 0; i < 3; i++) {
                    menu *current_menu = all_menus[i];
                    current_menu->is_open = false;
                }
            }
            if (hovering != &file_bt)
                file_menu.is_open = false;
        }
    }
}

void draw_gui() {
    DrawRectangle(0, 0, GetScreenWidth(), toolbar_height, RAYWHITE);
    DrawLine(0, toolbar_height, GetScreenWidth(), toolbar_height, GRAY);
    draw_button(&file_bt, 0, 0, title_1_font_size);
    if (file_menu.is_open)
        draw_menu(&file_menu, 0, toolbar_height, title_1_font_size);
}

void resize_font() {
    int min_axis = GetScreenWidth() < GetScreenHeight() ? GetScreenWidth() : GetScreenHeight();
    title_1_font_size = min_axis / 20;
    title_2_font_size = title_1_font_size / 1.5f;
    toolbar_height = title_1_font_size + GetScreenHeight() * 0.01 * 2;
}

void draw_zoom_and_pos() {
    int zoom_int = (int) zoom;
    unsigned int places = num_places(zoom_int);
    unsigned int buffer_zoom_size = 8 + places + 1;
    char buffer_zoom[buffer_zoom_size];
    snprintf(buffer_zoom, buffer_zoom_size, "Zoom: %f", zoom);
    unsigned int width_zoom = MeasureText(buffer_zoom, title_2_font_size);
    int zoom_x = GetScreenWidth() - width_zoom - 20;
    int zoom_y = toolbar_height + 10;//pos_y + title_2_font_size + 10 + 10;
    DrawRectangle(zoom_x - 10, zoom_y - 10, GetScreenWidth() - (zoom_x - 10), 10 + 10 + title_2_font_size, (Color){255, 255, 255, 200});
    DrawText(buffer_zoom, zoom_x, zoom_y, title_2_font_size, GRAY);

    unsigned int width_reset_text = MeasureText("Press R to reset zoom and pos", title_2_font_size);
    int reset_x = 10;
    int reset_y = GetScreenHeight() - 10 - title_2_font_size;
    DrawRectangle(reset_x - 10, reset_y - 10, width_reset_text + 20, GetScreenHeight() - (reset_y - 10), (Color){255, 255, 255, 200});
    DrawText("Press R to reset zoom and pos", reset_x, reset_y, title_2_font_size, GRAY);
}

unsigned long extract_layer_from_text(button* button) {
    if (button == NULL)
        return 0;
    char* text = button->text;
    unsigned long layer = strtoul(text + 6, NULL, 10) - 1;
    if (layer >= layers.num_elements)
        layer = 0;
    return layer;
}

void click_layer(button* self) {
    unsigned long layer = extract_layer_from_text(self);
    current_layer = layer;
    update_layer_menu();
}

void click_delete_layer(button* self) {
    button *corresponding_layer_button;
    for (int i = 0; i < 3; i++) {
        if (layer_delete_buttons + i == self)
            corresponding_layer_button = layer_buttons + i;
    }
    unsigned long layer = extract_layer_from_text(corresponding_layer_button);
    remove_layer(layer);
    update_layer_menu();
}

unsigned int layer_buttons_y_padding() {
    unsigned int frame_height = title_1_font_size * 2;
    return frame_height - title_2_font_size;
}

unsigned int layer_delete_menu_spacing() {
    return title_1_font_size * 2 - (title_2_font_size + get_padding_height() * 2);
}

void update_layer_menu() { // on adding/removing layer, on scroll, on change current layer
    unsigned long first_to_show = (unsigned long) max(layers.num_elements - 3, 0) * scroll_bar_state;
    for (unsigned long i = first_to_show; i < first_to_show + 3; i++) {
        unsigned int index = i - first_to_show;
        unsigned int places = num_places(i);
        free(layer_buttons[index].text);
        char* text = malloc(6 + places + 1);
        snprintf(text, 6 + places + 1, "Layer %lu", i + 1);
        layer_buttons[index] = (button){.text = text, .background_color = BLANK, .hover_color = (Color){102, 191, 255, 128},
                                        .border_color = current_layer == i ? SKYBLUE : GRAY, .text_color = GRAY,
                                        .border_size = current_layer == i ? 2 : 1, .is_disabled = false, .is_hidden = i >= layers.num_elements ? true : false,
                                        .on_click = click_layer, .padding_y = layer_buttons_y_padding};
        layer_delete_buttons[index].is_hidden = i >= layers.num_elements;
    }
    scroll_downwards_button.is_disabled = first_to_show == 0;
    scroll_upwards_button.is_disabled = first_to_show + 3 >= layers.num_elements;
}

void draw_triangle_inscribed_in_square(Rectangle square, bool flipped, Color color, float padding) {
    square.x += padding;
    square.y += padding;
    square.width -= padding * 2;
    square.height -= padding * 2;
    Vector2 v1 = {square.x + square.width / 2, square.y + (flipped ? square.height : 0)};
    Vector2 v2 = {square.x, square.y + (flipped ? 0 : square.height)};
    Vector2 v3 = {square.x + square.width, square.y + (flipped ? 0 : square.height)};
    DrawTriangle(v1, flipped ? v3 : v2, flipped ? v2 : v3, color);
}

bool mouse_inside_layer_menu() {
    if (layers.num_elements == 0)
        return false;
    Image layer_0 = self(get_node_at(&layers, 0))->image;

    unsigned int delete_button_size = layer_delete_menu.__area.width;
    unsigned int frame_height = title_1_font_size * 2;
    unsigned int frame_width = layer_0.width * ((float) frame_height / layer_0.height);
    vector_2i scrollbar_size = get_button_size(&scroll_upwards_button, title_2_font_size);
    unsigned int height = frame_height * 3 + title_2_font_size + 10;
    unsigned int width = frame_width + 5 + layer_menu.__area.width + delete_button_size + 5 + scrollbar_size.x;

    Vector2 mouse = GetMousePosition();
    return mouse.x >= GetScreenWidth() - width && mouse.y >= GetScreenHeight() - height;
}

void draw_layer_menu() {
    if (layers.num_elements == 0)
        return;
    Image layer_0 = self(get_node_at(&layers, 0))->image;

    unsigned int delete_button_size = layer_delete_menu.__area.width;
    unsigned int frame_height = title_1_font_size * 2;
    unsigned int frame_width = layer_0.width * ((float) frame_height / layer_0.height);
    vector_2i scrollbar_size = get_button_size(&scroll_upwards_button, title_2_font_size);
    unsigned int layer_text_width = layer_menu.__area.width;
    unsigned int height = frame_height * 3 + title_2_font_size + 10;
    unsigned int width = frame_width + 5 + layer_menu.__area.width + delete_button_size + 5 + scrollbar_size.x;

    DrawRectangle(GetScreenWidth() - width, GetScreenHeight() - height, width, height, (Color){255, 255, 255, 200});
    DrawText("Layers", GetScreenWidth() - width + 10, GetScreenHeight() - height + 5, title_2_font_size, GRAY);
    draw_menu(&layer_menu, GetScreenWidth() - width + frame_width + 5, GetScreenHeight() - height + 5 + title_2_font_size + 5, title_2_font_size);
    draw_menu(&layer_delete_menu, GetScreenWidth() - width + frame_width + 5 + layer_text_width + 5, GetScreenHeight() - 
        height + 5 + title_2_font_size + 5 + frame_height / 2 - (get_button_size(&layer_delete_buttons[0], title_2_font_size).y / 2), title_2_font_size);
    draw_button(&scroll_upwards_button, GetScreenWidth() - scrollbar_size.x, GetScreenHeight() - height + 5 + title_2_font_size + 5, title_2_font_size);
    draw_button(&scroll_downwards_button, GetScreenWidth() - scrollbar_size.x, GetScreenHeight() - scrollbar_size.y, title_2_font_size);
    
    // no "▲" or "▼" in default font...
    draw_triangle_inscribed_in_square(scroll_upwards_button.__area, false, scroll_upwards_button.is_disabled ? GRAY : BLACK, (GetScreenWidth() + GetScreenHeight()) / 2 * 0.01);
    draw_triangle_inscribed_in_square(scroll_downwards_button.__area, true, scroll_downwards_button.is_disabled ? GRAY : BLACK, (GetScreenWidth() + GetScreenHeight()) / 2 * 0.01);
    
        unsigned long first_to_show = (unsigned long) max(layers.num_elements - 3, 0) * scroll_bar_state;
    for (unsigned long i = first_to_show; i < layers.num_elements && i < first_to_show + 3; i++) {
        int index = i - first_to_show;

        Texture2D tex = self(get_node_at(&layers, i))->texture;
        DrawTextureEx(tex, (Vector2){GetScreenWidth() - width, GetScreenHeight() - (frame_height * (index + 1))}, 0.f, (float) frame_height / tex.height, WHITE);

        //int text_places = /*Layer */ 6 + num_places(i) + /*\0*/ + 1;
        /*char buffer[text_places];
        snprintf(buffer, text_places, "Layer %lu", i);
        DrawText(buffer, GetScreenWidth() - width + frame_width + 5, GetScreenHeight() - (frame_height * (index + 1) - frame_height / 2 + title_2_font_size / 2), title_2_font_size, GRAY);

        DrawRectangle(GetScreenWidth() - width + frame_width + 5 + layer_text_width + 5, );
        DrawText("x", GetScreenWidth() - width + frame_width + 5 + layer_text_width + 5, GetScreenHeight() - (frame_height * (index + 1) - frame_height / 2 + title_2_font_size / 2), title_2_font_size, RED);*/
    }
}

void debug_value(float value, unsigned int index) {
    int value_int = (int) value;
    int places_value = num_places(abs(value_int)) + (value_int < 0 ? 1 : 0);
    int buffer_value_size = places_value + 4;
    char buffer_value[buffer_value_size];
    snprintf(buffer_value, buffer_value_size, "%f", value);
    int width_value = MeasureText(buffer_value, title_2_font_size);
    int pos_x = GetScreenWidth() - width_value - 20;
    int pos_y = toolbar_height + 50 + index * (title_2_font_size + 10);
    DrawRectangle(pos_x - 10, pos_y - 10, GetScreenWidth() - (pos_x - 10), 10 + 10 + title_2_font_size, (Color){255, 255, 255, 200});
    DrawText(buffer_value, pos_x, pos_y, title_2_font_size, GRAY);
}