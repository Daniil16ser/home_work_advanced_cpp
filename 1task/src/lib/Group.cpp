#include<Group.h>
#include<User.h>


void Group::add_member(std::shared_ptr<User> user) {
    members.push_back(user);
    user->set_group(shared_from_this());
}


void Group::remove_member(int user_id) {
    members.erase(std::remove_if(members.begin(), members.end(),
        [user_id](const std::weak_ptr<User>& wp) {
            auto sp = wp.lock();
            return !sp || sp->get_id() == user_id;
        }), members.end());
}


std::vector<std::shared_ptr<User>> Group::get_active_members() const {
    std::vector<std::shared_ptr<User>> active;
    for (const auto& wp : members) {
        auto sp = wp.lock();
        if (sp) active.push_back(sp);
    }
    return active;
}