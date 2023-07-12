#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/async.h"

#define LOGGER_NAME "ModLoader"

namespace Logger {
	static std::shared_ptr<spdlog::logger> Instance;

	static void Init() {
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stderr_color_sink_st>());
		sinks.push_back(std::make_shared<spdlog::sinks::daily_file_format_sink_st>("logs/modloader.log", 23, 59));
		Instance = std::make_shared<spdlog::async_logger>(LOGGER_NAME, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
		//Instance = std::make_shared<spdlog::logger>(LOGGER_NAME, begin(sinks), end(sinks));

		spdlog::set_default_logger(Instance);
		spdlog::flush_every(std::chrono::seconds(5));
	}
};
