#include "pch.h"
#include "core/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Log::Logger;

void Log::Init() {
	spdlog::set_pattern("%^[%T] %n: %v%$");
	Logger = spdlog::stdout_color_mt("Minecraft");
	Logger->set_level(spdlog::level::trace);
}
