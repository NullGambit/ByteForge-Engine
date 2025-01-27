#pragma once

#include <atomic>

namespace forge
{
	// a channel meant to be used by multiple sources
	struct MultiChannel
	{
		std::atomic_uint32_t users;

		u8* input = nullptr;

		std::vector<u8*> outputs;

		~MultiChannel()
		{
			delete input;

			for (auto *output : outputs)
			{
				delete output;
			}
		}

		template<class T, class ...Args>
		T* create_input(Args &&...args)
		{
			auto *ptr = new T(std::forward<Args>(args)...);

			input = (u8*)ptr;

			return ptr;
		}

		template<class T, class ...Args>
		T* create_output(Args &&...args)
		{
			auto *output = new T(std::forward<Args>(args)...);

			outputs.emplace_back((u8*)output);

			return (T*)output;
		}

		void wait(u32 target = 0) const
		{
			while (users > target);
		}
	};

	template<class I, class O>
	struct Channel
	{
		I input;
		O output;

		std::atomic_bool is_done = false;

		void wait() const
		{
			while (!is_done);
		}
	};
}
