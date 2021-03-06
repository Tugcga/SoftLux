#include "render_engine_lux.h"
#include "../utilities/logs.h"
#include "../utilities/export_common.h"
#include "../utilities/arrays.h"
#include "lux_session/lux_session.h"

#include "xsi_primitive.h"
#include "xsi_application.h"
#include "xsi_project.h"
#include "xsi_library.h"
#include "xsi_renderchannel.h"
#include "xsi_framebuffer.h"
#include "xsi_projectitem.h"

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
	is_update_camera = false;
	prev_full_width = 0;
	prev_full_height = 0;
	prev_corner_x = 0;
	prev_corner_y = 0;
	prev_width = 0;
	prev_height = 0;
	updated_xsi_ids.clear();
	xsi_materials_in_lux.clear();
	xsi_environment_in_lux.clear();
	reinit_environments = false;
	lux_visual_output_type = luxcore::Film::OUTPUT_RGB_IMAGEPIPELINE;
	last_lux_visual_output_type = luxcore::Film::OUTPUT_RGB_IMAGEPIPELINE;

	xsi_id_to_lux_names_map.clear();
	master_to_instance_map.clear();
	material_with_shape_to_polymesh_map.clear();
	object_name_to_shape_name.clear();
	object_name_to_material_name.clear();
	session_channels_set.clear();

	prev_motion = { false, 1.0f, 2 };
	prev_service_aov = { false, false };
	prev_service_strands = { 3, true, 12, true, false, 8, 0.1f };
	prev_world_volume = false;
	session_output_samples = 0.0f;
}

//when we delete the engine, then at first this method is called, and then the method from base class
RenderEngineLux::~RenderEngineLux()
{

}

void RenderEngineLux::lux_log_handler(const char* msg)
{
	XSI::Application().LogMessage(XSI::CString(msg));
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
		luxcore::Init();
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
	xsi_materials_in_lux.clear();
	xsi_environment_in_lux.clear();
	xsi_id_to_lux_names_map.clear();
	master_to_instance_map.clear();
	material_with_shape_to_polymesh_map.clear();
	object_name_to_shape_name.clear();
	object_name_to_material_name.clear();
}

//here we clear session and render config
void RenderEngineLux::clear_session()
{
	if (is_session)
	{
		delete session;
		session = NULL;
		is_session = false;
		session_channels_set.clear();
	}
}

//called every time before scene update
//here we should setup parameters, which should be done for both situations: create scene from scratch or update the scene
XSI::CStatus RenderEngineLux::pre_scene_process()
{
	try_to_init();

	//activate the log
	if (render_type == RenderType_Shaderball)
	{
		luxcore::SetLogHandler();
	}
	else
	{
		//RenderEngineLux::is_log = false;
		bool is_log_luxcore = m_render_parameters.GetValue("service_log_luxcore", eval_time);
		if (is_log_luxcore)
		{
			luxcore::SetLogHandler(RenderEngineLux::lux_log_handler);
			luxcore::SetEnableLogSubSystem(luxcore::LOG_LUXRAYS, m_render_parameters.GetValue("service_log_luxcore_luxrays", eval_time));
			luxcore::SetEnableLogSubSystem(luxcore::LOG_SDL, m_render_parameters.GetValue("service_log_luxcore_sdl", eval_time));
			luxcore::SetEnableLogSubSystem(luxcore::LOG_SLG, m_render_parameters.GetValue("service_log_luxcore_slg", eval_time));
			luxcore::SetEnableLogSubSystem(luxcore::LOG_API, m_render_parameters.GetValue("service_log_luxcore_api", eval_time));
		}
		else
		{
			luxcore::SetLogHandler();
		}
	}

	//if we change region limits (corners or render size), then we should recreate the session, so, delete it
	if (prev_corner_x != visual_buffer.corner_x || prev_corner_y != visual_buffer.corner_y ||
		prev_width != visual_buffer.width || prev_height != visual_buffer.height ||
		prev_full_width != visual_buffer.full_width || prev_full_height != visual_buffer.full_height ||
		render_type == RenderType_Rendermap)
	{
		clear_session();
		prev_corner_x = visual_buffer.corner_x;
		prev_corner_y = visual_buffer.corner_y;
		prev_width = visual_buffer.width;
		prev_height = visual_buffer.height;
		prev_full_width = visual_buffer.full_width;
		prev_full_height = visual_buffer.full_height;
	}

	is_update_camera = false;

	updated_xsi_ids.clear();
	update_instances.clear();

	//get visual renderbuffer
	XSI::Framebuffer frame_buffer = m_render_context.GetDisplayFramebuffer();
	XSI::RenderChannel render_channel = frame_buffer.GetRenderChannel();
	lux_visual_output_type = output_string_to_lux(render_channel.GetName());

	if (lux_visual_output_type != last_lux_visual_output_type)
	{
		clear_session();
		last_lux_visual_output_type = lux_visual_output_type;
	}

	//check output extension for rendermap
	if (render_type == RenderType_Rendermap)
	{
		for (ULONG i = 0; i < output_formats.GetCount(); i++)
		{
			XSI::CString ext = output_formats[i];
			if (!is_image_format_supported(ext))
			{
				//output extension is not supported, replace it to exr
				log_message("Invalid image extension " + ext + " for the rendermap output. Change it to exr.", XSI::siWarningMsg);
				XSI::CString output_path = output_paths[i];
				ULONG dot_index = output_path.ReverseFindString(".");
				output_path = output_path.GetSubString(0, dot_index) + ".exr";
				ext = "exr";

				output_formats[i] = ext;
				output_paths[i] = output_path;
				output_data_types[i] = "RGBA";
				output_bits[i] = 21;
			}
		}
	}

	return XSI::CStatus::OK;
}

void RenderEngineLux::update_instance_masters(ULONG xsi_id)
{
	if (master_to_instance_map.contains(xsi_id))
	{
		//this object is the master object for some instance
		//we should delete corresponding instances and create it again
		for (ULONG i = 0; i < master_to_instance_map[xsi_id].size(); i++)
		{
			update_instances.insert(master_to_instance_map[xsi_id][i]);
		}
	}
}

void RenderEngineLux::update_object(XSI::X3DObject& xsi_object)
{
	ULONG xsi_id = xsi_object.GetObjectID();
	if (xsi_id_to_lux_names_map.contains(xsi_id))
	{
		std::vector<std::string> object_names = xsi_id_to_lux_names_map[xsi_object.GetObjectID()];
		for (ULONG i = 0; i < object_names.size(); i++)
		{
			scene->DeleteObject(object_names[i]);
		}

		object_names.clear();
		object_names.shrink_to_fit();

		xsi_id_to_lux_names_map.erase(xsi_object.GetObjectID());
	}
	//sync object again
	if (is_xsi_object_visible(eval_time, xsi_object))
	{
		std::set<std::string> names_to_delete;
		bool is_sync = sync_object(scene, xsi_object, m_render_parameters, xsi_materials_in_lux, xsi_id_to_lux_names_map, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
		if (is_sync)
		{
			ULONG xsi_id = xsi_object.GetObjectID();
			updated_xsi_ids.push_back(xsi_id);
			xsi_id_to_lux_names_map[xsi_id] = xsi_object_id_string(xsi_object);
		}
	}
	update_instance_masters(xsi_id);
}

//return OK, if object successfully updates, Abort in other case
XSI::CStatus RenderEngineLux::update_scene(XSI::X3DObject& xsi_object, const UpdateType update_type)
{
	if (m_isolation_list.GetCount() > 0)
	{
		//check that updated object in the isolation view
		//if so, update it, otherwise - return abort
		if (!is_isolation_list_contains_object(m_isolation_list, xsi_object))
		{
			//or may we should simply ignore this update call?
			return XSI::CStatus::Abort;
		}
	}

	if (is_scene_create)
	{
		if (!is_contains(updated_xsi_ids, xsi_object.GetObjectID()))
		{
			if (update_type == UpdateType_Camera)
			{
				sync_camera(scene, render_type, m_render_context, m_render_parameters, eval_time);
				is_update_camera = true;

				return XSI::CStatus::OK;
			}
			else if (update_type == UpdateType_Visibility)
			{
				//change object visibility
				//delete the object
				XSI::CString xsi_type = xsi_object.GetType();
				if (xsi_type == "light")
				{
					bool is_sync = update_light_object(scene, xsi_object, m_render_parameters, eval_time);
					if (is_sync)
					{
						updated_xsi_ids.push_back(xsi_object.GetObjectID());
					}
				}
				else
				{
					if (xsi_object.GetType() == "pointcloud" && !is_pointcloud_strands(xsi_object, eval_time))
					{
						//we can not delete all object from particles, because one pointcloud create mny objects in Luxcore scene
						//so, simply clear the whole scene
						clear_scene();
						return XSI::CStatus::Abort;
					}
					else
					{
						//only plygonmesh object in XSI corresponds to several objects in Luxcore
						//each of the has the name: id_materialID
						update_object(xsi_object);
					}
				}
				return XSI::CStatus::OK;
			}
			else if (update_type == UpdateType_GlobalAmbient)
			{
				sync_ambient(scene, eval_time);
			}
			else if(update_type == UpdateType_Transform)
			{
				//if motion blur is active, recreate the scene
				//because we can change the position not only of the object, but master of the instance
				//in this case we should reassign new motion for all instances, but there are some problems with this
				//update transform reset motion transforms and update motion crash the render
				if (prev_motion.motion_objects)
				{
					return XSI::CStatus::Abort;
				}
				//does not remember the object, because we can update mesh of the object later
				//here we only change it transform
				ULONG xsi_id = xsi_object.GetObjectID();
				XSI::CString xsi_type = xsi_object.GetType();
				if (xsi_type == "light")
				{
					//completely update the light
					bool is_sync = update_light_object(scene, xsi_object, m_render_parameters, eval_time);
					if (is_sync)
					{
						updated_xsi_ids.push_back(xsi_object.GetObjectID());
					}
				}
				else if (xsi_type == "#model")
				{
					XSI::Model xsi_model(xsi_object);
					XSI::siModelKind model_kind = xsi_model.GetModelKind();
					if (model_kind == XSI::siModelKind_Instance && xsi_id_to_lux_names_map.contains(xsi_id))
					{
						sync_instance_transform(scene, xsi_model);
					}
					//ignore change transform of all other models
				}
				else
				{
					//for other objects in the scene, update only transform
					if (xsi_id_to_lux_names_map.contains(xsi_id))
					{
						std::vector<std::string> object_names = xsi_id_to_lux_names_map[xsi_object.GetObjectID()];
						for (ULONG i = 0; i < object_names.size(); i++)
						{
							sync_transform(scene, object_names[i], m_render_parameters, xsi_object.GetKinematics().GetGlobal(), eval_time);
						}
						object_names.clear();
						object_names.shrink_to_fit();

						update_instance_masters(xsi_id);
					}
					else
					{
						return XSI::CStatus::Abort;
					}
				}
				
				return XSI::CStatus::OK;
			}
			else if (update_type == UpdateType_XsiLight)
			{
				bool is_sync = update_light_object(scene, xsi_object, m_render_parameters, eval_time);
				if (is_sync)
				{
					updated_xsi_ids.push_back(xsi_object.GetObjectID());
				}
			}
			else if (update_type == UpdateType_Mesh)
			{
				//here we delete corresponding objects form Luxcore scene and create the mesh again
				update_object(xsi_object);
			}
			else if (update_type == UpdateType_Pointcloud)
			{
				//for simplicity, if we change pointcloud, reset the scene
				//also clear the scene to prevent all other updates
				if (is_pointcloud_strands(xsi_object, eval_time))
				{
					update_object(xsi_object);
				}
				else
				{
					clear_scene();
					return XSI::CStatus::Abort;
				}
			}
			else if (update_type == UpdateType_Hair)
			{
				update_object(xsi_object);
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

XSI::CStatus RenderEngineLux::update_scene(XSI::SIObject& si_object, const UpdateType update_type)
{
	if (is_scene_create)
	{
		if (update_type == UpdateType_Pass)
		{
			//update pass, may be change output nodes for image pipline
			//so, we should recreate settings and session from scratch
			clear_session();

			//also we can change environment light
			//we delete each light and then recreate all of them
			for (ULONG i = 0; i < xsi_environment_in_lux.size(); i++)
			{
				ULONG id = xsi_environment_in_lux[i];
				std::string id_name = std::to_string(id);

				scene->DeleteLight(id_name);
			}

			reinit_environments = true;
		}

		return XSI::CStatus::OK;
	}
	else
	{
		return XSI::CStatus::Abort;
	}
}

XSI::CStatus RenderEngineLux::update_scene(XSI::Material& xsi_material, bool material_assigning)
{
	if (is_scene_create)
	{
		//here we update only materials
		sync_material(scene, xsi_material, xsi_materials_in_lux, eval_time);
		if (material_assigning)
		{
			//get host object
			XSI::X3DObject host_object = xsi_material.GetParent3DObject();
			ULONG xsi_id = host_object.GetObjectID();
			if (!is_contains(updated_xsi_ids, xsi_id))
			{
				update_object(host_object);
			}
		}

		ULONG material_id = xsi_material.GetObjectID();
		if (material_with_shape_to_polymesh_map.contains(material_id))
		{
			for (auto it = material_with_shape_to_polymesh_map[material_id].begin(); it != material_with_shape_to_polymesh_map[material_id].end(); ++it)
			{
				ULONG mesh_id = *it;
				XSI::X3DObject mesh_object(XSI::Application().GetObjectFromID(mesh_id));
				if (mesh_object.IsValid())
				{
					update_object(mesh_object);
				}
			}
		}

		return XSI::CStatus::OK;
	}
	else
	{
		return XSI::CStatus::Abort;
	}
	
}

MotionParameters RenderEngineLux::read_motion_params()
{
	MotionParameters motion =
	{
		m_render_parameters.GetValue("motion_objects", eval_time),
		m_render_parameters.GetValue("motion_shutter_time", eval_time),
		m_render_parameters.GetValue("motion_steps", eval_time)
	};
	return motion;
}

ServiceAOVParameters RenderEngineLux::read_service_aov_params()
{
	ServiceAOVParameters service_aov =
	{
		m_render_parameters.GetValue("service_export_pointness", eval_time),
		m_render_parameters.GetValue("service_export_random_islands", eval_time)
	};
	return service_aov;
}

ServiceStrandsParameters RenderEngineLux::read_service_strands_params()
{
	ServiceStrandsParameters service_strands =
	{
		m_render_parameters.GetValue("service_strands_type", eval_time),
		m_render_parameters.GetValue("service_strands_use_camera", eval_time),
		m_render_parameters.GetValue("service_strands_sidecount", eval_time),
		m_render_parameters.GetValue("service_strands_cap_top", eval_time),
		m_render_parameters.GetValue("service_strands_cap_bottom", eval_time),
		m_render_parameters.GetValue("service_strands_adaptive_maxdepth", eval_time),
		m_render_parameters.GetValue("service_strands_adaptive_error", eval_time)
	};

	return service_strands;
}

XSI::CStatus RenderEngineLux::update_scene_render()
{
	//if we change render parameters, then we should recreate the session
	//we clear it, and in post_scene() call we recreate it 
	clear_session();

	//if we change one of motion properties, then we should recreate the scene from scratch
	//because we should setup motion for all object in the scene (or disable it)
	MotionParameters current_motion = read_motion_params();
	if (prev_motion.is_changed(current_motion))
	{
		prev_motion = current_motion;
		return XSI::CStatus::Abort;
	}
	prev_motion = current_motion;

	ServiceAOVParameters current_service_aov = read_service_aov_params();
	if (prev_service_aov.is_changed(current_service_aov))
	{
		prev_service_aov = current_service_aov;
		return XSI::CStatus::Abort;
	}
	prev_service_aov = current_service_aov;

	ServiceStrandsParameters current_service_strands = read_service_strands_params();
	if (prev_service_strands.is_changed(current_service_strands))
	{
		prev_service_strands = current_service_strands;
		return XSI::CStatus::Abort;
	}
	prev_service_strands = current_service_strands;
	
	return XSI::CStatus::OK;
}

//here we create the scene for rendering from scratch
XSI::CStatus RenderEngineLux::create_scene()
{
	clear_scene();
	clear_session();
	scene = luxcore::Scene::Create();
	is_scene_create = true;
	reinit_environments = false;
	prev_motion = read_motion_params();
	prev_service_aov = read_service_aov_params();
	prev_service_strands = read_service_strands_params();

	//always update the camera
	sync_camera(scene, render_type, m_render_context, m_render_parameters, eval_time);
	is_update_camera = true;

	if (render_type == RenderType_Shaderball)
	{
		//for shaderball render use other sync method
		XSI::CRef shaderball_item = m_render_context.GetAttribute("Material");
		XSI::siClassID shaderball_item_class = shaderball_item.GetClassID();
		if (shaderball_item_class == XSI::siMaterialID)
		{
			XSI::Material shaderball_material(shaderball_item);
			sync_material(scene, shaderball_material, xsi_materials_in_lux, eval_time);
			sync_shaderball(scene, m_render_context, m_render_parameters, xsi_id_to_lux_names_map, material_with_shape_to_polymesh_map, eval_time, shaderball_material.GetObjectID());
		}
		else
		{
			//preview shader node
			render_type == RenderType_Unknown;
			//at the next render session we will recreate the scene
		}
	}
	else
	{
		//sync materials
		sync_materials(scene, XSI::Application().GetActiveProject().GetActiveScene(), xsi_materials_in_lux, eval_time);

		//ambient light
		sync_ambient(scene, eval_time);

		//sync scene objects
		sync_scene_objects(scene, m_render_context, m_render_parameters, xsi_materials_in_lux, xsi_id_to_lux_names_map, master_to_instance_map, material_with_shape_to_polymesh_map, object_name_to_shape_name, object_name_to_material_name, eval_time);

		//environment light (hdri, sky, sun)
		bool is_world_volume = false;
		xsi_environment_in_lux = sync_environment(scene, is_world_volume, eval_time);

		prev_world_volume = is_world_volume;
	}

	return XSI::CStatus::OK;
}

//call this method after scene created or updated but before unlock
XSI::CStatus RenderEngineLux::post_scene()
{
	//update camera, if we did not do it earlier
	if (!is_update_camera)
	{
		sync_camera(scene, render_type, m_render_context, m_render_parameters, eval_time);
		is_update_camera = true;
	}

	if (reinit_environments)
	{
		xsi_environment_in_lux.clear();
		bool is_world_volume = false;
		xsi_environment_in_lux = sync_environment(scene, is_world_volume, eval_time);

		//after update we disable the world volume, but at the previous render session it was used
		//so, recreate the scene
		if (prev_world_volume && !is_world_volume)
		{
			return XSI::CStatus::Abort;
		}
		prev_world_volume = is_world_volume;
	}
	reinit_environments = false;

	//may be we should update instances
	std::set<unsigned long>::iterator it;
	for (it = update_instances.begin(); it != update_instances.end(); ++it) 
	{
		ULONG xsi_id = *it;
		//remove instance
		remove_from_scene(scene, xsi_id, xsi_id_to_lux_names_map);

		//next create it again
		XSI::X3DObject xsi_instance(XSI::Application().GetObjectFromID(xsi_id));
		std::set<std::string> names_to_delete;
		bool is_sync = sync_object(scene, xsi_instance, m_render_parameters, xsi_materials_in_lux, xsi_id_to_lux_names_map, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
		if (is_sync)
		{
			xsi_id_to_lux_names_map[xsi_instance.GetObjectID()] = xsi_object_id_string(xsi_instance);
		}
		//remove names to delete
		std::set<std::string>::iterator names_it;
		for (names_it = names_to_delete.begin(); names_it != names_to_delete.end(); ++names_it)
		{
			std::string name = *names_it;
			scene->DeleteObject(name);
		}
	}

	update_instances.clear();

	//here we should create the session and render parameters
	if (!is_session)
	{
		//get rendermap settings
		int bake_uv_index = 0;
		std::vector<std::string> bake_object_names;
		int bake_mode = 0;  // combined
		if (render_type == RenderType_Rendermap)
		{
			//override bake parameters
			XSI::CRefArray rendermap_list = m_render_context.GetAttribute("RenderMapList");
			if (rendermap_list.GetCount() > 0)
			{
				XSI::Property rendermap_prop(rendermap_list[0]);
				setup_mesh_bake_settings(rendermap_prop, bake_uv_index, bake_object_names, eval_time);

				int randermap_attribute = rendermap_prop.GetParameterValue("attribute", eval_time);
				if (randermap_attribute != 0)
				{
					bake_mode = 1;  // lightmap
				}
			}
		}

		session_channels_set.clear();
		bool is_skip = m_render_context.GetAttribute("SkipExistingFiles");
		session = sync_render_config(scene, 
			session_channels_set,
			render_type, m_render_property, eval_time,
			lux_visual_output_type, output_channels, output_paths, archive_folder, XSI::Application().GetActiveProject().GetActiveScene().GetName(),
			image_corner_x, image_corner_x + image_size_width, image_corner_y, image_corner_y + image_size_height,
			image_full_size_width, image_full_size_height,
			bake_uv_index, bake_object_names, is_skip, bake_mode);
		is_session = true;

		//for rendermap we should not save output image, because buffer does not contains anything
		//the Luxcore render save it himself by using bake.maps.i.filename property
		//so, clear outputs list
		if (render_type == RenderType_Rendermap)
		{
			output_paths.Clear();
		}
	}

	return XSI::CStatus::OK;
}

void RenderEngineLux::render()
{
	session_output_samples = 0.0f;
	if (render_type == RenderType_Export)
	{
		export_scene(session, m_render_context, archive_folder, m_render_property.GetParameterValue("export_mode", eval_time), XSI::Application().GetActiveProject().GetActiveScene().GetName());
	}
	else
	{
		m_render_context.ProgressUpdate("Rendering...", "Rendering...", 0);

		session->Start();

		const luxrays::Properties& stats = session->GetStats();
		const unsigned int halt_time = session->GetRenderConfig().GetProperties().Get(luxrays::Property("batch.halttime")(0)).Get<unsigned int>();
		const unsigned int halt_spp = session->GetRenderConfig().GetProperties().Get(luxrays::Property("batch.haltspp")(0)).Get<unsigned int>();
		luxcore::Film& film = session->GetFilm();
		float service_update = m_render_parameters.GetValue("service_update", eval_time);  // in seconds
		int service_update_mls = (int)(service_update * 1000.0f);
		int halt_condition = m_render_parameters.GetValue("halt_condition", eval_time);
		const double buffer_size_double = visual_buffer.width * visual_buffer.height;
		while (!session->HasDone())
		{
			session->UpdateStats();

			const double elapsed_time = stats.Get("stats.renderengine.time").Get<double>();
			const unsigned int pass = stats.Get("stats.renderengine.pass").Get<unsigned int>();
			const double total_samples = stats.Get("stats.renderengine.total.samplecount").Get<double>();
			const float convergence = stats.Get("stats.renderengine.convergence").Get<unsigned int>();

			session_output_samples = total_samples / buffer_size_double;

			if (render_type != RenderType_Rendermap && session_channels_set.contains(lux_visual_output_type))
			{
				read_visual_buffer(film, lux_visual_output_type, visual_buffer, true);  // execute ip, but with active non-optix denoising it spend many times
				m_render_context.NewFragment(RenderTile(visual_buffer.corner_x, visual_buffer.corner_y, visual_buffer.width, visual_buffer.height, visual_buffer.pixels, false, 4));
			}

			if (halt_condition == 0)
			{//by samples
				m_render_context.ProgressUpdate("Rendering...", "Rendering...", int((session_output_samples * 100.0f / (float)halt_spp)));
			}
			else if (halt_condition == 1)
			{//by time
				m_render_context.ProgressUpdate("Rendering...", "Rendering...", int((elapsed_time * 100.0f / (float)halt_time)));
			}
			else
			{//by noise level
				m_render_context.ProgressUpdate("Rendering...", "Rendering...", 0);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(service_update_mls));
		}

		session->Stop();

		//after render fill the buffer at last time
		if (render_type != RenderType_Rendermap && session_channels_set.contains(lux_visual_output_type))
		{
			read_visual_buffer(film, lux_visual_output_type, visual_buffer, true);
			m_render_context.NewFragment(RenderTile(visual_buffer.corner_x, visual_buffer.corner_y, visual_buffer.width, visual_buffer.height, visual_buffer.pixels, false, 4));
		}

		//after render is finish, copy all rendered pixels from film to output_pixels
		//next in post_render (before post_render_engine) we save it into images
		copy_film_to_output_pixels(film, output_pixels, output_channels, session_channels_set);
	}
}

XSI::CStatus RenderEngineLux::post_render_engine()
{
	//log render time
	double time = (finish_render_time - start_prepare_render_time) / CLOCKS_PER_SEC;
	if (render_type != RenderType_Shaderball)
	{
		if (render_type == RenderType_Export)
		{
			log_message("Export time: " + XSI::CString(time) + " sec.");
		}
		else
		{
			log_message("Render statistics: " + XSI::CString((int)session_output_samples) + " samples, " + XSI::CString(time) + " seconds");
		}
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