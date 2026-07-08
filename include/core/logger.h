#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

class Logger {
public:
    static void init(const std::string& logFilePath = "miniedr.log") {
        auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file    = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath, true);
        std::vector<spdlog::sink_ptr> sinks{console, file};

        auto logger = std::make_shared<spdlog::logger>("miniedr", sinks.begin(), sinks.end());
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::info);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
    }

    template<typename... Args>
    static void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        spdlog::info(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void warn(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        spdlog::warn(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        spdlog::error(fmt, std::forward<Args>(args)...);
    }
};
