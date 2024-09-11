#pragma once
#include <string_view>

class GLFWwindow;

namespace forge
{
	class Window
	{
	public:
		Window();

		~Window();

		void close();

		bool open(std::string_view name, int width, int height);

		bool should_close() const;

		bool should_stay_open() const;

		void set_should_close(bool value);

		void swap_buffers() const;

	private:
		GLFWwindow *m_handle;
		inline static bool sm_is_init;
	};
}
