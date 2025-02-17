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