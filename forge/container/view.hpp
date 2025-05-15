#pragma once

namespace forge
{
	template<class T, class S = u32>
	struct View
	{
		T *data = nullptr;
		S size {};

		View() = default;

		View(T *data, u32 size) :
			data(data),
			size(size)
		{}

		template<class T2>
		requires requires(T2 t) {t.size(); t.data(); }
		View(T2 &value) :
			data(value.data()),
			size(value.size())
		{}

		template<class T2>
		requires requires(T2 t) {t.get_length(); t.get_memory(); }
		View(T2 &value) :
			data(value.get_memory()),
			size(value.get_length())
		{}

		T& operator[](u32 index)
		{
			return data[index];
		}

		const T& operator[](u32 index) const
		{
			return data[index];
		}

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