#include "../../utilities/export_common.h"
#include "../../utilities/arrays.h"

#include "lux_scene.h"

#include "xsi_application.h"

void sync_scene_object(luxcore::Scene* scene, const XSI::CRefArray& xsi_list, XSI::siClassID class_id, const XSI::CTime& eval_time)
{
	//store here all synced objects
	std::vector<ULONG> synced_xsi_geometries_id;

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

		if (is_xsi_object_visible(eval_time, xsi_object) && !is_contains(synced_xsi_geometries_id, xsi_object.GetObjectID()))
		{
			if (xsi_object.GetType() == "polymsh")
			{
				sync_polymesh(scene, xsi_object, eval_time);
				synced_xsi_geometries_id.push_back(xsi_object.GetObjectID());
			}
			else if (xsi_object.GetType() == "pointcloud")
			{
				
			}
			else if (xsi_object.GetType() == "hair")
			{
				
			}
			else
			{
				//unsupported object type
			}
		}
	}
}

void sync_scene_objects(luxcore::Scene* scene, const XSI::RendererContext& xsi_render_context, const RenderType render_type, const XSI::CTime& eval_time)
{
	if (render_type == RenderType_Shaderball)
	{

	}
	else
	{
		const XSI::CRefArray& xsi_isolation_list = xsi_render_context.GetArrayAttribute("ObjectList");
		if (xsi_isolation_list.GetCount() > 0)
		{//we are in isolation mode
			sync_scene_object(scene, xsi_isolation_list, XSI::siX3DObjectID, eval_time);
		}
		else
		{//we should check all objects in the scene
		 //for simplicity get all X3Dobjects and then filter by their types
			const XSI::CRefArray& xsi_objects_list = XSI::Application().FindObjects(XSI::siGeometryID);
			sync_scene_object(scene, xsi_objects_list, XSI::siGeometryID, eval_time);
		}
	}
}