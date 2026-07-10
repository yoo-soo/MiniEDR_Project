#pragma once
#include "collectors/iEventCollector.h"
#include "core/ringBufferPoller.h"

struct processExec_bpf;

class ProcessCollector : public IEventCollector {
public:
    ~ProcessCollector() override;

    bool start(EventCallback callback) override;
    void stop() override;
    std::string name() const override { return "ProcessCollector(eBPF)"; }

private:
    int handleRingBufferEvent(void* data, size_t size);
    
    processExec_bpf* m_skeleton = nullptr;
    RingBufferPoller m_poller;
    EventCallback m_callback;
};
