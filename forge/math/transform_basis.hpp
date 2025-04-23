#pragma once

#include "glm/glm.hpp"

namespace forge
{
	struct TransformBasis
	{
		glm::mat3 basis;
		glm::vec3 origin;
	};
}
