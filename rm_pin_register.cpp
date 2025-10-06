#include "RegisterManager.h"
#include <iostream>

void RegisterManager::pin_register(const std::string& reg_name) {
    pinned_registers_.insert(reg_name);
    std::cout << "[REGISTER PIN] Pinned register: " << reg_name << std::endl;
}

void RegisterManager::unpin_register(const std::string& reg_name) {
    pinned_registers_.erase(reg_name);
    std::cout << "[REGISTER UNPIN] Unpinned register: " << reg_name << std::endl;
}

bool RegisterManager::is_pinned(const std::string& reg_name) const {
    return pinned_registers_.find(reg_name) != pinned_registers_.end();
}