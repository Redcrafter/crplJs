#pragma once
#include <ostream>

#ifdef _WIN32
#include <Windows.h>
#endif

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
    BgBrightBlack = 90,
    BgBrightRed,
    BgBrightGreen,
    BgBrightYellow,
    BgBrightBlue,
    BgBrightMagenta,
    BgBrightCyan,
    BgBrightWhite
};

inline int attributes = 0;

inline std::ostream& operator<<(std::ostream& stream, const Colors& col) {
#ifdef _WIN32
    int color = attributes & 0xF0;
    switch (col) {
        case FgBrightRed:
            color |= 12;
            break;
        case FgBrightMagenta:
            color |= 13;
            break;
        case Reset:
            color = attributes;
            break;
        default:
#ifdef _DEBUG
            throw std::logic_error("Windows color code not added");
#endif
			break;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    return stream;
#else
    return stream << "\033[" << (int)col << "m";
#endif
}

#ifdef _WIN32
inline void LoadDefaultColor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);
    attributes = info.wAttributes;
}
#endif