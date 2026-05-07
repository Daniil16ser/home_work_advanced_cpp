#include "Util.h"
#include <iostream>
#include <iomanip>

// User management

void ConsoleUtil::create_user(int id, const std::string& name) {
    if (users.find(id) != users.end()) {
        std::cout << "Error: User with ID " << id << " already exists.\n";
        return;
    }
    auto user = std::make_shared<User>(name, id);
    users[id] = user;
    std::cout << "User " << id << " (" << name << ") created.\n";
}

void ConsoleUtil::delete_user(int id) {
    auto it = users.find(id);
    if (it == users.end()) {
        std::cout << "Error: User " << id << " not found.\n";
        return;
    }

    // rm user from all groups that still hold a weak_ptr
    for (auto& group_pair : groups) {
        group_pair.second->remove_member(id);
    }

    users.erase(it);
    std::cout << "User " << id << " deleted.\n";
}

void ConsoleUtil::get_user(int id) const {
    auto it = users.find(id);
    if (it == users.end()) {
        std::cout << "Error: User " << id << " not found.\n";
        return;
    }
    const auto& user = it->second;
    std::cout << "User ID   : " << user->get_id() << "\n"
              << "Name      : " << user->get_name() << "\n"
              << "Group ID  : ";
    int gid = user->get_group_id();
    if (gid == -1)
        std::cout << "(none)\n";
    else
        std::cout << gid << "\n";
}

void ConsoleUtil::all_users() const {
    if (users.empty()) {
        std::cout << "No users.\n";
        return;
    }
    std::cout << "All users (" << users.size() << "):\n";
    for (const auto& pair : users) {
        const auto& user = pair.second;
        int gid = user->get_group_id();
        std::cout << "  " << user->get_id() << " : " << user->get_name()
                  << " (group " << (gid == -1 ? "none" : std::to_string(gid)) << ")\n";
    }
}

// Group management


void ConsoleUtil::create_group(int id) {
    if (groups.find(id) != groups.end()) {
        std::cout << "Error: Group with ID " << id << " already exists.\n";
        return;
    }
    auto group = std::make_shared<Group>(id);
    groups[id] = group;
    std::cout << "Group " << id << " created.\n";
}

void ConsoleUtil::delete_group(int id) {
    auto it = groups.find(id);
    if (it == groups.end()) {
        std::cout << "Error: Group " << id << " not found.\n";
        return;
    }
    groups.erase(it);
    std::cout << "Group " << id << " deleted.\n";
    // no need to delete up users – their weak_ptr will be deleted automatically.
}

void ConsoleUtil::get_group(int id) const {
    auto it = groups.find(id);
    if (it == groups.end()) {
        std::cout << "Error: Group " << id << " not found.\n";
        return;
    }
    const auto& group = it->second;
    std::cout << "Group ID: " << group->get_id() << "\n";
    auto members = group->get_active_members();
    if (members.empty()) {
        std::cout << "  No active members.\n";
    } else {
        std::cout << "  Members (" << members.size() << "):\n";
        for (const auto& user : members) {
            std::cout << "    " << user->get_id() << " : " << user->get_name() << "\n";
        }
    }
}

void ConsoleUtil::all_groups() const {
    if (groups.empty()) {
        std::cout << "No groups.\n";
        return;
    }
    std::cout << "All groups (" << groups.size() << "):\n";
    for (const auto& pair : groups) {
        const auto& group = pair.second;
        std::cout << "  Group " << group->get_id() << ":\n";
        auto members = group->get_active_members();
        if (members.empty()) {
            std::cout << "    (no members)\n";
        } else {
            for (const auto& user : members) {
                std::cout << "      " << user->get_id() << " : " << user->get_name() << "\n";
            }
        }
    }
}

// Additional command: add user to group

void ConsoleUtil::add_user_to_group(int user_id, int group_id) {
    auto user_it = users.find(user_id);
    if (user_it == users.end()) {
        std::cout << "Error: User " << user_id << " not found.\n";
        return;
    }
    auto group_it = groups.find(group_id);
    if (group_it == groups.end()) {
        std::cout << "Error: Group " << group_id << " not found.\n";
        return;
    }

    auto user = user_it->second;
    auto group = group_it->second;

    // if user already belongs to a existign group, leave it first
    int old_gid = user->get_group_id();
    if (old_gid != -1) {
        auto old_group_it = groups.find(old_gid);
        if (old_group_it != groups.end()) {
            old_group_it->second->remove_member(user_id);
        }
        user->leave_group();  // clear weak_ptr in User
        std::cout << "User " << user_id << " left group " << old_gid << ".\n";
    }

    group->add_member(user);
    std::cout << "User " << user_id << " added to group " << group_id << ".\n";
}