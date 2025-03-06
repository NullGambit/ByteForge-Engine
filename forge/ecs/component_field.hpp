#pragma once
#include <functional>
#include <string_view>
#include <variant>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <magic_enum/magic_enum.hpp>

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
		std::variant<glm::vec3*, glm::vec4*> color_value;
	};

	struct FieldSeperator
	{
		std::string_view name;
	};

	struct EnumStorage
	{
		std::vector<std::pair<std::string_view, int>> entries;
		int *value;

		template<class T>
		EnumStorage(T *enum_value)
		{
			auto values = magic_enum::enum_values<T>();
			auto names = magic_enum::enum_names<T>();

			value = (int*)enum_value;

			for (auto i = 0; auto &n : names)
			{
				entries.emplace_back(n, (int)values[i++]);
			}
		}

		int get_selected_index() const
		{
			if (value == nullptr)
			{
				return 0;
			}

			for (auto i = 0; auto [_, v] : entries)
			{
				if (v == *value)
				{
					return i;
				}

				i++;
			}

			return 0;
		}
	};

#define COLOR_FIELD(field) forge::ColorField(#field, field)

#define BASE_FIELD_VAR_SET \
	float*, double*, int*, bool*,	\
	std::string*,			\
	ButtonField, ColorField, FieldSeperator,\
	glm::vec4*, glm::vec3*, glm::vec2*, glm::quat*, \
	EnumStorage \

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

		// ComponentField() = default;
		//
		// template<class T>
		// ComponentField(std::string_view name, T value) :
		// 	name(name)
		// {
		// 	if constexpr (std::is_enum_v<T>)
		// 	{
		// 		auto values = magic_enum::enum_values<T>();
		// 		auto names = magic_enum::enum_names<T>();
		//
		// 		EnumStorage storage;
		//
		// 		storage.value = (int*)&value;
		//
		// 		for (auto i = 0; auto &n : names)
		// 		{
		// 			storage.entries.emplace_back(n, values[i++]);
		// 		}
		//
		// 		this->name = name;
		// 		var = storage;
		// 	}
		// 	else
		// 	{
		// 		var = value;
		// 	}
		// }
	};
}
