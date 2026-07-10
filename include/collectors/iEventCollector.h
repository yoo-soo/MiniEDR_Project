#pragma once
#include <functional>
#include <memory>
#include "core/event.h"

/**
 * @brief Interface for event collectors.
 *
 * process/file/network collector 가 공통으로 구현할 interface 를 정의합니다.
 * detectionengine 등 상위 계층은 이 인터페이스만 알면 되고, 내부가 eBPF 기반인지, 다른 방식인지 알 필요가 없습니다.
 */

class IEventCollector {
public:
    using EventCallback = std::function<void(std::shared_ptr<Event>)>;

    virtual ~IEventCollector() = default;
    virtual bool start(EventCallback callback) = 0;
    virtual void stop() = 0;
    virtual std::string name() const = 0;
};