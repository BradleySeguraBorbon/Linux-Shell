#include "Shell.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sstream>
#include <vector>
#include <cstring>
#include <stdlib.h>

using namespace std;

void setRawMode(bool enable) {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    if (enable) {
        t.c_lflag &= ~(ICANON | ECHO);  // Modo sin buffer y sin eco
    } else {
        t.c_lflag |= (ICANON | ECHO);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Función para capturar texto y flechas
string Shell::readCommand() {
    setRawMode(true);  // Activar modo sin buffer
    string command;

    cout << "tiger> ";

    while (true) {
        char ch = getchar();

        if (ch == 27) {  // Tecla ESC (posible flecha)
            if (getchar() == 91) {  // Verificar que es una secuencia ANSI
                ch = getchar();
                if (ch == 'A') {  // Flecha Arriba
                    string historyCommand = history.getCommand("up");
                    if (!historyCommand.empty()) {
                        command = historyCommand;
                        cout << "\r\033[Ktiger> " << command;  // Borrar línea y mostrar historial
                    }
                }
                else if (ch == 'B') {  // Flecha Abajo
                    string historyCommand = history.getCommand("down");
                    if (!historyCommand.empty() || history.getOnShown() == history.getHistory().size()) {
                        command = historyCommand;
                        cout << "\r\033[Ktiger> " << command;
                    }
                }
            }
        }
        else if (ch == 10) {  // ENTER
            cout << endl;
            break;
        }
        else if (ch == 127) {  // BACKSPACE
            if (!command.empty()) {
                command.pop_back();
                cout << "\b \b";
            }
        }
        else {
            command += ch;
            cout << ch;
        }
    }

    setRawMode(false);
    return command;
}

Shell::Shell()
{
    loadEnvironment();
    updateCurrentDirectory();
    syncEnvironmentWithProcess();
}

void Shell::loadEnvironment()
{
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (pw)
    {
        environment["USER"] = pw->pw_name;
        environment["HOME"] = pw->pw_dir;
        environment["SHELL"] = pw->pw_shell;
    }
    else
    {
        cerr << "Error: No se pudo obtener la información del usuario" << endl;
        environment["USER"] = "unknown";
        environment["HOME"] = "/";
        environment["SHELL"] = "/bin/sh";
    }

    char *path = getenv("PATH");
    if (path)
    {
        environment["PATH"] = path;
    }
}

void Shell::syncEnvironmentWithProcess()
{
    for (const auto &pair : environment)
    {
        setenv(pair.first.c_str(), pair.second.c_str(), 1);
    }
}

bool Shell::setEnvironmentVariable(const string &name, const string &value)
{
    environment[name] = value;

    if (setenv(name.c_str(), value.c_str(), 1) != 0)
    {
        cerr << "Error al establecer variable de entorno: " << strerror(errno) << endl;
        return false;
    }

    return true;
}

void Shell::updateCurrentDirectory()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        currentDirectory = cwd;
        environment["PWD"] = currentDirectory;
        setenv("PWD", currentDirectory.c_str(), 1);
    }
    else
    {
        cerr << "Error getting current directory" << endl;
    }
}

bool Shell::changeDirectory(const std::string &path)
{
    string newPath = path;

    if (newPath.empty())
    {
        newPath = environment["HOME"];
    }

    if (newPath[0] == '~' && (newPath.length() == 1 || newPath[1] == '/'))
    {
        newPath.replace(0, 1, environment["HOME"]);
    }

    if (chdir(newPath.c_str()) == 0)
    {
        updateCurrentDirectory();
        return true;
    }
    else
    {
        cerr << "cd: " << newPath << ": " << strerror(errno) << endl;
        return false;
    }
}

bool Shell::executeBuiltInCommand(const string &command)
{
    istringstream iss(command);
    string cmd;
    iss >> cmd;

    if (cmd == "cd")
    {
        string path;
        getline(iss, path);
        path.erase(0, path.find_first_not_of(" \t"));
        return changeDirectory(path);
    }
    else if (cmd == "pwd")
    {
        cout << currentDirectory << endl;
        return true;
    }
    else if (cmd == "history")
    {
        history.showHistory();
        return true;
    }
    else if (cmd == "echo")
    {
        string arg;
        while (iss >> arg)
        {
            if (arg.length() > 1 && arg[0] == '$')
            {
                string varName = arg.substr(1);
                auto it = environment.find(varName);
                if (it != environment.end())
                {
                    cout << it->second << " ";
                }
                else
                {
                    cout << " ";
                }
            }
            else
            {
                cout << arg << " ";
            }
        }
        cout << endl;
        return true;
    }
    else if (cmd == "export")
    {
        string exportArg;
        iss >> exportArg;

        size_t equalsPos = exportArg.find('=');
        if (equalsPos != string::npos)
        {
            string name = exportArg.substr(0, equalsPos);
            string value = exportArg.substr(equalsPos + 1);

            if (value.length() >= 2 &&
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\'')))
            {
                value = value.substr(1, value.length() - 2);
            }

            setEnvironmentVariable(name, value);
            return true;
        }
        else
        {
            cerr << "export: usage: export NAME=VALUE" << endl;
            return false;
        }
    }

    return false;
}

void Shell::printPrompt() {
    string user = environment["USER"];

    cout << "\033[1;31m" << user << "@" << "CustomShell" << "\033[0m"
         << ":"
         << "\033[1;36m" << currentDirectory << "\033[0m"
         << "$ ";
}
const unordered_map<string, string> &Shell::getEnvironment() const
{
    return environment;
}

const string &Shell::getCurrentDirectory() const
{
    return currentDirectory;
}

void Shell::run() {
    string command;
    while (true) {
        command = readCommand();
        if (command.empty())
        {
            continue;
        }

        if (command == "exit")
        {
            break;
        }

        history.addCommand(command);

        if (!executeBuiltInCommand(command))
        {
            commandHandler.execute(command);
        }
    }
}