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

bool sync_instance(luxcore::Scene* scene, XSI::Model &xsi_model, 
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map, 
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map,
	const XSI::CTime& eval_time)
{
	//std::vector<std::string> model_names = xsi_object_id_string(xsi_model);
	ULONG model_id = xsi_model.GetObjectID();
	std::string model_id_str = std::to_string(xsi_model.GetObjectID());
	//this array start from id of the model, and then contain names of subobkects
	//for example, if the mode has it 1458 and it contains three subobjects (12 with cluster materials 3 and 9, 127 with material 62 and 56 with material 3)
	//then it returns [1458_12_3, 1458_12_9, 1458_127_62, 1458_56_3]

	//get names of the master objects
	XSI::Model master = xsi_model.GetInstanceMaster();
	XSI::CStringArray str_families_subobject;
	XSI::CRefArray children = master.FindChildren2("", "", str_families_subobject);
	XSI::MATH::CTransformation root_tfm = master.GetKinematics().GetGlobal().GetTransform();
	XSI::MATH::CTransformation model_tfm = xsi_model.GetKinematics().GetGlobal().GetTransform();
	for (ULONG i = 0; i < children.GetCount(); i++)
	{
		XSI::X3DObject object(children[i]);
		ULONG xsi_id = object.GetObjectID();
		XSI::CString object_type = object.GetType();
		//TODO: add other types when exporter will supports it
		//the similar on lux_scene (in transform section) and export_common (in nema section)
		if (object_type == "polymsh")
		{
			std::vector<float> lux_matrix = get_instance_object_tfm(children, i, root_tfm, model_tfm);

			std::vector<std::string> object_names = xsi_object_id_string(object);
			if (!xsi_id_to_lux_names_map.contains(xsi_id))
			{
				//master object is not exported yet
				//this is because it outside of the isolation view, because in non-isolation mode we at first export geometry and only then - instances
				if (is_xsi_object_visible(eval_time, object))
				{
					bool is_sync = sync_object(scene, object, xsi_materials_in_lux, xsi_id_to_lux_names_map, master_to_instance_map, eval_time);
					if (is_sync)
					{
						xsi_id_to_lux_names_map[xsi_id] = xsi_object_id_string(object);

						//set zero transform for the exported object
						//may be later we will reset it, but if object outside isolation view, then it will be invisible
						std::vector<std::string> names = xsi_id_to_lux_names_map[xsi_id];
						XSI::MATH::CTransformation tfm;
						tfm.SetIdentity();
						tfm.SetScalingFromValues(0.0, 0.0, 0.0);
						for (ULONG j = 0; j < names.size(); j++)
						{
							sync_transform(scene, names[j], tfm, eval_time);
						}
					}
				}
			}

			//add master id to the map
			master_to_instance_map[xsi_id].push_back(model_id);

			//check is we really export the object, or may be it was invisible
			if (xsi_id_to_lux_names_map.contains(xsi_id))
			{
				for (ULONG j = 0; j < object_names.size(); j++)
				{
					scene->DuplicateObject(object_names[j], model_id_str + "_" + object_names[j], &lux_matrix[0]);  //does we need generate new id for the instance, or use id from the instance?
				}
			}
		}
	}

	return true;
}