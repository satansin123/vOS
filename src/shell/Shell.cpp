#include "Shell.h"
#include <algorithm>
#include <cctype>
#include <iterator>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include<iostream>
using namespace std;

Shell::Shell(){
    registerCommand("help", "shwo this help message", [this](const vector<string>& args){
        printHelp(args);
    });
    registerCommand("exit", "Shutdown sys", [](const vector<string>& ){
        cout<<"Exiting shell...\n";
        exit(0);
    });
}
Shell::~Shell(){}

void Shell::registerCommand(const string& name, const string& help, CommandHandler handle){
    lock_guard<mutex> lock(shellMutex);
    command[name] = make_pair(help, handle);
}

void Shell::run(){
    string line;
    while (true) {
        printPrompt();
        if (!getline(cin, line)) {
            cout<<"\n";
            break;
        }
        handleInput(line);
    }
}

void Shell::printPrompt(){
    cout<<"vOS> "<<flush;
}

void Shell::handleInput(const string& input){
    string trimmed = input;
    trimmed.erase(trimmed.begin(), find_if(trimmed.begin(), trimmed.end(), [](int ch){return !isspace(ch);}));
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](int ch) { return !std::isspace(ch); }).base(), trimmed.end());

    if (trimmed.empty()) {
        return;
    }

    auto args = parseArguments(trimmed);
    if (args.empty()) {
        return;
    }

    string cmd = args[0];
    transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

    lock_guard<mutex> lock(shellMutex);

    auto it = command.find(cmd);
    if (it!= command.end()) {
        it->second.second(args);
    }
    else{
        printUnknownCommand(cmd);
    }
}

vector<string> Shell::parseArguments(const string& line){
    vector<string> args;
    istringstream iss(line);
    string arg;
    bool inQuotes = false;
    string current;

    while (iss) {
        char c = iss.get();
        if (!iss) {
            break;
        }
        if (c=='"') {
            inQuotes = !inQuotes;
            continue;
        }
        if (isspace(c) && !inQuotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        }
        else{
            current+=c;
        }
    }
    if (!current.empty()) {
        args.push_back(current);
    }
    return args;
}

void Shell::printUnknownCommand(const std::string& cmd) {
    std::cout << "Error: Unknown command '" << cmd << "'\n";
}

void Shell::printHelp(const std::vector<std::string>&) {
    std::cout << "Available commands:\n";
    for (const auto& entry : command) {
        std::cout << "  " << entry.first << " - " << entry.second.first << "\n";
    }
}