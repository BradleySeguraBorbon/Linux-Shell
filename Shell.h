#ifndef SHELL_H
#define SHELL_H

class Shell {
private:
    CommandHandler commandHandler;
    History history;
    void printPrompt();
public:
    void run();
};

#endif
