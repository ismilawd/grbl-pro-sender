// GCodeLangDef.h
#pragma once
#include "TextEditor.h"

inline TextEditor::LanguageDefinition GCodeLanguageDefinition() {
    auto langDef = TextEditor::LanguageDefinition();

    // Gcode keywords
    static const char *gcodeKeywords[] = {
        "G0", "G1", "G2", "G3", "G4",
        "G10", "G17", "G18", "G19", "G20",
        "G21", "G28", "G90", "G91", "G92",
        "M0", "M1", "M2", "M3", "M4", "M5", "M30",
        nullptr
    };

    for (int i = 0; gcodeKeywords[i] != nullptr; ++i) {
        langDef.mKeywords.insert(gcodeKeywords[i]);
    }

    langDef.mCommentStart = "(";
    langDef.mCommentEnd = ")";
    langDef.mSingleLineComment = ";";
    langDef.mCaseSensitive = false;

    // Numeric literals
    langDef.mTokenRegexStrings.push_back(
        std::make_pair<std::string, TextEditor::PaletteIndex>(
            "[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)", TextEditor::PaletteIndex::Number));

    // Parameters and axes (X, Y, Z, F, S, E, etc.)
    langDef.mTokenRegexStrings.push_back(
        std::make_pair<std::string, TextEditor::PaletteIndex>(
            "[XYZFABCUVWES]", TextEditor::PaletteIndex::Keyword));

    // Comments
    langDef.mTokenRegexStrings.push_back(
        std::make_pair<std::string, TextEditor::PaletteIndex>(";.*", TextEditor::PaletteIndex::Comment));
    langDef.mTokenRegexStrings.push_back(
        std::make_pair<std::string, TextEditor::PaletteIndex>("\\(.*?\\)", TextEditor::PaletteIndex::Comment));

    langDef.mAutoIndentation = false;

    langDef.mName = "G-code";
    return langDef;
}
