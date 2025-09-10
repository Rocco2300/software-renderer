#include <iostream>
#include <thread>
#include <functional>
#include <condition_variable>
#include <queue>
#include <vector>
#include <numeric>
#include <span>

// Applies a function over a vector of elements
class SingleApplier {
private:
    std::function<void(int&)> m_func;

public:
    SingleApplier(const std::function<void(int&)>& func) {
        m_func = func;
    }

    void process(std::vector<int>& data) {
        for (auto& el : data) {
            m_func(el);
        }
    }
};

class MultiApplier {
private:
    std::function<void(int&)> m_func;

    bool m_processing = false;
    bool m_shouldTerminate = false;
    int m_total = 0;
    std::atomic<int> m_counter = 0;
    std::mutex m_queueMutex;
    std::mutex m_processingMutex;
    std::condition_variable m_mutexCondition;
    std::condition_variable m_processingCondition;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_jobs;

public:
    MultiApplier(const std::function<void(int&)>& func) {
        m_func = func;

        auto threadNo = std::thread::hardware_concurrency();
        for (int i = 0; i < threadNo; i++) {
            m_threads.emplace_back(&MultiApplier::threadLoop, this);
        }
    }

    ~MultiApplier() {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_shouldTerminate = true;
        }

        m_mutexCondition.notify_all();

        for (auto& thread : m_threads) {
            thread.join();
        }

        m_threads.clear();
    }

    void process(std::vector<int>& data) {
        {
            std::unique_lock<std::mutex> lock(m_processingMutex);
            m_processingCondition.wait(lock, [&]{ return !m_processing; });
        }

        m_processing = true;
        m_total = data.size();
        m_counter = 0;

        for (auto& el : data) {
            queueJob([&] () {
                m_func(el);

                m_counter++;
                if (m_counter == m_total) {
                    m_processing = false;
                    m_processingCondition.notify_one();
                }
            });
        }
    }

private:
    void queueJob(const std::function<void()>& job) {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_jobs.push(job);
        }

        m_mutexCondition.notify_one();
    }

    void threadLoop() {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_mutexCondition.wait(lock, [this] {
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
};

void printData(const std::vector<int>& data) {
    for (const auto& el : data) {
        std::cout << el << ' ';
    }
    std::cout << '\n';
}

int main() {
    std::vector<int> testData1{1, 2, 3, 4, 5};
    std::vector<int> testData2{2, 3, 4, 5, 6};
    std::vector<int> testData3{3, 4, 5, 6, 7};

    printData(testData1);
    printData(testData2);
    printData(testData3);

    std::vector<int> bufferData1(1000000);
    std::vector<int> bufferData2(1000000);
    std::iota(bufferData1.begin(), bufferData1.end(), 0);
    std::iota(bufferData2.begin(), bufferData2.end(), 0);

    auto testFunction = [](int& el) {
        el += 2;
    };

    //SingleApplier applier(testFunction);
    {
        MultiApplier applier(testFunction);

        applier.process(testData1);
        applier.process(bufferData1);
        applier.process(testData2);
        applier.process(bufferData2);
        applier.process(testData3);
    }

    printData(testData1);
    printData(testData2);
    printData(testData3);

    std::span<int> s(testData1.begin() + 1, 3);
    for (const auto& el : s) {
        std::cout << el << '\n';
    }

    return 0;
}