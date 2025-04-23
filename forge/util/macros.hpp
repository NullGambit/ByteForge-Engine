#pragma once

#define CONCAT(a, b) __CONCAT__(a, b)
#define __CONCAT__(a, b) a##b

#define UNWRAP_OR_RETURN(var_name, expr)								\
	auto CONCAT(var_name, _012819212) = (expr);							\
	if (!CONCAT(var_name, _012819212).has_value()) return std::nullopt; \
	auto var_name = std::move(CONCAT(var_name, _012819212).value())		\

#define UNWRAP_OR_RETURN_FALSE(var_name, expr)							\
	auto CONCAT(var_name, _012819212) = (expr);							\
	if (!CONCAT(var_name, _012819212).has_value()) return false;		\
	auto var_name = std::move(CONCAT(var_name, _012819212).value())		\


#define EXPAND_VEC4(vec) vec.x, vec.y, vec.z, vec.w
#define EXPAND_VEC3(vec) vec.x, vec.y, vec.z
#define EXPAND_VEC2(vec) vec.x, vec.y

#define FORGE_GENERATE_ENUM_OPERATORS(EnumType)                    \
    inline EnumType operator|(EnumType lhs, EnumType rhs) {                          \
        using T = std::underlying_type_t<EnumType>;                                  \
        return static_cast<EnumType>(static_cast<T>(lhs) | static_cast<T>(rhs));     \
    }                                                                                 \
    inline EnumType operator&(EnumType lhs, EnumType rhs) {                          \
        using T = std::underlying_type_t<EnumType>;                                  \
        return static_cast<EnumType>(static_cast<T>(lhs) & static_cast<T>(rhs));     \
    }                                                                                 \
    inline EnumType operator^(EnumType lhs, EnumType rhs) {                          \
        using T = std::underlying_type_t<EnumType>;                                  \
        return static_cast<EnumType>(static_cast<T>(lhs) ^ static_cast<T>(rhs));     \
    }                                                                                 \
    inline EnumType operator~(EnumType val) {                                        \
        using T = std::underlying_type_t<EnumType>;                                  \
        return static_cast<EnumType>(~static_cast<T>(val));                          \
    }                                                                                 \
    inline EnumType& operator|=(EnumType& lhs, EnumType rhs) {                       \
        lhs = lhs | rhs;                                                             \
        return lhs;                                                                  \
    }                                                                                 \
    inline EnumType& operator&=(EnumType& lhs, EnumType rhs) {                       \
        lhs = lhs & rhs;                                                             \
        return lhs;                                                                  \
    }                                                                                 \
    inline EnumType& operator^=(EnumType& lhs, EnumType rhs) {                       \
        lhs = lhs ^ rhs;                                                             \
        return lhs;                                                                  \
    }                                                                                 \
    inline bool has_flag(EnumType value, EnumType flag) {                            \
        using T = std::underlying_type_t<EnumType>;                                  \
        return (static_cast<T>(value) & static_cast<T>(flag)) != 0;                  \
    }