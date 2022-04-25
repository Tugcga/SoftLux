#pragma once

#include "../render_base/render_engine_base.h"

#include <luxcore/luxcore.h>

#include <set>

class RenderEngineLux : public RenderEngineBase 
{
public:
	RenderEngineLux();
	~RenderEngineLux();

	//ui events
	XSI::CStatus render_option_define(XSI::CustomProperty& property);
	XSI::CStatus render_option_define_layout(XSI::Context& context);
	XSI::CStatus render_options_update(XSI::PPGEventContext& event_context);

	//render events
	XSI::CStatus pre_render_engine();  // this called before the xsi scene is locked, after skiping existed images
	XSI::CStatus pre_scene_process();  // this method is called after xsi scene is locked
	XSI::CStatus create_scene();
	XSI::CStatus post_scene();
	void render();
	XSI::CStatus post_render_engine();

	//update scene events
	XSI::CStatus update_scene(XSI::X3DObject& xsi_object, const UpdateType update_type);
	XSI::CStatus update_scene(const XSI::SIObject& si_object, const UpdateType update_type);
	XSI::CStatus update_scene(const XSI::Material& xsi_material);
	XSI::CStatus update_scene_render();
	
	void abort();
	void clear_engine();

	inline static bool is_log;
	static void lux_log_handler(const char* msg);

private:
	//-----------------------------------------------
	//internal methods

	//call before each render call in pre scene process and try to init luxcore
	//if already inited, then nothing to do here
	void try_to_init();
	void clear_scene();
	void clear_session();

	//-----------------------------------------------
	//internal class members

	//make true when init luxcore
	bool is_init;
	//scene for rendering
	luxcore::Scene* scene;
	bool is_scene_create;
	//render session
	luxcore::RenderSession* session;
	bool is_session;
	bool is_update_camera;

	//render region from the previous render session
	unsigned int prev_full_width, prev_full_height;
	unsigned int prev_corner_x, prev_corner_y;
	unsigned int prev_width, prev_height;

	//store here ids of xsi_object which we already update
	//to prevent update twise
	std::vector<ULONG> updated_xsi_ids;
	//store here xsi object, which added to the lux scene
	//and use this array when try update some object
	std::set<ULONG> xsi_objects_in_lux;
	//the same list for exported materials
	std::set<ULONG> xsi_materials_in_lux;
};