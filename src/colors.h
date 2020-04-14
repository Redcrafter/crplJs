#pragma once
#include <ostream>

#ifdef _WIN32
#include <Windows.h>

enum Colors {
    Reset = -1,

    FgBlack = 0x100,
    FgBlue = FOREGROUND_BLUE,
    FgGreen = FOREGROUND_GREEN,
    FgCyan = FgGreen | FgBlue,
    FgRed = FOREGROUND_RED,
    FgMagenta = FgRed | FgBlue,
    FgYellow = FgRed | FgGreen,
    FgWhite = FgRed | FgGreen | FgBlue,

    FgBrightBlack = FOREGROUND_INTENSITY,
    FgBrightBlue = FgBlue | FOREGROUND_INTENSITY,
    FgBrightGreen = FgGreen | FOREGROUND_INTENSITY,
    FgBrightCyan = FgCyan | FOREGROUND_INTENSITY,
    FgBrightRed = FgRed | FOREGROUND_INTENSITY,
    FgBrightMagenta = FgMagenta | FOREGROUND_INTENSITY,
    FgBrightYellow = FgYellow | FOREGROUND_INTENSITY,
    FgBrightWhite = FgWhite | FOREGROUND_INTENSITY,

    BgBlack = 0x200,
    BgBlue = BACKGROUND_BLUE,
    BgGreen = BACKGROUND_GREEN,
    BgCyan = BgGreen | BgBlue,
    BgRed = BACKGROUND_RED,
    BgMagenta = BgRed | BgBlue,
    BgYellow = BgRed | BgGreen,
    BgWhite = BgRed | BgGreen | BgBlue,

    BgBrightBlack = BACKGROUND_INTENSITY,
    BgBrightBlue = BgBlue | BACKGROUND_INTENSITY,
    BgBrightGreen = BgGreen | BACKGROUND_INTENSITY,
    BgBrightCyan = BgCyan | BACKGROUND_INTENSITY,
    BgBrightRed = BgRed | BACKGROUND_INTENSITY,
    BgBrightMagenta = BgMagenta | BACKGROUND_INTENSITY,
    BgBrightYellow = BgYellow | BACKGROUND_INTENSITY,
    BgBrightWhite = BgWhite | BACKGROUND_INTENSITY,
};

inline int attributes = 0;
inline int currentAttributes = 0;

inline std::ostream& operator<<(std::ostream& stream, const Colors& col) {
    int color;

    if(col == Reset) {
        color = attributes;
    } else {
        color = currentAttributes;
        if(col & 0xF) {
            color = (color & 0xF0) | (col & 0xF);
        } else if(col & FgBlack) {
            color = color & 0xF0;
        }
        if(col & 0xF0) {
            color = (color & 0xF) | (col & 0xF0);
        } else if(col & BgBlack) {
            color = color & 0xF;
        }
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    return stream;
}

inline void LoadDefaultColor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);
    attributes = info.wAttributes;
}

#else
enum Colors {
    Reset = 0,

    FgBlack = 30,
    FgRed,
    FgGreen,
    FgYellow,
    FgBlue,
    FgMagenta,
    FgCyan,
    FgWhite,
    FgBrightBlack = 90,
    FgBrightRed,
    FgBrightGreen,
    FgBrightYellow,
    FgBrightBlue,
    FgBrightMagenta,
    FgBrightCyan,
    FgBrightWhite,

    BgBlack = 40,
    BgRed,
    BgGreen,
    BgYellow,
    BgBlue,
    BgMagenta,
    BgCyan,
    BgWhite,
    BgBrightBlack = 100,
    BgBrightRed,
    BgBrightGreen,
    BgBrightYellow,
    BgBrightBlue,
    BgBrightMagenta,
    BgBrightCyan,
    BgBrightWhite
};

inline std::ostream& operator<<(std::ostream& stream, const Colors& col) {
    return stream << "\033[" << (int)col << "m";
}
#endif