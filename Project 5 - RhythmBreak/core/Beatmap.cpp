#include "Beatmap.h"
#include <fstream>
#include "nlohmann/json.hpp"

// Return the total number of notes in the beatmap
int Beatmap::totalNotes() const
{
    int count = 0;
    for (auto& section : sections) {
        count += static_cast<int>(section.fallingNotes.size());
        count += static_cast<int>(section.circleNotes.size());
    }
    return count;
}

// Create a Beatmap from a JSON file
Beatmap Beatmap::loadJSON(const char* fpath)
{
    std::ifstream f;
    Beatmap beatmap;
    
    f.open(fpath);
    if (!f.is_open())
        throw std::runtime_error("Error: Failed to open beatmap.");

    // Parse file data into a JSON object
    nlohmann::json fData = nlohmann::json::parse(f);

    beatmap.title  = fData.at("title").get<std::string>();
    beatmap.audio  = fData.at("audio").get<std::string>();
    beatmap.bpm    = fData.at("bpm").get<float>();
    beatmap.offset = fData.value("offset", 0.0f);  // optional offset

    // Parse data for each beatmap section
    for (auto& sectionData : fData.at("sections")) {
        Section section;

        // Convert mode to corresponding NoteMode
        std::string mode = sectionData.at("mode").get<std::string>();
        if      (mode == "falling") section.noteMode = FALLING;
        else if (mode == "circle")  section.noteMode = CIRCLE;
        else                        section.noteMode = SCANLINE;

        section.startTime = sectionData.at("start").get<float>();
        section.endTime   = sectionData.at("end").get<float>();

        // Parse data for each section's notes
        for (auto& noteData : sectionData.at("notes")) {
            // Create and append a circle note
            if (section.noteMode == CIRCLE || section.noteMode == SCANLINE) {
                CircleNote currNote;
                currNote.hitTime    = noteData.at("hitTime").get<float>();
                currNote.position.x = noteData.at("x").get<float>();
                currNote.position.y = noteData.at("y").get<float>();
                currNote.notesIndex = -1;

                section.circleNotes.push_back(currNote);

            // Create and append a falling note
            } else {
                FallingNote currNote;
                currNote.hitTime    = noteData.at("hitTime").get<float>();
                currNote.lane       = noteData.at("lane").get<int>();
                currNote.notesIndex = -1;

                section.fallingNotes.push_back(currNote);
            }
        }

        // Add the section to the beatmap
        beatmap.sections.push_back(section);
    }

    return beatmap;
}
