#include "type_name.h"
#include "string_utils.h"

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#endif

std::string typename_utils::demangleTypeName(const char *typeName)
{
    std::string demangledTypeName = typeName;

#ifdef __GNUG__ // On other platforms there should not be mangling.

    int demangleStatus = 1; // 1 doesn't mean anything, 0 == Success, < 0 are specific errors.

    char *demangledPtr = abi::__cxa_demangle(typeName, nullptr, nullptr, &demangleStatus);

    if (demangleStatus == 0)
    {
        demangledTypeName = demangledPtr;
    }
    std::free(demangledPtr);
#endif
    return demangledTypeName;
}

void typename_utils::removeKeywords(std::string &str)
{
    auto splitted = su::split(str, " ");
    str = splitted.back();
    if (splitted.size() >= 2 && splitted[splitted.size() - 2] == "unsigned") // lol
    {
        str = "unsigned " + str;
    }
}

void typename_utils::removeTemplates(std::string &str)
{
    const int pos = str.find('<');
    if (pos == std::string::npos)
    {
        return;
    }
    str = str.substr(0, pos);
}

void typename_utils::removeNamespaces(std::string &str)
{
    const int pos = str.rfind("::");
    if (pos == std::string::npos)
    {
        return;
    }
    str = str.substr(pos + 2, str.size() - 1);
}

