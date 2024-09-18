#pragma once
#include <string_view>

#include "events/signal.hpp"
#include "GLFW/glfw3.h"

class GLFWwindow;

namespace forge
{
	class Window
	{
	public:
		~Window();

		void close();

		bool open(std::string_view name, int width, int height);

		bool should_close() const;

		bool should_stay_open() const;

		void set_should_close(bool value);

		void swap_buffers() const;

		void set_context();

		GLFWwindow* get_handle() const;

		Signal<void(int, int)> on_resize;

	private:
		GLFWwindow *m_handle;
	};

	template<class T>
	class SharedWindowContext
	{
	public:
		SharedWindowContext(T &mutex, const Window &window) :
			m_mutex(mutex),
			m_window(window)
		{
			m_mutex.lock();
			m_worker_window = glfwCreateWindow(1, 1, "", nullptr, window.get_handle());
			glfwMakeContextCurrent(m_worker_window);
		}

		~SharedWindowContext()
		{
			glfwDestroyWindow(m_worker_window);
			m_mutex.unlock();
		}

	private:
		GLFWwindow *m_worker_window;
		T &m_mutex;
		const Window &m_window;
	};
}
