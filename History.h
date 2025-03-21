#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <string>

using namespace std;

class History {
private:
    vector<string> commands;
    int count = 1;
public:
    void addCommand(const string& command);
    void showHistory() const;
};

#endif
