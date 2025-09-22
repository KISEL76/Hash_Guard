#pragma once

#include <filesystem>
#include <mutex>
#include <condition_variable>
#include <queue>

class PathQueue {
public:
    using T = std::filesystem::path;

    void push(T v);

    bool pop(T &out);

    void close();

private:
    std::queue<T> q;
    std::mutex m;
    std::condition_variable cv;
    bool closed = false;
};