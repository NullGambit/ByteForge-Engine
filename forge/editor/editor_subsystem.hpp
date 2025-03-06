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

		std::vector<std::pair<std::string_view, std::function<void()>>> demos;


	private:
		Nexus m_nexus {};
	};
}
