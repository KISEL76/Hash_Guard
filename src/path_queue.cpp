#include "../include/path_queue.h"

void PathQueue::push(T v) {
    std::lock_guard<std::mutex> lk(m);
    q.push(std::move(v));
    cv.notify_one();
}

bool PathQueue::pop(T &out) {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [&]{ return closed || !q.empty(); });
    if (q.empty()) return false;
    out = std::move(q.front());
    q.pop();
    return true;
}

void PathQueue::close() {
    std::lock_guard<std::mutex> lk(m);
    closed = true;
    cv.notify_all();
}