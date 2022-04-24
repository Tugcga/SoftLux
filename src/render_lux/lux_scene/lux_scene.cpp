#include "../../utilities/export_common.h"
#include "../../utilities/arrays.h"
#include "../../utilities/logs.h"

#include "lux_scene.h"

#include "xsi_application.h"
#include "xsi_model.h"

bool sync_object(luxcore::Scene* scene, XSI::X3DObject &xsi_object, const XSI::CTime& eval_time)
{
	XSI::CString xsi_type = xsi_object.GetType();
	if (xsi_type == "polymsh")
	{
		return sync_polymesh(scene, xsi_object, eval_time);
	}
	else if (xsi_type == "pointcloud")
	{

	}
	else if (xsi_type == "hair")
	{

	}
	else
	{
		//unsupported object type
	}

	return false;
}

void sync_scene_objects(luxcore::Scene* scene, const XSI::CRefArray& xsi_list, XSI::siClassID class_id, std::set<ULONG> &xsi_objects_in_lux, const XSI::CTime& eval_time)
{
	for (ULONG i = 0; i < xsi_list.GetCount(); i++)
	{
		XSI::X3DObject xsi_object;
		if (class_id == XSI::siX3DObjectID)
		{
			xsi_object = XSI::X3DObject(xsi_list[i]);
		}
		else if (XSI::siGeometryID)
		{
			XSI::SIObject si_ob(xsi_list[i]);
			si_ob = XSI::SIObject(si_ob.GetParent());
			si_ob = XSI::SIObject(si_ob.GetParent());
			xsi_object = XSI::X3DObject(si_ob);
		}

		if (is_xsi_object_visible(eval_time, xsi_object))
		{
			bool is_sync = sync_object(scene, xsi_object, eval_time);
			if (is_sync)
			{
				//remember object in the array
				xsi_objects_in_lux.insert(xsi_object.GetObjectID());
			}
		}
	}
}

void sync_scene_objects(luxcore::Scene* scene, XSI::RendererContext& xsi_render_context, std::set<ULONG> &xsi_objects_in_lux, const XSI::CTime& eval_time)
{
	const XSI::CRefArray& xsi_isolation_list = xsi_render_context.GetArrayAttribute("ObjectList");
	if (xsi_isolation_list.GetCount() > 0)
	{//we are in isolation mode
		sync_scene_objects(scene, xsi_isolation_list, XSI::siX3DObjectID, xsi_objects_in_lux, eval_time);
	}
	else
	{//we should check all objects in the scene
		//for simplicity get all X3Dobjects and then filter by their types
		const XSI::CRefArray& xsi_objects_list = XSI::Application().FindObjects(XSI::siGeometryID);
		sync_scene_objects(scene, xsi_objects_list, XSI::siGeometryID, xsi_objects_in_lux, eval_time);
	}

	//next lights
	const XSI::CRefArray& xsi_scene_lights = xsi_render_context.GetArrayAttribute("Lights");
	for (ULONG i = 0; i < xsi_scene_lights.GetCount(); i++)
	{
		XSI::Light xsi_light(xsi_scene_lights.GetItem(i));
		sync_xsi_light(scene, xsi_light, eval_time);
	}
}

void sync_transform(luxcore::Scene* scene, const std::string &object_name, const XSI::MATH::CTransformation &xsi_tfm, const XSI::CTime &eval_time)
{
	std::vector<double> lux_matrix = xsi_to_lux_matrix(xsi_tfm.GetMatrix4());
	float matrix[16] = {};
	for (size_t i = 0; i < 16; i++)
	{
		matrix[i] = lux_matrix[i];
	}
	scene->UpdateObjectTransformation(object_name, matrix);
}