#pragma once

#include "system/input_keys.hpp"

namespace forge
{
	bool is_key_pressed(Key key, Modifier mod = Modifier::None);
	bool is_key_released(Key key, Modifier mod = Modifier::None);
	bool is_key_held(Key key);
}
