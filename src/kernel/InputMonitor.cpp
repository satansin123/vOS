#include "InputMonitor.h"
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
        cout<<"> ";
        getline(cin, command);
        if (command == "exit") {
            shutdownRequested = true;
            break;
        }
        if (!command.empty()) {
            cout<<"This command is not applicable right now"<<endl;
        }
    }
}

