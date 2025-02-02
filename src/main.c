#include "raylib.h"
#include "window.h"
#include "hashmap.h"
#include "gui_elements.h"
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <stdint.h>

#include "icon_32px.h"
#include "icon_64px.h"
#include "icon_128px.h"
#include "icon_256px.h"

Image sprite_atlas;
Texture2D sprite_atlas_texture;
float zoom = 1.0f;
Vector2 pos = {0.f, 0.f};
int title_1_font_size = 450 / 20;
int title_2_font_size = 450 / 20 / 1.5f;
FILE *fptr;

menu file_menu;
button file_bt;

void upload_image();
void export_pallette();
void export_raylib_header();
void set_icons();
void draw_zoom_and_pos();
void save_canvas(const char* path);
void check_click_gui();
void draw_gui();

int main(void) {
    InitWindow(800, 450, "Sprite atlas to palletes");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    set_icons();

    button file_buttons[3] = {  {.text = "Open image", .on_click = upload_image, .opens_menu = false},
                                {.text = "Export pallette file", .on_click = export_pallette, .opens_menu = false},
                                {.text = "Export raylib header file", .on_click = export_raylib_header, .opens_menu = false}
                             };
    file_menu = (menu){.buttons = file_buttons, .num_buttons = 3, .is_open = false};
    file_bt = (button){.text = "File", .on_click = NULL, .opens_menu = true, .menu_to_open = &file_menu};
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose()) {
        check_click_gui();
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
            if (sprite_atlas.data != NULL) {
                DrawTextureEx(sprite_atlas_texture, pos, 0.f, zoom, WHITE);
                draw_zoom_and_pos();
            }
            draw_gui();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    CloseWindow();
    return 0;
}

void upload_image() {
    char* path = open_file_menu();
    if (path) {
        if (sprite_atlas.data)
            UnloadImage(sprite_atlas);
        UnloadTexture(sprite_atlas_texture);
        sprite_atlas = LoadImage(path);
        sprite_atlas_texture = LoadTexture(path);
        free(path);
    }
}

void export_pallette() {
    const char* output_path = save_file_menu();
    if (output_path)
        save_canvas(output_path);
}

void export_raylib_header() {
    const char* output_path = save_file_menu();
    if (output_path)
        ExportImageAsCode(sprite_atlas, output_path);
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

    int width_controls_text = MeasureText("Press R to reset zoom and pos", title_2_font_size);
    DrawText("Press R to reset zoom and pos", GetScreenWidth() - width_controls_text - 20, GetScreenHeight() - 10 - title_2_font_size, title_2_font_size, LIGHTGRAY);
}

typedef struct {
    Color color;
    size_t value;
} hashmap_color_entry;

int color_compare(const void *color_1_void, const void *color_2_void, void *udata) {
    const hashmap_color_entry *color_1 = color_1_void;
    const hashmap_color_entry *color_2 = color_2_void;

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
    const hashmap_color_entry *color = item;
    char data[12 + 1];
    snprintf(data, 12 + 1, "%03d%03d%03d%03d", color->color.r, color->color.g, color->color.b, color->color.a);
    return hashmap_sip(data, 13, seed0, seed1);
}

bool color_iter(const void *item, void *udata) {
    const hashmap_color_entry *color = item;
    if (fptr)
        fprintf(fptr, "%llu -> (%d, %d, %d, %d)\n", color->value, color->color.r, color->color.g, color->color.b, color->color.a);
    else
        printf("%llu -> (%d, %d, %d, %d)\n", color->value, color->color.r, color->color.g, color->color.b, color->color.a);
    return true;
}


void save_canvas(const char* path) {
    fptr = fopen(path, "w");
    if (!fptr)
        return;
    fprintf(fptr, "[");

    struct hashmap *map = hashmap_new(sizeof(hashmap_color_entry), 0, 0, 0, color_hash, color_compare, NULL, NULL);
    size_t palletes_key = 0;
    int bytes_per_pixel;
    switch (sprite_atlas.format) {
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
            bytes_per_pixel = 4;
            break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8:
            bytes_per_pixel = 3;
            break;
    }
    bool is_last;
    for (int row = 0; row < sprite_atlas.height; row++) {
        for (int column = 0; column < sprite_atlas.width; column++) {
            void* current_pixel = sprite_atlas.data + (row * sprite_atlas.width * bytes_per_pixel) + column * bytes_per_pixel;
            Color current_color =   (Color){.r = ((char*)current_pixel)[0],
                                            .g = ((char*)current_pixel)[1],
                                            .b = ((char*)current_pixel)[2], 
                                            .a = bytes_per_pixel == 3 ? 255 : ((char*)current_pixel)[3]};
            const  hashmap_color_entry *entry = hashmap_get(map, &(hashmap_color_entry){.color = current_color});
            size_t current_value;
            if (entry) {
                current_value = entry->value;
            } else {
                current_value = palletes_key++;
                hashmap_set(map, &(hashmap_color_entry){.color = current_color, .value = current_value});
            }
            is_last = (row == sprite_atlas.height - 1 && column == sprite_atlas.width - 1);
            fprintf(fptr, is_last? "%llu" : "%llu, ", current_value);
        }
        if (!is_last)
            fprintf(fptr, "\n");
    }
    fprintf(fptr, "]\n\n");
    hashmap_scan(map, color_iter, NULL);

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

void check_click_gui() {
    button *mouse_over = NULL;
    if (mouse_hovering_button(&file_bt)) mouse_over = &file_bt;
    else if (file_menu.is_open) {
        for (int i = 0; i < file_menu.num_buttons; i++)
            if (mouse_hovering_button(file_menu.buttons + i)) mouse_over = file_menu.buttons + i;
    }
    if (mouse_over == NULL) {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (mouse_over != NULL) {
            if (mouse_over->opens_menu)
                toggle_menu(mouse_over->menu_to_open);
            else {
                mouse_over->on_click();
                file_menu.is_open = false;
            }
        } else {
            file_menu.is_open = false;
        }
    }
}

void draw_gui() {
    draw_button(&file_bt, 0, 0, title_1_font_size);
    if (file_menu.is_open)
        draw_menu(&file_menu, 0, title_1_font_size + get_border_height(), title_1_font_size);
}