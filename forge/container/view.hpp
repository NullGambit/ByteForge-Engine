#pragma once

namespace forge
{
	template<class T>
	struct View
	{
		T *data = nullptr;
		u32 size {};

		T* begin()
		{
			return data;
		}

		T* end()
		{
			return data + size;
		}
	};
}