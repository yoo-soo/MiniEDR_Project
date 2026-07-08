#pragma once
#include <string>
#include <chrono>
#include <sys/types.h>

enum class EventType { Process, File, Network };

struct Event {
    EventType type;
    std::chrono::system_clock::time_point timestamp;
    virtual ~Event() = default;
};

struct ProcessEvent : public Event {
    pid_t pid;
    pid_t ppid;
    std::string exePath;
    std::string cmdline;
    uid_t uid;
    bool isExit = false;
};

struct FileEvent : public Event {
    enum class Action { Created, Modified, Deleted, Renamed };
    std::string path;
    Action action;
    pid_t triggeringPid = -1;
};

struct NetworkEvent : public Event {
    std::string localAddr, remoteAddr;
    uint16_t localPort, remotePort;
    pid_t pid = -1;
    std::string protocol;
};
