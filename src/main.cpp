#include "core/logger.h"
#include "core/configManager.h"
#include "core/eventQueue.h"

int main() {
    Logger::init("miniedr.log");
    Logger::info("MiniEDR core 인프라 테스트 시작");

    ConfigManager config;
    if (config.load("config/config.json")) {
        Logger::info("config 로드 성공, dry_run={}", config.dryRun());
    } else {
        Logger::warn("config 로드 실패, 기본값 사용");
    }

    EventQueue queue;
    Logger::info("EventQueue 생성 완료, 현재 크기={}", queue.size());

    return 0;
}
