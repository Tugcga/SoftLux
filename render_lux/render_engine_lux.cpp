#include "render_engine_lux.h"
#include "../utilities/logs.h"
#include "lux_scene/lux_scene.h"
#include "lux_session/lux_session.h"

#include "xsi_primitive.h"

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

RenderEngineLux::RenderEngineLux()
{
	is_init = false;
	scene = NULL;
	render_config = NULL;
	session = NULL;
	is_scene_create = false;
	is_render_config = false;
	is_session = false;
}

//when we delete the engine, then at first this method is called, and then the method from base class
RenderEngineLux::~RenderEngineLux()
{

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
		luxcore::Init(lux_log_handler);
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
}

//here we clear session and render config
void RenderEngineLux::clear_session()
{
	if (is_render_config)
	{
		delete render_config;
		render_config = NULL;
		is_render_config = false;
	}

	if (is_session)
	{
		delete session;
		session = NULL;
		is_session = false;
	}
}

//called every time before scene update
//here we should setup paramters, which should be done for both situations: screate scene from scratch or update the scene
XSI::CStatus RenderEngineLux::pre_scene_process()
{
	try_to_init();

	//return abort, this means that we should create the scene from scratch
	return XSI::CStatus::Abort;
}

//here we create the scene for rendering from scratch
XSI::CStatus RenderEngineLux::create_scene()
{
	clear_scene();
	clear_session();
	scene = luxcore::Scene::Create();

	//setup camera
	XSI::Primitive camera_prim(m_render_context.GetAttribute("Camera"));
	XSI::X3DObject camera_obj = camera_prim.GetOwners()[0];
	XSI::Camera	xsi_camera(camera_obj);
	sync_camera(scene, xsi_camera, eval_time);

	//add sky and sun for test
	scene->Parse(
		luxrays::Property("scene.lights.skyl.type")("sky2") <<
		luxrays::Property("scene.lights.skyl.dir")(0.166974f, 0.59908f, 0.783085f) <<
		luxrays::Property("scene.lights.skyl.turbidity")(2.2f) <<
		luxrays::Property("scene.lights.skyl.gain")(0.8f, 0.8f, 0.8f) <<
		luxrays::Property("scene.lights.sunl.type")("sun") <<
		luxrays::Property("scene.lights.sunl.dir")(0.166974f, 0.59908f, 0.783085f) <<
		luxrays::Property("scene.lights.sunl.turbidity")(2.2f) <<
		luxrays::Property("scene.lights.sunl.gain")(0.8f, 0.8f, 0.8f)
	);

	return XSI::CStatus::OK;
}

//call this method after scene created or updated but before unlock
XSI::CStatus RenderEngineLux::post_scene()
{
	//here we should create the session and render parameters
	if (!is_session)
	{
		render_config = luxcore::RenderConfig::Create(
			luxrays::Property("renderengine.type")("PATHCPU") <<
			luxrays::Property("sampler.type")("SOBOL"), scene);
		is_render_config = true;

		//update render parameters
		//we should set it before connect it to the session
		//may be it will be better to create parameters each render call
		//or may be delete session when we should change render parameters and recreate it
		sync_render_config(render_config, m_render_property, eval_time,
			image_corner_x, image_corner_x + image_size_width, image_corner_y, image_corner_y + image_size_height,
			image_full_size_width, image_full_size_height);

		session = luxcore::RenderSession::Create(render_config);
		is_session = true;
	}

	return XSI::CStatus::OK;
}

void RenderEngineLux::render()
{
	m_render_context.ProgressUpdate("Rendering...", "Rendering...", 0);

	session->Start();
	const luxrays::Properties& stats = session->GetStats();
	while (!session->HasDone())
	{
		session->UpdateStats();
		const double elapsedTime = stats.Get("stats.renderengine.time").Get<double>();
		const unsigned int pass = stats.Get("stats.renderengine.pass").Get<unsigned int>();
		const float convergence = stats.Get("stats.renderengine.convergence").Get<unsigned int>();

		//we should read pixels from the film of the session to the visual buffer
		//and create render tile from this buffer

		std::this_thread::sleep_for(100ms);
	}

	session->Stop();
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