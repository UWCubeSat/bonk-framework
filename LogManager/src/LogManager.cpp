// Copyright Eli Reed, 2020 released under GPLv3

#include "LogManager.h"

bool BONK::LogManager::begin(const char* log_path, const char* data_path) {
    if (log_path == nullptr || data_path == nullptr) {
        return false;
    }
    log_path_ = log_path;
    data_path_ = data_path;
    if (!data_file_.open(data_path_, O_WRITE | O_APPEND) ||
        !log_file_.open(log_path_, O_WRITE | O_APPEND))
        return false;
    }
    return true;
}

size_t BONK::LogManager::log(BONK::LogType log_type, const String& msg) {
    return BONK::LogManager::log(log_type, msg.c_str(), msg.length());
}

size_t BONK::LogManager::log(BONK::LogType log_type, const char* msg) {
    if (msg == nullptr) return 0;
    return BONK::LogManager::log(log_type, (const uint8_t *)msg, strlen(msg));
}

size_t BONK::LogManager::log(BONK::LogType log_type, const char msg[]) {
    return BONK::LogManager::log(log_type, msg);
}

size_t BONK::LogManager::log(BONK::LogType log_type, uint8_t byte) {
    SdFile* target_file = nullptr;
    String tag;
    switch(log_type) {
        case BONK::LogType.DEBUG:
            break;
        case BONK::LogType.DATA_DEBUG:
            break;
        case BONK::LogType.DATA:
            break;
        case BONK::LogType.WARNING:
            break;
        case BONK::LogType.ERROR:
            break;
        case BONK::LogType.NOTIFY:
        case default:
            break;
    }
}