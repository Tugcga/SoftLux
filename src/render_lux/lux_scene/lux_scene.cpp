#include "../../utilities/export_common.h"
#include "../../utilities/arrays.h"
#include "../../utilities/logs.h"

#include "lux_scene.h"

#include "xsi_application.h"
#include "xsi_model.h"

#include <set>

bool sync_object(luxcore::Scene* scene, 
	XSI::X3DObject &xsi_object, 
	MotionParameters& motion,
	std::set<ULONG>& xsi_materials_in_lux, 
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map, 
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map,
	std::unordered_map<ULONG, std::set<ULONG>> &material_with_shape_to_polymesh_map,
	std::set<std::string> &names_to_delete,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time)
{
	XSI::CString xsi_type = xsi_object.GetType();
	if (xsi_type == "polymsh")
	{
		return sync_polymesh(scene, xsi_object, motion, xsi_materials_in_lux, material_with_shape_to_polymesh_map, object_name_to_shape_name, object_name_to_material_name, eval_time);
	}
	else if (xsi_type == "pointcloud")
	{
		if (is_pointcloud_strands(xsi_object, eval_time))
		{
			return sync_pointcloud_strands(scene, xsi_object, motion, xsi_materials_in_lux, object_name_to_shape_name, object_name_to_material_name, eval_time);
		}
		else
		{
			return sync_pointcloud(scene, xsi_object, motion, xsi_id_to_lux_names_map, xsi_materials_in_lux, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
		}
	}
	else if (xsi_type == "hair")
	{
		return sync_hair(scene, xsi_object, motion, xsi_materials_in_lux, object_name_to_shape_name, object_name_to_material_name, eval_time);
	}
	else if (xsi_type == "#model")
	{
		XSI::Model xsi_model(xsi_object);
		XSI::siModelKind model_kind = xsi_model.GetModelKind();
		if (model_kind == XSI::siModelKind_Instance)
		{
			return sync_instance(scene, xsi_model, motion, xsi_id_to_lux_names_map, xsi_materials_in_lux, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
		}
		else
		{
			return false;
		}
	}
	else
	{
		//unsupported object type
	}

	return false;
}

void sync_models(luxcore::Scene* scene, 
	const XSI::CRefArray& xsi_models_list, 
	MotionParameters& motion,
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map,
	std::set<ULONG>& xsi_materials_in_lux, 
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map,
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::set<std::string>& names_to_delete,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time)
{
	for (ULONG i = 0; i < xsi_models_list.GetCount(); i++)
	{
		XSI::Model xsi_model = xsi_models_list[i];
		XSI::siModelKind model_kind = xsi_model.GetModelKind();
		if (model_kind == XSI::siModelKind_Instance && is_xsi_object_visible(eval_time, xsi_model))
		{
			bool is_sync = sync_instance(scene, xsi_model, motion, xsi_id_to_lux_names_map, xsi_materials_in_lux, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
			if (is_sync)
			{
				xsi_id_to_lux_names_map[xsi_model.GetObjectID()] = xsi_object_id_string(xsi_model);
			}
		}
	}
}

void sync_scene_objects(luxcore::Scene* scene, 
	const XSI::CRefArray& xsi_list, 
	XSI::siClassID class_id, 
	MotionParameters& motion,
	std::set<ULONG>& xsi_materials_in_lux, 
	std::unordered_map<ULONG, std::vector<std::string>> &xsi_id_to_lux_names_map,
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map,
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::set<std::string> &names_to_delete,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time)
{
	for (ULONG i = 0; i < xsi_list.GetCount(); i++)
	{
		XSI::X3DObject xsi_object;
		if (class_id == XSI::siX3DObjectID)
		{
			xsi_object = XSI::X3DObject(xsi_list[i]);
		}
		else if (class_id == XSI::siGeometryID)
		{
			XSI::SIObject si_ob(xsi_list[i]);
			si_ob = XSI::SIObject(si_ob.GetParent());
			si_ob = XSI::SIObject(si_ob.GetParent());
			xsi_object = XSI::X3DObject(si_ob);
		}

		//if polygonmesh contains clusters, then the whole list contains as mesh as clusters
		//we should export only mesh and ignore clusters
		ULONG xsi_id = xsi_object.GetObjectID();
		if (!xsi_id_to_lux_names_map.contains(xsi_id))
		{
			if (is_xsi_object_visible(eval_time, xsi_object))
			{
				bool is_sync = sync_object(scene, xsi_object, motion, xsi_materials_in_lux, xsi_id_to_lux_names_map, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
				if (is_sync)
				{
					//remember object in the map
					xsi_id_to_lux_names_map[xsi_id] = xsi_object_id_string(xsi_object);
				}
			}
		}
		else
		{
			if (is_xsi_object_visible(eval_time, xsi_object))
			{
				std::vector<std::string> names = xsi_id_to_lux_names_map[xsi_id];
				for (ULONG j = 0; j < names.size(); j++)
				{
					names_to_delete.erase(names[j]);
				}
			}
		}
	}
}

void sync_scene_objects(luxcore::Scene* scene, 
	XSI::RendererContext& xsi_render_context, 
	MotionParameters& motion,
	std::set<ULONG>& xsi_materials_in_lux, 
	std::unordered_map<ULONG, std::vector<std::string>> &xsi_id_to_lux_names_map, 
	std::unordered_map<ULONG, std::vector<ULONG>> &master_to_instance_map,
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time)
{
	std::set<std::string> names_to_delete;  // store here names of objects, that we may be should delete after export all the scene
	//because this is can be hidden master of the instances
	//we need it for creation instances, but after that should delete all these objects

	const XSI::CRefArray& xsi_isolation_list = xsi_render_context.GetArrayAttribute("ObjectList");
	if (xsi_isolation_list.GetCount() > 0)
	{//we are in isolation mode
		sync_scene_objects(scene, xsi_isolation_list, XSI::siX3DObjectID, motion, xsi_materials_in_lux, xsi_id_to_lux_names_map, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
	}
	else
	{//we should check all objects in the scene
		//for simplicity get all X3Dobjects and then filter by their types
		const XSI::CRefArray& xsi_objects_list = XSI::Application().FindObjects(XSI::siGeometryID);
		sync_scene_objects(scene, xsi_objects_list, XSI::siGeometryID, motion, xsi_materials_in_lux, xsi_id_to_lux_names_map, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);

		//also try to find model instances
		//in the isolation mode these models aready in the list
		const XSI::CRefArray& xsi_models_list = XSI::Application().FindObjects(XSI::siModelID);
		sync_models(scene, xsi_models_list, motion, xsi_id_to_lux_names_map, xsi_materials_in_lux, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
	}

	//next lights
	const XSI::CRefArray& xsi_scene_lights = xsi_render_context.GetArrayAttribute("Lights");
	for (ULONG i = 0; i < xsi_scene_lights.GetCount(); i++)
	{
		XSI::Light xsi_light(xsi_scene_lights.GetItem(i));
		sync_xsi_light(scene, xsi_light, eval_time);
	}

	std::set<std::string>::iterator it;
	for (it = names_to_delete.begin(); it != names_to_delete.end(); ++it)
	{
		std::string value = *it;
		scene->DeleteObject(value);

		//also delete corresponding id from xsi_id_to_lux_names_map
		int k = get_key_by_value(xsi_id_to_lux_names_map, value);
		if (k >= 0)
		{
			xsi_id_to_lux_names_map.erase(k);
		}
	}
}

void remove_from_scene(luxcore::Scene* scene, ULONG xsi_id, std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map)
{
	std::vector<std::string> names = xsi_id_to_lux_names_map[xsi_id];
	for (ULONG i = 0; i < names.size(); i++)
	{
		scene->DeleteObject(names[i]);
	}
	xsi_id_to_lux_names_map.erase(xsi_id);

	names.clear();
	names.shrink_to_fit();
}

void sync_transform(luxcore::Scene* scene, 
	const std::string &object_name, 
	const MotionParameters &motion,
	const XSI::KinematicState& kinematics,
	const XSI::CTime &eval_time,
	const bool force_tfm,
	const XSI::MATH::CTransformation& override_tfm)
{
	XSI::MATH::CTransformation xsi_tfm = kinematics.GetTransform(eval_time);
	std::vector<double> lux_matrix = xsi_to_lux_matrix(force_tfm ? override_tfm.GetMatrix4() : xsi_tfm.GetMatrix4());
	float matrix[16] = {};
	for (size_t i = 0; i < 16; i++)
	{
		matrix[i] = lux_matrix[i];
	}
	scene->UpdateObjectTransformation(object_name, matrix);

	lux_matrix.clear();
	lux_matrix.shrink_to_fit();
}

void sync_instance_transform(luxcore::Scene* scene, XSI::Model &xsi_model)
{
	std::string model_id_str = std::to_string(xsi_model.GetObjectID());
	XSI::Model master = xsi_model.GetInstanceMaster();
	XSI::CStringArray str_families_subobject;
	XSI::CRefArray children = master.FindChildren2("", "", str_families_subobject);
	XSI::MATH::CTransformation root_tfm = master.GetKinematics().GetGlobal().GetTransform();
	XSI::MATH::CTransformation model_tfm = xsi_model.GetKinematics().GetGlobal().GetTransform();
	for (ULONG i = 0; i < children.GetCount(); i++)
	{
		XSI::X3DObject object(children[i]);
		XSI::CString object_type = object.GetType();
		//TODO: add other types when exporter will supports it
		if (object_type == "polymsh" || object_type == "hair")
		{
			std::vector<float> lux_matrix = get_instance_object_tfm(children, i, root_tfm, model_tfm);

			std::vector<std::string> object_names = xsi_object_id_string(object);
			for (ULONG j = 0; j < object_names.size(); j++)
			{
				scene->UpdateObjectTransformation(model_id_str + "_" + object_names[j], &lux_matrix[0]);
			}

			lux_matrix.clear();
			lux_matrix.shrink_to_fit();
		}
	}
}