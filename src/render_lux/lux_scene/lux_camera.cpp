#include "lux_scene.h"
#include "../../utilities/math.h"
#include "../../utilities/logs.h"
#include "../../utilities/xsi_shaders.h"

#include "xsi_kinematics.h"

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

	int xsi_ortho_mode = xsi_camera.GetParameterValue("proj", eval_time);  // 0 - orthographic, 1 - perspective
	//try to find LensePanorama shader
	//then this camera will be environment type
	XSI::CRefArray camera_shaders = xsi_camera.GetShaders();
	std::vector<XSI::ShaderParameter> panorama_root_parameter = get_root_shader_parameter(camera_shaders, GRSPM_NodeName, "", false, "LensePanorama");
	luxrays::Properties camera_props;
	if (panorama_root_parameter.size() > 0)
	{//find panorama node
		XSI::Shader panorama_shader = get_input_node(panorama_root_parameter[0]);
		//parametrs for environment camera
		//environment.degrees(360.0f)
		camera_props.Set(luxrays::Property("scene.camera.type")("environment"));
		XSI::CParameterRefArray all_params = panorama_shader.GetParameters();
		XSI::Parameter degrees_param = all_params.GetItem("degrees");
		XSI::Parameter degrees_param_final = get_source_parameter(degrees_param);
		float degrees = degrees_param_final.GetValue(eval_time);
		camera_props.Set(luxrays::Property("scene.camera.environment.degrees")(degrees));
	}
	else
	{//camera is perspective or orthographic
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

		camera_props.Set(luxrays::Property("scene.camera.type")(xsi_ortho_mode == 1 ? "perspective" : "orthographic"));
		//TODO: orthographic mode works incorrectly
		camera_props.Set(luxrays::Property("scene.camera.fieldofview")(fov));

		//try focus blur
		camera_props.Set(luxrays::Property("scene.camera.lensradius")(0.0f));
		camera_props.Set(luxrays::Property("scene.camera.focaldistance")(get_distance(xsi_position, xsi_target_position)));

		//additional parameters of perspective camera
		//bokeh.blades(0u)
		//bokeh.power(3u)
		//bokeh.distribution.type("NONE"), variants are: NONE, UNIFORM, EXPONENTIAL, INVERSEEXPONENTIAL, GAUSSIAN, INVERSEGAUSSIAN, TRIANGULAR, CUSTOM
		//bokeh.distribution.image("image.png") if type is DIST_CUSTOM
		//bokeh.scale.x(1.0f)
		//bokeh.scale.y(1.0f)

		//for all cameras except environment
		//autofocus.enable(false)
	}

	set_lux_camera_positions(camera_props, xsi_position, xsi_target_position);

	//clipping planes
	float near_clip = xsi_camera.GetParameterValue("near", eval_time);
	float far_clip = xsi_camera.GetParameterValue("far", eval_time);
	camera_props.Set(luxrays::Property("scene.camera.cliphither")(near_clip));
	camera_props.Set(luxrays::Property("scene.camera.clipyon")(far_clip));

	camera_props.Set(luxrays::Property("scene.camera.shutteropen")(0.0f));
	camera_props.Set(luxrays::Property("scene.camera.shutterclose")(1.0f));

	scene->Parse(camera_props);
}

void sync_camera(luxcore::Scene* scene, const RenderType render_type, XSI::RendererContext& xsi_render_context, const XSI::CTime &eval_time)
{
	if (render_type == RenderType_Shaderball)
	{
		sync_camera_shaderball(scene);
	}
	else
	{
		XSI::Primitive camera_prim(xsi_render_context.GetAttribute("Camera"));
		XSI::X3DObject camera_obj = camera_prim.GetOwners()[0];
		XSI::Camera	xsi_camera(camera_obj);
		sync_camera_scene(scene, xsi_camera, eval_time);
	}
}