#include "Shell.h"
#include <iostream>

using namespace std;

void Shell::printPrompt() {
    cout << "tiger> ";
}

void Shell::printPrompt() {
    string command;
    while (true) {
        printPrompt();
        getline(cin, command);
        if ( command == "exit" )
            break;
        history.addCommand(command);
        commandHandler.execute();
    }
}
