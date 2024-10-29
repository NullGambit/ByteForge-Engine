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

#define COLOR_FIELD(field) forge::ColorField(#field, field)

#define BASE_FIELD_VAR_SET \
	float*, double*, int*,	\
	std::string*,			\
	ButtonField*, ColorField,\
	glm::vec4*, glm::vec3*, glm::vec2*, glm::quat* \

	using BaseFieldVar = std::variant<BASE_FIELD_VAR_SET>;

	struct WatchedField
	{
		std::variant<BASE_FIELD_VAR_SET, WatchedField*> field;
		std::function<void(std::variant<BASE_FIELD_VAR_SET, WatchedField*>)> on_changed;
	};

	using FieldVar = std::variant<BASE_FIELD_VAR_SET, WatchedField*>;

	struct ComponentField
	{
		std::string_view name;
		FieldVar var;
	};
}
