#include <Application/ProjectManagerApp.hpp>

namespace ag
{
	ProjectManagerApp::ProjectManagerApp()
	{
		push_layer(new ProjectManagerLayer());
	}


	void ProjectManagerApp::on_create()
	{
		WindowProps props;
		props.Size = { 650, 720 };
		props.Title = "Project Manager";
		init(props);
		get().get_window().center_window();
		//get().get_window().show_decoration(false);
	}

	void ProjectManagerApp::on_destroy()
	{

	}
}