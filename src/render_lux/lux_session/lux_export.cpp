#include "lux_session.h"
#include "../../utilities/export_common.h"
#include "../../utilities/logs.h"

void export_scene(luxcore::RenderSession *session, XSI::RendererContext &render_context, const XSI::CString &archive_folder)
{
	std::string folder_path_str = std::string(archive_folder.GetAsciiString()) + "\\";
	if (create_dir(folder_path_str))
	{
		//at first export text-base files
		session->GetRenderConfig().Export(folder_path_str);

		//next also export *.rsm
		session->Start();
		session->Pause();
		session->SaveResumeFile(folder_path_str + "session.rsm");
		session->Resume();
		session->Stop();
	}
	else
	{
		log_message("Fails to create dirctory for the path " + archive_folder, XSI::siWarningMsg);
	}
}