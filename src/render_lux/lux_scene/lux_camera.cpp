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

void setup_camera(luxrays::Properties &lux_props,
	const bool panorama_enable,
	const float panorama_degrees,
	const int xsi_ortho_mode,
	const XSI::Camera& xsi_camera,
	const XSI::CTime &eval_time,
	XSI::CRefArray &camera_shaders,
	const float focaldistance,
	const bool set_positions,
	const XSI::MATH::CVector3 &xsi_position, 
	const XSI::MATH::CVector3 &xsi_target_position,
	const float shutterclose)
{
	if (panorama_enable)
	{
		//parametrs for environment camera
		//environment.degrees(360.0f)
		lux_props.Set(luxrays::Property("scene.camera.type")("environment"));
		lux_props.Set(luxrays::Property("scene.camera.environment.degrees")(panorama_degrees));
	}
	else
	{//there is no panorama shader node or it disabled
		//camera is perspective or orthographic
		if (xsi_ortho_mode == 0)
		{//orthograpjic camera
			lux_props.Set(luxrays::Property("scene.camera.type")("orthographic"));
			float aspect = xsi_camera.GetParameterValue("aspect", eval_time);
			float ortho_height = xsi_camera.GetParameterValue("orthoheight", eval_time);
			float y = ortho_height / 2.0f;
			float x = y * aspect;

			lux_props.Set(luxrays::Property("scene.camera.screenwindow")(-x, x, -y, y));
		}
		else
		{//perspective camera
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

			lux_props.Set(luxrays::Property("scene.camera.type")("perspective"));
			lux_props.Set(luxrays::Property("scene.camera.fieldofview")(fov));

			//try to find bokeh lense shader
			std::vector<XSI::ShaderParameter> bokeh_root_parameter = get_root_shader_parameter(camera_shaders, GRSPM_NodeName, "", false, "LUXShadersPlugin", "LenseBokeh");
			bool use_bokeh = false;
			float lensradius = 0.0f;
			int bokeh_blades = 0;
			int bokeh_power = 0;
			float bokeh_scale_x = 1.0f;
			float bokeh_scale_y = 1.0f;
			std::string bokeh_type = "NONE";
			if (bokeh_root_parameter.size() > 0)
			{
				XSI::Shader bokeh_shader = get_input_node(bokeh_root_parameter[0]);
				XSI::CParameterRefArray bokeh_params = bokeh_shader.GetParameters();
				use_bokeh = get_bool_parameter_value(bokeh_params, "enable", eval_time);

				//also read other parameters
				lensradius = get_float_parameter_value(bokeh_params, "lensradius", eval_time);
				bokeh_blades = get_int_parameter_value(bokeh_params, "blades", eval_time);
				bokeh_power = get_int_parameter_value(bokeh_params, "power", eval_time);
				bokeh_scale_x = get_float_parameter_value(bokeh_params, "scale_x", eval_time);
				bokeh_scale_y = get_float_parameter_value(bokeh_params, "scale_y", eval_time);
				bokeh_type = get_string_parameter_value(bokeh_params, "distribution_type", eval_time).GetAsciiString();
			}
			bokeh_root_parameter.clear();

			if (use_bokeh)
			{
				lux_props.Set(luxrays::Property("scene.camera.lensradius")(lensradius));
				lux_props.Set(luxrays::Property("scene.camera.bokeh.blades")(bokeh_blades));
				lux_props.Set(luxrays::Property("scene.camera.bokeh.power")(bokeh_power));
				lux_props.Set(luxrays::Property("scene.camera.bokeh.distribution.type")(bokeh_type));
				//bokeh.distribution.image("image.png") if type is DIST_CUSTOM
				lux_props.Set(luxrays::Property("scene.camera.bokeh.scale.x")(bokeh_scale_x));
				lux_props.Set(luxrays::Property("scene.camera.bokeh.scale.y")(bokeh_scale_y));
			}
			else
			{
				//disable dof
				lux_props.Set(luxrays::Property("scene.camera.lensradius")(0.0f));
			}

			//lux_props.Set(luxrays::Property("scene.camera.focaldistance")(get_distance(xsi_position, xsi_target_position)));
			lux_props.Set(luxrays::Property("scene.camera.focaldistance")(focaldistance));
		}
		//use always autofocus false
		lux_props.Set(luxrays::Property("scene.camera.autofocus.enable")(false));
	}

	if (set_positions)
	{
		set_lux_camera_positions(lux_props, xsi_position, xsi_target_position);
	}

	//clipping planes
	float near_clip = xsi_camera.GetParameterValue("near", eval_time);
	float far_clip = xsi_camera.GetParameterValue("far", eval_time);
	lux_props.Set(luxrays::Property("scene.camera.cliphither")(near_clip));
	lux_props.Set(luxrays::Property("scene.camera.clipyon")(far_clip));

	//we assume that camera is open from 0 to the shutter time
	lux_props.Set(luxrays::Property("scene.camera.shutteropen")(0.0f));
	//lux_props.Set(luxrays::Property("scene.camera.shutterclose")(motion.motion_objects ? motion.motion_shutter_time : 1.0f));
	lux_props.Set(luxrays::Property("scene.camera.shutterclose")(shutterclose));
}

void sync_camera_scene(luxcore::Scene* scene,
	const XSI::Camera& xsi_camera, 
	const XSI::CParameterRefArray& render_params,
	const XSI::CTime& eval_time)
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
	//try to find LensePanoramic shader
	//then this camera will be environment type
	XSI::CRefArray camera_shaders = xsi_camera.GetShaders();
	std::vector<XSI::ShaderParameter> panorama_root_parameter = get_root_shader_parameter(camera_shaders, GRSPM_NodeName, "", false, "LUXShadersPlugin", "LensePanoramic");
	bool panorama_enable = false;
	float panorama_degrees = 360.0f;
		
	if (panorama_root_parameter.size() > 0)
	{//find panorama node
		XSI::Shader panorama_shader = get_input_node(panorama_root_parameter[0]);
		XSI::CParameterRefArray all_params = panorama_shader.GetParameters();
		panorama_degrees = get_float_parameter_value(all_params, "degrees", eval_time);
		panorama_enable = get_bool_parameter_value(all_params, "enable", eval_time);

		panorama_root_parameter.clear();
	}
	else
	{
		//try to find default xsi panoramic lense node
		panorama_root_parameter = get_root_shader_parameter(camera_shaders, GRSPM_NodeName, "", false, "Softimage", "Cylindric_len");
		if (panorama_root_parameter.size() > 0)
		{
			XSI::Shader panorama_shader = get_input_node(panorama_root_parameter[0]);
			XSI::CParameterRefArray all_params = panorama_shader.GetParameters();
			panorama_degrees = get_float_parameter_value(all_params, "HORIZONTAL_FOV", eval_time);
			panorama_enable = true;

			panorama_root_parameter.clear();
		}
	}

	luxrays::Properties camera_props;
	set_lux_camera_positions(camera_props, xsi_position, xsi_target_position);
	
	bool is_motion = render_params.GetValue("motion_objects", eval_time);
	float motion_shutter_time = render_params.GetValue("motion_shutter_time", eval_time);
	if (is_motion)
	{
		camera_props.Set(luxrays::Property("scene.camera.up")(0.0, 1.0, 0.0));
		scene->Parse(camera_props);

		luxrays::Properties motion_props;
		bool is_motion = sync_motion(motion_props, "scene.camera", render_params, xsi_camera.GetKinematics().GetGlobal(), eval_time);
		
		motion_props.Set(luxrays::Property("scene.camera.up")(0.0, 1.0, 0.0));
		motion_props.Set(luxrays::Property("scene.camera.lookat.orig")(0.0, 0.0, 0.0));
		motion_props.Set(luxrays::Property("scene.camera.lookat.target")(0.0, 0.0, -1.0));

		//setup camera settings again
		setup_camera(motion_props, panorama_enable, panorama_degrees, xsi_ortho_mode, xsi_camera, eval_time, camera_shaders,
			get_distance(xsi_position, xsi_target_position),
			false,
			xsi_position, xsi_target_position,
			is_motion ? motion_shutter_time : 1.0f);

		scene->Parse(motion_props);
	}
	else
	{
		setup_camera(camera_props, panorama_enable, panorama_degrees, xsi_ortho_mode, xsi_camera, eval_time, camera_shaders,
			get_distance(xsi_position, xsi_target_position),
			false,
			xsi_position, xsi_target_position,
			is_motion ? motion_shutter_time : 1.0f);

		scene->Parse(camera_props);
	}
}

void sync_camera(luxcore::Scene* scene, 
	const RenderType render_type,
	XSI::RendererContext& xsi_render_context, 
	const XSI::CParameterRefArray& render_params,
	const XSI::CTime &eval_time)
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
		sync_camera_scene(scene, xsi_camera, render_params, eval_time);
	}
}