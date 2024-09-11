#pragma once
#include "../../core/isub_system.hpp"

namespace forge
{

	class OglRenderSubSystem : public ISubSystem
	{
	public:
		void init() override;
		void update() override;
		void shutdown() override;

	private:
		bool m_ok = true;
		std::string_view m_error_message;
	};

}
