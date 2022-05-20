#include "lux_scene.h"
#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "xsi_project.h"
#include "xsi_scene.h"
#include "xsi_model.h"
#include "xsi_pass.h"
#include "xsi_imageclip2.h"

#include <cmath>

void set_light_intencity(luxrays::Properties &light_props, std::string prefix,
	const XSI::CString& color_mode,
	const XSI::CString &unit,
	bool is_spot,
	float spot_size,
	float color_r, float color_g, float color_b,
	float temperature,
	float gain,
	float power,
	float efficency,
	float lumen,
	float candela,
	bool normalized)
{
	if (color_mode == "rgb")
	{
		light_props.Set(luxrays::Property(prefix + ".color")(color_r, color_g, color_b));
	}
	else
	{
		light_props.Set(luxrays::Property(prefix + ".color")(1.0f, 1.0f, 1.0f));
		light_props.Set(luxrays::Property(prefix + ".temperature")(temperature));
		light_props.Set(luxrays::Property(prefix + ".temperature.normalize")(true));
	}

	if (unit == "power")
	{
		if (power < 0.0001f || efficency < 0.0001f)
		{
			gain = 0.0f;
		}
		else
		{
			gain = 1.0f;
		}
	}
	else if (unit == "lumen")
	{
		efficency = 1.0f;
		power = lumen;
		if (lumen < 0.0001f)
		{
			gain = 0.0f;
		}
		else
		{
			gain = 1.0f;
		}
	}
	else if (unit == "candela")
	{
		efficency = 1.0f;
		if (is_spot)
		{
			power = candela * 2.0f * M_PI * (1.0f - cos(DEG2RADF(spot_size) / 2));
		}
		else
		{
			power = candela * 4.0f * M_PI;
		}
		if (candela < 0.0001f)
		{
			gain = 0.0f;
		}
		else
		{
			gain = 1.0f;
		}
	}
	else
	{
		efficency = 0.0f;
		power = 0.0f;
		normalized = false;
	}

	//set calculated values of gain, power, efficency and normalized
	light_props.Set(luxrays::Property(prefix + ".efficency")(efficency));
	light_props.Set(luxrays::Property(prefix + ".power")(power));
	light_props.Set(luxrays::Property(prefix + ".normalizebycolor")(normalized));
	light_props.Set(luxrays::Property(prefix + ".gain")(gain, gain, gain));
}

XSI::CString get_map_path(XSI::ShaderParameter &map_parameter)
{
	XSI::Parameter tex_param_finall = get_source_parameter(map_parameter);
	XSI::CRef tex_source = tex_param_finall.GetSource();
	if (tex_source.IsValid())
	{
		XSI::ImageClip2 clip(tex_source);
		XSI::CString file_path = clip.GetFileName();
		if (is_file_exists(file_path))
		{
			return file_path;
		}
		else
		{
			return "";
		}
	}
	else
	{
		return "";
	}
}

void setup_area_light(luxcore::Scene *scene, luxrays::Properties &light_props, const std::string &prefix, const std::string & light_name,
	bool is_visible,
	float color_r, float color_g, float color_b,
	float gain, float power, float efficency, float lumen, float candela,
	float spread_angle,
	const XSI::CString &unit,
	bool normalized,
	const XSI::CString &map_file, float gamma,
	float size_x, float size_y,
	float rotation_x, float rotation_y, float rotation_z,
	XSI::MATH::CTransformation &xsi_transform,
	int lightgroup, float importance,
	const XSI::CParameterRefArray& render_params,
	const XSI::KinematicState &xsi_kine,
	const XSI::CTime &eval_time)
{
	//export are shape
	std::string area_shape_name = "area_light";
	if (!scene->IsMeshDefined(area_shape_name))
	{
		define_area_light_mesh(scene, area_shape_name);
	}
	//add object to the scene
	light_props.Set(luxrays::Property(prefix + ".shape")(area_shape_name));
	//set it visible/invisible for the camera
	light_props.Set(luxrays::Property(prefix + ".camerainvisible")(!is_visible));
	//define emission material
	std::string mat_name = light_name + "_emission";
	std::string material_prefix = "scene.materials." + mat_name + ".emission";
	luxrays::Properties material_props;
	material_props.Set(luxrays::Property("scene.materials." + mat_name + ".type")("matte"));
	material_props.Set(luxrays::Property("scene.materials." + mat_name + ".kd")(0.0, 0.0, 0.0));
	material_props.Set(luxrays::Property("scene.materials." + mat_name + ".emission")(color_r, color_g, color_b));

	if (unit == "power")
	{
		material_props.Set(luxrays::Property(material_prefix + ".power")(power / (2.0f * M_PI * (1.0f - cos(DEG2RADF(spread_angle) / 2.0f)))));
		material_props.Set(luxrays::Property(material_prefix + ".efficency")(efficency));
		material_props.Set(luxrays::Property(material_prefix + ".normalizebycolor")(normalized));

		if (power < 0.0001f || efficency < 0.0001f) { material_props.Set(luxrays::Property(material_prefix + ".gain")(0.0, 0.0, 0.0)); }
		else { material_props.Set(luxrays::Property(material_prefix + ".gain")(1.0, 1.0, 1.0)); }
	}
	else if (unit == "lumen")
	{
		material_props.Set(luxrays::Property(material_prefix + ".power")(lumen / (2.0f * M_PI * (1.0f - cos(DEG2RADF(spread_angle) / 2.0f)))));
		material_props.Set(luxrays::Property(material_prefix + ".efficency")(1.0));
		material_props.Set(luxrays::Property(material_prefix + ".normalizebycolor")(normalized));
		if (lumen < 0.0001f) { material_props.Set(luxrays::Property(material_prefix + ".gain")(0.0, 0.0, 0.0)); }
		else { material_props.Set(luxrays::Property(material_prefix + ".gain")(1.0, 1.0, 1.0)); }
	}
	else if (unit == "candela")
	{
		material_props.Set(luxrays::Property(material_prefix + ".power")(0));
		material_props.Set(luxrays::Property(material_prefix + ".efficency")(0));
		material_props.Set(luxrays::Property(material_prefix + ".gain")(candela, candela, candela));
		material_props.Set(luxrays::Property(material_prefix + ".normalizebycolor")(normalized));
	}
	else
	{
		material_props.Set(luxrays::Property(material_prefix + ".power")(0));
		material_props.Set(luxrays::Property(material_prefix + ".efficency")(0));
		material_props.Set(luxrays::Property(material_prefix + ".gain")(gain, gain, gain));
		material_props.Set(luxrays::Property(material_prefix + ".normalizebycolor")(false));
		material_props.Set(luxrays::Property(material_prefix + ".gain.normalizebycolor")(false));
	}
	material_props.Set(luxrays::Property(material_prefix + ".theta")(spread_angle));
	material_props.Set(luxrays::Property(material_prefix + ".id")(lightgroup));
	material_props.Set(luxrays::Property(material_prefix + ".importance")(importance));
	if (map_file.Length() > 0)
	{
		material_props.Set(luxrays::Property(material_prefix + ".mapfile")(map_file.GetAsciiString()));
		material_props.Set(luxrays::Property(material_prefix + ".gamma")(gamma));
	}
	scene->Parse(material_props);

	light_props.Set(luxrays::Property(prefix + ".material")(mat_name));
	//set transform
	XSI::MATH::CMatrix4 xsi_matrix = xsi_transform.GetMatrix4();
	XSI::MATH::CMatrix4 scale_matrix;
	scale_matrix.SetIdentity();
	scale_matrix.SetValue(0, 0, size_x * 0.5 / xsi_transform.GetSclX());
	scale_matrix.SetValue(1, 1, size_y * 0.5 / xsi_transform.GetSclY());
	scale_matrix.SetValue(2, 2, 1.0 / xsi_transform.GetSclZ());

	//also we should rotate the light source
	XSI::MATH::CTransformation rotate_tfm;
	rotate_tfm.SetIdentity();
	rotate_tfm.SetRotX(rotation_x);
	rotate_tfm.SetRotY(rotation_y);
	rotate_tfm.SetRotZ(rotation_z);
	XSI::MATH::CMatrix4 rotate_matrix = rotate_tfm.GetMatrix4();
	rotate_matrix.MulInPlace(xsi_matrix);
	scale_matrix.MulInPlace(rotate_matrix);

	std::vector<double> lux_matrix = xsi_to_lux_matrix(scale_matrix);
	light_props.Set(luxrays::Property(prefix + ".transformation")(lux_matrix));
	bool is_motion = sync_motion(light_props, prefix, render_params, xsi_kine, eval_time);
}

//here we convert built-in XSI lights
bool sync_xsi_light(luxcore::Scene* scene, 
	XSI::Light &xsi_light, 
	const XSI::CParameterRefArray& render_params,
	const XSI::CTime &eval_time)
{
	if (is_xsi_object_visible(eval_time, xsi_light))
	{
		//get light type
		int light_type = xsi_light.GetParameterValue("Type", eval_time);
		bool is_area = xsi_light.GetParameterValue("LightArea", eval_time);
		//and other parameters from XSI primitives
		bool is_visible = xsi_light.GetParameterValue("LightAreaVisible", eval_time);
		//area light params
		float size_x = xsi_light.GetParameterValue("LightAreaXformSX", eval_time);
		float size_y = xsi_light.GetParameterValue("LightAreaXformSY", eval_time);
		float rotation_x = xsi_light.GetParameterValue("LightAreaXformRX", eval_time);
		float rotation_y = xsi_light.GetParameterValue("LightAreaXformRY", eval_time);
		float rotation_z = xsi_light.GetParameterValue("LightAreaXformRZ", eval_time);

		//get cone parameter
		float cone = (float)xsi_light.GetParameterValue("LightCone", eval_time);
		XSI::MATH::CTransformation xsi_transform = xsi_light.GetKinematics().GetGlobal().GetTransform();

		//try to get light shader
		XSI::CRefArray light_shaders = xsi_light.GetShaders();
		std::vector<XSI::ShaderParameter> root_parameter_array = get_root_shader_parameter(light_shaders, GRSPM_ParameterName, "LightShader", false, "", "");
		if (root_parameter_array.size() > 0)
		{//there is a node, connected to the LightShader port of the root node
			XSI::Shader light_node = get_input_node(root_parameter_array[0]);
			if (light_node.IsValid() && light_node.GetProgID() == "Softimage.soft_light.1.0")
			{
				//root node connected to the soft light node
				//so, we can use it to define light parameters
				//get the color
				XSI::CParameterRefArray all_params = light_node.GetParameters();
				XSI::MATH::CColor4f color = get_color_parameter_value(all_params, "color", eval_time);
				float color_r = color.GetR();
				float color_g = color.GetG();
				float color_b = color.GetB();
				//get spread
				float spread = get_float_parameter_value(all_params, "spread", eval_time);
				//get intensity
				float intensity = get_float_parameter_value(all_params, "intensity", eval_time);
				//and umbra
				float umbra = get_float_parameter_value(all_params, "factor", eval_time);

				//we does not need shader parameters, because we already memorize it
				root_parameter_array.clear();

				std::string light_name = xsi_object_id_string(xsi_light)[0];

				//next we can export light by it type
				if (is_area)
				{
					//if area is active, then interpret this light as are light
					luxrays::Properties light_props;
					setup_area_light(scene, light_props, "scene.objects." + light_name, light_name, is_visible, color_r, color_g, color_b, 
						intensity, 0.0, 0.0, 0.0, 0.0,
						90.0f, "artistic", false, "", 2.2f,
						size_x, size_y, rotation_x, rotation_y, rotation_z, xsi_transform,
						0, 1.0f,
						render_params, xsi_light.GetKinematics().GetGlobal(), eval_time);

					scene->Parse(light_props);
					return true;
				}
				else
				{
					if (light_type == 0)
					{//this is point light
						luxrays::Properties light_props;
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("point"));
						std::array<float, 3> position = xsi_to_lux_vector(xsi_transform.GetTranslation());
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".position")(position[0], position[1], position[2]));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".color")(color_r, color_g, color_b));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".gain")(intensity, intensity, intensity));
						scene->Parse(light_props);

						return true;
					}
					else if (light_type == 1)
					{//this is infinite light
						//export infinite light as lux distant
						luxrays::Properties light_props;
						if (spread < 0.1f)
						{
							light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("sharpdistant"));
						}
						else
						{
							light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("distant"));
							light_props.Set(luxrays::Property("scene.lights." + light_name + ".theta")(spread));
							intensity *= get_distant_light_normalization_factor(spread);
						}
						
						//direction is local z-direction
						XSI::MATH::CMatrix4 tfm_matrix = xsi_transform.GetMatrix4();
						//set direction z-row of the matrix, but swap values (x, y, z) -> (z, x, y)
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".direction")(-tfm_matrix.GetValue(2, 2), -tfm_matrix.GetValue(2, 0), -tfm_matrix.GetValue(2, 1)));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".gain")(intensity, intensity, intensity));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".color")(color_r, color_g, color_b));
						scene->Parse(light_props);

						return true;
					}
					else if (light_type == 2)
					{//this is spot light
						luxrays::Properties light_props;
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("spot"));
						//set position
						XSI::MATH::CMatrix4 xsi_matrix = xsi_transform.GetMatrix4();
						std::array<float, 3> position = xsi_to_lux_vector(xsi_transform.GetTranslation());
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".position")(position[0], position[1], position[2]));
						//spot target is default (0, 0, 1)
						//set transform
						std::vector<double> lux_matrix = xsi_to_lux_matrix(xsi_matrix);
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".transformation")(lux_matrix));

						//set color and intensity
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".gain")(intensity, intensity, intensity));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".color")(color_r, color_g, color_b));
						//set cone parameters
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".coneangle")(cone));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".conedeltaangle")(cone * umbra));
						scene->Parse(light_props);

						return true;
					}
					else
					{//unknown light type
						return false;
					}
				}
			}
			else if (light_node.IsValid() && light_node.GetProgID() == "LUXShadersPlugin.LightLuxcore.1.0")
			{
				//this is custom light shader
				XSI::CParameterRefArray all_params = light_node.GetParameters();
				XSI::MATH::CColor4f color = get_color_parameter_value(all_params, "color", eval_time);
				float color_r = color.GetR();
				float color_g = color.GetG();
				float color_b = color.GetB();
				float temperature = get_float_parameter_value(all_params, "temperature", eval_time);
				XSI::CString color_mode = get_string_parameter_value(all_params, "color_mode", eval_time);

				//next values for different intencity modes
				float gain = get_float_parameter_value(all_params, "gain", eval_time);
				float exposure = get_float_parameter_value(all_params, "exposure", eval_time);
				float power = get_float_parameter_value(all_params, "power", eval_time);
				float efficency = get_float_parameter_value(all_params, "efficency", eval_time);
				float lumen = get_float_parameter_value(all_params, "lumen", eval_time);
				float candela = get_float_parameter_value(all_params, "candela", eval_time);
				bool normalized = get_float_parameter_value(all_params, "normalized", eval_time);

				//light type
				XSI::CString light_type = get_string_parameter_value(all_params, "light_type", eval_time);
				XSI::CString unit = get_string_parameter_value(all_params, "unit", eval_time);

				float importance = get_float_parameter_value(all_params, "importance", eval_time);
				int lightgroup_id = get_int_parameter_value(all_params, "lightgroup_id", eval_time);

				root_parameter_array.clear();
				
				//setup light object common parameters
				std::string light_name = xsi_object_id_string(xsi_light)[0];
				if (light_type == "area")
				{
					//for area light we should create an object and setup emission material for it
					luxrays::Properties light_props;
					float theta = get_float_parameter_value(all_params, "theta", eval_time);
					float gamma = get_float_parameter_value(all_params, "gamma", eval_time);

					XSI::ShaderParameter map_param = all_params.GetItem("map");
					setup_area_light(scene, light_props, "scene.objects." + light_name, light_name, is_visible, color_r, color_g, color_b, 
						gain * powf(2, exposure), power, efficency, lumen, candela, theta, unit, normalized, get_map_path(map_param), gamma,
						size_x, size_y, rotation_x, rotation_y, rotation_z, xsi_transform,
						lightgroup_id, importance, render_params, xsi_light.GetKinematics().GetGlobal(), eval_time);

					scene->Parse(light_props);
					return true;
				}
				else
				{
					luxrays::Properties light_props;
					light_props.Set(luxrays::Property("scene.lights." + light_name + ".id")(lightgroup_id));
					light_props.Set(luxrays::Property("scene.lights." + light_name + ".importance")(importance));
					//also transform
					XSI::MATH::CMatrix4 xsi_matrix = xsi_transform.GetMatrix4();
					std::vector<double> lux_matrix = xsi_to_lux_matrix(xsi_matrix);
					if (light_type != "distant")
					{
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".transformation")(lux_matrix));
					}
					
					//next setup color and intencity
					float gain_exposure = gain * powf(2, exposure);
					float point_size = get_float_parameter_value(all_params, "point_size", eval_time);
					if (light_type != "distant")
					{
						//for all lights except distante set color amd intensity
						set_light_intencity(light_props, "scene.lights." + light_name, color_mode, unit, light_type == "spot", cone, color_r, color_g, color_b, temperature, gain_exposure, power, efficency, lumen, candela, normalized);
					}
					else
					{
						//for distant light we should set only the color
						if (point_size > 0.1f)
						{
							gain_exposure *= get_distant_light_normalization_factor(point_size);
						}
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".gain")(gain_exposure, gain_exposure, gain_exposure));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".color")(color_r, color_g, color_b));
					}

					//next set type of the light and all other parameters
					if (light_type == "point")
					{
						//if the size is small then the light source is point or mappoint
						//is the size is not very small, then this is sphere or mapsphere
						XSI::ShaderParameter map_param = all_params.GetItem("map");
						XSI::CString map_path = get_map_path(map_param);
						XSI::CString ies_file = get_string_parameter_value(all_params, "ies_file", eval_time);
						bool is_ies = ies_file.Length() > 0 && is_file_exists(ies_file) && get_file_extension(ies_file) == "ies";
						bool is_map = map_path.Length() > 0;
						bool is_map_light = is_map || is_ies;
						float gamma = get_float_parameter_value(all_params, "gamma", eval_time);
						bool try_setup_ies = false;
						if (point_size < 0.0001f)
						{//point
							if (is_map_light)
							{//mappoint
								light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("mappoint"));
								if (is_ies)
								{
									light_props.Set(luxrays::Property("scene.lights." + light_name + ".iesfile")(ies_file.GetAsciiString()));
									light_props.Set(luxrays::Property("scene.lights." + light_name + ".flipz")(true));
								}
								if (is_map)
								{
									light_props.Set(luxrays::Property("scene.lights." + light_name + ".mapfile")(map_path.GetAsciiString()));
									light_props.Set(luxrays::Property("scene.lights." + light_name + ".gamma")(gamma));
								}
							}
							else
							{//point
								light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("point"));
							}
						}
						else
						{//sphere
							if (is_map_light)
							{//mapsphere
								light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("mapsphere"));
								light_props.Set(luxrays::Property("scene.lights." + light_name + ".radius")(point_size));
								if (is_ies)
								{
									light_props.Set(luxrays::Property("scene.lights." + light_name + ".iesfile")(ies_file.GetAsciiString()));
									light_props.Set(luxrays::Property("scene.lights." + light_name + ".flipz")(true));
								}
								if (is_map)
								{
									light_props.Set(luxrays::Property("scene.lights." + light_name + ".mapfile")(map_path.GetAsciiString()));
									light_props.Set(luxrays::Property("scene.lights." + light_name + ".gamma")(gamma));
								}
							}
							else
							{//sphere
								light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("sphere"));
								light_props.Set(luxrays::Property("scene.lights." + light_name + ".radius")(point_size));
							}
						}

						scene->Parse(light_props);
						return true;
					}
					else if (light_type == "distant")
					{
						if (point_size < 0.1f)
						{
							light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("sharpdistant"));
						}
						else
						{
							light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("distant"));
							light_props.Set(luxrays::Property("scene.lights." + light_name + ".theta")(point_size));
						}

						//direction is local z-direction
						XSI::MATH::CMatrix4 tfm_matrix = xsi_transform.GetMatrix4();
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".direction")(-tfm_matrix.GetValue(2, 2), -tfm_matrix.GetValue(2, 0), -tfm_matrix.GetValue(2, 1)));
						
						scene->Parse(light_props);
						return true;
					}
					else if (light_type == "spot")
					{
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("spot"));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".position")(0.0f, 0.0f, 0.0f));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".target")(0.0f, 0.0f, -1.0f));

						light_props.Set(luxrays::Property("scene.lights." + light_name + ".coneangle")(cone));
						float spot_delta_angle = get_float_parameter_value(all_params, "spot_delta_angle", eval_time);
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".conedeltaangle")(spot_delta_angle));

						scene->Parse(light_props);
						return true;
					}
					else if (light_type == "projection")
					{
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("projection"));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".position")(0.0f, 0.0f, 0.0f));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".target")(0.0f, 0.0f, -1.0f));

						//set fov and gamma
						float projection_fov = get_float_parameter_value(all_params, "projection_fov", eval_time);
						float gamma = get_float_parameter_value(all_params, "gamma", eval_time);
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".fov")(projection_fov));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".gamma")(gamma));
						//try to get the image
						XSI::ShaderParameter map_param = all_params.GetItem("map");
						XSI::CString map_path = get_map_path(map_param);
						if (map_path.Length() > 0)
						{
							light_props.Set(luxrays::Property("scene.lights." + light_name + ".mapfile")(map_path.GetAsciiString()));
						}

						scene->Parse(light_props);
						return true;
					}
					else if (light_type == "laser")
					{
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("laser"));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".radius")(point_size));
						std::array<float, 3> position = xsi_to_lux_vector(xsi_transform.GetTranslation());
						//position and target defined by transforms
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".position")(0.0f, 0.0f, 0.0f));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".target")(0.0f, 0.0f, -1.0f));

						scene->Parse(light_props);
						return true;
					}
					else
					{
						//unknown light type
						return false;
					}
				}
			}
			else
			{//root node connected to something non-supported, skip the light, because we can not objtain it parameters
				root_parameter_array.clear();
				return false;
			}
		}
		else
		{//root node does not connected to the shader node, skip the light
			root_parameter_array.clear();
			return false;
		}
	}
	else
	{
		//the light object is hidden from the render, skip it
		return false;
	}
}

bool update_light_object(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object, 
	const XSI::CParameterRefArray& render_params,
	const XSI::CTime& eval_time)
{
	if (xsi_object.GetType() == "light")
	{
		//delete the light
		std::string object_name = xsi_object_id_string(xsi_object)[0];
		XSI::Light xsi_light(xsi_object);
		//delete both from objects and lights, because we can switch light type from are to point, and in this case we should delete from object, but now this light is not are
		scene->DeleteObject(object_name);
		scene->DeleteLight(object_name);
		
		return sync_xsi_light(scene, xsi_light, render_params, eval_time);
	}
	else
	{
		return false;
	}
}

void sync_ambient(luxcore::Scene* scene, const XSI::CTime &eval_time)
{
	std::string ambient_name = "ambient_light";  // hardcoded name
	scene->DeleteLight(ambient_name);
	XSI::Project xsi_project = XSI::Application().GetActiveProject();
	XSI::Scene xsi_scene = xsi_project.GetActiveScene();
	XSI::Model xsi_root = xsi_scene.GetRoot();
	XSI::Property xsi_prop;
	xsi_root.GetPropertyFromName("AmbientLighting", xsi_prop);
	if (xsi_prop.IsValid())
	{
		XSI::Parameter xsi_param = xsi_prop.GetParameter("ambience");
		XSI::CParameterRefArray color_params = xsi_param.GetParameters();
		float r = color_params.GetValue("red", eval_time);
		float g = color_params.GetValue("green", eval_time);
		float b = color_params.GetValue("blue", eval_time);

		//add ambient light
		luxrays::Properties ambient_props;
		ambient_props.Set(luxrays::Property("scene.lights." + ambient_name + ".type")("constantinfinite"));
		ambient_props.Set(luxrays::Property("scene.lights." + ambient_name + ".color")(r, g, b));

		scene->Parse(ambient_props);
	}
}

void update_environment_map(std::map<int, XSI::Shader>& environment_map, const XSI::CRefArray& pass_shaders, std::vector<XSI::ShaderParameter>& environment_shader, const XSI::CString& name)
{
	XSI::CStringArray name_parts = name.Split(".");
	environment_shader = get_root_shader_parameter(pass_shaders, GRSPM_NodeName, "", false, name_parts[0], name_parts[1]);
	if (environment_shader.size() > 0)
	{//find the port, connected to the shader
		XSI::CString port_name = environment_shader[0].GetName();
		//get shader
		XSI::Shader node = get_input_node(environment_shader[0]);

		//conver port name to the index
		ULONG p = port_name.FindString("Item");
		if (p < UINT_MAX)
		{
			int index = 0;
			if (port_name != "Item")
			{
				XSI::CString number_str = port_name.GetSubString(p + 5, port_name.Length() - p - 6);
				if (number_str.Length() > 0)
				{//convert number to string
					index = std::stoi(number_str.GetAsciiString());
				}
			}
			if (index >= 0)
			{
				environment_map[index] = node;
			}
		}
	}
}

//return array with ids of the added lights
//id is id of the shader node
std::vector<ULONG> sync_environment(luxcore::Scene* scene, const XSI::CTime& eval_time)
{
	std::vector<ULONG> environment_lights_ids;
	static std::vector<std::string> light_names = {
		"LUXShadersPlugin.PassInfinite",
		"LUXShadersPlugin.PassSky",
		"LUXShadersPlugin.PassSun",
		"Softimage.sib_environment"
	};

	XSI::Project xsi_project = XSI::Application().GetActiveProject();
	XSI::Scene xsi_scene = xsi_project.GetActiveScene();
	XSI::Pass xsi_pass = xsi_scene.GetActivePass();
	XSI::CRefArray pass_shaders = xsi_pass.GetAllShaders();
	std::vector<XSI::ShaderParameter> environment_shader;
	std::map<int, XSI::Shader> environment_map;  // key - index in thelist, value - link to the shader node

	if (pass_shaders.GetCount() > 0)
	{
		for (size_t i = 0; i < light_names.size(); i++)
		{
			update_environment_map(environment_map, pass_shaders, environment_shader, XSI::CString(light_names[i].c_str()));
		}
	}

	if (environment_map.size() > 0)
	{
		for (auto& [key, value] : environment_map)
		{
			XSI::CString name = value.GetProgID().Split(".")[1];
			//we use id of the shader node as name
			std::string lux_light_name = xsi_object_id_string(value)[0];

			XSI::CParameterRefArray all_parameters = value.GetParameters();

			if (name == "sib_environment")
			{//this is builtin environment node
				luxrays::Properties light_props;

				XSI::ShaderParameter tex_param = all_parameters.GetItem("tex");
				XSI::Parameter tex_param_final = get_source_parameter(tex_param);
				XSI::CRef tex_source = tex_param_final.GetSource();
				if (tex_source.IsValid())
				{
					XSI::ImageClip2 clip(tex_source);
					std::string tex = std::string(clip.GetFileName().GetAsciiString());
					if (tex.size() > 0)
					{
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".type")("infinite"));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".file")(tex));

						//read transforms
						XSI::Parameter tfm_param = all_parameters.GetItem("transform");
						XSI::CParameterRefArray tfm_parameters = tfm_param.GetParameters();
						XSI::MATH::CMatrix4 xsi_matrix = XSI::MATH::CMatrix4(
							(double)((XSI::ShaderParameter)(tfm_parameters[0])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[1])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[2])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[3])).GetValue(eval_time),
							(double)((XSI::ShaderParameter)(tfm_parameters[4])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[5])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[6])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[7])).GetValue(eval_time),
							(double)((XSI::ShaderParameter)(tfm_parameters[8])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[9])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[10])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[11])).GetValue(eval_time),
							(double)((XSI::ShaderParameter)(tfm_parameters[12])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[13])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[14])).GetValue(eval_time), (double)((XSI::ShaderParameter)(tfm_parameters[15])).GetValue(eval_time)
							);
						XSI::MATH::CTransformation xsi_tfm;
						xsi_tfm.SetMatrix4(xsi_matrix);
						double rotation_x;
						double rotation_y;
						double rotation_z;
						double scale_x;
						double scale_y;
						double scale_z;
						xsi_tfm.GetRotationFromXYZAnglesValues(rotation_x, rotation_y, rotation_z);
						xsi_tfm.GetScalingValues(scale_x, scale_y, scale_z);
						XSI::MATH::CTransformation tfm;
						tfm.SetIdentity();
						tfm.SetRotationFromXYZAnglesValues(rotation_z, rotation_x, rotation_y);
						XSI::MATH::CTransformation scale_tfm;
						scale_tfm.SetIdentity();
						scale_tfm.SetScalingFromValues(scale_x, scale_y, scale_z);
						scale_tfm.MulInPlace(tfm);

						XSI::MATH::CMatrix4 tfm_matrix = scale_tfm.GetMatrix4();
						std::vector<double> lux_matrix = {
							tfm_matrix.GetValue(0, 0), tfm_matrix.GetValue(0, 1) , tfm_matrix.GetValue(0, 2), tfm_matrix.GetValue(0, 3),
							tfm_matrix.GetValue(1, 0), tfm_matrix.GetValue(1, 1) , tfm_matrix.GetValue(1, 2), tfm_matrix.GetValue(1, 3),
							tfm_matrix.GetValue(2, 0), tfm_matrix.GetValue(2, 1) , tfm_matrix.GetValue(2, 2), tfm_matrix.GetValue(2, 3),
							tfm_matrix.GetValue(3, 0), tfm_matrix.GetValue(3, 1) , tfm_matrix.GetValue(3, 2), tfm_matrix.GetValue(3, 3),
						};
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".transformation")(lux_matrix));

						//use image parameter as gain
						float intensity = get_float_parameter_value(all_parameters, "fg_intensity", eval_time);
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".gain")(intensity, intensity, intensity));

						scene->Parse(light_props);
					}
				}
			}
			else
			{
				//these are SoftLux nodes
				bool enable = get_bool_parameter_value(all_parameters, "enable", eval_time);
				if (enable)
				{
					if (name == "PassInfinite")
					{
						luxrays::Properties light_props;

						XSI::ShaderParameter file_param = all_parameters.GetItem("file");
						XSI::Parameter file_param_final = get_source_parameter(file_param);
						XSI::CRef file_source = file_param_final.GetSource();
						if (file_source.IsValid())
						{
							XSI::ImageClip2 clip(file_source);
							std::string file = std::string(clip.GetFileName().GetAsciiString());
							if (file.size() > 0)
							{
								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".type")("infinite"));
								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".file")(file));

								//next read and setup other settings
								float gamma = get_float_parameter_value(all_parameters, "gamma", eval_time);
								XSI::MATH::CColor4f gain = get_color_parameter_value(all_parameters, "gain", eval_time);
								bool visibility_indirect_diffuse_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_diffuse_enable", eval_time);
								bool visibility_indirect_glossy_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_glossy_enable", eval_time);
								bool visibility_indirect_specular_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_specular_enable", eval_time);
								bool sampleupperhemisphereonly = get_bool_parameter_value(all_parameters, "sampleupperhemisphereonly", eval_time);

								//transforms
								float rotation_x = get_float_parameter_value(all_parameters, "rotation_x", eval_time);
								float rotation_y = get_float_parameter_value(all_parameters, "rotation_y", eval_time);
								float rotation_z = get_float_parameter_value(all_parameters, "rotation_z", eval_time);
								XSI::MATH::CTransformation tfm;
								tfm.SetIdentity();
								tfm.SetRotationFromXYZAnglesValues(DEG2RADF(rotation_z), DEG2RADF(rotation_x), DEG2RADF(rotation_y));
								XSI::MATH::CMatrix4 tfm_matrix = tfm.GetMatrix4();
								std::vector<double> lux_matrix = {
									tfm_matrix.GetValue(0, 0), tfm_matrix.GetValue(0, 1) , tfm_matrix.GetValue(0, 2), tfm_matrix.GetValue(0, 3),
									tfm_matrix.GetValue(1, 0), tfm_matrix.GetValue(1, 1) , tfm_matrix.GetValue(1, 2), tfm_matrix.GetValue(1, 3),
									tfm_matrix.GetValue(2, 0), tfm_matrix.GetValue(2, 1) , tfm_matrix.GetValue(2, 2), tfm_matrix.GetValue(2, 3),
									tfm_matrix.GetValue(3, 0), tfm_matrix.GetValue(3, 1) , tfm_matrix.GetValue(3, 2), tfm_matrix.GetValue(3, 3),
								};
								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".transformation")(lux_matrix));

								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".gamma")(gamma));
								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".gain")(gain.GetR(), gain.GetG(), gain.GetB()));
								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.diffuse.enable")(visibility_indirect_diffuse_enable));
								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.glossy.enable")(visibility_indirect_glossy_enable));
								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.specular.enable")(visibility_indirect_specular_enable));
								light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".sampleupperhemisphereonly")(sampleupperhemisphereonly));

								scene->Parse(light_props);
								environment_lights_ids.push_back(value.GetObjectID());
							}
						}
					}
					else if (name == "PassSky")
					{
						luxrays::Properties light_props;
						XSI::MATH::CColor4f gain = get_color_parameter_value(all_parameters, "gain", eval_time);
						float turbidity = get_float_parameter_value(all_parameters, "turbidity", eval_time);
						XSI::MATH::CColor4f groundalbedo = get_color_parameter_value(all_parameters, "groundalbedo", eval_time);
						bool ground_enable = get_bool_parameter_value(all_parameters, "ground_enable", eval_time);
						bool ground_autoscale = get_bool_parameter_value(all_parameters, "ground_autoscale", eval_time);
						XSI::MATH::CColor4f ground_color = get_color_parameter_value(all_parameters, "ground_color", eval_time);
						float dir_x = get_float_parameter_value(all_parameters, "dir_x", eval_time);
						float dir_y = get_float_parameter_value(all_parameters, "dir_y", eval_time);
						float dir_z = get_float_parameter_value(all_parameters, "dir_z", eval_time);
						//normalize direction
						float l = sqrt(dir_x*dir_x + dir_y * dir_y + dir_z * dir_z);
						if (l > 0.01f)
						{
							dir_x /= l;
							dir_y /= l;
							dir_z /= l;
						}

						bool visibility_indirect_diffuse_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_diffuse_enable", eval_time);
						bool visibility_indirect_glossy_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_glossy_enable", eval_time);
						bool visibility_indirect_specular_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_specular_enable", eval_time);
						int distribution_width = get_int_parameter_value(all_parameters, "distribution_width", eval_time);
						int distribution_height = get_int_parameter_value(all_parameters, "distribution_height", eval_time);

						float rotation_x = get_float_parameter_value(all_parameters, "rotation_x", eval_time);
						float rotation_y = get_float_parameter_value(all_parameters, "rotation_y", eval_time);
						float rotation_z = get_float_parameter_value(all_parameters, "rotation_z", eval_time);
						XSI::MATH::CTransformation tfm;
						tfm.SetIdentity();
						tfm.SetRotationFromXYZAnglesValues(DEG2RADF(rotation_z), DEG2RADF(rotation_x), DEG2RADF(rotation_y));
						XSI::MATH::CMatrix4 tfm_matrix = tfm.GetMatrix4();
						std::vector<double> lux_matrix = {
							tfm_matrix.GetValue(0, 0), tfm_matrix.GetValue(0, 1) , tfm_matrix.GetValue(0, 2), tfm_matrix.GetValue(0, 3),
							tfm_matrix.GetValue(1, 0), tfm_matrix.GetValue(1, 1) , tfm_matrix.GetValue(1, 2), tfm_matrix.GetValue(1, 3),
							tfm_matrix.GetValue(2, 0), tfm_matrix.GetValue(2, 1) , tfm_matrix.GetValue(2, 2), tfm_matrix.GetValue(2, 3),
							tfm_matrix.GetValue(3, 0), tfm_matrix.GetValue(3, 1) , tfm_matrix.GetValue(3, 2), tfm_matrix.GetValue(3, 3),
						};

						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".type")("sky2"));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".transformation")(lux_matrix));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".gain")(gain.GetR(), gain.GetG(), gain.GetB()));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".turbidity")(turbidity));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".dir")(dir_z, dir_x, dir_y));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".groundalbedo")(groundalbedo.GetR(), groundalbedo.GetG(), groundalbedo.GetB()));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".ground.enable")(ground_enable));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".ground.autoscale")(ground_autoscale));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".ground.color")(ground_color.GetR(), ground_color.GetG(), ground_color.GetB()));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.diffuse.enable")(visibility_indirect_diffuse_enable));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.glossy.enable")(visibility_indirect_glossy_enable));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.specular.enable")(visibility_indirect_specular_enable));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".distribution.width")(distribution_width));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".distribution.height")(distribution_height));

						scene->Parse(light_props);
						environment_lights_ids.push_back(value.GetObjectID());
					}
					else if (name == "PassSun")
					{
						luxrays::Properties light_props;
						XSI::MATH::CColor4f gain = get_color_parameter_value(all_parameters, "gain", eval_time);
						float turbidity = get_float_parameter_value(all_parameters, "turbidity", eval_time);
						float relsize = get_float_parameter_value(all_parameters, "relsize", eval_time);
						bool visibility_indirect_diffuse_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_diffuse_enable", eval_time);
						bool visibility_indirect_glossy_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_glossy_enable", eval_time);
						bool visibility_indirect_specular_enable = get_bool_parameter_value(all_parameters, "visibility_indirect_specular_enable", eval_time);
						float rotation_x = get_float_parameter_value(all_parameters, "rotation_x", eval_time);
						float rotation_y = get_float_parameter_value(all_parameters, "rotation_y", eval_time);
						float rotation_z = get_float_parameter_value(all_parameters, "rotation_z", eval_time);
						float dir_x = get_float_parameter_value(all_parameters, "dir_x", eval_time);
						float dir_y = get_float_parameter_value(all_parameters, "dir_y", eval_time);
						float dir_z = get_float_parameter_value(all_parameters, "dir_z", eval_time);
						//normalize direction
						float l = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
						if (l > 0.01f)
						{
							dir_x /= l;
							dir_y /= l;
							dir_z /= l;
						}
						XSI::MATH::CTransformation tfm;
						tfm.SetIdentity();
						tfm.SetRotationFromXYZAnglesValues(DEG2RADF(rotation_z), DEG2RADF(rotation_x), DEG2RADF(rotation_y));
						XSI::MATH::CMatrix4 tfm_matrix = tfm.GetMatrix4();
						std::vector<double> lux_matrix = {
							tfm_matrix.GetValue(0, 0), tfm_matrix.GetValue(0, 1) , tfm_matrix.GetValue(0, 2), tfm_matrix.GetValue(0, 3),
							tfm_matrix.GetValue(1, 0), tfm_matrix.GetValue(1, 1) , tfm_matrix.GetValue(1, 2), tfm_matrix.GetValue(1, 3),
							tfm_matrix.GetValue(2, 0), tfm_matrix.GetValue(2, 1) , tfm_matrix.GetValue(2, 2), tfm_matrix.GetValue(2, 3),
							tfm_matrix.GetValue(3, 0), tfm_matrix.GetValue(3, 1) , tfm_matrix.GetValue(3, 2), tfm_matrix.GetValue(3, 3),
						};


						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".type")("sun"));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".transformation")(lux_matrix));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".gain")(gain.GetR(), gain.GetG(), gain.GetB()));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".dir")(dir_z, dir_x, dir_y));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".turbidity")(turbidity));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".relsize")(relsize));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.diffuse.enable")(visibility_indirect_diffuse_enable));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.glossy.enable")(visibility_indirect_glossy_enable));
						light_props.Set(luxrays::Property("scene.lights." + lux_light_name + ".visibility.indirect.specular.enable")(visibility_indirect_specular_enable));

						scene->Parse(light_props);
						environment_lights_ids.push_back(value.GetObjectID());
					}
				}
			}
		}
	}

	return environment_lights_ids;
}