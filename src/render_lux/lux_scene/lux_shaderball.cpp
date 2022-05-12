#include "lux_scene.h"
#include "../../utilities/export_common.h"

#include "xsi_model.h"

//add to the scene custom hardcoded lights
void sync_shaderball_lights(luxcore::Scene* scene)
{
	//we should add three area lights
	//add shape
	std::string area_shape_name = "area_light";
	if (!scene->IsMeshDefined(area_shape_name))
	{
		define_area_light_mesh(scene, area_shape_name);
	}

	//left and right light has the same material
	std::string main_mat_name = "shaderball_main_lights_emission";
	luxrays::Properties main_material_props;
	main_material_props.Set(luxrays::Property("scene.materials." + main_mat_name + ".type")("matte"));
	main_material_props.Set(luxrays::Property("scene.materials." + main_mat_name + ".kd")(0.0, 0.0, 0.0));
	main_material_props.Set(luxrays::Property("scene.materials." + main_mat_name + ".emission")(1.0, 1.0, 1.0));
	float main_intensity = 0.75 * 4;
	main_material_props.Set(luxrays::Property("scene.materials." + main_mat_name + ".emission.gain")(main_intensity, main_intensity, main_intensity));
	main_material_props.Set(luxrays::Property("scene.materials." + main_mat_name + ".emission.theta")(90.0));
	scene->Parse(main_material_props);

	//additional light material
	std::string secondary_mat_name = "shaderball_secondary_lights_emission";
	luxrays::Properties secondary_material_props;
	secondary_material_props.Set(luxrays::Property("scene.materials." + secondary_mat_name + ".type")("matte"));
	secondary_material_props.Set(luxrays::Property("scene.materials." + secondary_mat_name + ".kd")(0.0, 0.0, 0.0));
	secondary_material_props.Set(luxrays::Property("scene.materials." + secondary_mat_name + ".emission")(1.0, 1.0, 1.0));
	float secondary_intensity = 0.5 * 4;
	secondary_material_props.Set(luxrays::Property("scene.materials." + secondary_mat_name + ".emission.gain")(secondary_intensity, secondary_intensity, secondary_intensity));
	secondary_material_props.Set(luxrays::Property("scene.materials." + secondary_mat_name + ".emission.theta")(90.0));
	scene->Parse(secondary_material_props);

	//names
	std::string light_left = "shaderball_light_left";
	std::string light_right = "shaderball_light_right";
	std::string light_bottom = "shaderball_light_bottom";

	//left light
	luxrays::Properties light_left_props;
	light_left_props.Set(luxrays::Property("scene.objects." + light_left + ".shape")(area_shape_name));
	light_left_props.Set(luxrays::Property("scene.objects." + light_left + ".camerainvisible")(true));
	light_left_props.Set(luxrays::Property("scene.objects." + light_left + ".material")(main_mat_name));
	//left light transform
	XSI::MATH::CTransformation left_tfm;
	left_tfm.SetTranslationFromValues(-0.2587, 4.496, 4.5072);
	left_tfm.SetScalingFromValues(1.5, 1.5, 1.5);
	left_tfm.SetRotationFromXYZAnglesValues(-0.427, 0.3483, 0.992);
	XSI::MATH::CMatrix4 left_matrix = left_tfm.GetMatrix4();
	std::vector<double> left_lux_matrix = xsi_to_lux_matrix(left_matrix);
	light_left_props.Set(luxrays::Property("scene.objects." + light_left + ".transformation")(left_lux_matrix));

	//right light
	luxrays::Properties light_right_props;
	light_right_props.Set(luxrays::Property("scene.objects." + light_right + ".shape")(area_shape_name));
	light_right_props.Set(luxrays::Property("scene.objects." + light_right + ".camerainvisible")(true));
	light_right_props.Set(luxrays::Property("scene.objects." + light_right + ".material")(main_mat_name));
	XSI::MATH::CTransformation right_tfm;
	right_tfm.SetTranslationFromValues(4.901, 4.901, 0.194);
	right_tfm.SetScalingFromValues(1.5, 1.5, 1.5);
	right_tfm.SetRotationFromXYZAnglesValues(-0.636, 1.379, -0.229);
	XSI::MATH::CMatrix4 right_matrix = right_tfm.GetMatrix4();
	std::vector<double> right_lux_matrix = xsi_to_lux_matrix(right_matrix);
	light_right_props.Set(luxrays::Property("scene.objects." + light_right + ".transformation")(right_lux_matrix));

	//secondary center light
	luxrays::Properties center_right_props;
	center_right_props.Set(luxrays::Property("scene.objects." + light_bottom + ".shape")(area_shape_name));
	center_right_props.Set(luxrays::Property("scene.objects." + light_bottom + ".camerainvisible")(true));
	center_right_props.Set(luxrays::Property("scene.objects." + light_bottom + ".material")(secondary_mat_name));
	XSI::MATH::CTransformation center_tfm;
	center_tfm.SetTranslationFromValues(3.310, -0.411, 5.178);
	center_tfm.SetScalingFromValues(1.5, 1.5, 1.5);
	center_tfm.SetRotationFromXYZAnglesValues(-0.765, -0.765, 3.857);
	XSI::MATH::CMatrix4 center_matrix = center_tfm.GetMatrix4();
	std::vector<double> center_lux_matrix = xsi_to_lux_matrix(center_matrix);
	center_right_props.Set(luxrays::Property("scene.objects." + light_bottom + ".transformation")(center_lux_matrix));

	scene->Parse(light_left_props);
	scene->Parse(light_right_props);
	scene->Parse(center_right_props);
}

void sync_shaderball_back_material(luxcore::Scene* scene)
{
	scene->Parse(
		luxrays::Property("scene.materials.background_material.type")("matte") <<
		luxrays::Property("scene.materials.background_material.kd")(0.8, 0.8, 0.8)
	);
}

void gather_all_subobjects(const XSI::X3DObject& xsi_object, XSI::CRefArray& output)
{
	output.Add(xsi_object.GetRef());
	XSI::CRefArray children = xsi_object.GetChildren();
	for (ULONG i = 0; i < children.GetCount(); i++)
	{
		gather_all_subobjects(children[i], output);
	}
}

XSI::CRefArray gather_all_subobjects(const XSI::Model& root)
{
	XSI::CRefArray output;
	XSI::CRefArray children = root.GetChildren();
	for (ULONG i = 0; i < children.GetCount(); i++)
	{
		gather_all_subobjects(children[i], output);
	}
	return output;
}

void sync_shaderball(luxcore::Scene* scene, 
	XSI::RendererContext& xsi_render_context, 
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map, 
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	const XSI::CTime& eval_time, 
	const ULONG override_material_id)
{
	//shaderball secene export
	sync_default_material(scene);
	std::set<ULONG> empty_set;
	XSI::CRefArray models = xsi_render_context.GetAttribute("Scene");
	if (models.GetCount() > 0)
	{
		//the first model is the hero model
		XSI::Model hero(models[0]);
		XSI::CRefArray shaderball_objects = gather_all_subobjects(hero);
		for (LONG j = 0; j < shaderball_objects.GetCount(); j++)
		{
			XSI::X3DObject xsi_object(shaderball_objects[j]);
			XSI::CString xsi_type = xsi_object.GetType();
			if (xsi_type == "polymsh")
			{
				//here we should override material of the object
				//because in the model it contains local model material instead of editor one
				bool is_sync = sync_polymesh(scene, xsi_object, empty_set, material_with_shape_to_polymesh_map, eval_time, override_material_id);
				if (is_sync)
				{
					xsi_id_to_lux_names_map[xsi_object.GetObjectID()] = xsi_object_id_string(xsi_object);
				}
			}
			else if (xsi_type == "light")
			{//here we should export lights from the shaderball model
				//ignore these items in the model
			}
		}
	}
	//for shaderball we will use custom three are lights
	sync_shaderball_lights(scene);
	//all other models are background
	//at first add background material
	sync_shaderball_back_material(scene);
	//next all models
	for (LONG i = 1; i < models.GetCount(); i++)
	{
		XSI::Model m(models[i]);
		XSI::CRefArray background_objects = gather_all_subobjects(m);
		for (LONG j = 0; j < background_objects.GetCount(); j++)
		{
			XSI::X3DObject xsi_object(background_objects[j]);
			if (xsi_object.GetType() == "polymsh")
			{
				//add object as background
				bool is_sync = sync_polymesh(scene, xsi_object, empty_set, material_with_shape_to_polymesh_map, eval_time, 0, true);
				if (is_sync)
				{
					//override material to the background material
					override_material(scene, xsi_object, "background_material");
					//xsi_objects_in_lux.insert(xsi_object.GetObjectID());
					xsi_id_to_lux_names_map[xsi_object.GetObjectID()] = xsi_object_id_string(xsi_object);
				}
			}
		}
	}
}

void sync_camera_shaderball(luxcore::Scene* scene)
{
	XSI::MATH::CVector3 xsi_position = XSI::MATH::CVector3(5.5618, 5.7636, 5.8259);
	XSI::MATH::CVector3 xsi_target_position = XSI::MATH::CVector3(0.0, 0.535, 0.0);
	luxrays::Properties camera_props;
	camera_props.Set(luxrays::Property("scene.camera.type")("perspective"));
	set_lux_camera_positions(camera_props, xsi_position, xsi_target_position);
	camera_props.Set(luxrays::Property("scene.camera.fieldofview")(48.0f));
	camera_props.Set(luxrays::Property("scene.camera.up")(0.0, 0.0, 1.0));

	scene->Parse(camera_props);
}