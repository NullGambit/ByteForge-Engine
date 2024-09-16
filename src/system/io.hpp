#pragma once

#include <optional>
#include <string>
#include <filesystem>

namespace forge
{
	std::optional<std::string> read_entire_file(const std::filesystem::path &path);
}
