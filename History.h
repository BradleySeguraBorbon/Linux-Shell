#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <string>

using namespace std;

class History {
private:
    struct Command {
        int index;
        string command;
        Command(int index, string command) : index(index), command(command) {}
    };
    vector<Command> commands;
    int count = 1;
    int onShown = 0; //0 means no command, from 1 to 10;
public:
    void addCommand(const string& command);
    void showHistory() const;
    string getCommand(const string& direction);
    int getOnShown() const {return onShown;}
    void setOnShown(int onShown) {this->onShown = onShown;}
    vector<Command> getHistory() const {return commands;}
    bool isBetweenCommandsRange(int number) const;
    string getNCommand(int number) const;
};

#endif
