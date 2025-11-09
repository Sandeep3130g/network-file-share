#pragma once
#include <string>
#include <sys/socket.h>
#include <unistd.h>

// send entire buffer
inline bool sendAll(int fd, const void* data, size_t len) {
    const char* p = (const char*)data;
    while (len > 0) {
        ssize_t n = send(fd, p, len, 0);
        if (n <= 0) return false;
        p += n;
        len -= n;
    }
    return true;
}

// receive exact length
inline bool recvAll(int fd, void* data, size_t len) {
    char* p = (char*)data;
    while (len > 0) {
        ssize_t n = recv(fd, p, len, 0);
        if (n <= 0) return false;
        p += n;
        len -= n;
    }
    return true;
}

// read line until '\n'
inline bool recvLine(int fd, std::string& out) {
    out.clear();
    char c;
    while (true) {
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0) return false;
        if (c == '\n') break;
        if (c != '\r') out.push_back(c);
    }
    return true;
}

inline bool sendLine(int fd, const std::string& s) {
    std::string msg = s;
    if (msg.empty() || msg.back() != '\n')
        msg.push_back('\n');
    return sendAll(fd, msg.data(), msg.size());
}
