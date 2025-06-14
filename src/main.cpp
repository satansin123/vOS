#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== C++ Application ===" << std::endl;
    std::cout << "Running in: " << (argc > 1 ? argv[1] : "local") << " environment" << std::endl;
    std::cout << "Hello from C++!" << std::endl;
    
    // Simple interactive loop for development
    std::string input;
    std::cout << "Enter 'quit' to exit, or any other text to echo: ";
    while (std::getline(std::cin, input)) {
        if (input == "quit") {
            break;
        }
        std::cout << "You entered: " << input << std::endl;
        std::cout << "Enter 'quit' to exit, or any other text to echo: ";
    }
    
    std::cout << "Goodbye!" << std::endl;
    return 0;
}
