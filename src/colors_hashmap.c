#include "colors_hashmap.h"
#include "commons.h"
#include "utils/hashmap.h"

unsigned int num_places(unsigned long n) {
    if (n < 10) return 1;
    return 1 + num_places (n / 10);
}

int color_compare_3B(const void *entry_1_void, const void *entry_2_void, void *udata) {
    struct hashmap_color_entry {
        Color colors[layers.num_elements];
        unsigned long value;
    };
    struct hashmap_color_entry *entry_1 = entry_1_void;
    struct hashmap_color_entry *entry_2 = entry_2_void;

    for (unsigned long i = 0; i < layers.num_elements; i++) {
        Color *color_1 = entry_1->colors + i;
        Color *color_2 = entry_2->colors + i;
        int r = color_1->r - color_2->r;
        if (r) return r;
        int g = color_1->g - color_2->g;
        if (g) return g;
        int b = color_1->b - color_2->b;
        if (b) return b;
    }
    return 0;
}

int color_compare_4B(const void *entry_1_void, const void *entry_2_void, void *udata) {
    struct hashmap_color_entry {
        Color colors[layers.num_elements];
        unsigned long value;
    };
    struct hashmap_color_entry *entry_1 = entry_1_void;
    struct hashmap_color_entry *entry_2 = entry_2_void;

    for (unsigned long i = 0; i < layers.num_elements; i++) {
        Color *color_1 = entry_1->colors + i;
        Color *color_2 = entry_2->colors + i;
        int r = color_1->r - color_2->r;
        if (r) return r;
        int g = color_1->g - color_2->g;
        if (g) return g;
        int b = color_1->b - color_2->b;
        if (b) return b;
        int a = color_1->a - color_2->a;
        if (a) return a;
    }
    return 0;
}

uint64_t color_hash_3B(const void *entry_void, uint64_t seed0, uint64_t seed1) {
    struct hashmap_color_entry {
        Color colors[layers.num_elements];
        unsigned long value;
    };
    struct hashmap_color_entry *entry = entry_void;
    char data[layers.num_elements * 9 + 1];
    for (unsigned long i = 0; i < layers.num_elements; i++) {
        Color *color = entry->colors + i;
        snprintf(data + i * 9, 9 + 1, "%03d%03d%03d", color->r, color->g, color->b);
    }
    return hashmap_sip(data, layers.num_elements * 9 + 1, seed0, seed1);
}

uint64_t color_hash_4B(const void *entry_void, uint64_t seed0, uint64_t seed1) {
    struct hashmap_color_entry {
        Color colors[layers.num_elements];
        unsigned long value;
    };
    struct hashmap_color_entry *entry = entry_void;
    char data[layers.num_elements * 12 + 1];
    for (unsigned long i = 0; i < layers.num_elements; i++) {
        Color *color = entry->colors + i;
        snprintf(data + i * 12, 12 + 1, "%03d%03d%03d%03d", color->r, color->g, color->b, color->a);
    }
    return hashmap_sip(data, layers.num_elements * 12 + 1, seed0, seed1);
    
}

bool color_iter_3B(const void *entry_void, void *udata) {
    struct hashmap_color_entry {
        Color colors[layers.num_elements];
        unsigned long value;
    };
    struct hashmap_color_entry *entry = entry_void;
    int places = num_places(entry->value);
    int chars_per_color = /*assuming all colors have 3 digits*/ 9 + /*"()"*/ + 2 + /*", "(between channels)*/ 4;
    char string_to_print[places + /*" -> "*/ + 4 + layers.num_elements * chars_per_color + /*", "(between colors)*/ 2 * (layers.num_elements - 1) + /*"\n\0"*/ + 2];
    snprintf(string_to_print, places + /*" -> "*/ + 4 + 1, "%lu -> ", entry->value);
    int starting_place = places + /*" -> "*/ + 4;
    for (unsigned long i = 0; i < layers.num_elements; i++) {
        Color *color = entry->colors + i;
        snprintf(string_to_print + starting_place + i * (chars_per_color + /*", "*/ 2), 20 + 1, "(%3d, %3d, %3d)", color->r, color->g, color->b);
        if (i + 1 < layers.num_elements)
            snprintf(string_to_print + starting_place + i * (chars_per_color + /*", "*/ 2) + chars_per_color, 2 + 1, ", ");
    }
    fptr ? fprintf(fptr, "%s\n", string_to_print) : printf("%s\n", string_to_print);
    return true;
}

bool color_iter_4B(const void *entry_void, void *udata) {
    struct hashmap_color_entry {
        Color colors[layers.num_elements];
        unsigned long value;
    };
    struct hashmap_color_entry *entry = entry_void;
    int places = num_places(entry->value);
    int chars_per_color = /*assuming all colors have 3 digits*/ 12 + /*"()"*/ + 2 + /*", "(between channels)*/ 6;
    char string_to_print[places + /*" -> "*/ + 4 + layers.num_elements * chars_per_color + /*", "(between colors)*/ 2 * (layers.num_elements - 1) + /*"\n\0"*/ + 2];
    snprintf(string_to_print, places + /*" -> "*/ + 4 + 1, "%lu -> ", entry->value);
    int starting_place = places + /*" -> "*/ + 4;
    for (unsigned long i = 0; i < layers.num_elements; i++) {
        Color *color = entry->colors + i;
        snprintf(string_to_print + starting_place + i * (chars_per_color + /*", "*/ 2), 20 + 1, "(%3d, %3d, %3d, %3d)", color->r, color->g, color->b, color->a);
        if (i + 1 < layers.num_elements)
            snprintf(string_to_print + starting_place + i * (chars_per_color + /*", "*/ 2) + chars_per_color, 2 + 1, ", ");
    }
    fptr ? fprintf(fptr, "%s\n", string_to_print) : printf("%s\n", string_to_print);
    return true;
}