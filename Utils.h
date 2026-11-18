#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

inline void waitSeconds(float seconds) {
#ifdef _WIN32
    Sleep(static_cast<DWORD>(seconds * 1000));
#else
    usleep(static_cast<useconds_t>(seconds * 1000000));
#endif
}

inline void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

inline void pauseConsole(const std::string& message = "\nPress Enter to continue...") {
    std::cout << message;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

#endif 
