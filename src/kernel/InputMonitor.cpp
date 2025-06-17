#include "InputMonitor.h"
#include"Kernel.h"
#include "Logger.h"
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

