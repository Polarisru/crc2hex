#pragma once

#include <iostream>
#include <string>

enum log_level {
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_ALL
};

//class Log {
//public:
//  Log(logLevel level) : level_(level) {}
//  void print(LogLevel lvl, const string& msg) {
//    if (lvl < level_)
//      return;
//    switch (lvl) {
//      case LogLevel::LOG_LEVEL_INFO:
//        cout << "INFO: ";
//        break;
//      case LogLevel::LOG_LEVEL_WARNING:
//        cout << "WARNING: ";
//        break;
//      case LogLevel::LOG_LEVEL_ERROR:
//        cout << "ERROR: ";
//        break;
//      case LogLevel::LOG_LEVEL_LAST:
//        break;
//    }
//    cout << msg << endl;
//  }
//private:
//  LogLevel  level_;
//};

class LOG {
public:
    LOG() {}
    LOG(log_level type) {
        msglevel = type;
        if (msglevel != LOG_LEVEL_ALL) {
            operator << "[" << getLabel(type) << "]";
        }
    }
    ~LOG() {
        if(opened) {
            cout << std::endl;
        }
        opened = false;
    }
    template<class T>
    LOG &operator<<(const T &msg) {
        if (msglevel >= LOG_LEVEL_INFO) {
            cout << msg;
            opened = true;
        }
        return *this;
    }
    static int level;
private:
    bool opened = false;
    log_level msglevel = LOG_LEVEL_INFO;
    std::string getLabel(log_level type) const {
        std::string label;
        switch(type) {
            case LOG_LEVEL_INFO:
              return "INFO";
            case LOG_LEVEL_WARNING:
              return "WARN";
            case LOG_LEVEL_ERROR:
              return "ERROR";
            case LOG_LEVEL_ALL:
            default:
              return "";
        }
    }
};
