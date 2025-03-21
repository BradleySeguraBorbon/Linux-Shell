#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <unordered_map>
#include "CommandHandler.h"
#include "History.h"

using namespace std;

class Shell {
private:
    CommandHandler commandHandler;
    History history;
    string currentDirectory;
    unordered_map<string, string> environment;

    void printPrompt();
    void loadEnvironment();
    bool executeBuiltInCommand(const string& command);
    bool changeDirectory(const string& path);
    void updateCurrentDirectory();
    void syncEnvironmentWithProcess();

public:
    Shell();
    void run();
    string readCommand();
    const unordered_map<string, string>& getEnvironment() const;
    const string& getCurrentDirectory() const;
    bool setEnvironmentVariable(const string& name, const string& value);
};

#endif