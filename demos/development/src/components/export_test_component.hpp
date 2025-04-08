#pragma once
#include <string>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "forge/ecs/component_field.hpp"
#include "forge/ecs/ecs.hpp"

enum class TestColors
{
	Red,
	Green,
	Blue,
	Yellow,
	Purple,
	Orange
};

class ExportFieldTestComponent : public forge::IComponent
{
public:
	float f1 = 10;
	float f2 = 20;
	int integer = 500;
	bool boolean = false;
	std::string string;
	glm::vec4 vec4;
	glm::vec3 vec3;
	glm::vec2 vec2;
	glm::quat quat;
	glm::vec4 color;
	int watched_int;
	TestColors colors;

	ExportFieldTestComponent()
	{
		colors = TestColors::Purple;
	}

	forge::ButtonField test_button =
	{
		"Test button",
		[]
		{
			log::info("Button pressed");
		}
	};

	void on_watched_field_changed(forge::FieldVar field)
	{
		log::info("field changed to {}", *std::get<int*>(field));
	}

	EXPORT_FIELDS(
		forge::FieldSeperator{"primitives"},
		&f1, &f2, &integer, &boolean,
		forge::FieldSeperator{"controls"},
		test_button, COLOR_FIELD(&color),
		forge::FieldSeperator{"linear algebra"},
		&vec4, &vec3, &vec2, &quat,
		&colors);
};
