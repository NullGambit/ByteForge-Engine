#pragma once

#include "forge/core/isub_system.hpp"
#include "forge/ecs/ecs.hpp"

namespace forge
{
	class EditorSubsystem final : public ISubSystem
	{
	public:
		std::string init(const EngineInitOptions &options) override;
		void shutdown() override;
		void update() override;

		std::vector<DependencyStorage> get_dependencies() override;

		void load_demo(std::string_view name);

		HashMap<std::string_view, std::function<void()>> demos;

	private:
		Nexus m_nexus {};
	};
}
