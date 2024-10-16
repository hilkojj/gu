
#ifndef GAME_FILEWATCHER_H
#define GAME_FILEWATCHER_H

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

    /**
     * set this to false in a callback to stop the watching thread.
     */
    bool bContinueWatching = true;

    void addDirectoryToWatch(const char *path, bool bRecursive);

    void startWatchingSync();

    void startWatchingAsync();

    ~FileWatcher();

  private:
    std::vector<std::string> paths;
    std::thread thread;
};


#endif
