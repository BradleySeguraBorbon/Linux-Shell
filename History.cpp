#include "History.h"
#include <iostream>

using namespace std;

void History::addCommand(const string& command) {
    if (commands.size() == 10)
        commands.erase(commands.begin()); 

    commands.push_back(Command(count++, command));
    onShown = 0; 
}

void History::showHistory() const {
    for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
        cout << it->index<< " " << it->command << endl; 
    }
}

string History::getCommand(const string& direction) {
    if (commands.empty()) return "";

    if (direction == "up") {
        if (onShown < commands.size()) {
            onShown++; 
        }
    } else if (direction == "down") {
        if (onShown > 1) {
            onShown--; 
        } else {
            onShown = 0; 
            return "";
        }
    }

    return (onShown > 0 && onShown <= commands.size()) ? commands[commands.size() - onShown].command : "";
}

bool History::isBetweenCommandsRange(int number) const {
    return number >= commands[0].index && number <= commands.back().index;
}

string History::getNCommand(int number) const {
    if (!isBetweenCommandsRange(number)) {
        cout << "El numero de comando indicado no se encuentra en el historial" << endl;
        return "";
    }
    for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
        if (number == it->index) return it->command;
    }
    cout <<"Comando no encontrado" << endl;
    return "";
}
