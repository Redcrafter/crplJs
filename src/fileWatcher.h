#pragma once
#include "fs.h"
#include <functional>

#if false && _WIN32
#include <windows.h>
#endif

enum class FileStatus {created, modified, erased};

class FileWatcher {
    #if false && _WIN32

    #else
    std::vector<std::string> Paths;
    std::unordered_map<std::string, fs::file_time_type> Files;
    #endif
public:
    FileWatcher(const std::vector<std::string>& files);

    void start(const std::function<void (std::string, FileStatus)>& callback);
    // void startAsync(std::function<void (std::string, FileStatus)>& callback);
};
