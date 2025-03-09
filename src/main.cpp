#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <unordered_map>
#include <filesystem>

enum class Options {
    ECHO, EXIT, TYPE, INVALID
};

const std::unordered_map <std::string, Options> optionMap = {
        {"echo", Options::ECHO},
        {"exit", Options::EXIT},
        {"type", Options::TYPE}
};

std::string getPathEnvString() {
    const char *pathVar = std::getenv("PATH");
    if (pathVar == nullptr) {
        return "";
    }
    return std::string(pathVar);
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

void typeCommand(const std::vector <std::string> &tokens) {
    if (tokens.size() != 2) {
        std::cerr << "Error: too many args " << std::endl;
        return;
    }

    std::string pathEnv = getPathEnvString();
    std::vector <std::string> paths = stringSplit(pathEnv, ':');
    // Check in PATH
    for (std::string path: paths) {
        if (checkIfFileExists(path, tokens.at(1))) {
            std::cout << tokens.at(1) << " is a shell builtin "<< std::endl;
            return;
        }
    }
    // Check in custom options
    auto it = optionMap.find(tokens.at(1));
    if (it != optionMap.end()) {
        std::cout << it->first << " is a shell builtin" << std::endl;
    } else {
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

void commandEvaluator(const std::string &command) {
    std::vector <std::string> tokens = stringSplit(command, ' ');
    Options option = stringToOptions(tokens.at(0));
    if (option == Options::EXIT) {
        exitCommand(tokens);
    } else if (option == Options::ECHO) {
        echoCommand(tokens);
    } else if (option == Options::TYPE) {
        typeCommand(tokens);
    } else {
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
