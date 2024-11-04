#pragma once

#include <string>
#include <vector>

struct FileDialogOptions;

namespace forge
{
	std::vector<std::string> linux_native_file_dialog(FileDialogOptions options);
}

