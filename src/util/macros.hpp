#pragma once

#define CONCAT(a, b) __CONCAT__(a, b)
#define __CONCAT__(a, b) a##b

#define UNWRAP_OR_RETURN(var_name, expr)								\
	auto CONCAT(var_name, _012819212) = (expr);							\
	if (!CONCAT(var_name, _012819212).has_value()) return std::nullopt; \
	auto var_name = std::move(CONCAT(var_name, _012819212).value())		\

