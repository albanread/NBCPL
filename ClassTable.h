#ifndef CLASSTABLE_H
#define CLASSTABLE_H

// Macro to easily enable/disable CLASSTABLE debug output
#define CLASSTABLE_DEBUG_OUTPUT 0

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

// Forward declaration of AST nodes if needed
#include "AST.h"
#include "SymbolTable.h" // For Symbol and VarType

// Represents a member variable within a class
struct ClassMemberInfo {
    std::string name;
    VarType type;
    size_t offset; // Offset from the beginning of the object instance
    Visibility visibility = Visibility::Public; // Default to public
};

// Represents a method within a class
struct ClassMethodInfo {
    std::string name;
    std::string qualified_name; // e.g., "ClassName::methodName"
    size_t vtable_slot; // Index in the vtable
    FunctionType type; // Return type and parameter types
    bool is_virtual = false;
    bool is_final = false;
    Visibility visibility = Visibility::Public; // Default to public
    
    // Parameter information for proper argument type handling
    struct ParameterInfo {
        VarType type;
        bool is_optional;
    };
    std::vector<ParameterInfo> parameters;
};

// Represents a single entry in the ClassTable
struct ClassTableEntry {
    std::string name;
    std::string parent_name; // Empty if no explicit parent
    ClassTableEntry* parent_ptr; // Pointer to parent entry for inheritance

    std::unordered_map<std::string, ClassMemberInfo> member_variables; // name -> info
    std::unordered_map<std::string, ClassMethodInfo> member_methods; // qualifiedname -> info
    std::unordered_map<std::string, ClassMethodInfo*> simple_name_to_method; // simple name -> pointer to info
    std::vector<std::string> vtable_blueprint; // Ordered list of method qualified names for vtable layout

    size_t instance_size; // Total size of an instance of this class

    // --- CREATE/RELEASE support ---
    RoutineDeclaration* constructor = nullptr; // Pointer to the CREATE routine AST node, if any.
    ClassMethodInfo* destructor = nullptr;  // Pointer to the RELEASE method, if any.

    // --- ADD: Tracks if the layout has been calculated ---
    bool is_layout_finalized = false;

    ClassTableEntry(std::string n, std::string p = "")
        : name(std::move(n)), parent_name(std::move(p)), parent_ptr(nullptr), instance_size(0),
          constructor(nullptr), destructor(nullptr), is_layout_finalized(false) {}

    void add_member_variable(const ClassMemberInfo& info) {
        ClassMemberInfo adjusted_info = info;
        // Ensure member variables always start at offset 8 or higher to avoid overwriting vtable pointer
        if (adjusted_info.offset < sizeof(int64_t)) {
            adjusted_info.offset = sizeof(int64_t); // 8 bytes for vtable pointer
        }
        
        // We don't need to check for overlaps - each member should be assigned a unique offset
        // by the ClassPass. If we're here, we trust that the offset calculation is correct.
        
        member_variables[adjusted_info.name] = adjusted_info;
        
        // Update instance size if this member extends beyond current size
        size_t member_end = adjusted_info.offset + sizeof(int64_t);
        if (member_end > instance_size) {
            instance_size = member_end;
        }
    }

    void add_member_method(const ClassMethodInfo& info) {
        member_methods[info.qualified_name] = info;
        // Also create a mapping from simple name to method info pointer
        // This allows lookups by both qualified name and simple name
        simple_name_to_method[info.name] = &member_methods[info.qualified_name];
        
        // For methods that override parent methods, ensure they get the same vtable slot
        if (parent_ptr) {
            auto parent_method = parent_ptr->simple_name_to_method.find(info.name);
            if (parent_method != parent_ptr->simple_name_to_method.end()) {
                // This is an override - use the parent's vtable slot
                member_methods[info.qualified_name].vtable_slot = parent_method->second->vtable_slot;
                
                // Also update the vtable_blueprint if needed
                if (vtable_blueprint.size() > parent_method->second->vtable_slot) {
                    vtable_blueprint[parent_method->second->vtable_slot] = info.qualified_name;
                }
            }
        }
    }
    
    // Helper method to lookup a method by either simple or qualified name
    ClassMethodInfo* lookup_method(const std::string& method_name, bool trace_enabled = false) {
#if CLASSTABLE_DEBUG_OUTPUT
        if (trace_enabled) {
            std::cout << "[CLASSTABLE] lookup_method called for method '" << method_name 
                      << "' in class '" << name << "'" << std::endl;
        }
#endif
        
        // First try simple name lookup
        auto simple_it = simple_name_to_method.find(method_name);
        if (simple_it != simple_name_to_method.end()) {
#if CLASSTABLE_DEBUG_OUTPUT
            if (trace_enabled) {
                std::cout << "[CLASSTABLE] Method found by simple name: " 
                          << simple_it->second->qualified_name << std::endl;
            }
#endif
            return simple_it->second;
        }
        
        // Then try qualified name lookup
        auto qualified_it = member_methods.find(method_name);
        if (qualified_it != member_methods.end()) {
#if CLASSTABLE_DEBUG_OUTPUT
            if (trace_enabled) {
                std::cout << "[CLASSTABLE] Method found by qualified name: " 
                          << qualified_it->second.qualified_name << std::endl;
            }
#endif
            return &qualified_it->second;
        }
        
#if CLASSTABLE_DEBUG_OUTPUT
        if (trace_enabled) {
            std::cout << "[CLASSTABLE] Looking for methods ending with ::" << method_name << std::endl;
        }
#endif
        
        // Look for any method that ends with ::method_name (inherited methods)
        // This is a fallback to catch inherited methods that might have been
        // copied with their original qualified names
        for (auto& [qual_name, method_info] : member_methods) {
            size_t pos = qual_name.rfind("::" + method_name);
            if (pos != std::string::npos && pos + 2 + method_name.length() == qual_name.length()) {
#if CLASSTABLE_DEBUG_OUTPUT
                if (trace_enabled) {
                    std::cout << "[CLASSTABLE] Method found by suffix match: " 
                              << method_info.qualified_name << std::endl;
                }
#endif
                return &method_info;
            }
        }
        
        // If method not found and we have a parent, check there
        if (parent_ptr) {
#if CLASSTABLE_DEBUG_OUTPUT
            if (trace_enabled) {
                std::cout << "[CLASSTABLE] Method not found in " << name 
                          << ", checking parent " << parent_name << std::endl;
            }
#endif
            return parent_ptr->lookup_method(method_name, trace_enabled);
        }
        
#if CLASSTABLE_DEBUG_OUTPUT
        if (trace_enabled) {
            std::cout << "[CLASSTABLE] Method '" << method_name << "' not found in class '" 
                      << name << "' or its ancestors" << std::endl;
            std::cout << "[CLASSTABLE] Available methods in simple_name_to_method:" << std::endl;
            for (const auto& pair : simple_name_to_method) {
                std::cout << "  - " << pair.first << " -> " << pair.second->qualified_name << std::endl;
            }
        }
#endif
        
        return nullptr;
    }
    
    // Find a method by name, traversing the inheritance chain if needed
    // This function wraps lookup_method but provides better error reporting
    ClassMethodInfo* findMethod(const std::string& method_name, bool trace_enabled = false) const {
        // Cast away const from 'this' for this specific call
        ClassMethodInfo* method = const_cast<ClassTableEntry*>(this)->lookup_method(method_name, trace_enabled);
        if (method) {
            return method;
        }
#if CLASSTABLE_DEBUG_OUTPUT
        // Method not found anywhere in the inheritance chain
        if (trace_enabled) {
            std::cout << "[CLASSTABLE] Method '" << method_name 
                      << "' not found in class '" << name
                      << "' or any ancestor" << std::endl;
        }
#endif
        return nullptr;
    }
};

class ClassTable {
public:
    ClassTable() = default;

    void add_class(const std::string& name, const std::string& parent_name = "") {
        entries_[name] = std::make_unique<ClassTableEntry>(name, parent_name);
        
        // Immediately set up parent pointer if parent exists
        if (!parent_name.empty()) {
            auto parent_it = entries_.find(parent_name);
            if (parent_it != entries_.end()) {
                entries_[name]->parent_ptr = parent_it->second.get();
                
                // Start with parent's instance size as the minimum
                if (parent_it->second->is_layout_finalized) {
                    entries_[name]->instance_size = parent_it->second->instance_size;
                }
            }
        }
    }

    bool class_exists(const std::string& name) const {
        return entries_.count(name) > 0;
    }

    ClassTableEntry* get_class(const std::string& name) const {
        auto it = entries_.find(name);
        if (it != entries_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    // Helper method to lookup a method in a class by name, searching the inheritance chain
    ClassMethodInfo* lookup_class_method(const std::string& class_name, const std::string& method_name, bool trace_enabled = false) const {
        if (trace_enabled) {
            std::cout << "[CLASSTABLE] lookup_class_method called for class '" << class_name 
                      << "' and method '" << method_name << "'" << std::endl;
        }
        
        ClassTableEntry* class_entry = get_class(class_name);
        if (!class_entry) {
            if (trace_enabled) {
                std::cout << "[CLASSTABLE] Class '" << class_name << "' not found in table" << std::endl;
            }
            return nullptr;
        }
        
        if (trace_enabled) {
            std::cout << "[CLASSTABLE] Found class entry for '" << class_name << "'" << std::endl;
            std::cout << "[CLASSTABLE] Available methods in this class:" << std::endl;
            for (const auto& pair : class_entry->member_methods) {
                std::cout << "  - " << pair.first << " (simple name: " << pair.second.name << ")" << std::endl;
            }
        }
        
        // Use the new findMethod for consistent method lookup with inheritance
        ClassMethodInfo* method = class_entry->findMethod(method_name, trace_enabled);
        if (method) {
            if (trace_enabled) {
                std::cout << "[CLASSTABLE] Method found in class hierarchy starting at '"
                          << class_name << "': " << method->qualified_name << std::endl;
            }
            return method;
        }
        
        if (trace_enabled) {
            std::cout << "[CLASSTABLE] Method '" << method_name << "' not found in class '"
                      << class_name << "' or its ancestors" << std::endl;
        }
        return nullptr;
    }

    // Check if descendant_class is a descendant of ancestor_class
    bool is_descendant_of(const std::string& descendant_class, const std::string& ancestor_class) const {
        if (descendant_class == ancestor_class) {
            return true; // A class is considered a descendant of itself
        }
        
        ClassTableEntry* descendant_entry = get_class(descendant_class);
        if (!descendant_entry) {
            return false;
        }
        
        // Walk up the inheritance chain
        ClassTableEntry* current = descendant_entry->parent_ptr;
        while (current) {
            if (current->name == ancestor_class) {
                return true;
            }
            current = current->parent_ptr;
        }
        
        return false;
    }

    const std::unordered_map<std::string, std::unique_ptr<ClassTableEntry>>& entries() const {
        return entries_;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<ClassTableEntry>> entries_;
};

#endif // CLASSTABLE_H
