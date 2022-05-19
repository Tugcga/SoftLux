#include "lux_scene.h"
#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "xsi_projectitem.h"

std::vector<float> get_instance_object_tfm(const XSI::CRefArray &children, ULONG index, const XSI::MATH::CTransformation &master_tfm, const XSI::MATH::CTransformation &model_tfm)
{
	XSI::X3DObject object(children[index]);

	XSI::MATH::CTransformation object_tfm = object.GetKinematics().GetGlobal().GetTransform();
	//we should calculate global position of the instance object
	XSI::MATH::CTransformation root_tfm_inverse;
	root_tfm_inverse.Invert(master_tfm);
	XSI::MATH::CTransformation object_root_tfm;
	object_root_tfm.Mul(object_tfm, root_tfm_inverse);
	XSI::MATH::CTransformation object_instance_tfm;
	object_instance_tfm.Mul(object_root_tfm, model_tfm);

	XSI::MATH::CMatrix4 object_matrix = object_instance_tfm.GetMatrix4();

	return xsi_to_lux_matrix_float(object_matrix);
}

void sync_instance(luxcore::Scene* scene, 
	ULONG model_id,
	std::string &model_id_str,
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
	const bool ignore_master_visibility,
	const bool is_branch,
	const std::vector<XSI::MATH::CTransformation>& time_transforms,
	const ULONG time_transforms_start)
{
	//time_transforms_start is a start index in the time_transforms array
	//for pointcloud this array contains time transforms for all point, so we should use transformations only for the given point
	XSI::CRefArray children;
	if (is_branch)
	{
		XSI::CStringArray str_families_subobject;
		children = master.FindChildren2("", "", str_families_subobject);
	}
	children.Add(master);

	XSI::MATH::CTransformation root_tfm = master.GetKinematics().GetGlobal().GetTransform();
	for (ULONG i = 0; i < children.GetCount(); i++)
	{
		XSI::X3DObject object(children[i]);
		ULONG xsi_id = object.GetObjectID();
		XSI::CString object_type = object.GetType();
		//TODO: add other types when exporter will supports it
		//the similar on lux_scene (in transform section) and export_common (in name section)
		if (object_type == "polymsh" || object_type == "hair")
		{
			std::vector<float> lux_matrix = get_instance_object_tfm(children, i, root_tfm, model_tfm);

			std::vector<std::string> object_names = xsi_object_id_string(object);
			if (!xsi_id_to_lux_names_map.contains(xsi_id))
			{
				//master object is not exported yet
				//this is because it outside of the isolation view, because in non-isolation mode we at first export geometry and only then - instances
				if (ignore_master_visibility || is_xsi_object_visible(eval_time, object))
				{
					bool is_sync = sync_object(scene, object, motion, xsi_materials_in_lux, xsi_id_to_lux_names_map, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time);
					if (is_sync)
					{
						xsi_id_to_lux_names_map[xsi_id] = xsi_object_id_string(object);

						std::vector<std::string> names = xsi_id_to_lux_names_map[xsi_id];
						//may these objects should not be exported
						for (ULONG j = 0; j < names.size(); j++)
						{
							names_to_delete.insert(names[j]);
						}

						names.clear();
						names.shrink_to_fit();
					}
				}
			}

			//add master id to the map
			master_to_instance_map[xsi_id].push_back(model_id);

			//check is we really export the object, or may be it was invisible
			if (xsi_id_to_lux_names_map.contains(xsi_id))
			{
				if (ignore_master_visibility || is_xsi_object_visible(eval_time, object))
				{
					for (ULONG j = 0; j < object_names.size(); j++)
					{
						std::string lux_name = object_names[j];
						std::string new_object_name = model_id_str + "_" + object_names[j];
						if (object_name_to_shape_name.contains(lux_name) && object_name_to_material_name.contains(lux_name))
						{
							//create new instance object by usng already created shape and material
							luxrays::Properties lux_props;
							lux_props.Set(luxrays::Property("scene.objects." + new_object_name + ".shape")(object_name_to_shape_name[lux_name]));
							lux_props.Set(luxrays::Property("scene.objects." + new_object_name + ".material")(object_name_to_material_name[lux_name]));
							//next sync transform
							lux_props.Set(luxrays::Property("scene.objects." + new_object_name + ".transformation")(lux_matrix));
							//and then sync motion
							bool is_motion = sync_instance_motion(lux_props, "scene.objects." + new_object_name, motion, time_transforms, time_transforms_start, master.GetKinematics().GetGlobal(), object.GetKinematics().GetGlobal(), eval_time);

							scene->Parse(lux_props);
						}
					}
				}
			}

			lux_matrix.clear();
			lux_matrix.shrink_to_fit();

			object_names.clear();
			object_names.shrink_to_fit();
		}
		else if (object_type == "light")
		{
			//TODO: make support of the light inside instance
			//for now only polygon mesh objects are supported
		}
	}
}

bool sync_instance(luxcore::Scene* scene,
	XSI::Model &xsi_model, 
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
	ULONG model_id = xsi_model.GetObjectID();
	//this array start from id of the model, and then contain names of subobkects
	//for example, if the mode has it 1458 and it contains three subobjects (12 with cluster materials 3 and 9, 127 with material 62 and 56 with material 3)
	//then it returns [1458_12_3, 1458_12_9, 1458_127_62, 1458_56_3]

	//get names of the master objects
	XSI::Model master = xsi_model.GetInstanceMaster();
	XSI::MATH::CTransformation model_tfm = xsi_model.GetKinematics().GetGlobal().GetTransform(eval_time);
	std::string model_id_str = std::to_string(model_id);

	std::vector<XSI::MATH::CTransformation> time_transforms = build_time_transforms(xsi_model.GetKinematics().GetGlobal(), motion, eval_time);
	sync_instance(scene, model_id, model_id_str, master, motion, model_tfm, xsi_id_to_lux_names_map, xsi_materials_in_lux, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time, false, true, time_transforms);

	return true;
}