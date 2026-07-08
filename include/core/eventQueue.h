#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "core/event.h"

class EventQueue {
public:
    void push(std::shared_ptr<Event> event) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(event));
        }
        m_cv.notify_one();
    }

    // timeout_ms 동안 대기, 이벤트 없으면 nullptr 반환
    std::shared_ptr<Event> pop(int timeout_ms = 200) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                            [this] { return !m_queue.empty(); })) {
            return nullptr;
        }
        auto event = m_queue.front();
        m_queue.pop();
        return event;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

private:
    std::queue<std::shared_ptr<Event>> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
};
