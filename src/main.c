#include "raylib.h"
#include "window.h"
#include "hashmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <stdint.h>

Image sprite_atlas;
Texture2D sprite_atlas_texture;
float zoom = 1.0f;
Vector2 pos = {0.f, 0.f};
int title_1_font_size = 450 / 20;
int title_2_font_size = 450 / 20 / 1.5f;

void draw_zoom_and_pos();
void save_canvas(const char* path);

int main(void) {
    InitWindow(800, 450, "Sprite atlas to pallete");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(60);
    bool output_canvas = false;
    char* output_path = NULL;
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose()) {
        if (sprite_atlas.data != NULL) { // Texture opened
            zoom *= 1 + GetMouseWheelMove() / 10;
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                Vector2 delta = GetMouseDelta();
                pos.x += delta.x;
                pos.y += delta.y;
            }
            if (IsKeyPressed(KEY_R)) {    // Reset zoom and pos
                zoom = 1.0f;
                pos.x = 0; pos.y = 0;
            } else if (IsKeyPressed(KEY_E)) {
                output_path = save_file_menu();
                if (output_path) {
                    output_canvas = true;
                }
            }
            
        }
        if (sprite_atlas.data == NULL && GetKeyPressed() != 0) {
            const char* path = open_file_menu();
            if (path) {
                sprite_atlas = LoadImage(path);
                sprite_atlas_texture = LoadTexture(path);
                free(path);
            }
        }

        if (IsWindowResized()) {
            int min_axis = GetScreenWidth() < GetScreenHeight() ? GetScreenWidth() : GetScreenHeight();
            title_1_font_size = min_axis / 20;
            title_2_font_size = title_1_font_size / 1.5f;
        }
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);
            if (sprite_atlas.data == NULL) {
                int width = MeasureText("Press any key to select sprite atlas", title_1_font_size);
                DrawText("Press any key to select sprite atlas", GetScreenWidth() / 2 - width / 2,
                            GetScreenHeight() / 2 - 10, title_1_font_size, LIGHTGRAY);
            } else {
                DrawTextureEx(sprite_atlas_texture, pos, 0.f, zoom, WHITE);
                draw_zoom_and_pos();
                if (output_canvas) {
                    int text_width = MeasureText("Saving canvas...", title_1_font_size);
                    DrawText("Saving canvas...", GetScreenWidth() / 2 - text_width / 2,
                                GetScreenHeight() / 2 - 10, title_1_font_size, LIGHTGRAY);
                }
            }
        EndDrawing();
        //----------------------------------------------------------------------------------

        if (output_canvas) {
            save_canvas(output_path);
            free(output_path);
            output_canvas = false;
        }
    }
    CloseWindow();
    return 0;
}

int numPlaces (int n) {
    if (n < 0) return numPlaces ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}

void draw_zoom_and_pos() {
    int zoom_int = (int) zoom;
    int num_places = numPlaces(zoom_int);
    int buffer_zoom_size = 8 + num_places + 1;
    char buffer_zoom[buffer_zoom_size];
    snprintf(buffer_zoom, buffer_zoom_size, "Zoom: %f", zoom);
    int width_zoom_text = MeasureText("Zoom: ", title_2_font_size);
    int width_zoom_value = MeasureText("0", title_2_font_size) * (num_places + 1) + MeasureText(".", title_2_font_size);
    DrawText(buffer_zoom, GetScreenWidth() - (width_zoom_text + width_zoom_value) - 20, 10, title_2_font_size, LIGHTGRAY);

    int x_int = (int) pos.x;
    int y_int = (int) pos.y;
    int num_places_x = numPlaces(x_int);
    int num_places_y = numPlaces(y_int);
    int buffer_pos_size = 9 + num_places_x + num_places_y;
    char buffer_pos[buffer_pos_size];
    snprintf(buffer_pos, buffer_pos_size, "x: %d, y: %d", x_int, y_int);
    int width_pos_text = MeasureText("x: , y: ", title_2_font_size);
    int width_pos_value = MeasureText("0", title_2_font_size) * (num_places_x + num_places_y);
    DrawText(buffer_pos, GetScreenWidth() - (width_pos_text + width_pos_value) - 20, 20 + title_2_font_size, title_2_font_size, LIGHTGRAY);

    int width_controls_text = MeasureText("Press R to reset zoom and pos\nPress E to save output pallete", title_2_font_size);
    DrawText("Press R to reset zoom and pos\nPress E to save output pallete", GetScreenWidth() - width_controls_text - 20,
                GetScreenHeight() - 10 - title_2_font_size * 2, title_2_font_size, LIGHTGRAY);
}

typedef struct {
    Color color;
    size_t value;
} hashmap_color_entry;

int color_compare(const void *a, const void *b, void *udata) {
    const hashmap_color_entry *color_1 = a;
    const hashmap_color_entry *color_2 = b;

    int r = color_1->color.r - color_2->color.r;
    if (r) return r;
    int g = color_1->color.g - color_2->color.g;
    if (g) return g;
    int b = color_1->color.b - color_2->color.b;
    if (b) return b;
    int a = color_1->color.a - color_2->color.a;
    if (a) return a;
    return 0;
}

uint64_t color_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    hashmap_color_entry *color = item;
    char data[12 + 1];
    snprintf(data, 12 + 1, "%03d%03d%03d%03d", color->color.r, color->color.g, color->color.b, color->color.a);
    return hashmap_sip(data, strlen(data), seed0, seed1);
}


void save_canvas(const char* path) {
    FILE *fptr;
    fptr = fopen(path, "w");
    if (!fptr)
        return;
    fprintf(fptr, "[");

    struct hashmap *map = hashmap_new(sizeof(Color), 0, 0, 0, color_hash, color_compare, NULL, NULL);
    size_t pallete_key = 0;
    int bytes_per_pixel;
    switch (sprite_atlas.format) {
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
            bytes_per_pixel = 4;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
            bytes_per_pixel = 3;
    }
    for (size_t row = 0; row < sprite_atlas.height; row++) {
        for (size_t column = 0; column < sprite_atlas.width; column++) {
            void* current_pixel = sprite_atlas.data + (row * sprite_atlas.width * bytes_per_pixel) + column * bytes_per_pixel;
            Color current_color =   (Color){.r = ((char*)current_pixel)[0],
                                            .g = ((char*)current_pixel)[1],
                                            .b = ((char*)current_pixel)[2], 
                                            .a = bytes_per_pixel == 3 ? 255 : ((char*)current_pixel)[3]};
            hashmap_color_entry *entry = hashmap_get(map, &(hashmap_color_entry){.color = current_color});
            size_t current_value;
            if (entry) {
                current_value = entry->value;
            } else {
                current_value = pallete_key++;
                hashmap_set(map, &(hashmap_color_entry){.color = current_color, .value = current_value});
            }
            bool is_last = (row == sprite_atlas.height - 1 && column == sprite_atlas.width - 1);
            fprintf(fptr, is_last? "%d" : "%d, ", current_value);
        }
    }

    fprintf(fptr, "]");
    fclose(fptr);
}