#include "raylib.h"
#include "utils/window.h"
#include "utils/hashmap.h"
#include "utils/button.h"
#include "gui_elements.h"
#include "utils/linked_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include "commons.h"
#include "colors_hashmap.h"
#include <string.h>

#include "icon_32px.h"
#include "icon_64px.h"
#include "icon_128px.h"
#include "icon_256px.h"

linked_list layers;
size_t current_layer = 0;
float zoom = 1.0f;
Vector2 draw_pos;
FILE *fptr;
Texture2D transparent_pattern;

void add_zoom(float zoom, Vector2 center_cords);
void set_icons();
void generate_transparent_pattern();

int main(void) {
    node *current_texture;
    Texture2D current_tex;
    InitWindow(800, 450, "Sprite atlas to pallette");
    Vector2 prev_win_size = {800.f, 450.f};
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    set_icons();
    resize_font();
    generate_transparent_pattern();
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose()) {
        check_click_gui();
        current_texture = get_node_at(&layers, current_layer);
        if (current_texture) {
            current_tex = self(current_texture)->texture;
            if (GetMouseWheelMove()) {
                if (mouse_inside_layer_menu())
                    GetMouseWheelMove() > 0 ? scroll_upwards() : scroll_downwards();
                else if (GetMousePosition().y > toolbar_height)
                    add_zoom((1 + GetMouseWheelMove() / 10) * zoom - zoom, GetMousePosition());
            }
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                if (!mouse_inside_layer_menu() && GetMousePosition().y > toolbar_height) {
                    Vector2 delta = GetMouseDelta();
                    Vector2 new_pos = {draw_pos.x + delta.x, draw_pos.y + delta.y};
                    if (new_pos.x >= GetScreenWidth())
                        new_pos.x = GetScreenWidth() - 1;
                    else if (new_pos.x + current_tex.width * zoom <= 0)
                        new_pos.x = -current_tex.width * zoom + 1;
                    if (new_pos.y + delta.y >= GetScreenHeight())
                        new_pos.y = GetScreenHeight() - 1;
                    else if (new_pos.y + current_tex.height * zoom <= toolbar_height)
                        new_pos.y = -current_tex.height * zoom + toolbar_height + 1;
                    draw_pos = new_pos;
                }
            }
            if (IsKeyPressed(KEY_R)) {      // Reset zoom and pos
                zoom = 1.0f;
                draw_pos = (Vector2){GetScreenWidth() / 2 - current_tex.width / 2, (GetScreenHeight() - toolbar_height) / 2 + toolbar_height - current_tex.height / 2};
            }
        }
        if (IsWindowResized()) {
            resize_font();
            generate_transparent_pattern();
            //float delta_zoom = (GetScreenWidth() - prev_win_size.x + GetScreenHeight() - prev_win_size.y) / 1000;
            draw_pos.x = draw_pos.x * (GetScreenWidth() / prev_win_size.x);
            draw_pos.y = draw_pos.y * (GetScreenHeight() / prev_win_size.y);
            //if (zoom + delta_zoom > 0)
            //    add_zoom(delta_zoom, (Vector2){draw_pos.x + (current_tex.width * zoom) / 2, draw_pos.y + (current_tex.height * zoom) / 2});
        }
        prev_win_size = (Vector2){GetScreenWidth(), GetScreenHeight()};
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(DARKGRAY);
            if (current_texture) {
                DrawTexture(transparent_pattern, 0, toolbar_height, WHITE);
                DrawTextureEx(current_tex, draw_pos, 0.f, zoom, WHITE);

                DrawRectangle(0, toolbar_height, max(0.f, draw_pos.x), GetScreenHeight() - toolbar_height, DARKGRAY);
                DrawRectangle(0, toolbar_height, GetScreenWidth(), max(0.f, draw_pos.y - toolbar_height), DARKGRAY);
                DrawRectangle(0, draw_pos.y + current_tex.height * zoom, GetScreenWidth(), max(0, GetScreenHeight() - (draw_pos.y + current_tex.height * zoom) + 1), DARKGRAY);
                DrawRectangle(draw_pos.x + current_tex.width * zoom, toolbar_height, max(0, GetScreenWidth() - (draw_pos.x + current_tex.width * zoom) + 1), GetScreenHeight() - toolbar_height, DARKGRAY);

                draw_zoom_and_pos();
                draw_layer_menu();
            }
            draw_gui();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    CloseWindow();
    return 0;
}

void add_zoom(float delta_zoom, Vector2 center_cords) {
    node *current_texture = get_node_at(&layers, current_layer);
    if (!current_texture)
        return;
    Vector2 center_relative_to_image = {(center_cords.x - draw_pos.x) / zoom, (center_cords.y - draw_pos.y) / zoom};
    draw_pos.x -= delta_zoom * center_relative_to_image.x;
    draw_pos.y -= delta_zoom * center_relative_to_image.y;
    zoom += delta_zoom;
}

void add_layer() {
    char* path = open_file_menu();
    if (path) {
        Image img_layer = LoadImage(path);
        if (img_layer.data == NULL) {
            return;
        }
        if (layers.num_elements == 0) {
            draw_pos = (Vector2){GetScreenWidth() / 2 - img_layer.width / 2, (GetScreenHeight() - toolbar_height) / 2 + toolbar_height - img_layer.height / 2};
        } else if ((img_layer.format != self(layers.head)->image.format || img_layer.height != self(layers.head)->image.height
                    || img_layer.width != self(layers.head)->image.width)) {
            UnloadImage(img_layer);
            return;
        }
        Texture2D tex_layer = LoadTexture(path);
        free(path);
        layer *new_layer = malloc(sizeof(layer));
        new_layer->image = img_layer;
        new_layer->texture = tex_layer;
        insert_node(&layers, new_layer);
        if (layers.num_elements == 1) {
            generate_transparent_pattern();
        }
        file_menu.buttons[1].is_disabled = false;
        file_menu.buttons[2].is_disabled = false;
        update_layer_menu();
    }
}

void remove_layer(size_t index) {
    node *node_to_delete = get_node_at(&layers, index);
    layer *layer_to_delete = self(node_to_delete);
    UnloadImage(layer_to_delete->image);
    UnloadTexture(layer_to_delete->texture);
    free(layer_to_delete);
    delete_node(&layers, node_to_delete);
    if (layers.num_elements == 0) {
        file_menu.buttons[1].is_disabled = true;
        file_menu.buttons[2].is_disabled = true;
    }
    if (current_layer >= index && current_layer != 0)
        current_layer--;
    update_layer_menu();
}

void save_canvas(const char* path) {
    fptr = fopen(path, "w");
    if (!fptr)
        return;
    fprintf(fptr, "[");

    struct hashmap_color_entry {
        Color colors[layers.num_elements];
        unsigned long value;
    };
    int bytes_per_pixel;
    switch (self(layers.head)->image.format) {
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
            bytes_per_pixel = 4;
            break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
            bytes_per_pixel = 3;
            break;
    }
    struct hashmap *map = hashmap_new(sizeof(struct hashmap_color_entry), 0, 0, 0, bytes_per_pixel == 4 ? color_hash_4B : color_hash_3B,
                                        bytes_per_pixel == 4 ? color_compare_4B : color_compare_3B, NULL, NULL);
    size_t pallettes_key = 0;
    bool is_last;
    for (int row = 0; row < self(layers.head)->image.height; row++) {
        for (int column = 0; column < self(layers.head)->image.width; column++) {
            Color colors[layers.num_elements];
            for (size_t current_layer_index = 0; current_layer_index < layers.num_elements; current_layer_index++) {
                Image current_layer = self(get_node_at(&layers, current_layer_index))->image;
                void* current_pixel = current_layer.data + (row * current_layer.width * bytes_per_pixel) + column * bytes_per_pixel;
                colors[current_layer_index] = (Color){  .r = ((char*)current_pixel)[0],
                                                        .g = ((char*)current_pixel)[1],
                                                        .b = ((char*)current_pixel)[2], 
                                                        .a = bytes_per_pixel == 3 ? 255 : ((char*)current_pixel)[3]};
            }
            struct hashmap_color_entry entry;
            memcpy(entry.colors, colors, layers.num_elements * sizeof(Color));
            const struct hashmap_color_entry *existing_entry = hashmap_get(map, &entry);
            size_t current_value;
            if (existing_entry) {
                current_value = existing_entry->value;
            } else {
                current_value = pallettes_key++;
                entry.value = current_value;
                hashmap_set(map, &entry);
            }
            is_last = (row == self(layers.head)->image.height - 1 && column == self(layers.head)->image.width - 1);
            fprintf(fptr, is_last? "%llu" : "%llu, ", current_value);
        }
        if (!is_last)
            fprintf(fptr, "\n");
    }
    fprintf(fptr, "]\n\n");
    hashmap_scan(map, bytes_per_pixel == 4 ? color_iter_4B : color_iter_3B, NULL);

    fclose(fptr);
}

void set_icons() {
    Image icons[4] = {  {.format = ICON_32PX_FORMAT, .height = ICON_32PX_HEIGHT, .width = ICON_32PX_WIDTH, .data = ICON_32PX_DATA, .mipmaps = 1},
                        {.format = ICON_64PX_FORMAT, .height = ICON_64PX_HEIGHT, .width = ICON_64PX_WIDTH, .data = ICON_64PX_DATA, .mipmaps = 1},
                        {.format = ICON_128PX_FORMAT, .height = ICON_128PX_HEIGHT, .width = ICON_128PX_WIDTH, .data = ICON_128PX_DATA, .mipmaps = 1},
                        {.format = ICON_256PX_FORMAT, .height = ICON_256PX_HEIGHT, .width = ICON_256PX_WIDTH, .data = ICON_256PX_DATA, .mipmaps = 1}
                     };
    SetWindowIcons(icons, 4);
}

void generate_transparent_pattern() {
    UnloadTexture(transparent_pattern);
    int square_size = (GetScreenHeight() + GetScreenWidth()) / 2 / 50;
    Image square_pattern_image = GenImageChecked(GetScreenWidth(), GetScreenHeight() - toolbar_height, square_size, square_size, LIGHTGRAY, WHITE);
    transparent_pattern = LoadTextureFromImage(square_pattern_image);
    UnloadImage(square_pattern_image);
}