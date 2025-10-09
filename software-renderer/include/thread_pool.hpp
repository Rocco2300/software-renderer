#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <condition_variable>

namespace sfr::renderer {

class thread_pool {
private:
    int m_total{};
    bool m_processing{};
    bool m_shouldTerminate{};

    std::atomic<int> m_counter = 0;
    std::mutex m_queueMutex;
    std::mutex m_processingMutex;
    std::condition_variable m_queueCondition;
    std::condition_variable m_processingCondition;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_jobs;

public:
    thread_pool(unsigned int count);
    ~thread_pool();

    void barrier(int count);
    void process(const std::function<void()>& job);

private:
    void threadLoop();
};

}