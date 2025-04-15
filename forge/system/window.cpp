#include "window.hpp"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "window_sub_system.hpp"
#include "forge/core/engine.hpp"
#include "forge/core/logging.hpp"
#include "forge/fmt/fmt.hpp"

void on_framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
	auto *windowPtr = (forge::Window*)glfwGetWindowUserPointer(window);

	windowPtr->on_resize(width, height);
}

void on_window_should_close_callback(GLFWwindow *window)
{
	// TODO: this should only be done for the main window
	g_engine.quit();
}

void on_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	auto *windowPtr = (forge::Window*)glfwGetWindowUserPointer(window);

	if (ImGui::GetCurrentContext() != nullptr)
	{
	    auto &io = ImGui::GetIO();

	    io.AddMousePosEvent(xpos, ypos);

	    if (io.WantCaptureMouse && windowPtr->get_cursor_mode() != forge::CursorMode::Disabled)
	    {
	        return;
	    }
	}

	auto &data = windowPtr->get_input_event_data();

    data.mouse_cords =
    {
        xpos,
        ypos
    };
}

void on_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	auto *windowPtr = (forge::Window*)glfwGetWindowUserPointer(window);

	auto &data = windowPtr->get_input_event_data();

    data.scroll_offsets =
    {
        xoffset,
        yoffset
    };
}

uint32_t translate_glfw_key(int key);

void on_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
		auto &io = ImGui::GetIO();

		if (io.WantCaptureKeyboard)
		{
			return;
		}
	}

	auto *windowPtr = (forge::Window*)glfwGetWindowUserPointer(window);

	auto &data = windowPtr->get_input_event_data();

	auto index = translate_glfw_key(key);

	if (action == GLFW_PRESS)
	{
		data.key_down.set(index);
	}
	else if (action == GLFW_RELEASE)
	{
		data.key_up.set(index);
	}
}

void on_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
		auto &io = ImGui::GetIO();

		if (io.WantCaptureKeyboard)
		{
			return;
		}
	}

	auto *windowPtr = (forge::Window*)glfwGetWindowUserPointer(window);

	auto &data = windowPtr->get_input_event_data();

	// auto &key_event = data.mouse_button_cache[button];
	//
	// key_event.called = true;
	// key_event.action = action;
	// key_event.mods = mods;
}

uint32_t translate_glfw_key(int key)
{
    // keys that dont follow a range of more than 3
    switch (key)
    {
        case GLFW_KEY_SPACE: return 0;
        case GLFW_KEY_APOSTROPHE: return 1;
        case GLFW_KEY_SEMICOLON: return 15;
        case GLFW_KEY_EQUAL: return 16;
        case GLFW_KEY_LEFT_BRACKET: return 43;
        case GLFW_KEY_BACKSLASH: return 44;
        case GLFW_KEY_RIGHT_BRACKET: return 45;
        case GLFW_KEY_GRAVE_ACCENT: return 46;
        case GLFW_KEY_WORLD_1: return 47;
        case GLFW_KEY_WORLD_2: return 48;
        case GLFW_KEY_MENU: return 118;
    }

    if (key >= GLFW_KEY_COMMA && key <= GLFW_KEY_9)
    {
        return 1 + (key - GLFW_KEY_COMMA);
    }
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
    {
        return 17 + (key - GLFW_KEY_A);
    }
    if (key >= GLFW_KEY_ESCAPE && key <= GLFW_KEY_END)
    {
        return 49 + (key - GLFW_KEY_ESCAPE);
    }
    if (key >= GLFW_KEY_CAPS_LOCK && key <= GLFW_KEY_PAUSE)
    {
        return 63 + (key - GLFW_KEY_CAPS_LOCK);
    }
    if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_KP_EQUAL)
    {
        return 69 + (key - GLFW_KEY_F1);
    }
    if (key >= GLFW_KEY_LEFT_SHIFT && key <= GLFW_KEY_RIGHT_SUPER)
    {
        return 110 + (key - GLFW_KEY_LEFT_SHIFT);
    }

    return 0;
}

forge::Window::~Window()
{
	close();
}

void forge::Window::close()
{
	if (is_open())
	{
		glfwDestroyWindow(m_handle);
	}
	m_handle = nullptr;
}

bool forge::Window::open(std::string_view name, int width, int height)
{
	if (!g_engine.is_subsystem_initialized<WindowSubSystem>())
	{
		return false;
	}

	m_handle = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);

	if (!m_handle)
	{
		return false;
	}

	set_context();

	glfwSetWindowUserPointer(m_handle, this);
	glfwSetFramebufferSizeCallback(m_handle, on_framebuffer_resize_callback);
	glfwSetKeyCallback(m_handle, on_key_callback);
	glfwSetCursorPosCallback(m_handle, on_mouse_callback);
	glfwSetScrollCallback(m_handle, on_scroll_callback);
	glfwSetWindowCloseCallback(m_handle, on_window_should_close_callback);
	glfwSwapInterval(0);

	return true;
}

bool forge::Window::should_close() const
{
	return glfwWindowShouldClose(m_handle);
}

bool forge::Window::should_stay_open() const
{
	return !glfwWindowShouldClose(m_handle);
}

void forge::Window::set_should_close(bool value)
{
	glfwSetWindowShouldClose(m_handle, value);
}

void forge::Window::swap_buffers() const
{
	glfwSwapBuffers(m_handle);
}

void forge::Window::set_context()
{
	glfwMakeContextCurrent(m_handle);
}

void forge::Window::set_cursor_mode(CursorMode mode) const
{
	glfwSetInputMode(m_handle, GLFW_CURSOR, (int)mode);
}

forge::CursorMode forge::Window::get_cursor_mode() const
{
	return (CursorMode)glfwGetInputMode(m_handle, GLFW_CURSOR);
}

bool forge::Window::is_key_pressed(Key key, KeyModifier mod) const
{
	return get_key(int(key), GLFW_PRESS, int(mod));
}

bool forge::Window::is_key_held(Key key) const
{
	auto imgui_wants_keyboard = false;

	if (ImGui::GetCurrentContext() != nullptr)
	{
		auto &io = ImGui::GetIO();
		imgui_wants_keyboard = io.WantCaptureKeyboard;
	}

	return !imgui_wants_keyboard && glfwGetKey(m_handle, (int)key);
}

bool forge::Window::is_mouse_button_pressed(MouseButton key, KeyModifier mod) const
{
	return get_mouse_button((int)key, (int)mod, GLFW_PRESS);
}

bool forge::Window::is_mouse_button_released(MouseButton key, KeyModifier mod) const
{
	return get_mouse_button((int)key, (int)mod, GLFW_RELEASE);
}

bool forge::Window::is_mouse_button_held(MouseButton key) const
{
	return glfwGetMouseButton(m_handle, (int)key);
}

glm::vec2 forge::Window::get_mouse_coords() const
{
	return m_input_event_data.mouse_cords;
}

bool forge::Window::is_key_released(Key key, KeyModifier mod) const
{
	return get_key(int(key), GLFW_RELEASE, int(mod));
}

void forge::Window::reset_input()
{
	m_input_event_data.key_down.reset();
	m_input_event_data.key_up.reset();
}

GLFWwindow* forge::Window::get_handle() const
{
	return m_handle;
}

glm::ivec2 forge::Window::get_size() const
{
	glm::ivec2 out;

	glfwGetWindowSize(m_handle, &out.x, &out.y);

	return out;
}

bool forge::Window::get_mouse_button(int button, int action, int mod) const
{
	const auto &bitset = action == GLFW_PRESS ? m_input_event_data.mouse_button_down : m_input_event_data.mouse_button_up;

	return bitset.test(button);
}

bool forge::Window::get_key(int key, int action, int mod) const
{
	const auto index = translate_glfw_key(key);
	const auto &bitset = action == GLFW_PRESS ? m_input_event_data.key_down : m_input_event_data.key_up;

	return bitset.test(index);
}

