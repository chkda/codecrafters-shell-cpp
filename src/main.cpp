#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

enum class Options {
    ECHO, EXIT, TYPE, INVALID, PWD, CD
};

const std::unordered_map <std::string, Options> optionMap = {
        {"echo", Options::ECHO},
        {"exit", Options::EXIT},
        {"type", Options::TYPE},
        {"pwd",  Options::PWD},
        {"cd",   Options::CD}
};

std::string getEnvString(const std::string &env) {
    const char *pathVar = std::getenv(env.c_str());
    if (pathVar == nullptr) {
        return "";
    }
    return std::string(pathVar);
}

std::string replaceSingleQuotes(std::string &str) {
    std::replace(str.begin(), str.end(), '\'', ' ');
    return str;
}

std::string preprocessCommand(std::string &command) {
//    std::string modifiedCommand;
//    modifiedCommand = removeSingleQuotes(command);
//    return modifiedCommand;
    return replaceSingleQuotes(command);
}

Options stringToOptions(const std::string &str) {
    auto it = optionMap.find(str);
    return (it != optionMap.end()) ? it->second : Options::INVALID;
}

std::vector <std::string> stringSplit(const std::string &input, char delimiter) {
    std::vector <std::string> tokens;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool checkIfFileExists(const std::string &dir, const std::string &file) {
    auto it = std::filesystem::directory_iterator(dir);
    std::string path = dir + "/" + file;
    for (const auto entry: it) {
        if (entry.path() == path) {
            return true;
        }
    }
    return false;
}

std::string getCurrentWorkingDirectory() {
    std::string path = std::filesystem::current_path().string();
    return path;
}

bool changeCurrentWorkingDirectory(const std::string &path) {
    try {
        std::filesystem::current_path(path);
        return true;
    } catch (const std::filesystem::filesystem_error &err) {
        std::cerr << "cd: " << path << ": No such file or directory" << std::endl;
        return false;
    }
}

void typeCommand(const std::vector <std::string> &tokens) {
    if (tokens.size() != 2) {
        std::cerr << "Error: too many args " << std::endl;
        return;
    }

    std::string pathEnv = getEnvString("PATH");
    std::vector <std::string> paths = stringSplit(pathEnv, ':');

    // Check in custom options
    auto it = optionMap.find(tokens.at(1));
    if (it != optionMap.end()) {
        std::cout << it->first << " is a shell builtin" << std::endl;
    } else {
        // Check in PATH
        for (std::string path: paths) {
            if (checkIfFileExists(path, tokens.at(1))) {
                std::cout << tokens.at(1) << " is " << path + "/" + tokens.at(1) << std::endl;
                return;
            }
        }
        std::cout << tokens.at(1) << ": not found" << std::endl;
    }
    return;
}

void echoCommand(const std::vector <std::string> &tokens) {
    for (int i = 1; i < tokens.size(); i++) {
        std::cout << tokens.at(i) << " ";
    }
    std::cout << std::endl;
}

void exitCommand(const std::vector <std::string> &tokens) {
    if (tokens.size() != 2) {
        std::cerr << "Error : too many args" << std::endl;
        return;
    }

    if (tokens.at(1) != "0") {
        std::cerr << "Error: invalid exit code: " << tokens.at(1) << std::endl;
        return;
    }
    exit(0);
}

void pwdCommand() {
    std::string path = getCurrentWorkingDirectory();
    std::cout << path << std::endl;
}

void cdCommand(const std::vector <std::string> &tokens) {
    if (tokens.size() > 2) {
        std::cerr << "Error: too many args" << std::endl;
        return;
    }
    if (tokens.size() < 2) {
        std::cerr << "Error: too few args" << std::endl;
        return;
    }

    if (tokens.at(1) == "~") {
        std::string homeEnv = getEnvString("HOME");
        changeCurrentWorkingDirectory(homeEnv);
    } else {
        changeCurrentWorkingDirectory(tokens.at(1));
    }

    return;
}

void commandEvaluator(std::string &command) {
    std::string processedCommand = preprocessCommand(command);
    std::vector <std::string> tokens = stringSplit(processedCommand, ' ');
    Options option = stringToOptions(tokens.at(0));
    if (option == Options::EXIT) {
        exitCommand(tokens);
    } else if (option == Options::ECHO) {
        echoCommand(tokens);
    } else if (option == Options::TYPE) {
        typeCommand(tokens);
    } else if (option == Options::PWD) {
        pwdCommand();
    } else if (option == Options::CD) {
        cdCommand(tokens);
    } else {
        // Check custom program
        std::string pathEnv = getEnvString("PATH");
        std::vector <std::string> paths = stringSplit(pathEnv, ':');

        for (std::string path: paths) {
            if (checkIfFileExists(path, tokens.at(0))) {
                system(command.c_str());
                return;
            }
        }

        std::cout << command << ": command not found " << std::endl;
    }
}

int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Uncomment this block to pass the first stage
    while (true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);

        commandEvaluator(input);


    }
    return 0;
}
