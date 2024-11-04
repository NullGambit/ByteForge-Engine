#pragma once

#include <utility>
#include "macros.hpp"

#define DEFER(exp) util::Defer CONCAT(__DEFER__, __COUNTER__) ((exp))

namespace util
{
	template<class FN>
	class Defer
	{
	public:
		Defer(FN &&fn) :
			m_fn(std::forward<FN>(fn))
		{}

		~Defer()
		{
			m_fn();
		}

	private:
		FN m_fn;
	};
}
