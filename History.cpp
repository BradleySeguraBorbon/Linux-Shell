#include "History.h"
#include <iostream>

using namespace std;

void History::addCommand(const string& command) {
    if (commands.size() == 10)
        commands.erase(commands.begin());  // Eliminar el comando más antiguo

    commands.push_back(Command(count++, command));
    onShown = 0;  // Reiniciar la posición al no estar navegando en el historial
}

void History::showHistory() const {
    for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
        cout << it->index<< " " << it->command << endl;  // Mostrar en orden inverso
    }
}

string History::getCommand(const string& direction) {
    if (commands.empty()) return "";

    if (direction == "up") {
        if (onShown < commands.size()) {
            onShown++;  // Mover hacia atrás en el historial
        }
    } else if (direction == "down") {
        if (onShown > 1) {
            onShown--;  // Mover hacia adelante en el historial
        } else {
            onShown = 0;  // Restablecer a vacío si se llega al final
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
        cout << "Command number is not in history" << endl;
        return "";
    }
    for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
        if (number == it->index) return it->command;
    }
    cout <<"Command not found" << endl;
    return "";
}
