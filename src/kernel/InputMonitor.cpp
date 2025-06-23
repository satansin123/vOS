#include "InputMonitor.h"
#include"Kernel.h"
#include "Logger.h"
#include <chrono>
#include<string>
#include<iostream>
#include <thread>

using namespace std;

InputMonitor::InputMonitor() : shutdownRequested(false), monitoring(false){}
InputMonitor::~InputMonitor(){
    stopMonitoring();
}

void InputMonitor::stopMonitoring(){
    if (monitoring) {
        monitoring = false;
        if (inputThread.joinable()) {
            inputThread.join();
        }
    }
}

bool InputMonitor::isShutdownRequested() const {
    return shutdownRequested;
}
void InputMonitor::startMonitoring(){
    if (!monitoring) {
        monitoring = true;
        inputThread = thread(&InputMonitor::inputLoop, this);
    }
}

void InputMonitor::inputLoop(){
    string command;
    while (monitoring) {
        Kernel::getInstance().getLogger().log(MessageType::PROMPT, "");
        if (cin.eof() || !cin.good() ) {
            Kernel::getInstance().getLogger().log(MessageType::INFO, 
                "Non-interactive environment detected. Running for 10 seconds...");
            this_thread::sleep_for(chrono::seconds(10));
            shutdownRequested =true;
            break;
        }
        getline(cin, command);
        if (command == "exit") {
            shutdownRequested = true;
            break;
        }
        if (!command.empty()) {
            Kernel::getInstance().getLogger().log(MessageType::USER_FEEDBACK, 
                "Unknown command '" + command + "'. Type 'exit' to shutdown");
        }
    }
}

