#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <conio.h>

#include "../include/filesystem.h"
#include "../include/file.h"
#include "../include/directory.h"
#include "../include/filesystem_node.h"

int main() {
    FileSystem fs;
    std::string line;
    std::string command;
    std::string arg1, arg2;

    std::vector<std::string> history;
    int history_index = -1;

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "In-Memory File System Simulator" << std::endl;
    std::cout << "Commands: ls [path], cd <path>, mkdir <path>, touch <path>, rm <path>" << std::endl;
    std::cout << "          pwd, cat <path>, echo \"text\" > <path>, rename <path> <new_name>, tree, clear, exit" << std::endl;

    while (true) {
        std::cout << "[" << fs.pwd() << "]$ ";
        line.clear();
        while (true) {
            char c = _getch();

            if (c == '\x0C') { // CTRL + L
#ifdef _WIN32
                system("cls");
#else
                system("clear");
#endif
                std::cout << "[" << fs.pwd() << "]$ " << line;
                continue;
            } else if (c == '\r') { // ENTER
                std::cout << std::endl;
                if (!line.empty()) {
                    history.push_back(line);
                }
                history_index = -1;
                break;
            } else if (c == 8) { // Backspace
                if (!line.empty()) {
                    line.pop_back();
                    std::cout << "\b \b";
                }
            } else if (c == 9) { // TAB autocomplete
                std::vector<std::string> commands = {
                    "ls", "cd", "mkdir", "touch", "rm", "pwd", "cat", "echo", "rename", "tree", "clear", "exit", "neofetch"
                };
                std::stringstream ss(line);
                std::string firstPart, secondPart;
                ss >> firstPart;
                ss >> secondPart;
                std::string completion = "";
                if (secondPart.empty()) {
                    for (const auto& cmd : commands) {
                        if (cmd.find(firstPart) == 0) {
                            completion = cmd.substr(firstPart.size());
                            break;
                        }
                    }
                } else {
                    std::vector<std::string> names = fs.getCurrentDirectory()->getChildNames();
                    for (const auto& name : names) {
                        if (name.find(secondPart) == 0) {
                            completion = name.substr(secondPart.size());
                            break;
                        }
                    }
                }
                if (!completion.empty()) {
                    line += completion;
                    std::cout << completion;
                }
            } else if (c == -32 || (unsigned char)c == 224) { // Arrow keys
                char c2 = _getch();
                if (c2 == 72) { // Up arrow
                    if (!history.empty()) {
                        if (history_index + 1 < (int)history.size()) {
                            history_index++;
                        }
                        std::string hist_cmd = history[history.size() - 1 - history_index];
                        while (!line.empty()) {
                            std::cout << "\b \b";
                            line.pop_back();
                        }
                        line = hist_cmd;
                        std::cout << line;
                    }
                }
            } else {
                line += c;
                std::cout << c;
            }
        }

        std::stringstream ss(line);
        ss >> command;

        arg1 = "";
        arg2 = "";

        if (command == "echo") {
            std::string content_part;
            char c;
            ss.get(c);
            while(ss.get(c) && c != '>') {
                content_part += c;
            }
            if (!content_part.empty() && content_part.front() == '"' && content_part.back() == '"') {
                if(content_part.length() >= 2)
                    content_part = content_part.substr(1, content_part.length() - 2);
                else content_part = "";
            } else if (!content_part.empty() && content_part.back() == ' ') {
                content_part.pop_back();
            }
            ss >> arg1;
            arg2 = content_part;
        } else {
            ss >> arg1;
            ss >> arg2;
        }

        if (command == "exit") {
            break;
        } else if (command == "ls") {
            fs.ls(arg1.empty() ? "." : arg1);
        } else if (command == "cd") {
            if (arg1.empty()) {
                std::cerr << "cd: missing operand" << std::endl;
            } else {
                fs.cd(arg1);
            }
        } else if (command == "mkdir") {
            if (arg1.empty()) {
                std::cerr << "mkdir: missing operand" << std::endl;
            } else {
                fs.mkdir(arg1);
            }
        } else if (command == "touch") {
            if (arg1.empty()) {
                std::cerr << "touch: missing operand" << std::endl;
            } else {
                fs.touch(arg1);
            }
        } else if (command == "rm") {
            if (arg1.empty()) {
                std::cerr << "rm: missing operand" << std::endl;
            } else {
                fs.rm(arg1);
            }
        } else if (command == "pwd") {
            std::cout << fs.pwd() << std::endl;
        } else if (command == "tree") {
            fs.printTree();
        } else if (command == "clear") {
            system("cls");
        } else if (command == "cat") {
            if (arg1.empty()) {
                std::cerr << "cat: missing operand" << std::endl;
            } else {
                fs.cat(arg1);
            }
        } else if (command == "echo") {
            if (arg1.empty()) {
                std::cerr << "echo: missing output file" << std::endl;
            } else {
                fs.echoToFile(arg2, arg1);
            }
        } else if (command == "neofetch") {
            fs.neofetch();
        } else if (command == "rename") {
            if (arg1.empty() || arg2.empty()) {
                std::cerr << "rename: missing operand" << std::endl;
            } else {
                fs.rename(arg1, arg2);
            }
        } else if (command.empty()) {
            // do nothing
        } else {
            std::cerr << "Command not found: " << command << std::endl;
        }
    }

    return 0;
}