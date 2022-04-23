#include "../../utilities/logs.h"

#include "lux_scene.h"

#include "xsi_materiallibrary.h"

void sync_material(luxcore::Scene* scene, const XSI::Material &xsi_material, const XSI::CTime& eval_time)
{
	//we should set the name of material equal to UniqueID of the object
	//next we should export some basic material
	//and assign it to the polygon meshes
}

void sync_materials(luxcore::Scene *scene, const XSI::Scene &xsi_scene, const XSI::CTime &eval_time)
{
	//export default material
	scene->Parse(
		luxrays::Property("scene.materials.default_material.type")("matte") <<
		luxrays::Property("scene.materials.default_material.kd")(0.8, 0.8, 0.8)
	);

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
				sync_material(scene, xsi_material, eval_time);
			}
		}
	}
}