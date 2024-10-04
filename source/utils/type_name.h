
#ifndef GAME_TYPE_NAME_H
#define GAME_TYPE_NAME_H

#include <string>
#include <typeinfo>

namespace typename_utils
{

std::string demangleTypeName(const char *typeName);
void removeKeywords(std::string &str);
void removeTemplates(std::string &str);
void removeNamespaces(std::string &str);

/**
 * This function tries to return a clean string representation of any type.
 * On GCC typeid::name() returns a mangled type name. This is unmangled first.
 *
 * Tested to return the same strings on all platforms when removeNamespacesAndTemplates = true.
 *
 * example: getTypeName<std::vector<std::string>>(true) returns "vector"
 */
template <typename Type>
std::string getTypeName(bool removeNamespacesAndTemplates = true)
{
    std::string name = demangleTypeName(typeid(Type).name());
    if (removeNamespacesAndTemplates)
    {
        removeTemplates(name);
        removeNamespaces(name);
    }
    removeKeywords(name);
    return name;
}
}

#endif //GAME_TYPE_NAME_H
