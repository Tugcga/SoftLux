#include "lux_scene.h"

#include "xsi_kinematics.h"

void sync_camera(luxcore::Scene* scene, const XSI::Camera& xsi_camera, const XSI::CTime& eval_time)
{
	XSI::MATH::CTransformation xsi_tfm = xsi_camera.GetKinematics().GetGlobal().GetTransform();
	XSI::MATH::CVector3 xsi_position = xsi_tfm.GetTranslation();

	XSI::X3DObject target = xsi_camera.GetInterest();
	XSI::MATH::CTransformation xsi_target_tfm = target.GetKinematics().GetGlobal().GetTransform();
	XSI::MATH::CVector3 xsi_target_position = xsi_target_tfm.GetTranslation();

	float fov = xsi_camera.GetParameterValue("fov", eval_time);

	//set lux camera
	scene->Parse(
		luxrays::Property("scene.camera.lookat.orig")(xsi_position.GetX(), xsi_position.GetY(), xsi_position.GetZ()) <<
		luxrays::Property("scene.camera.lookat.target")(xsi_target_position.GetX(), xsi_target_position.GetY(), xsi_target_position.GetZ()) <<
		luxrays::Property("scene.camera.fieldofview")(fov));
}