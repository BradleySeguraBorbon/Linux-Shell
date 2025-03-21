#include "History.h"
#include <iostream>

using namespace std;

void History::addCommand(const string& command) {
    if (commands.size() == 10)
        commands.erase(commands.begin());  // Eliminar el comando más antiguo

    commands.push_back(Command(count++, command));
    onShown = commands.size() + 1;  // Iniciar desde el más reciente
}

void History::showHistory() const {
    for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
        cout << it->command << endl;  // Mostrar en orden inverso
    }
}

string History::getCommand(const string& direction) {
    if (commands.empty()) return "";

    if (direction == "up") {
        if (onShown > 0) {
            onShown--;
        }
    } else if (direction == "down") {
        if (onShown < commands.size()) {
            onShown++;
        }else {
            onShown = commands.size();
            return "";
        }
    }

    return (onShown > 0 && onShown <= commands.size()) ? commands[onShown - 1].command : "";
}
