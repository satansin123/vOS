#pragma once
#include <functional>
#include<string>
#include<vector>
#include<mutex>
#include<map>
using namespace std;

class Shell{
    public:
        using CommandHandler = function<void(const vector<string>& args)>;
        Shell();
        ~Shell();
        void registerCommand(const string& name, const string& help, CommandHandler handler);
        void run();

    private:
        map<string, pair<string, CommandHandler>> command;
        mutex shellMutex;

        void printPrompt();
        void handleInput(const string& input);
        vector<string> parseArguments(const string& input);
        void printUnknownCommand(const string& cmd);
        void printHelp(const vector<string>& args);
};