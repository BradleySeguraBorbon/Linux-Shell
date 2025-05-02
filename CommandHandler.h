#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <string>
#include <vector>

using namespace std;

class CommandHandler {
private:
    void executeCommand(vector<string>& args, bool inBackground);
    void executePipe(vector<string>& firstCommand, vector<string>& secondCommand);
    vector<string> parseCommand(const string& command);
public:
    void execute(const string& command);
};

#endif 
