// Copyright Eli Reed, 2020 released under GPLv3

#include <string.h>

#include "LogManager.h"

bool BONK::LogManager::begin(const char* log_path, const char* data_path) {
    if (log_path == nullptr) {
        return false;
    }
    log_path_ = log_path;
    return log_file_.open(log_path_, O_WRITE | O_APPEND);
}

// structure of following functions shamelessly stolen from Print.cpp

size_t BONK::LogManager::log(BONK::LogType level, const String& msg) {
    return BONK::LogManager::log(level, msg.c_str());
}

size_t BONK::LogManager::log(BONK::LogType level, const char* msg) {
    if (msg == nullptr) return 0;
    size_t bytes = BONK::LogManager::print_tag(level);
    bytes += log_file_.write(msg);
    bytes += log_file_.println();
    return bytes;
}

size_t BONK::LogManager::log(BONK::LogType level, const uint8_t* buf, size_t size) {
    size_t bytes = print_tag(level);
    switch (level) {
        case BONK::LogType::DEBUG:
            bytes += Serial.write(buf, size);
            bytes += Serial.println();
            break;
        case BONK::LogType::WARNING:
        case BONK::LogType::ERROR:
        case BONK::LogType::NOTIFY:
            bytes += log_file_.write(buf, size);
            bytes += log_file_.println();
            break;
    }
    return bytes;
}

size_t BONK::LogManager::print_tag(BONK::LogType level) {
    String msg;
    switch (level) {
        case BONK::LogType::DEBUG:
            return 0;
        case BONK::LogType::WARNING:
            msg = "[WARN] ";
            break;
        case BONK::LogType::ERROR:
            msg = "[ERR] ";
            break;
        case BONK::LogType::NOTIFY:
            msg = "[NOTIFY] ";
            break;
    }
    msg += millis() + ": ";
    return log_file_.print(msg.c_str());
}
