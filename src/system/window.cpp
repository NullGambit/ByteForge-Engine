#include "window.hpp"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "core/engine.hpp"
#include "core/logging.hpp"
#include "fmt/fmt.hpp"

void on_framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
	auto *windowPtr = (forge::Window*)glfwGetWindowUserPointer(window);

	windowPtr->on_resize(width, height);
}

void on_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
	    auto &io = ImGui::GetIO();

	    io.AddMousePosEvent(xpos, ypos);

	    if (io.WantCaptureMouse)
	    {
	        return;
	    }
	}


	auto *windowPtr = (forge::Window*)glfwGetWindowUserPointer(window);

	auto &data = windowPtr->get_input_event_data();

    data.mouse_cords =
    {
        xpos,
        ypos
    };

    data.mouse_called = true;
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

    data.scroll_called = true;
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
	auto &key_event = data.key_cache[index];

	key_event.called = true;
	key_event.action = action;
	key_event.mods = mods;
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

	auto &key_event = data.mouse_button_cache[button];

	key_event.called = true;
	key_event.action = action;
	key_event.mods = mods;
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
	glfwSwapInterval(1);

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

bool forge::Window::is_key_pressed(Key key, Modifier mod) const
{
	return get_key(int(key), GLFW_PRESS, int(mod));
}

bool forge::Window::is_key_held(Key key) const
{
	bool imgui_wants_keyboard = false;

	if (ImGui::GetCurrentContext() != nullptr)
	{
		auto &io = ImGui::GetIO();
		imgui_wants_keyboard = io.WantCaptureKeyboard;
	}

	return !imgui_wants_keyboard && glfwGetKey(m_handle, (int)key);
}

bool forge::Window::is_mouse_button_pressed(MouseButton key, Modifier mod) const
{
	return get_mouse_button((int)key, (int)mod, GLFW_PRESS);
}

bool forge::Window::is_mouse_button_released(MouseButton key, Modifier mod) const
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

bool forge::Window::is_key_released(Key key, Modifier mod) const
{
	return get_key(int(key), GLFW_RELEASE, int(mod));
}

void forge::Window::reset_input()
{
	m_input_event_data.mouse_called = false;
	m_input_event_data.scroll_called = false;

	for (auto &key : m_input_event_data.key_cache)
	{
		key.called = false;
	}

	for (auto &button : m_input_event_data.mouse_button_cache)
	{
		button.called = false;
	}
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

bool was_key_called(forge::WindowKeyEventData key_event, int action, int mod)
{
	if (key_event.called && key_event.action == action && key_event.mods == mod)
	{
		return true;
	}

	return false;
}

bool forge::Window::get_mouse_button(int button, int action, int mod) const
{
	auto &key_event = m_input_event_data.mouse_button_cache[button];

	return was_key_called(key_event, action, mod);
}

bool forge::Window::get_key(int key, int action, int mod) const
{
	auto index = translate_glfw_key(key);
	auto &key_event = m_input_event_data.key_cache[index];

	return was_key_called(key_event, action, mod);
}

