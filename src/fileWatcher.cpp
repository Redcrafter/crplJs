#include "fileWatcher.h"
#include <thread>
#include <chrono>


#if false && defined(_WIN32)
void FileWatcher::start(const std::function<void (std::string, FileStatus)>& callback) {
    HANDLE fwChangeHandle = FindFirstChangeNotificationA();

    while (true) {
        auto dwWaitStatus = WaitForSingleObject(fwChangeHandle, INFINITE);

        switch (dwWaitStatus) {
            case WAIT_OBJECT_0:
                break;
        }
    }
    
}
#else 
FileWatcher::FileWatcher(const std::vector<std::string>& paths): Paths(paths) {
    for(auto &path : paths) {
        if(fs::is_directory(path)) {
            for (auto &file : fs::recursive_directory_iterator(path)) {
                Files[file.path().string()] = fs::last_write_time(file);
            }
        } else {
            auto p = fs::canonical(path);
            Files[p.string()] = fs::last_write_time(p);
        }
    }
}

void FileWatcher::start(const std::function<void (std::string, FileStatus)>& callback) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto it = Files.begin();
        while (it != Files.end()) {
            if(!fs::exists(it->first)) {
                callback(it->first, FileStatus::erased);
                it = Files.erase(it);
            } else {
                it++;
            }
        }
        
        for(auto &path : Paths) {
            if(fs::is_directory(path)) {
                for (auto &file : fs::recursive_directory_iterator(path)) {
                    fs::file_time_type lastWriteTime = fs::last_write_time(file);
                    auto str = file.path().string();

                    if(Files.count(str)) {
                        auto& val = Files[str];

                        if(val != lastWriteTime) {
                            val = lastWriteTime;
                            callback(str, FileStatus::modified);
                        }
                    } else {
                        Files[str] = lastWriteTime;
                        callback(str, FileStatus::created);
                    }
                }
            } else {
                auto file = fs::canonical(path);
                auto lastWriteTime = fs::last_write_time(file);
                auto str = file.string();

                if(Files.count(str)) {
                    auto& val = Files[str];

                    if(val != lastWriteTime) {
                        val = lastWriteTime;
                        callback(str, FileStatus::modified);
                    }
                } else {
                    Files[str] = lastWriteTime;
                    callback(str, FileStatus::created);
                }
            }
        }
    }
}
#endif
