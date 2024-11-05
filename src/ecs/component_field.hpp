#pragma once
#include <functional>
#include <string_view>
#include <variant>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace forge
{
	struct ButtonField
	{
		std::string_view name;
		std::function<void()> callback;

		ButtonField(std::string_view name, std::function<void()> callback) :
			name(name),
			callback(callback)
		{}

		ButtonField(std::function<void()> callback) :
			callback(callback)
		{}
	};

	struct ColorField
	{
		std::string_view name;
		glm::vec4 *rgba;
	};

	struct FieldSeperator
	{
		std::string_view name;
	};

#define COLOR_FIELD(field) forge::ColorField(#field, field)

#define BASE_FIELD_VAR_SET \
	float*, double*, int*, bool*,	\
	std::string*,			\
	ButtonField*, ColorField, FieldSeperator,\
	glm::vec4*, glm::vec3*, glm::vec2*, glm::quat* \

	using BaseFieldVar = std::variant<BASE_FIELD_VAR_SET>;

	struct WatchedField;

	using FieldVar = std::variant<BASE_FIELD_VAR_SET, WatchedField*>;

	struct WatchedField
	{
		FieldVar field;
		Delegate<void, FieldVar> on_changed;
	};

#define WATCH_FIELD(field, method) forge::WatchedField{field, forge::Delegate<void, forge::FieldVar>(this, method)}

	struct ComponentField
	{
		std::string_view name;
		FieldVar var;
	};
}
