#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <fstream>

#include "../include/path_queue.h"  

namespace fs = std::filesystem;

TEST(PathQueue, PushPopAndClose) {
    PathQueue q;

    fs::path a = "/tmp/a", b = "/tmp/b", c = "/tmp/c";

    std::thread prod([&]{
        q.push(a);
        q.push(b);
        q.push(c);
        q.close();
    });

    std::vector<fs::path> got;
    fs::path p;
    while (q.pop(p)) {
        got.push_back(p);
    }

    prod.join();

    ASSERT_EQ(got.size(), 3u);
    EXPECT_EQ(got[0], a);
    EXPECT_EQ(got[1], b);
    EXPECT_EQ(got[2], c);
}

TEST(PathQueue, PopBlocksUntilNotifyOrClose) {
    PathQueue q;
    std::atomic<bool> thread_started{false};
    std::atomic<bool> popped_false{false};

    std::thread t([&]{
        thread_started = true;
        fs::path p;
        bool ok = q.pop(p);
        popped_false = !ok;
    });

    while (!thread_started.load()) std::this_thread::yield();

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    q.close();

    t.join();

    EXPECT_TRUE(popped_false.load());
}

TEST(PathQueue, PushAfterClose) {
    PathQueue q;
    q.close();
    fs::path p = "/tmp/x";
    
    q.push(p);
    fs::path out;
    EXPECT_FALSE(q.pop(out));
}

TEST(PathQueue, PopFromClosedEmpty) {
    PathQueue q;
    q.close();
    fs::path out;
    EXPECT_FALSE(q.pop(out));
}

TEST(PathQueue, PopBlocksUntilPush) {
    PathQueue q;
    std::atomic<bool> popped{false};
    fs::path expected = "/tmp/zzz";

    std::thread consumer([&]{
        fs::path out;
        bool ok = q.pop(out);
        if (ok && out == expected) popped = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    q.push(expected);
    q.close();

    consumer.join();
    EXPECT_TRUE(popped.load());
}
