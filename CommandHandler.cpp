#include "CommandHandler.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <fcntl.h>

void CommandHandler::execute(const string& command) {
    vector<string> args;
    istringstream inputStream(command);
    string w;

    bool inPipe = false;
    bool inBackground = false;

    vector<string> firstCommand, secondCommand;

    while (inputStream >> w) {
        if (w == "|") {
            inPipe = true;
            continue;
        }
        if (inPipe)
            secondCommand.push_back(w);
        else
            firstCommand.push_back(w);

        if (firstCommand.empty()) return;

        if (!inPipe && firstCommand.back() == "&") {
            inBackground = true;
            firstCommand.pop_back();
        }

        if (inPipe) {
            executePipe(firstCommand, secondCommand);
        } else {
            executeCommand(firstCommand, inBackground);
        }
    }
}

void CommandHandler::executeCommand(vector<string>& args, bool inBackground) {
    vector<char*> c_args;
    for (auto& arg : args) c_args.push_back(&arg[0]);
    c_args.push_back(nullptr);

    pid_t pid = fork();
    if (pid == 0) {
        execvp(c_args[0], c_args.data());
        cerr << "Error ejecutando comando\n";
        exit(EXIT_FAILURE);
    } else {
        if (!inBackground) {
            wait(nullptr);
        } else {
            cout << "Proceso ejecutándose en segundo plano [" << pid << "]\n";
        }
    }
}

void CommandHandler::executePipe(vector<string>& firstCommand, vector<string>& secondCommand) {
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
        std::cerr << "Error ejecutando primer comando\n";
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
        cerr << "Error ejecutando segundo comando\n";
        exit(EXIT_FAILURE);
    }

    close(pipeHandlers[0]);
    close(pipeHandlers[1]);
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);
}



