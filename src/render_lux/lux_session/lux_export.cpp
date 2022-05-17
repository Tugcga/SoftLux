#include "lux_session.h"
#include "../../utilities/export_common.h"
#include "../../utilities/logs.h"

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

void export_scene(luxcore::RenderSession *session, XSI::RendererContext &render_context, const XSI::CString &archive_folder, const int export_mode, const XSI::CString &scene_name)
{
	std::string folder_path_str = std::string(archive_folder.GetAsciiString()) + "\\";
	if (create_dir(folder_path_str))
	{
		if (export_mode == 0 || export_mode == 1)
		{//use filesaver
			session->Start();
			while (!session->HasDone())
			{
				std::this_thread::sleep_for(100ms);
			}
			session->Stop();
		}
		else
		{
			session->Start();
			session->Pause();
			session->SaveResumeFile(folder_path_str + scene_name.GetAsciiString() + ".rsm");
			session->Resume();
			session->Stop();
		}
	}
	else
	{
		log_message("Fails to create dirctory for the path " + archive_folder, XSI::siWarningMsg);
	}
}