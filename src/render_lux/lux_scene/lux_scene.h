#pragma once
#include "../../render_base/render_engine_base.h"
#include "../../utilities/math.h"
#include "../../utilities/xsi_shaders.h"

#include <luxcore/luxcore.h>

#include "xsi_camera.h"
#include "xsi_time.h"
#include "xsi_renderercontext.h"
#include "xsi_scene.h"
#include "xsi_transformation.h"
#include "xsi_application.h"
#include "xsi_primitive.h"
#include "xsi_light.h"
#include "xsi_model.h"

#include <set>
#include <unordered_map>

struct MotionParameters
{
	bool motion_objects;
	float motion_shutter_time;
	int motion_steps;

	bool is_changed(MotionParameters& other)
	{
		return motion_objects != other.motion_objects ||
			motion_shutter_time != other.motion_shutter_time ||
			motion_steps != other.motion_steps;
	}
};

void set_lux_camera_positions(luxrays::Properties& camera_props, const XSI::MATH::CVector3& xsi_position, const XSI::MATH::CVector3& xsi_target_position);

//return true if we add the object to the scene, and false in other case (unsupported object, for example)
bool sync_object(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object,
	MotionParameters& motion,
	std::set<ULONG>& xsi_materials_in_lux, 
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map, 
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map, 
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::set<std::string>& names_to_delete,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time);

void sync_shaderball(luxcore::Scene* scene, 
	XSI::RendererContext& xsi_render_context, 
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map, 
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	const XSI::CTime& eval_time, 
	const ULONG override_material_id);

void sync_scene_objects(luxcore::Scene* scene, 
	XSI::RendererContext& xsi_render_context, 
	MotionParameters &motion,
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<ULONG, std::vector<std::string>> &xsi_id_to_lux_names_map, 
	std::unordered_map<ULONG, std::vector<ULONG>> &master_to_instance_map, 
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time);

bool sync_motion(luxrays::Properties& lux_props,
	const std::string& prefix,
	const MotionParameters& motion,
	const XSI::KinematicState& xsi_kinematic,
	const XSI::CTime& eval_time);

bool sync_instance_motion(luxrays::Properties& lux_props,
	const std::string& prefix,
	const MotionParameters& motion,
	const std::vector<XSI::MATH::CTransformation>& time_transforms,
	const ULONG time_start_index,
	const XSI::KinematicState& master_root_kine,
	const XSI::KinematicState& master_object_kine,
	const XSI::CTime& eval_time);

std::vector<XSI::MATH::CTransformation> build_time_transforms(const XSI::KinematicState& xsi_kine,
	const MotionParameters& motion,
	const XSI::CTime& eval_time);

std::vector<XSI::MATH::CTransformation> build_time_points_transforms(const XSI::X3DObject& particles,
	const MotionParameters& motion,
	const XSI::CTime& eval_time);

void sync_shaderball_back_material(luxcore::Scene* scene);

void sync_default_material(luxcore::Scene* scene);

void override_material(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object,
	const std::string material_name);

//if ignore_set_branch is true, then only get connections, but does not set values for free ports
void set_material_value(luxcore::Scene* scene, 
	luxrays::Properties& material_props, 
	const XSI::CString& xsi_param_name, 
	const std::string& lux_param_name, 
	XSI::CParameterRefArray& parameters, 
	std::unordered_map<ULONG, std::string>& exported_nodes_map,
	const XSI::CTime& eval_time,
	bool ignore_set_branch = false);

std::string add_material(luxcore::Scene* scene, 
	luxrays::Properties &material_props,
	XSI::Shader& material_node, 
	std::unordered_map<ULONG, std::string>& exported_nodes_map,
	const XSI::CTime& eval_time, 
	std::string override_name = "");

std::string add_shape(luxcore::Scene* scene, 
	std::string& input_shape_name,
	XSI::Shader& node, 
	std::unordered_map<ULONG, std::string>& exported_nodes_map,
	const bool ignore_subdivision,
	const XSI::CTime& eval_time);

std::string sync_polymesh_shapes(luxcore::Scene* scene, 
	std::string& input_shape_name, 
	XSI::Material& xsi_material,
	const bool ignore_subdivision,
	const XSI::CTime& eval_time);

void sync_material(luxcore::Scene* scene, 
	XSI::Material& xsi_material, 
	std::set<ULONG>& xsi_materials_in_lux, 
	const XSI::CTime& eval_time);

void sync_materials(luxcore::Scene* scene, 
	const XSI::Scene& xsi_scene, 
	std::set<ULONG>& xsi_materials_in_lux, 
	const XSI::CTime& eval_time);

void reassign_all_materials(luxcore::Scene* scene, 
	const XSI::Scene& xsi_scene, 
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map, 
	const XSI::CTime& eval_time);

void define_area_light_mesh(luxcore::Scene* scene, 
	const std::string& shape_name);

bool sync_pointcloud(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object, 
	MotionParameters& motion,
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map,
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map,
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::set<std::string>& names_to_delete,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time);

bool sync_polymesh(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object, 
	MotionParameters& motion,
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<ULONG, std::set<ULONG>> &material_with_shape_to_polymesh_map,
	std::unordered_map<std::string, std::string> &object_name_to_shape_name,
	std::unordered_map<std::string, std::string> &object_name_to_material_name,
	const XSI::CTime& eval_time, 
	const ULONG override_material = 0, 
	const bool use_default_material = false);

std::vector<float> get_instance_object_tfm(const XSI::CRefArray& children, ULONG index, const XSI::MATH::CTransformation& master_tfm, const XSI::MATH::CTransformation& model_tfm);

//this method use from pointcloud export
void sync_instance(luxcore::Scene* scene,
	ULONG model_id,
	std::string& model_id_str,
	XSI::X3DObject &master,
	MotionParameters& motion,
	XSI::MATH::CTransformation& model_tfm,
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map,
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map,
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::set<std::string>& names_to_delete,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time,
	const bool ignore_master_visibility = false,
	const bool is_branch = true,
	const std::vector<XSI::MATH::CTransformation> &time_transforms = std::vector<XSI::MATH::CTransformation>(0),
	const ULONG time_transforms_start = 0);

bool sync_pointcloud_strands(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object, 
	MotionParameters& motion,
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time);

bool sync_hair(luxcore::Scene* scene,
	XSI::X3DObject& xsi_object,
	MotionParameters& motion,
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time);

//this method used for instances, next it call the more general method
bool sync_instance(luxcore::Scene* scene, 
	XSI::Model& xsi_model, 
	MotionParameters& motion,
	std::unordered_map<ULONG, 
	std::vector<std::string>>& xsi_id_to_lux_names_map,
	std::set<ULONG>& xsi_materials_in_lux, 
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map, 
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::set<std::string>& names_to_delete,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time);

void sync_shaderball_lights(luxcore::Scene* scene);

void sync_shaderball_imagepipline(luxrays::Properties& render_props, 
	const XSI::CTime& eval_time);

bool sync_xsi_light(luxcore::Scene* scene,
	XSI::Light& xsi_light,
	const XSI::CTime& eval_time);

bool update_light_object(luxcore::Scene* scene,
	XSI::X3DObject& xsi_object,
	const XSI::CTime& eval_time);

void sync_ambient(luxcore::Scene* scene, 
	const XSI::CTime& eval_time);

std::vector<ULONG> sync_environment(luxcore::Scene* scene, 
	const XSI::CTime& eval_time);

void sync_camera_scene(luxcore::Scene* scene, 
	const XSI::Camera& xsi_camera, 
	MotionParameters& motion,
	const XSI::CTime& eval_time);

void sync_camera_shaderball(luxcore::Scene* scene);

void sync_camera(luxcore::Scene* scene, 
	const RenderType render_type,
	XSI::RendererContext& xsi_render_context,
	MotionParameters& motion,
	const XSI::CTime& eval_time);

void remove_from_scene(luxcore::Scene* scene, 
	ULONG xsi_id, 
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map);

void sync_transform(luxcore::Scene* scene, 
	const std::string& object_name,
	const MotionParameters& motion,
	const XSI::KinematicState &kinematics,
	const XSI::CTime& eval_time,
	const bool force_tfm = false,
	const XSI::MATH::CTransformation &override_tfm = XSI::MATH::CTransformation());

void sync_instance_transform(luxcore::Scene* scene,
	XSI::Model& xsi_model);