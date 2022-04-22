#include "render_engine_lux.h"
#include "../utilities/logs.h"
#include "../utilities/export_common.h"
#include "../utilities/arrays.h"
#include "lux_scene/lux_scene.h"
#include "lux_session/lux_session.h"

#include "xsi_primitive.h"
#include "xsi_application.h"
#include "xsi_project.h"

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

RenderEngineLux::RenderEngineLux()
{
	is_init = false;
	scene = NULL;
	session = NULL;
	is_scene_create = false;
	is_session = false;
	prev_full_width = 0;
	prev_full_height = 0;
	prev_corner_x = 0;
	prev_corner_y = 0;
	prev_width = 0;
	prev_height = 0;
	updated_xsi_ids.clear();
	xsi_objects_in_lux.clear();

	RenderEngineLux::is_log = false;
}

//when we delete the engine, then at first this method is called, and then the method from base class
RenderEngineLux::~RenderEngineLux()
{

}

void RenderEngineLux::lux_log_handler(const char* msg)
{
	if (RenderEngineLux::is_log)
	{
		XSI::Application().LogMessage(XSI::CString(msg));
	}
}

//nothing to do here
XSI::CStatus RenderEngineLux::pre_render_engine()
{
	return XSI::CStatus::OK;
}

//init luxcore if we need this
void RenderEngineLux::try_to_init()
{
	if (!is_init)
	{
		luxcore::Init(RenderEngineLux::lux_log_handler);
		is_init = true;
	}
}

void RenderEngineLux::clear_scene()
{
	if (is_scene_create)
	{
		delete scene;
		scene = NULL;
		is_scene_create = false;
	}
	xsi_objects_in_lux.clear();
}

//here we clear session and render config
void RenderEngineLux::clear_session()
{
	if (is_session)
	{
		delete session;
		session = NULL;
		is_session = false;
	}
}

//called every time before scene update
//here we should setup parameters, which should be done for both situations: screate scene from scratch or update the scene
XSI::CStatus RenderEngineLux::pre_scene_process()
{
	try_to_init();

	//activate the log
	if (render_type == RenderType_Shaderball)
	{
		RenderEngineLux::is_log = false;
	}
	else
	{
		RenderEngineLux::is_log = false;
	}

	//if we change region limits (corners or render size), then we should recreate the session, so, delete it
	if (prev_corner_x != visual_buffer.corner_x || prev_corner_y != visual_buffer.corner_y ||
		prev_width != visual_buffer.width || prev_height != visual_buffer.height ||
		prev_full_width != visual_buffer.full_width || prev_full_height != visual_buffer.full_height)
	{
		clear_session();
		prev_corner_x = visual_buffer.corner_x;
		prev_corner_y = visual_buffer.corner_y;
		prev_width = visual_buffer.width;
		prev_height = visual_buffer.height;
		prev_full_width = visual_buffer.full_width;
		prev_full_height = visual_buffer.full_height;
	}

	updated_xsi_ids.clear();
	
	return XSI::CStatus::OK;
}

//return OK, if object successfully updates, Abort in other case
XSI::CStatus RenderEngineLux::update_scene(XSI::X3DObject& xsi_object, const UpdateType update_type)
{
	if (is_scene_create)
	{
		if (!is_contains(updated_xsi_ids, xsi_object.GetObjectID()))
		{
			if (update_type == UpdateType_Camera)
			{
				XSI::Primitive camera_prim(m_render_context.GetAttribute("Camera"));
				XSI::X3DObject camera_obj = camera_prim.GetOwners()[0];
				XSI::Camera	xsi_camera(camera_obj);
				sync_camera_scene(scene, xsi_camera, eval_time);

				return XSI::CStatus::OK;
			}
			else if (update_type == UpdateType_Visibility)
			{
				//change object visibility
				//delete the object
				std::string object_name = XSI::CString(xsi_object.GetObjectID()).GetAsciiString();
				scene->DeleteObject(object_name);
				if (is_xsi_object_visible(eval_time, xsi_object))
				{
					//add it, if it come visible
					bool is_sync = sync_object(scene, xsi_object, eval_time);
					if (is_sync)
					{
						updated_xsi_ids.push_back(xsi_object.GetObjectID());
					}
				}
				return XSI::CStatus::OK;
			}
			else if(update_type == UpdateType_Transform)
			{
				//does not remember the object, because we can update mesh of the object later
				//here we only change it transform
				ULONG xsi_id = xsi_object.GetObjectID();
				//TODO: this is too long in the big scene
				//may be optimize this method (use tree, or check by object type)
				if (is_contains(xsi_objects_in_lux, xsi_id))
				{
					sync_transform(scene, xsi_id, xsi_object.GetKinematics().GetGlobal().GetTransform(), eval_time);
				}
				
				return XSI::CStatus::OK;
			}
			else if (update_type == UpdateType_XsiLight)
			{

			}
			else if (update_type == UpdateType_Mesh)
			{

			}
			else
			{
				
			}
		}
		//else, we already update this object
		
		return XSI::CStatus::OK;
	}
	else
	{
		return XSI::CStatus::Abort;
	}
}

XSI::CStatus RenderEngineLux::update_scene(const XSI::SIObject& si_object, const UpdateType update_type)
{
	//nothing to do here
	return XSI::CStatus::OK;
}

XSI::CStatus RenderEngineLux::update_scene(const XSI::Material& xsi_material)
{
	//here we update only materials

	return XSI::CStatus::OK;
}

XSI::CStatus RenderEngineLux::update_scene_render()
{
	//if we change render parameters, then we should recreate the session
	//we clear it, and in post_scene() call we recreate it 
	clear_session();

	return XSI::CStatus::OK;
}

//here we create the scene for rendering from scratch
XSI::CStatus RenderEngineLux::create_scene()
{
	clear_scene();
	clear_session();
	scene = luxcore::Scene::Create();
	is_scene_create = true;

	//sync materials
	sync_materials(scene, XSI::Application().GetActiveProject().GetActiveScene(), eval_time);
	//sync scene objects
	sync_scene_objects(scene, m_render_context, render_type, xsi_objects_in_lux, eval_time);

	//setup camera
	if (render_type == RenderType_Shaderball)
	{
		//for shaderball render use other sync method
		sync_camera_shaderball(scene);
	}
	else
	{
		XSI::Primitive camera_prim(m_render_context.GetAttribute("Camera"));
		XSI::X3DObject camera_obj = camera_prim.GetOwners()[0];
		XSI::Camera	xsi_camera(camera_obj);
		sync_camera_scene(scene, xsi_camera, eval_time);
	}

	//add sky and sun for test
	scene->Parse(
		luxrays::Property("scene.lights.skyl.type")("sky2") <<
		luxrays::Property("scene.lights.skyl.dir")(0.166974f, 0.59908f, 0.783085f) <<
		luxrays::Property("scene.lights.skyl.turbidity")(2.2f) <<
		luxrays::Property("scene.lights.skyl.gain")(0.8f, 0.8f, 0.8f)
	);

	return XSI::CStatus::OK;
}

//call this method after scene created or updated but before unlock
XSI::CStatus RenderEngineLux::post_scene()
{
	//here we should create the session and render parameters
	if (!is_session)
	{
		session = sync_render_config(scene, m_render_property, eval_time,
			image_corner_x, image_corner_x + image_size_width, image_corner_y, image_corner_y + image_size_height,
			image_full_size_width, image_full_size_height);
		is_session = true;
	}

	return XSI::CStatus::OK;
}

void RenderEngineLux::render()
{
	m_render_context.ProgressUpdate("Rendering...", "Rendering...", 0);

	session->Start();
	const luxrays::Properties& stats = session->GetStats();
	luxcore::Film& film = session->GetFilm();
	while (!session->HasDone())
	{
		session->UpdateStats();
		const double elapsedTime = stats.Get("stats.renderengine.time").Get<double>();
		const float convergence = stats.Get("stats.renderengine.convergence").Get<unsigned int>();

		read_visual_buffer(film, visual_buffer);
		m_render_context.NewFragment(RenderTile(visual_buffer.corner_x, visual_buffer.corner_y, visual_buffer.width, visual_buffer.height, visual_buffer.pixels, true, 4));

		//calculate percentage of the render samples, pass is done samples
		const unsigned int pass = stats.Get("stats.renderengine.pass").Get<unsigned int>();
		m_render_context.ProgressUpdate("Rendering...", "Rendering...", int((float)pass * 100.0f / (float)40));

		std::this_thread::sleep_for(100ms);
	}

	session->Stop();

	//after render fill the buffer at last time
	read_visual_buffer(film, visual_buffer);
	m_render_context.NewFragment(RenderTile(visual_buffer.corner_x, visual_buffer.corner_y, visual_buffer.width, visual_buffer.height, visual_buffer.pixels, true, 4));
}

XSI::CStatus RenderEngineLux::post_render_engine()
{
	//log render time
	double time = (finish_render_time - start_prepare_render_time) / CLOCKS_PER_SEC;
	if (render_type != RenderType_Shaderball)
	{
		log_message("Render time: " + XSI::CString(time) + " sec.");
	}

	return XSI::CStatus::OK;
}

void RenderEngineLux::abort()
{
	if (is_session)
	{
		session->Stop();
	}
}

void RenderEngineLux::clear_engine()
{
	clear_scene();
	clear_session();
}