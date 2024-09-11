#pragma once 

#include <typeinfo>
#include <string_view>

#ifdef __has_include
#  if __has_include(<cxxabi.h>)
#    include <cxxabi.h>
#    define HAS_CXXABI
#  endif
#endif

namespace util
{
    #ifdef HAS_CXXABI
    static inline std::string_view demangle_name(const char* mangled_name)
    {
        int status = 0;
        size_t size = 0;
        return abi::__cxa_demangle(mangled_name, nullptr, &size, &status);
    }
    #else
    static inline std::string_view pge::demangle_name(const char* mangled_name)
    {
        return mangled_name;
    }
    #endif

    template<class T>
    static inline std::string_view type_name()
    {
        return demangle_name(typeid(T).name());
    }

    static inline std::string_view type_name(std::type_index index)
    {
        return demangle_name(index.name());
    }
}