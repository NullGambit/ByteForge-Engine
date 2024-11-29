#pragma once

#include "fmt/fmt.hpp"

#include <ctime>
#include <chrono>
#include <cstdint>
#include <cstdio>

class log
{
public:
	static constexpr uint8_t Disabled = 1 << 0;
	static constexpr uint8_t Silent   = 1 << 1;
	static constexpr uint8_t LogTime  = 1 << 2;

	static void set_outfile(std::string_view path)
	{
		if (m_file)
		{
			fclose(m_file);
		}

		m_file = fopen(path.data(), "a");
	}

	static void set_time_fmt(std::string_view fmt)
	{
		m_time_fmt = fmt;
	}

	static void disable_file()
	{
		if (m_file == nullptr)
		{
			return;
		}

		fclose(m_file);

		m_file = nullptr;
	}

	static void set_flags(uint8_t flags)
	{
		m_flags = flags;
	}

	template<typename... A>
    static void info(std::string_view fmt, A&&... a)
    {
		do_log("INFO", stdout, fmt, a...);
    }

	template<typename... A>
	static void warn(std::string_view fmt, A&&... a)
    {
		do_log("WARN", stdout, fmt, a...);
    }

	template<typename... A>
	static void fatal(std::string_view fmt, A&&... a)
    {
		do_log("FATAL", stderr, fmt, a...);
    }

    template<typename... A>
	static void debug(std::string_view fmt, A&&... a)
    {
#if DEBUG
		do_log("DEBUG", stdout, fmt, a...);
#endif
    }

private:
	inline static FILE *m_file;
	inline static uint8_t m_flags;
	inline static std::string_view m_time_fmt = "%x %X";

	template<typename... A>
	static void do_log(std::string_view level, FILE *file, std::string_view fmt, A&&... a)
	{
		if (m_flags & Disabled)
		{
			return;
		}

		auto message = fmt::format(fmt, std::forward<A>(a)...);

		auto time_str_value = m_flags & LogTime ? time_str() : "";

		_log(fmt::format("[{}] {} | {} {}\n", level, time_str_value, message), file);
	}

    static void _log(std::string_view data, FILE *file)
    {
		if (m_file)
		{
            fwrite(data.data(), 1, data.size(), m_file);
		}

		if (m_flags & Silent)
		{
			return;
		}

		fwrite(data.data(), 1, data.size(), file);
    }

    static char* time_str()
    {
		time_t tt;

        time(&tt);

        auto *info = localtime(&tt);

		static constexpr int MAX_LEN = 18;

		thread_local char buff[MAX_LEN];

        strftime(buff, MAX_LEN, m_time_fmt.data(), info);

        return buff;
    }
};
