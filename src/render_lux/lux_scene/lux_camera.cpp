#include "lux_scene.h"
#include "../../utilities/math.h"
#include "../../utilities/logs.h"

#include "xsi_kinematics.h"

#define M_PI 3.14159265358979323846
#define DEG2RADF(_deg) ((_deg) * (float)(M_PI / 180.0))
#define RAD2DEGF(_rad) ((_rad) * (float)(180.0 / M_PI))

void set_lux_camera_positions(luxrays::Properties &camera_props, const XSI::MATH::CVector3& xsi_position, const XSI::MATH::CVector3& xsi_target_position)
{
	std::array<float, 3> lux_position = xsi_to_lux_vector(xsi_position);
	std::array<float, 3> lux_target_position = xsi_to_lux_vector(xsi_target_position);
	camera_props.Set(luxrays::Property("scene.camera.lookat.orig")(lux_position[0], lux_position[1], lux_position[2]));
	camera_props.Set(luxrays::Property("scene.camera.lookat.target")(lux_target_position[0], lux_target_position[1], lux_target_position[2]));
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

	float aspect = xsi_camera.GetParameterValue("aspect", eval_time);
	float fov = xsi_camera.GetParameterValue("fov", eval_time);
	int fov_type = xsi_camera.GetParameterValue("fovtype", eval_time);
	if (aspect >= 1.0)
	{
		if (fov_type == 0)
		{
			//recalculate fov from horizontal to vertical
			fov = RAD2DEGF(2 * atan(tan(DEG2RADF(fov) / 2.0) * aspect));
		}
	}
	else
	{
		if (fov_type == 1)
		{
			fov = RAD2DEGF(2 * atan(tan(DEG2RADF(fov) / 2.0) / aspect));
		}
	}
	
	int xsi_ortho_mode = xsi_camera.GetParameterValue("proj", eval_time);  // 0 - orthographic, 1 - perspective

	luxrays::Properties camera_props;
	camera_props.Set(luxrays::Property("scene.camera.type")(xsi_ortho_mode == 1 ? "perspective" : "orthographic"));
	//TODO: orthographic mode works incorrectly
	set_lux_camera_positions(camera_props, xsi_position, xsi_target_position);
	camera_props.Set(luxrays::Property("scene.camera.fieldofview")(fov));

	//up vector
	//TODO: can not understand, how to properly setup it
	//XSI::MATH::CMatrix4 xsi_tfm_matrix = xsi_tfm.GetMatrix4();
	//camera_props.Set(luxrays::Property("scene.camera.up")(xsi_tfm_matrix.GetValue(0, 1), xsi_tfm_matrix.GetValue(1, 1), xsi_tfm_matrix.GetValue(2, 1)));

	//clipping planes
	float near_clip = xsi_camera.GetParameterValue("near", eval_time);
	float far_clip = xsi_camera.GetParameterValue("far", eval_time);
	camera_props.Set(luxrays::Property("scene.camera.cliphither")(near_clip));
	camera_props.Set(luxrays::Property("scene.camera.clipyon")(far_clip));

	//set default for motion blur
	camera_props.Set(luxrays::Property("scene.camera.shutteropen")(0));
	camera_props.Set(luxrays::Property("scene.camera.shutterclose")(0));
	camera_props.Set(luxrays::Property("scene.camera.lensradius")(0));
	camera_props.Set(luxrays::Property("scene.camera.focaldistance")(get_distance(xsi_position, xsi_target_position)));
	camera_props.Set(luxrays::Property("scene.camera.autofocus.enable")(0));

	//TODO: look at parsecamera.cpp source file to find all camera attributes
	//TODO: make panoramic camera export (using external propery)

	scene->Parse(camera_props);
}

void sync_camera_shaderball(luxcore::Scene * scene)
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