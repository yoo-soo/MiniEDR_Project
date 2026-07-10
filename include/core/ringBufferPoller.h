#pragma once
#include <bpf/libbpf.h>
#include <atomic>
#include <thread>
#include <functional>

class RingBufferPoller {
public:
    using rawCallback = std::function<int(void* data, size_t size)>;

    bool start(int mapFd, rawCallback cb) {
        m_callback = std::move(cb);
        m_ringBuffer = ring_buffer__new(mapFd, &RingBufferPoller::trampoline, this, nullptr);
        
        if (!m_ringBuffer) {
            return false;
        }
        m_running = true;
        m_thread = std::thread([this] {
            while (m_running) {
                ring_buffer__poll(m_ringBuffer, 100 /* timeout in ms */);
            }
        });
        return true;
    }

    void stop() {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
        if (m_ringBuffer) {
            ring_buffer__free(m_ringBuffer);
            m_ringBuffer = nullptr;
        }
    }

private:
    static int trampoline(void* ctx, void* data, size_t size) {
        return static_cast<RingBufferPoller*>(ctx)->m_callback(data, size);
    }

    ring_buffer* m_ringBuffer = nullptr;
    rawCallback m_callback;
    std::atomic<bool> m_running{false};
    std::thread m_thread;
};