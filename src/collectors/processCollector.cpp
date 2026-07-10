    #include "collectors/processCollector.h"
    #include "core/logger.h"
    #include "processExec.skel.h"
    #include "../../bpf/processExec.h"

    ProcessCollector::~ProcessCollector() {
        stop();
    }

    bool ProcessCollector::start(EventCallback callback) {
        m_callback = std::move(callback);

        m_skeleton = processExec_bpf__open();
        if (!m_skeleton) {
            Logger::error("Failed to open BPF skeleton");
            return false;
        }
        
        if (processExec_bpf__load(m_skeleton)) {
            Logger::error("Failed to load BPF program(check dmesg for details)");
            processExec_bpf__destroy(m_skeleton);
            m_skeleton = nullptr;
            return false;
        }

        if (processExec_bpf__attach(m_skeleton)) {
            Logger::error("Failed to attach BPF program(check dmesg for details)");
            processExec_bpf__destroy(m_skeleton);
            m_skeleton = nullptr;
            return false;
        }

        int mapFd = bpf_map__fd(m_skeleton->maps.events);

        bool ok = m_poller.start(mapFd, [this](void* data, size_t size) {
            return handleRingBufferEvent(data, size);
        });
        if (ok) Logger::info("ProcessCollector: eBPF exec hook attach 완료");
        return ok;
    }

    int ProcessCollector::handleRingBufferEvent(void* data, size_t size) {
        if (size < sizeof(exec_event)) return 0;   // 방어적 체크: 예상보다 작은 데이터는 무시

        auto* raw = static_cast<exec_event*>(data);

        // eBPF의 exec_event(POD struct) → 우리 파이프라인의 ProcessEvent(공통 인터페이스)로 변환.
        // 이 변환 지점이 "eBPF 세부사항"과 "상위 파이프라인"의 경계선이다.
        auto event = std::make_shared<ProcessEvent>();
        event->type      = EventType::Process;
        event->timestamp = std::chrono::system_clock::now();
        event->pid       = raw->pid;
        event->ppid      = raw->ppid;
        event->uid       = raw->uid;
        event->exePath   = raw->filename;
        event->cmdline   = raw->comm;   // 인자 없는 comm만 있음. 필요시 /proc/[pid]/cmdline으로 보강 가능
        event->isExit    = false;

        if (m_callback) m_callback(event);
        return 0;
    }

    void ProcessCollector::stop() {
        m_poller.stop();
        if (m_skeleton) {
            processExec_bpf__destroy(m_skeleton);
            m_skeleton = nullptr;
        }
    }