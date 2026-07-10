#include "core/logger.h"
#include "core/configManager.h"
#include "core/eventQueue.h"
#include "collectors/processCollector.h"
#include <csignal>

static volatile bool g_running = true;
void handleSignal(int) { g_running = false; }

int main() {
    Logger::init("miniedr.log");
    Logger::info("MiniEDR core 인프라 테스트 시작");

    ConfigManager config;
    config.load("config/config.json");
    
    EventQueue queue;
    ProcessCollector processCollector;

    signal(SIGINT, handleSignal);
    
    bool started = processCollector.start([&queue](std::shared_ptr<Event> event) {
        queue.push(event);
    });

    if (!started) {
        Logger::error("ProcessCollector start failed (root 권한 필요)");
        return 1;
    }

    Logger::info("ProcessCollector 시작 완료, 이벤트 수집 중... (종료하려면 Ctrl+C)");

    while(g_running) {
        auto event = queue.pop(200); // 200ms 동안 대기 후 이벤트가 없으면 nullptr 반환

        if (event) {
            auto pe = std::dynamic_pointer_cast<ProcessEvent>(event);
            Logger::info("[EXEC] pid={}, ppid={}, exePath={}, cmdline={}, uid={}, isExit={}",
                         pe->pid, pe->ppid, pe->exePath, pe->cmdline, pe->uid, pe->isExit);    
        }
    }

    processCollector.stop();
    Logger::info("종료");

    return 0;
}
