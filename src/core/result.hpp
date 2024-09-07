#pragma once
#include <variant>

template<class R, class Or>
class Result
{
public:
	Result(R &&result) :
		m_value(std::forward<R>(result))
	{}

	Result(Or &&Or) :
		m_value(std::forward<Or>(Or))
	{}

	bool is_ok() const
	{
		return m_value.index() == 0;
	}

	R value() const
	{
		return std::move(std::get<R>(m_value));
	}

private:
	std::variant<R, Or> m_value;
};
