#pragma once

#include <string_view>

namespace Engine
{
    template <typename T> constexpr std::string_view TypeName();

    template <>
    constexpr std::string_view TypeName<void>()
    {
        return "void";
    }

    using TypeNameProber = void;

    template<typename T>
    constexpr std::string_view WrappedTypeName()
    {
#ifdef __clang__
        return __PRETTY_FUNCTION__;
#elif defined (__GNUC__)
        return __PRETTY_FUNCTION__;
#elif defined (_MSC_VER)
        return __FUNCSIG__;
#else
#error "Unsupported compiler"
#endif
    }

    constexpr std::size_t WrappedTypeNamePrefixLength()
    {
        return WrappedTypeName<TypeNameProber>().find_first_of(TypeName<TypeNameProber>());
    }

    constexpr std::size_t WrappedTypeNameSuffixLength()
    {
        return WrappedTypeName<TypeNameProber>().length() - WrappedTypeNamePrefixLength() - TypeName<TypeNameProber>().length();
    }

    template <typename T>
    constexpr std::string_view TypeName()
    {
        constexpr auto wrappedName = WrappedTypeName<T>();
        constexpr auto prefixLength = WrappedTypeNamePrefixLength();
        constexpr auto suffixLength = WrappedTypeNameSuffixLength();
        constexpr auto TypeNameLength = wrappedName.length() - prefixLength - suffixLength;
        return wrappedName.substr(prefixLength, TypeNameLength);
    }

    constexpr uint64_t TypeHash(std::string_view str)
    {
        std::uint64_t hashValue = 0xbf29ce484222325ULL;
        constexpr auto prime = 0x100000001b3ULL;
        for (char c : str)
        {
            hashValue ^= static_cast<std::uint64_t>(c);
            hashValue *= prime;
        }
        return hashValue;
    }

    template <typename Type>
    constexpr uint64_t TypeID()
    {
        return TypeHash(TypeName<Type>());
    }
}