#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "lux_scene.h"

#include "xsi_materiallibrary.h"

void sync_material(luxcore::Scene* scene, XSI::Material &xsi_material, std::set<ULONG>& xsi_materials_in_lux, const XSI::CTime& eval_time)
{
	//we should set the name of material equal to UniqueID of the object
	//next we should export some basic material
	//and assign it to the polygon meshes

	std::string material_name = xsi_object_id_string(xsi_material)[0];
	luxrays::Properties material_props;
	material_props.Set(luxrays::Property("scene.materials." + material_name + ".type")("matte"));
	material_props.Set(luxrays::Property("scene.materials." + material_name + ".kd")(
		int(material_name[material_name.size() - 1] - '0') / (float)10,
		int(material_name[material_name.size() - 2] - '0') / (float)10,
		int(material_name[material_name.size() - 3] - '0') / (float)10));
	//set default id
	material_props.Set(luxrays::Property("scene.materials." + material_name + ".id")(0));

	scene->Parse(material_props);

	xsi_materials_in_lux.insert(xsi_material.GetObjectID());
}

void override_material(luxcore::Scene* scene, XSI::X3DObject &xsi_object, const std::string material_name)
{
	//get object materials
	XSI::CRefArray xsi_materials = xsi_object.GetMaterials();

	std::vector<std::string> object_names = xsi_object_id_string(xsi_object);
	for (ULONG i = 0; i < object_names.size(); i++)
	{
		scene->UpdateObjectMaterial(object_names[i], material_name);
	}
}

void sync_default_material(luxcore::Scene* scene)
{
	//export default material
	luxrays::Properties default_props;
	default_props.Set(luxrays::Property("scene.materials.default_material.type")("matte"));
	default_props.Set(luxrays::Property("scene.materials.default_material.kd")(0.8, 0.8, 0.8));
	default_props.Set(luxrays::Property("scene.materials.default_material.id")(0));
	scene->Parse(default_props);
}

void sync_materials(luxcore::Scene *scene, const XSI::Scene &xsi_scene, std::set<ULONG>& xsi_materials_in_lux, const XSI::CTime &eval_time)
{
	if (!scene->IsMaterialDefined("default_material"))
	{
		sync_default_material(scene);
	}

	XSI::CRefArray material_libraries = xsi_scene.GetMaterialLibraries();
	for (LONG lib_index = 0; lib_index < material_libraries.GetCount(); lib_index++)
	{
		XSI::MaterialLibrary libray = material_libraries.GetItem(lib_index);
		XSI::CRefArray materials = libray.GetItems();
		for (LONG mat_index = 0; mat_index < materials.GetCount(); mat_index++)
		{
			XSI::Material xsi_material = materials.GetItem(mat_index);
			XSI::CRefArray used_objects = xsi_material.GetUsedBy();
			if (used_objects.GetCount() > 0)
			{
				sync_material(scene, xsi_material, xsi_materials_in_lux, eval_time);
			}
		}
	}
}

void reassign_all_materials(luxcore::Scene* scene, const XSI::Scene& xsi_scene, std::set<ULONG>& xsi_materials_in_lux, std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map, const XSI::CTime &eval_time)
{
	XSI::CRefArray material_libraries = xsi_scene.GetMaterialLibraries();
	for (LONG lib_index = 0; lib_index < material_libraries.GetCount(); lib_index++)
	{
		XSI::MaterialLibrary libray = material_libraries.GetItem(lib_index);
		XSI::CRefArray materials = libray.GetItems();
		for (LONG mat_index = 0; mat_index < materials.GetCount(); mat_index++)
		{
			XSI::Material xsi_material = materials.GetItem(mat_index);
			std::string lux_material_name = xsi_object_id_string(xsi_material)[0];
			ULONG material_id = xsi_material.GetObjectID();
			XSI::CRefArray used_objects = xsi_material.GetUsedBy();
			for (ULONG obj_index = 0; obj_index < used_objects.GetCount(); obj_index++)
			{
				XSI::CRef object_ref = used_objects[obj_index];
				XSI::siClassID object_class = object_ref.GetClassID();
				if (object_class == XSI::siX3DObjectID)
				{
					XSI::X3DObject object(object_ref);
					ULONG object_id = object.GetObjectID();
					if (!scene->IsMaterialDefined(lux_material_name))
					{
						sync_material(scene, xsi_material, xsi_materials_in_lux, eval_time);
					}

					if(xsi_materials_in_lux.contains(material_id) && xsi_id_to_lux_names_map.contains(object_id))
					{
						std::vector<std::string> object_names = xsi_id_to_lux_names_map[object_id];
						for (ULONG i = 0; i < object_names.size(); i++)
						{
							scene->UpdateObjectMaterial(object_names[i], lux_material_name);
						}
					}
					else
					{
						
					}
				}
			}
		}
	}
}