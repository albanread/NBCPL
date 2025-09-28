#ifndef NAME_MANGLER_H
#define NAME_MANGLER_H

#include <string>

/**
 * @class NameMangler
 * @brief Utility class for consistent name mangling across compiler passes
 * 
 * This class centralizes the logic for name mangling throughout the compiler,
 * ensuring that method names are consistently constructed and resolved across
 * all compiler passes and components.
 */
class NameMangler {
public:
    /**
     * @brief Mangles a method name with its class name
     * @param className The name of the class
     * @param methodName The name of the method
     * @return The mangled name in the form "ClassName::methodName"
     */
    static std::string mangleMethod(const std::string& className, const std::string& methodName) {
        // Don't mangle if it's already mangled
        if (isQualifiedName(methodName)) {
            return methodName;
        }
        return className + "::" + methodName;
    }
    
    /**
     * @brief Checks if a name is already qualified with a class name
     * @param name The name to check
     * @return true if the name is already qualified (contains "::"), false otherwise
     */
    static bool isQualifiedName(const std::string& name) {
        return name.find("::") != std::string::npos;
    }
    
    /**
     * @brief Extracts the class name from a qualified name
     * @param qualifiedName The qualified name (e.g., "ClassName::methodName")
     * @return The class name or empty string if no qualifier found
     */
    static std::string extractClassName(const std::string& qualifiedName) {
        size_t pos = qualifiedName.find("::");
        if (pos == std::string::npos) {
            return "";
        }
        return qualifiedName.substr(0, pos);
    }
    
    /**
     * @brief Extracts the method name from a qualified name
     * @param qualifiedName The qualified name (e.g., "ClassName::methodName")
     * @return The method name or the original name if no qualifier found
     */
    static std::string extractMethodName(const std::string& qualifiedName) {
        size_t pos = qualifiedName.find("::");
        if (pos == std::string::npos) {
            return qualifiedName;
        }
        return qualifiedName.substr(pos + 2);
    }
};

#endif // NAME_MANGLER_H