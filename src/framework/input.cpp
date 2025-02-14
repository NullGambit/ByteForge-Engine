#include "input.hpp"

#include "core/engine.hpp"

bool forge::is_key_pressed(Key key, KeyModifier mod)
{
	return g_engine.window.is_key_pressed(key, mod);
}

bool forge::is_key_released(Key key, KeyModifier mod)
{
	return g_engine.window.is_key_released(key, mod);
}

bool forge::is_key_held(Key key)
{
	return g_engine.window.is_key_held(key);
}

bool forge::is_mouse_button_pressed(MouseButton key, KeyModifier mod)
{
	return g_engine.window.is_mouse_button_pressed(key, mod);
}

bool forge::is_mouse_button_released(MouseButton key, KeyModifier mod)
{
	return g_engine.window.is_mouse_button_released(key, mod);
}

bool forge::is_mouse_button_held(MouseButton key)
{
	return g_engine.window.is_mouse_button_held(key);
}

void forge::set_cursor_mode(CursorMode mode)
{
	g_engine.window.set_cursor_mode(mode);
}

forge::CursorMode forge::get_cursor_mode()
{
	return g_engine.window.get_cursor_mode();
}

glm::vec2 forge::get_mouse_coords()
{
	return g_engine.window.get_mouse_coords();
}
