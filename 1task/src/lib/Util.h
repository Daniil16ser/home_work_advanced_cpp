#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "User.h"
#include "Group.h"

class ConsoleUtil {
    std::unordered_map<int, std::shared_ptr<User>>  users;
    std::unordered_map<int, std::shared_ptr<Group>> groups;
public:
    void create_user(int id, const std::string& name);
    void delete_user(int id);
    void get_user(int id) const;
    void all_users() const;

    void create_group(int id);
    void delete_group(int id);
    void get_group(int id) const;
    void all_groups() const;

    void add_user_to_group(int user_id, int group_id);
};