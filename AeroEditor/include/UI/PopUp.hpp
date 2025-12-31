#pragma once

namespace ag
{
	struct PopUp
	{
		struct PopUpModel_Props
		{
			std::string name;
			std::string id;
			std::function<void()> draw_content;
			std::function<void()> on_open;
			std::function<void()> on_close;
			std::function<void()> on_confirm;
			std::function<void()> on_cancel;
			vec2f window_size;

			std::string confirm_name;
			std::string cancel_name;
		};
	};
}