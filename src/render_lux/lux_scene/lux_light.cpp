#include "lux_scene.h"
#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

//here we convert built-in XSI lights
bool sync_xsi_light(luxcore::Scene* scene, XSI::Light &xsi_light, const XSI::CTime &eval_time)
{
	if (is_xsi_object_visible(eval_time, xsi_light))
	{
		

		return true;
	}
	else
	{
		return false;
	}
}

bool update_light_object(luxcore::Scene* scene, XSI::X3DObject& xsi_object, const XSI::CTime& eval_time)
{
	if (xsi_object.GetType() == "light")
	{
		//delete the light
		std::string object_name = XSI::CString(xsi_object.GetObjectID()).GetAsciiString();
		scene->DeleteLight(object_name);
		XSI::Light xsi_light(xsi_object);
		return sync_xsi_light(scene, xsi_light, eval_time);
	}
	else
	{
		return false;
	}
}