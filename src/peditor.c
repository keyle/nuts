#include <string.h>
#include "nuts.h"

#define PIECES_INIT_CAP 128
#define PIECES_INIT_BUFFER 4096

typedef struct {
    bool is_original;
    size_t start;
    size_t end;
} piece_t;

typedef struct {
    char* original;
    char* buffer;
    piece_t* pieces;
    size_t size;
    size_t cap;
    size_t buffer_pos;
} peditor_t;

piece_t make_piece(bool is_original, size_t start, size_t end) {
    return (piece_t){
        .is_original = is_original,
        .start = start,
        .end = end,
    };
}

piece_t add_piece(peditor_t* pe, bool is_original, size_t start, size_t end, size_t at_index) {
    piece_t p = make_piece(is_original, start, end);

    if (pe->size == pe->cap) {
        pe->pieces = realloc(pe->pieces, pe->cap * 2);
        pe->cap *= 2;
    }

    for (size_t j = pe->size; j > at_index; j--) {
        pe->pieces[j] = pe->pieces[j - 1];
    }

    pe->pieces[at_index] = p;
    pe->size++;
    return p;
}

peditor_t new_peditor(char text[static 1]) {
    peditor_t pe = {
        .original = strdup(text),
        .buffer = calloc(PIECES_INIT_BUFFER, sizeof(char)),
        .pieces = calloc(PIECES_INIT_CAP, sizeof(piece_t)),
        .size = 0,
        .cap = PIECES_INIT_CAP,
        .buffer_pos = 0,
    };

    add_piece(&pe, pe.original, 0, strlen(text), 0);

    return pe;
}

void insert_text(peditor_t* pe, char c, size_t pos) {
    pe->buffer[pe->buffer_pos] = c;

    for (size_t i = 0; i < pe->size; i++) {
        piece_t p = pe->pieces[i];

        if (pos >= p.start && pos < p.end) {
            // found the piece
            pe->pieces[i].end = pos;

            // new piece takes our new text
            add_piece(pe, false, pe->buffer_pos, pe->buffer_pos + 1, ++i);

            // next piece starts where previous ended
            add_piece(pe, p.is_original, pos, p.end, ++i);
            break;
        }
    }
    pe->buffer_pos++;
}

char* peditor_render(peditor_t* pe) {
    size_t size = 1;
    size_t k = 0;
    char* res;

    // count chars
    for (size_t i = 0; i < pe->size; i++) {
        piece_t p = pe->pieces[i];
        size += p.end - p.start;
    }

    res = calloc(size, sizeof(char));

    // set chars
    for (size_t i = 0; i < pe->size; i++) {
        piece_t p = pe->pieces[i];
        for (size_t j = p.start; j < p.end; j++) {
            char c = p.is_original ? pe->original[j] : pe->buffer[j];
            printf("%c", c);
            res[k] = c;
            k++;
        }
    }

    res[size] = '\0';

    printf("\n");

    return res;
}

void debug_pieces(peditor_t* pe) {
    for (size_t i = 0; i < pe->size; i++) {
        piece_t p = pe->pieces[i];
        printf("piece: %zu, @original: %i, start index: %zu, end index: %zu\n", i, p.is_original, p.start, p.end);
    }
    printf("\n");
}

int main() {
    peditor_t pe = new_peditor("1234567890");
    peditor_render(&pe);
    debug_pieces(&pe);

    assert(pe.size == 1 && "there should be one piece");
    assert(pe.pieces[0].is_original == true && "first piece should be original");
    assert(pe.pieces[0].start == 0 && "first piece should start at 0");
    assert(pe.pieces[0].end == 10 && "first piece should end at 10");
    assert(strncmp(peditor_render(&pe), "1234567890", 10) == 0 && "unexpected string result");

    insert_text(&pe, 'V', 5);
    peditor_render(&pe);
    debug_pieces(&pe);

    assert(pe.size == 3 && "there should be three pieces");
    assert(pe.pieces[0].is_original == true && "first piece should be original");
    assert(pe.pieces[1].is_original == false && "second piece should be buffer");
    assert(pe.pieces[2].is_original == true && "third piece should be original");

    assert(pe.pieces[0].start == 0 && "first piece should start at 0");
    assert(pe.pieces[0].end == 5 && "first piece should end at 5");

    assert(pe.pieces[1].start == 0 && "second piece should start at 0 (buffer)");
    assert(pe.pieces[1].end == 1 && "second piece should end at 1 (buffer");

    assert(pe.pieces[2].start == 5 && "third piece should start at 5");
    assert(pe.pieces[2].end == 10 && "third piece should end at 10");

    assert(strncmp(peditor_render(&pe), "12345V67890", 11) == 0 && "unexpected string result");
}

// user inserts text at POS
// append to the buffer and update the buffer pos
// new piece from the added text
// go through pieces, work out whether POS is within this piece
// split this piece in two, insert the new pos in between, shift the following pos right by 1
