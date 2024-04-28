#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <assert.h>

typedef struct gaped_t {
    size_t start;
    size_t end;
    size_t size;
    size_t cap;
    char* buffer;
} gaped_t;

#define GAP_SIZE 20

gaped_t gap_new(char* text) {
    size_t len = strlen(text);
    size_t bs = len + GAP_SIZE;

    gaped_t g = {
        .start = 0,
        .end = GAP_SIZE,
        .buffer = calloc(bs, sizeof(char)),
        .size = len,
        .cap = bs,
    };

    memcpy(g.buffer + GAP_SIZE, text, len);

    return g;
}

void gap_left(gaped_t* g) {
    if (g->start == 0)
        return;
    g->start--;
    g->end--;
    g->buffer[g->end] = g->buffer[g->start];
    g->buffer[g->start] = g->buffer[g->start + 1];
}

void gap_right(gaped_t* g) {
    if (g->end == g->cap)
        return;

    g->buffer[g->start] = g->buffer[g->end];
    g->buffer[g->end] = g->buffer[g->end - 1];
    g->start++;
    g->end++;
}

void gap_insert(gaped_t* g, const char c) {
    g->size++;
    if (g->size == g->cap) {
        const size_t new_gap = g->cap * 1.55; // (g->cap - GAP_SIZE) * 2;
        printf("\n-- resize sz %zu, cap %zu --> %zu", g->size, g->cap, g->cap + new_gap);
        g->cap += new_gap;
        g->end += new_gap;
        g->buffer = realloc(g->buffer, g->cap);
    }
    g->buffer[g->start++] = c;
}

void gap_delete(gaped_t* g) {
    if (g->start == 0)
        return;

    g->size--;
    g->start--;
    g->buffer[g->start] = '\0';
}

void gap_delete_under(gaped_t* g) {
    gap_right(g);
    gap_delete(g);
}

char* gap_render(gaped_t* g) {
    size_t j = 0;
    char* res = malloc(g->size);
    assert(res && "could not alloc");

    if (g->cap == 0) {
        free(res);
        return NULL;
    }
    for (size_t i = 0; i < g->cap; i++) {
        const char c = g->buffer[i];
        if (c == '\0')
            continue;
        res[j++] = c;
    }
    return res;
}

char* gap_test_render(gaped_t* g) {
    size_t j = 0;
    char* res = malloc(g->cap);
    assert(res && "could not alloc");

    if (g->cap == 0) {
        free(res);
        return NULL;
    }
    for (size_t i = 0; i < g->cap; i++) {
        char c = g->buffer[i];
        if (c == '\0')
            c = '_';
        res[j++] = c;
    }
    return res;
}

void gap_debug(gaped_t* g) {
    return;
    size_t i = 0;
    printf("\n");
    while (i < g->cap) {
        char c = g->buffer[i];
        printf("%c", c == '\0' ? '_' : c);
        i++;
    }
    printf("  \t start: %zu end: %zu  \t(%zu of %zu)", g->start, g->end, g->size, g->cap);
}
