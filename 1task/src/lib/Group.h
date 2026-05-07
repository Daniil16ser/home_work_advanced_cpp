#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "User.h"


class Group : public GroupInterface, public std::enable_shared_from_this<Group> {
    int group_id;
    std::vector<std::weak_ptr<User>> members;

public:
    explicit Group(int id) : group_id(id) {}  // forbidden hidden casts

    void add_member(std::shared_ptr<User> user);

    void remove_member(int user_id);
    
    std::vector<std::shared_ptr<User>> get_active_members() const;

    int get_id() const override {
        return group_id;
    }
};