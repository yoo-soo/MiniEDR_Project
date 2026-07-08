#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

class ConfigManager {
public:
    bool load(const std::string& path) {
        std::ifstream in(path);
        if (!in.is_open()) return false;
        in >> m_json;
        return true;
    }

    bool dryRun() const { return m_json.value("dry_run", true); }
    std::string rulesPath() const { return m_json.value("rules_path", "rules/rules.json"); }
    std::string logPath() const { return m_json.value("log_path", "miniedr.log"); }

private:
    nlohmann::json m_json;
};
