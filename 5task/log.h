#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <mutex>
#include <iostream>

// Event importance levels
enum LogLevel {
    LOG_NORMAL = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2
};

// Structure to hold a single log event
struct LogEvent {
    std::string timestamp;
    LogLevel level;
    std::string message;
    
    LogEvent(LogLevel l, const std::string& msg) 
        : level(l), message(msg) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        timestamp = std::ctime(&now_time);
        // Remove trailing newline from ctime
        if (!timestamp.empty() && timestamp.back() == '\n') {
            timestamp.pop_back();
        }
    }
};

class Log {
private:
    // Maximum number of events to store
    static constexpr size_t MAX_EVENTS = 10;
    
    // Private constructor for Singleton pattern
    Log() = default;
    
    // Delete copy constructor and assignment operator
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    
    // Storage for last events
    std::vector<LogEvent> events;
    
    // Mutex for thread safety
    mutable std::mutex mutex;
    
    // Convert log level to string
    std::string levelToString(LogLevel level) const {
        switch(level) {
            case LOG_NORMAL:  return "NORMAL";
            case LOG_WARNING: return "WARNING";
            case LOG_ERROR:   return "ERROR";
            default:          return "UNKNOWN";
        }
    }

public:
    // Get the singleton instance
    static Log* Instance() {
        static Log instance;
        return &instance;
    }
    
    // Add a message to the log
    void message(LogLevel level, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Add new event
        events.emplace_back(level, msg);
        
        // Keep only last MAX_EVENTS events
        if (events.size() > MAX_EVENTS) {
            events.erase(events.begin());
        }
    }
    
    // Print last events
    void print() const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::cout << "\n Last " << events.size() << " Events" << std::endl;
        if (events.empty()) {
            std::cout << "No events recorded." << std::endl;
        } else {
            for (size_t i = 0; i < events.size(); ++i) {
                const auto& event = events[i];
                std::cout << "[" << i + 1 << "] "
                         << event.timestamp << " | "
                         << levelToString(event.level) << " | "
                         << event.message << std::endl;
            }
        }
    }
    
    // Clear all events
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        events.clear();
    }
    
    // Get number of stored events
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return events.size();
    }
};

#endif // LOG_H