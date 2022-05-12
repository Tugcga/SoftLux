#pragma once

#include "../render_base/render_engine_base.h"

#include <luxcore/luxcore.h>

#include <set>
#include <unordered_map>

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
	XSI::CStatus update_scene(XSI::SIObject& si_object, const UpdateType update_type);
	XSI::CStatus update_scene(XSI::Material& xsi_material, bool material_assigning);
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

	void update_object(XSI::X3DObject& xsi_object);
	void update_instance_masters(ULONG xsi_id);

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
	//store here all instaces, we should update after general update process
	std::set<ULONG> update_instances;
	//this map used when we should reset some object in the Luxcore scene
	//the key - object id in XSI, value - corresponding names in Luxcore scene
	//polygonmesh objects can corresponds several object in scene
	std::unordered_map<ULONG, std::vector<std::string>> xsi_id_to_lux_names_map;
	//this map allows update instances when some master object is changed
	//key - id of the master object, values - array of ids of corresponding instances
	//different keys can contains the same set of instances, if (for example) instance contains several master objects
	std::unordered_map<ULONG, std::vector<ULONG>> master_to_instance_map;
	//the same list for exported materials
	std::set<ULONG> xsi_materials_in_lux;
	//should we reassign materials after scene creation
	//activate when we update material and this update contains reassign to some object
	//ids of pass environment shader node, whcich recognized as lights
	std::vector<ULONG> xsi_environment_in_lux;
	bool reinit_environments;
	//this map to each material with active shape node corresponds ids (in XSI) of polygonmeshes which use this material
	//when we change material in the keys list, then we should reexport all polygonmeshes
	std::unordered_map<ULONG, std::set<ULONG>> material_with_shape_to_polymesh_map;

	luxcore::Film::FilmOutputType lux_visual_output_type;
	luxcore::Film::FilmOutputType last_lux_visual_output_type;
};