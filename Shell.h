#ifndef SHELL_H
#define SHELL_H

#include <string>
#include "CommandHandler.h"
#include "History.h"

class Shell {
private:
    CommandHandler commandHandler;
    History history;
    void printPrompt();
public:
    void run();
};

#endif
