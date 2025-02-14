#pragma once

#include <string_view>
#include "config/arg_parser.hpp"

namespace forge
{
	struct EngineInitOptions
	{
		std::string_view window_title;
		i32 window_width;
		i32 window_height;
		u8 log_flags;
		std::string_view log_file;
		std::string_view log_time_fmt;
		ArgParser *arg_parser = nullptr;
	};
}
