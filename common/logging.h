#pragma once

#include <string_view>
#include <utility>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace logging {
	using console_logger_t = typename decltype(spdlog::stdout_logger_mt("console"));
	extern console_logger_t console;

	void setup(console_logger_t& console) {
		namespace spd = spdlog;
		console.swap(spd::stdout_logger_mt("console"));
		console->set_pattern("[%T] [?] [%l] %v");
		console->flush_on(spdlog::level::trace);
		console->set_level(spdlog::level::trace);
		console->info("Logging set up");
	}

	void set_process_name(std::string name) {
		std::string fmt = "[%T] [" + name + "] [%l] %v";
		console->set_pattern(fmt);
	}

	template<typename... Args>
	void log_and_throw(const char* fmt, const Args&... args) {
		auto&& msg = fmt::format(fmt, args...);
		console->error(msg);
		throw std::runtime_error(msg);
	}
}