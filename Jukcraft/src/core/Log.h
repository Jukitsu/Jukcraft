#pragma once
#include "spdlog/spdlog.h"

namespace Jukcraft {
	class Log {
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetLogger() { return Logger; }
	private:
		static std::shared_ptr<spdlog::logger> Logger;
	};
}

/* Core log macros */


#define LOG_ERROR(...)	::Jukcraft::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)	::Jukcraft::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)	::Jukcraft::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...)	::Jukcraft::Log::GetLogger()->trace(__VA_ARGS__)
