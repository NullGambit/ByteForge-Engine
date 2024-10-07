#include "imgui_subsystem.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/engine.hpp"

std::string forge::ImGuiSubsystem::init()
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.IniFilename = "editor_config.ini";

	auto &window = Engine::get_instance().window;

	auto ok = true;

#define VALIDATE(expr, message) ok = (expr); if (!ok) return message

	VALIDATE(ImGui_ImplGlfw_InitForOpenGL(window.get_handle(), true), "could not init imgui for glfw");

	// TODO update this when vulkan backend is implemented
	VALIDATE(ImGui_ImplOpenGL3_Init(), "could not init imgui for opengl backend");

#undef VALIDATE

	return {};
}

void forge::ImGuiSubsystem::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();

	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}

void forge::ImGuiSubsystem::update()
{

}

void forge::ImGuiSubsystem::start_tick()
{
	ImGui_ImplOpenGL3_NewFrame();

	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
}

void forge::ImGuiSubsystem::end_tick()
{
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
