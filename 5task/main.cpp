#include <iostream>
#include <thread>
#include "log.h"

int main() {
    // Get singleton instance
    Log *log = Log::Instance();
    
    // Test basic functionality
    log->message(LOG_NORMAL, "program loaded");
    log->message(LOG_NORMAL, "initializing components");
    log->message(LOG_WARNING, "configuration file not found, using defaults");
    log->message(LOG_NORMAL, "connecting to database");
    log->message(LOG_ERROR, "error happens! help me!");
    
    // Print current events
    log->print();
    
    // Test with more than 10 events to verify circular buffer
    std::cout << "Adding 10 more events to test circular buffer..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        log->message(LOG_NORMAL, "test message " + std::to_string(i + 1));
    }
    
    // Print again - should show last 10 events
    log->print();
    
    // Test thread safety
    std::cout << "Testing thread safety..." << std::endl;
    auto threadFunc = [](int id) {
        Log::Instance()->message(LOG_NORMAL, 
            "thread " + std::to_string(id) + " started");
    };
    
    std::thread t1(threadFunc, 1);
    std::thread t2(threadFunc, 2);
    std::thread t3(threadFunc, 3);
    
    t1.join();
    t2.join();
    t3.join();
    
    // Final print
    log->print();
    
    return 0;
}