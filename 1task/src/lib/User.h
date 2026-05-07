#pragma once

#include<string>
#include <memory>


class GroupInterface {
    public:
    virtual int get_id() const = 0;
};


class User {
    std::string user_name;
    int user_id;
    std::weak_ptr<GroupInterface> user_group_ptr;  // helps keeping object that can be deleted

    public:
    User(std::string name, int id) :user_name{name},user_id{id} {}

    void set_group(std::shared_ptr<GroupInterface> group_ptr) {
        user_group_ptr = group_ptr;
    }

    void leave_group() {
        user_group_ptr.reset();
    }

    std::string get_name() const {
        return user_name;
    }

    int get_id() const {
        return user_id;
    }
    int get_group_id() const {
        auto group_ptr = user_group_ptr.lock();
        return group_ptr ? group_ptr->get_id() : -1;
    }
};