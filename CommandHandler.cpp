#include "CommandHandler.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include <cstring>
#include <stdlib.h>

vector<string> CommandHandler::parseCommand(const string& command) {
    vector<string> args;
    istringstream iss(command);
    string token;
    
    while (iss >> token) {
        args.push_back(token);
    }
    
    return args;
}

void CommandHandler::execute(const string& command) {
    vector<string> args = parseCommand(command);
    
    if (args.empty()) return;
    
    bool inPipe = false;
    bool inBackground = false;
    
    vector<string> firstCommand, secondCommand;
    
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "|") {
            inPipe = true;
            continue;
        }
        
        if (inPipe)
            secondCommand.push_back(args[i]);
        else
            firstCommand.push_back(args[i]);
    }
    
    if (!firstCommand.empty() && firstCommand.back() == "&") {
        inBackground = true;
        firstCommand.pop_back();
    }
    
    if (inPipe && !secondCommand.empty()) {
        executePipe(firstCommand, secondCommand);
    } else if (!firstCommand.empty()) {
        executeCommand(firstCommand, inBackground);
    }
}

void CommandHandler::executeCommand(vector<string>& args, bool inBackground) {
    for (auto& arg : args) {
        if (arg.length() > 1 && arg[0] == '$') {
            string varName = arg.substr(1);
            char* envValue = getenv(varName.c_str());
            if (envValue != nullptr) {
                arg = envValue;
            } else {
                arg = ""; 
            }
        }
    }
    
    vector<char*> c_args;
    for (auto& arg : args) c_args.push_back(&arg[0]);
    c_args.push_back(nullptr);
    
    pid_t pid = fork();
    if (pid == 0) {
        execvp(c_args[0], c_args.data());
        
        cerr << "Error executing command '" << args[0] << "': " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        cerr << "Fork failed: " << strerror(errno) << endl;
    } else {
        if (!inBackground) {
            waitpid(pid, nullptr, 0);
        } else {
            cout << "Process running in background [" << pid << "]\n";
        }
    }
}

void CommandHandler::executePipe(vector<string>& firstCommand, vector<string>& secondCommand) {
    for (auto& arg : firstCommand) {
        if (arg.length() > 1 && arg[0] == '$') {
            string varName = arg.substr(1);
            char* envValue = getenv(varName.c_str());
            if (envValue != nullptr) {
                arg = envValue;
            } else {
                arg = "";
            }
        }
    }
    
    for (auto& arg : secondCommand) {
        if (arg.length() > 1 && arg[0] == '$') {
            string varName = arg.substr(1);
            char* envValue = getenv(varName.c_str());
            if (envValue != nullptr) {
                arg = envValue;
            } else {
                arg = "";
            }
        }
    }
    
    int pipeHandlers[2];
    pipe(pipeHandlers);
    
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(pipeHandlers[0]);
        dup2(pipeHandlers[1], STDOUT_FILENO);
        close(pipeHandlers[1]);
        
        vector<char*> c_args;
        for (auto& arg : firstCommand) c_args.push_back(&arg[0]);
        c_args.push_back(nullptr);
        
        execvp(c_args[0], c_args.data());
        cerr << "Error executing first command: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipeHandlers[1]);
        dup2(pipeHandlers[0], STDIN_FILENO);
        close(pipeHandlers[0]);
        
        vector<char*> c_args;
        for (auto& arg : secondCommand) c_args.push_back(&arg[0]);
        c_args.push_back(nullptr);
        
        execvp(c_args[0], c_args.data());
        cerr << "Error executing second command: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    
    close(pipeHandlers[0]);
    close(pipeHandlers[1]);
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);
}