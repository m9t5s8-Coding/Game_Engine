// Log.cpp - Fixed Implementation
#include <Core/Log.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>

namespace ag
{
	std::shared_ptr<spdlog::logger> Log::s_core_logger;
	std::shared_ptr<spdlog::logger> Log::s_client_logger;
	std::shared_ptr<ImGuiConsoleSink> Log::s_console_sink;

	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_core_logger = spdlog::stdout_color_mt("AERO");
		s_core_logger->set_level(spdlog::level::trace);
		s_client_logger = spdlog::stdout_color_mt("APP");
		s_client_logger->set_level(spdlog::level::trace);
	}

	void Log::init_with_console()
	{
		// Create console sink FIRST
		s_console_sink = std::make_shared<ImGuiConsoleSink>();

		// Then initialize loggers with console sink attached
		spdlog::set_pattern("%^[%T] %n: %v%$");

		// Create custom sink that outputs to both console and ImGui
		auto console_color_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

		// Core logger with console sink
		s_core_logger = std::make_shared<spdlog::logger>("AERO", console_color_sink);
		s_core_logger->set_level(spdlog::level::trace);
		spdlog::register_logger(s_core_logger);

		// Client logger with console sink
		s_client_logger = std::make_shared<spdlog::logger>("APP", console_color_sink);
		s_client_logger->set_level(spdlog::level::trace);
		spdlog::register_logger(s_client_logger);
	}

	void Log::add_to_console(const std::string& message, spdlog::level::level_enum level, const char* logger_name)
	{
		if (s_console_sink)
		{
			spdlog::details::log_msg msg;
			msg.time = std::chrono::system_clock::now();
			msg.level = level;
			msg.payload = spdlog::string_view_t(message.data(), message.size());
			msg.logger_name = spdlog::string_view_t(logger_name, strlen(logger_name));
			s_console_sink->AddLog(msg);
		}
	}

	void Log::draw_console(const char* title, bool* p_open)
	{
		if (s_console_sink)
		{
			s_console_sink->Draw(title, p_open);
		}
		else
		{
			if (ImGui::Begin(title, p_open))
			{
				ImGui::Text("Console not initialized. Call Log::init_with_console() first.");
				ImGui::End();
			}
		}
	}
}