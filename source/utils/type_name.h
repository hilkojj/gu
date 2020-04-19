
#ifndef GAME_TYPE_NAME_H
#define GAME_TYPE_NAME_H

#include <sstream>

std::string demangle(const char *name);
void removeKeywords(std::string &str);
void removeTemplates(std::string &str);
void removeNamespaces(std::string &str);

/**
 * returns the name of a type.
 *
 * will probably return the same string on all platforms when removeNamespacesAndTemplates = true.
 *
 * example: getTypeName<std::vector<std::string>>(true) returns "vector"
 *
 * Thanks to https://stackoverflow.com/a/4541470
 */
template <typename Type>
std::string getTypeName(bool removeNamespacesAndTemplates=true)
{
    std::string name = demangle(typeid(Type).name());
    if (removeNamespacesAndTemplates)
    {
        removeTemplates(name);
        removeNamespaces(name);
    }
    removeKeywords(name);
    return name;
}

#endif //GAME_TYPE_NAME_H
