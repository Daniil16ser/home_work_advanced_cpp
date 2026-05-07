#include "util.h"
#include <iostream>
#include <string>
#include <sstream>

int main() {
    ConsoleUtil util;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "createUser") {
            int id; std::string name;
            iss >> id >> name;
            util.create_user(id, name);
        } else if (cmd == "deleteUser") {
            int id; iss >> id;
            util.delete_user(id);
        } else if (cmd == "allUsers") {
            util.all_users();
        } else if (cmd == "getUser") {
            int id; iss >> id;
            util.get_user(id);
        } else if (cmd == "createGroup") {
            int id; iss >> id;
            util.create_group(id);
        } else if (cmd == "deleteGroup") {
            int id; iss >> id;
            util.delete_group(id);
        } else if (cmd == "allGroups") {
            util.all_groups();
        } else if (cmd == "getGroup") {
            int id; iss >> id;
            util.get_group(id);
        } else if (cmd == "addToGroup") {
            int uid, gid; iss >> uid >> gid;
            util.add_user_to_group(uid, gid);
        } else if (cmd == "exit") {
            break;
        } else {
            std::cout << "Unknown command.\n";
        }
    }
    return 0;
}