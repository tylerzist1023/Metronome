#pragma once
#include <stdlib.h>

static const int TEMPO_MARKINGS_SIZE = 13;
static const char* TEMPO_MARKINGS[] =
{
    "Larghissimo (20)",
    "Grave (40)",
    "Lento (50)",
    "Largo (60)",
    "Adagio (70)",
    "Adagietto (80)",
    "Andante (90)",
    "Moderato (100)",
    "Allegretto (120)",
    "Allegro (140)",
    "Vivace (170)",
    "Presto (180)",
    "Prestissimo (200)"
};
static const int TEMPO_MARKING_VALUES[] =
{
    20,
    40,
    50,
    60,
    70,
    80,
    90,
    100,
    120,
    140,
    170,
    180,
    200
};

enum NoteValue
{
    NOTE_SIXTEENTH = 0,
    NOTE_EIGHTH,
    NOTE_QUARTER,
    NOTE_HALF,
    NOTE_WHOLE
};
static const int NOTE_NAMES_SIZE = 5;
static const char* NOTE_NAMES[] =
{
    "Sixteenth",
    "Eighth",
    "Quarter",
    "Half",
    "Whole"
};

bool show_tempo_dialog(int def, int* result);
bool show_signature_dialog(int beats_def, NoteValue value_def, int* beats, NoteValue* value);

int tempo_to_marking(int tempo);