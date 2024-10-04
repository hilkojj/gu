#include "type_name.h"
#include "string_utils.h"

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

std::string demangle(const char *name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    std::unique_ptr<char, void(*)(void*)> res {
            abi::__cxa_demangle(name, NULL, NULL, &status),
            std::free
    };
    return (status == 0) ? res.get() : name;
}

#else

// does nothing if not g++
std::string demangle(const char *name) {
    return name;
}

#endif

void removeKeywords(std::string &str)
{
    auto splitted = su::split(str, " ");
    str = splitted.back();
    if (splitted.size() >= 2 && splitted[splitted.size() - 2] == "unsigned") // lol
        str = "unsigned " + str;
}

void removeTemplates(std::string &str)
{
    int pos = str.find('<');
    if (pos == std::string::npos) return;
    str = str.substr(0, pos);
}

void removeNamespaces(std::string &str)
{
    int pos = str.rfind("::");
    if (pos == std::string::npos) return;
    str = str.substr(pos + 2, str.size() - 1);
}

