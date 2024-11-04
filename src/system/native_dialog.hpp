#pragma once

#include <string>

struct FileDialogOptions
{
	std::string_view root = "./assets/";
	bool allow_multiple = true;
	std::string_view allowed_mimes;
};

#ifdef __linux__
#include "linux/linux_native_dialog.hpp"
#define native_file_dialog linux_native_file_dialog
#endif
