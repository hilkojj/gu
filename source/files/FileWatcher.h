
#ifndef GAME_FILEWATCHER_H
#define GAME_FILEWATCHER_H

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <thread>

class FileWatcher
{

    std::vector<std::string> paths;
    std::thread watchThread;

  public:

    using callback = std::function<void(const std::string &filePath)>;

    callback onCreate = [] (auto) {};
    callback onDelete = [] (auto) {};
    callback onChange = [] (auto) {};

    /**
     * set this to false in a callback to stop the watching thread.
     */
    bool continueWatching = true;

    void addDirectoryToWatch(const std::string &path, bool recursive);

    void startWatchingSync();

    void startWatchingAsync();

};


#endif
