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

void echoCommand(const std::vector<std::string> &tokens) {
    for (int i=1; i<tokens.size(); i++) {
        std::cout<<tokens.at(i)<<" ";
    }
    std::cout<<std::endl;
}

void exitCommand(const std::vector<std::string> &tokens) {
    if (tokens.size() != 2) {
        std::cerr << "Error : too many args"<<std::endl;
        return ;
    }

    if (tokens.at(1) != "0"){
        std::cerr <<"Error: invalid exit code: " << tokens.at(1) << std::endl;
        return;
    }
    exit(0);

}

void commandEvaluator(const std::string &command) {
    std::vector<std::string> tokens = stringSplit(command, ' ');
    if (tokens.at(0) == "exit") {
        exitCommand(tokens);
    } else if (tokens.at(0) == "echo") {
        echoCommand(tokens);
    } else {
        std::cout << command <<": command not found "<< std::endl;
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
