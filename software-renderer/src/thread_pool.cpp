#include "thread_pool.hpp"

#include <algorithm>

namespace sfr::renderer {

thread_pool::thread_pool(unsigned int count) {
    auto threadCount = std::min(std::thread::hardware_concurrency(), count / 2);
    for (int i = 0; i < threadCount; i++) {
        m_threads.emplace_back(&thread_pool::threadLoop, this);
    }
}

thread_pool::~thread_pool() {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_shouldTerminate = true;
    }

    m_queueCondition.notify_all();
    m_processingCondition.notify_one();

    for (auto& thread : m_threads) {
        thread.join();
    }

    m_threads.clear();
}

void thread_pool::barrier(int count) {
    {
        std::unique_lock<std::mutex> lock(m_processingMutex);
        m_processingCondition.wait(lock, [&] {return !m_processing || m_shouldTerminate; });
    }

    m_total = count;
    m_processing = true;
}

void thread_pool::process(const std::function<void()>& job) {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_jobs.push(job);
        //m_jobs.emplace([&] {
        //    job();

        //    m_counter++;
        //    if (m_counter == m_total) {
        //        m_processing = false;
        //        m_processingCondition.notify_one();
        //    }
        //});

    }

    m_queueCondition.notify_one();
}

void thread_pool::threadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCondition.wait(lock, [this] {
                return !m_jobs.empty() || m_shouldTerminate;
            });

            if (m_shouldTerminate && m_jobs.empty()) {
                return;
            }

            job = m_jobs.front();
            m_jobs.pop();
        }
        job();
    }
}

}