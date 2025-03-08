#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>

std::vector<std::string> stringSplit(const std::string &input, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;

    while(std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool checkExitCommand(const std::string &command) {
    std::vector<std::string> tokens = stringSplit(command, ' ');
    if (tokens.size() != 2) {
        return false;
    }

    if (tokens.at(0) != "exit") {
        return false;
    }

    if (tokens.at(1) != "0"){
//        std::cerr <<"Error: invalid exit code: " << tokens.at(1) << std::endl;
        return false;
    }
    return true;

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

      if (checkExitCommand(input)) {
          exit(0);
      }

      std::cout << input <<": command not found "<< std::endl;
  }

}
