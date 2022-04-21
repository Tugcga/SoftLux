#include "lux_scene.h"
#include "../../utilities/math.h"

#include "xsi_kinematics.h"

void set_lux_camera(luxcore::Scene* scene, const float fov, const XSI::MATH::CVector3 &xsi_position, const XSI::MATH::CVector3 &xsi_target_position)
{
	std::array<float, 3> lux_position = xsi_to_lux_vector(xsi_position);
	std::array<float, 3> lux_target_position = xsi_to_lux_vector(xsi_target_position);
	scene->Parse(
		luxrays::Property("scene.camera.lookat.orig")(lux_position[0], lux_position[1], lux_position[2]) <<
		luxrays::Property("scene.camera.lookat.target")(lux_target_position[0], lux_target_position[1], lux_target_position[2]) <<
		luxrays::Property("scene.camera.fieldofview")(fov));
}

void sync_camera_scene(luxcore::Scene* scene, const XSI::Camera& xsi_camera, const XSI::CTime& eval_time)
{
	XSI::MATH::CTransformation xsi_tfm = xsi_camera.GetKinematics().GetGlobal().GetTransform();
	XSI::MATH::CVector3 xsi_position = xsi_tfm.GetTranslation();

	XSI::X3DObject target = xsi_camera.GetInterest();
	XSI::MATH::CTransformation xsi_target_tfm = target.GetKinematics().GetGlobal().GetTransform();
	XSI::MATH::CVector3 xsi_target_position = xsi_target_tfm.GetTranslation();
	
	//target and camera are in one place, move target in some direction
	if (get_distance(xsi_position, xsi_target_position) < 0.0001)
	{
		xsi_target_position.Set(xsi_target_position.GetX(), xsi_target_position.GetY() - 0.1, xsi_target_position.GetZ());
	}

	float fov = xsi_camera.GetParameterValue("fov", eval_time);

	//set lux camera
	set_lux_camera(scene, fov, xsi_position, xsi_target_position);
}

void sync_camera_shaderball(luxcore::Scene* scene)
{
	XSI::MATH::CVector3 xsi_position = XSI::MATH::CVector3(15.68, 5.89, 0.0);
	XSI::MATH::CVector3 xsi_direction = XSI::MATH::CVector3(-0.94, -0.34, 0.0);

	XSI::MATH::CVector3 xsi_target_position = XSI::MATH::CVector3(xsi_position.GetX() + xsi_direction.GetX(),
																  xsi_position.GetY() + xsi_direction.GetY(),
																  xsi_position.GetZ() + xsi_direction.GetZ());
	set_lux_camera(scene, 28.94f, xsi_position, xsi_target_position);
}