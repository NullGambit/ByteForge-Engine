#pragma once

#include <glm/vec2.hpp>

#include "forge/system/input_keys.hpp"

namespace forge
{
	bool is_key_pressed(Key key, KeyModifier mod = KeyModifier::None);
	bool is_key_released(Key key, KeyModifier mod = KeyModifier::None);
	bool is_key_held(Key key);
	bool is_mouse_button_pressed(MouseButton key, KeyModifier mod = KeyModifier::None);
	bool is_mouse_button_released(MouseButton key, KeyModifier mod = KeyModifier::None);
	bool is_mouse_button_held(MouseButton key);
	void set_cursor_mode(CursorMode mode);
	CursorMode get_cursor_mode();
	glm::vec2 get_mouse_coords();
}
