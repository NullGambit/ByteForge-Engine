#include "input.hpp"

#include "core/engine.hpp"

bool forge::is_key_pressed(Key key, Modifier mod)
{
	auto &engine = Engine::get_instance();
	return engine.window.is_key_pressed(key, mod);
}

bool forge::is_key_released(Key key, Modifier mod)
{
	auto &engine = Engine::get_instance();
	return engine.window.is_key_released(key, mod);
}

bool forge::is_key_held(Key key)
{
	auto &engine = Engine::get_instance();
	return engine.window.is_key_held(key);
}

bool forge::is_mouse_button_pressed(MouseButton key, Modifier mod)
{
	auto &engine = Engine::get_instance();
	return engine.window.is_mouse_button_pressed(key, mod);
}

bool forge::is_mouse_button_released(MouseButton key, Modifier mod)
{
	auto &engine = Engine::get_instance();
	return engine.window.is_mouse_button_released(key, mod);
}

bool forge::is_mouse_button_held(MouseButton key)
{
	auto &engine = Engine::get_instance();
	return engine.window.is_mouse_button_held(key);
}

void forge::set_cursor_mode(CursorMode mode)
{
	auto &engine = Engine::get_instance();
	engine.window.set_cursor_mode(mode);
}

forge::CursorMode forge::get_cursor_mode()
{
	auto &engine = Engine::get_instance();
	return engine.window.get_cursor_mode();
}

glm::vec2 forge::get_mouse_coords()
{
	auto &engine = Engine::get_instance();
	return engine.window.get_mouse_coords();
}
