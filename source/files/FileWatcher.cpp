#include "FileWatcher.h"

#include "file_utils.h"

#include "../math/math_utils.h"
#include "../utils/gu_error.h"
#include "../utils/string_utils.h"

#include <thread>
#include <map>

FileWatcher::FileWatcher(const char *path, bool bRecursive) :
    bRecursive(bRecursive)
{
    paths.emplace_back(path);
    if (!su::endsWith(path, "/"))
    {
        paths.back() += '/';
    }

#ifdef linux
    if (bRecursive)
    {
        fu::iterateDirectoryRecursively(path, [&] (auto childPath, bool bIsDir)
        {
            if (bIsDir)
            {
                paths.emplace_back(childPath);
                if (!su::endsWith(childPath, "/"))
                {
                    paths.back() += '/';
                }
            }
        });
    }
#endif
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

#elif defined(_WIN32)

#include <windows.h>

void FileWatcher::startWatchingSync()
{
    if (paths.empty())
    {
        throw gu_err("No path specified");
    }

    const std::string &rootPath = paths[0];

    auto toWide = [] (const std::string &str)
    {
        const int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        std::wstring result(size - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
        return result;
    };

    auto toUtf8 = [] (const std::wstring &wide)
    {
        const int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string result(size - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &result[0], size, nullptr, nullptr);
        return result;
    };

    const std::wstring widePath = toWide(rootPath);

    windowsHandle = CreateFileW(
        widePath.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );

    if (windowsHandle == INVALID_HANDLE_VALUE)
    {
        throw gu_err("Failed to open " + rootPath);
    }

    const HANDLE stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent)
    {
        throw gu_err("Failed to create stop event");
    }

    std::cout << "Started watching " << rootPath << std::endl;

    constexpr DWORD BUFFER_SIZE = 16 * 1024;
    std::vector<BYTE> buffer(BUFFER_SIZE);

    while (true)
    {
        DWORD bytesReturned = 0;

        const bool bSuccess = ReadDirectoryChangesW(
            windowsHandle,
            buffer.data(),
            BUFFER_SIZE,
            bRecursive,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            nullptr,
            nullptr
        );

        if (!bSuccess)
        {
            if (GetLastError() == ERROR_OPERATION_ABORTED)
            {
                break;
            }
            throw gu_err("ReadDirectoryChangesW failed");
        }

        const FILE_NOTIFY_INFORMATION *info = (FILE_NOTIFY_INFORMATION *) buffer.data();

        while (true)
        {
            const std::wstring wideName(info->FileName, info->FileNameLength / sizeof(WCHAR));

            std::string relativePath = toUtf8(wideName);
            for (char &c : relativePath)
            {
                if (c == '\\')
                {
                    c = '/';
                }
            }
            const std::string fullPath = rootPath + relativePath;

            switch (info->Action)
            {
                case FILE_ACTION_ADDED:
                    if (onCreate)
                    {
                        onCreate(fullPath);
                    }
                    break;
                case FILE_ACTION_REMOVED:
                    if (onDelete)
                    {
                        onDelete(fullPath);
                    }
                    break;
                case FILE_ACTION_MODIFIED:
                    if (onChange)
                    {
                        onChange(fullPath);
                    }
                    break;
                default:
                    break;
            }

            if (info->NextEntryOffset == 0)
            {
                break;
            }

            info = (FILE_NOTIFY_INFORMATION *) (((BYTE *) info) + info->NextEntryOffset);
        }

        if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)
        {
            break;
        }
    }

    CloseHandle(windowsHandle);
    CloseHandle(stopEvent);

    windowsHandle = INVALID_HANDLE_VALUE;

    std::cout << "Stopped watching " << rootPath << std::endl;
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
#ifdef _WIN32
    if (windowsHandle != INVALID_HANDLE_VALUE)
    {
        CancelIoEx(windowsHandle, nullptr);
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
