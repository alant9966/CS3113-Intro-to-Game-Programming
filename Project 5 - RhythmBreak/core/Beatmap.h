#include "../entities/Entity.h"

#ifndef BEATMAP_H
#define BEATMAP_H

enum NoteMode { FALLING, CIRCLE, SCANLINE };

// Represents a note in falling-note mode
struct FallingNote
{
    float hitTime;
    int   lane;
    int   notesIndex;  // index in notes pool
};

// Represents a note in circle mode
struct CircleNote
{
    float   hitTime;
    Vector2 position;    // screen position (circle center)
    int     notesIndex;  // index in notes pool
};

// Represents a beatmap section (one mode)
struct Section
{
    NoteMode noteMode;

    float startTime;
    float endTime;
    float transitionStart;

    // Stores data for all mapped notes
    std::vector<FallingNote> fallingNotes;
    std::vector<CircleNote>  circleNotes;
};

// Represents the beatmap!
struct Beatmap
{
    std::string title;
    std::string audio;

    float bpm;  // bpm for UI
    float offset;  // calibrated input offset
    
    // List of all beatmap sections
    std::vector<Section> sections; 

    int totalNotes() const;

    // Create a beatmap from an input JSON file
    static Beatmap loadJSON(const char* fpath);
};

#endif
