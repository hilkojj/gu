#include "FileWatcher.h"

#include "file_utils.h"

#include "../math/math_utils.h"
#include "../utils/gu_error.h"
#include "../utils/string_utils.h"

#include <thread>
#include <map>

void FileWatcher::addDirectoryToWatch(const char *path, bool bRecursive)
{
    paths.emplace_back(path);
    if (!su::endsWith(path, "/"))
    {
        paths.back() += '/';
    }

    if (bRecursive)
    {
        fu::iterateDirectoryRecursively(path, [&] (auto childPath, bool bIsDir)
        {
            if (bIsDir)
            {
                addDirectoryToWatch(childPath.c_str(), false);
            }
        });
    }
}

void FileWatcher::startWatchingAsync()
{
    thread = std::thread(&FileWatcher::startWatchingSync, this);
}

#ifdef linux

#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE sizeof(inotify_event)
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

void FileWatcher::startWatchingSync()
{
    inotifyInstance = inotify_init();

    if (inotifyInstance < 0)
    {
        throw gu_err("Failed to get inotify instance");
    }

    if (pipe(stopPipe) < 0)
    {
        throw gu_err("Failed to create stop-pipe for inotify");
    }

    std::map<int, std::string> watchToPath;

    for (auto &path : paths)
    {
        const int watch = inotify_add_watch(inotifyInstance, path.c_str(), IN_CREATE | IN_DELETE | IN_CLOSE_WRITE);
        watchToPath[watch] = path;
    }

    std::cout << "Started watching " << watchToPath.size() << " directories.\n";

    while (true)
    {
        fd_set fileDescriptors;
        FD_ZERO(&fileDescriptors);
        FD_SET(stopPipe[0], &fileDescriptors);
        FD_SET(inotifyInstance, &fileDescriptors);

        if (select(max(inotifyInstance, stopPipe[0]) + 1, &fileDescriptors, nullptr, nullptr, nullptr) < 0)
        {
            throw gu_err("Error while watching.");
        }

        if (FD_ISSET(stopPipe[0], &fileDescriptors))
        {
            // Stop pipe was written to.
            break;
        }
        if (FD_ISSET(inotifyInstance, &fileDescriptors))
        {
            char buffer[EVENT_BUF_LEN];
            const int eventLength = read(inotifyInstance, buffer, EVENT_BUF_LEN);

            if (eventLength < 0)
            {
                throw gu_err("Reading inotify events failed");
            }

            inotify_event *event = (inotify_event *) &buffer[0];
            if (!event->len || event->mask & IN_ISDIR)
            {
                continue;
            }

            const std::string &directoryPath = watchToPath[event->wd];
            const std::string filePath = directoryPath + std::string(event->name);

            if (event->mask & IN_CREATE && onCreate)
            {
                onCreate(filePath);
            }
            else if (event->mask & IN_DELETE && onDelete)
            {
                onDelete(filePath);
            }
            else if (event->mask & IN_CLOSE_WRITE && onChange)
            {
                onChange(filePath);
            }
        }
    }
    for (auto &[watch, path] : watchToPath)
    {
        inotify_rm_watch(inotifyInstance, watch);
    }

    close(inotifyInstance);
    close(stopPipe[0]);
    close(stopPipe[1]);
    inotifyInstance = -1;

    std::cout << "Stopped watching " << watchToPath.size() << " directories.\n";
}

#else

void FileWatcher::startWatchingSync()
{
    throw gu_err("Not implemented on this platform");
}

#endif

void FileWatcher::stopWatching()
{
#if linux
    if (inotifyInstance >= 0)
    {
        write(stopPipe[1], "bye", 3);
    }
#endif
    if (thread.joinable())
    {
        thread.join();
    }
}

FileWatcher::~FileWatcher()
{
    stopWatching();
}
