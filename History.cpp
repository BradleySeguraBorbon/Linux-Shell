#include "History.h"
#include <iostream>

using namespace std;

void History::addCommand(const string& command) {
    if (commands.size() == 10) commands.erase(commands.begin());
    commands.push_back(to_string(count++) + " " + command);
}

void History::showHistory() const {
    for (const auto& command : commands)
        cout << command << endl;
}
