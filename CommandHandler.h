//
// Created by Bradley on 3/20/2025.
//

#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <string>
#include <vector>

using namespace std;

class CommandHandler {
private:
    void executeCommand(vector<string>& args, bool inBackground);
    void executePipe(vector<string>& firstCommand, vector<string>& secondCommand);
public:
    void execute(const string& command);
};

#endif //COMMANDHANDLER_H
