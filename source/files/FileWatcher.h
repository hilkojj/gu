#pragma once

#include <functional>
#include <string>
#include <vector>
#include <thread>

class FileWatcher
{
  public:

    using callback = std::function<void(const std::string &filePath)>;

    callback onCreate;
    callback onDelete;
    callback onChange;

    void addDirectoryToWatch(const char *path, bool bRecursive);

    void startWatchingSync();

    void startWatchingAsync();

    void stopWatching();

    ~FileWatcher();

  private:
    std::vector<std::string> paths;
    std::thread thread;
    int inotifyInstance = -1;
    int stopPipe[2];
};
