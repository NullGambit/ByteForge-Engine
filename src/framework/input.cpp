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
