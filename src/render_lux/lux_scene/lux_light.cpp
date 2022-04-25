#include "lux_scene.h"
#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "xsi_project.h"
#include "xsi_scene.h"
#include "xsi_model.h"

//here we convert built-in XSI lights
bool sync_xsi_light(luxcore::Scene* scene, XSI::Light &xsi_light, const XSI::CTime &eval_time)
{
	if (is_xsi_object_visible(eval_time, xsi_light))
	{
		//get light type
		int light_type = xsi_light.GetParameterValue("Type", eval_time);
		bool is_area = xsi_light.GetParameterValue("LightArea", eval_time);

		//try to get soft light shader
		XSI::CRefArray light_shaders = xsi_light.GetShaders();
		std::vector<XSI::ShaderParameter> root_parameter_array = get_root_shader_parameter(light_shaders, GRSPM_ParameterName, "LightShader", false, "");
		if (root_parameter_array.size() > 0)
		{//there is a node, connected to the LightShader port of the root node
			XSI::Shader light_node = get_input_node(root_parameter_array[0]);
			if (light_node.IsValid() && light_node.GetProgID() == "Softimage.soft_light.1.0")
			{
				//root node connected to the soft light node
				//so, we can use it to define light parameters
				//get the color
				XSI::CParameterRefArray all_params = light_node.GetParameters();
				XSI::Parameter color_param = all_params.GetItem("color");
				XSI::Parameter color_param_final = get_source_parameter(color_param);
				XSI::MATH::CColor4f color = color_param_final.GetValue(eval_time);
				float color_r = color.GetR();
				float color_g = color.GetG();
				float color_b = color.GetB();
				//get spread
				XSI::Parameter spread_param = all_params.GetItem("spread");
				XSI::Parameter spread_param_final = get_source_parameter(spread_param);
				float spread = spread_param_final.GetValue(eval_time);
				//get intensity
				XSI::Parameter intensity_param = all_params.GetItem("intensity");
				XSI::Parameter intensity_param_final = get_source_parameter(intensity_param);
				float intensity = intensity_param_final.GetValue(eval_time);
				//and umbra
				XSI::Parameter umbra_param = all_params.GetItem("factor");
				XSI::Parameter umbra_param_final = get_source_parameter(umbra_param);
				float umbra = umbra_param_final.GetValue(eval_time);

				//we does not need shader parameters, because we already memorize it
				root_parameter_array.clear();

				std::string light_name = xsi_object_id_string(xsi_light);

				//next we can export light by it type
				if (is_area)
				{
					//if area is active, then interpret this light as are light
					//export are shape
					std::string area_shape_name = "area_light";
					if (!scene->IsMeshDefined(area_shape_name))
					{
						define_area_light_mesh(scene, area_shape_name);
					}
					//add object to the scene
					luxrays::Properties light_props;
					light_props.Set(luxrays::Property("scene.objects." + light_name + ".shape")(area_shape_name));
					//set it visible/invisible for the camera
					bool is_visible = xsi_light.GetParameterValue("LightAreaVisible", eval_time);
					light_props.Set(luxrays::Property("scene.objects." + light_name + ".camerainvisible")(!is_visible));
					//define emission material
					std::string mat_name = light_name + "_emission";
					luxrays::Properties material_props;
					material_props.Set(luxrays::Property("scene.materials." + mat_name + ".type")("matte"));
					material_props.Set(luxrays::Property("scene.materials." + mat_name + ".kd")(0.0, 0.0, 0.0));
					material_props.Set(luxrays::Property("scene.materials." + mat_name + ".emission")(color_r, color_g, color_b));
					//apply scale along x and y axis
					float size_x = xsi_light.GetParameterValue("LightAreaXformSX", eval_time);
					float size_y = xsi_light.GetParameterValue("LightAreaXformSY", eval_time);
					material_props.Set(luxrays::Property("scene.materials." + mat_name + ".emission.gain")(intensity, intensity, intensity));
					//set spread = 90.0
					material_props.Set(luxrays::Property("scene.materials." + mat_name + ".emission.theta")(90.0));
					scene->Parse(material_props);

					light_props.Set(luxrays::Property("scene.objects." + light_name + ".material")(mat_name));
					//set transform
					XSI::MATH::CTransformation xsi_transform = xsi_light.GetKinematics().GetGlobal().GetTransform();
					XSI::MATH::CMatrix4 xsi_matrix = xsi_transform.GetMatrix4();
					XSI::MATH::CMatrix4 scale_matrix;
					scale_matrix.SetIdentity();
					scale_matrix.SetValue(0, 0, size_x * 0.5 / xsi_transform.GetSclX());
					scale_matrix.SetValue(1, 1, size_y * 0.5 / xsi_transform.GetSclY());
					scale_matrix.SetValue(2, 2, 1.0 / xsi_transform.GetSclZ());
					scale_matrix.MulInPlace(xsi_matrix);
					std::vector<double> lux_matrix = xsi_to_lux_matrix(scale_matrix);
					light_props.Set(luxrays::Property("scene.objects." + light_name + ".transformation")(lux_matrix));
					scene->Parse(light_props);

					return true;
				}
				else
				{
					if (light_type == 0)
					{//this is point light
						luxrays::Properties light_props;
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("point"));
						std::array<float, 3> position = xsi_to_lux_vector(xsi_light.GetKinematics().GetGlobal().GetTransform().GetTranslation());
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
						XSI::MATH::CMatrix4 tfm_matrix = xsi_light.GetKinematics().GetGlobal().GetTransform().GetMatrix4();
						//set direction z-row of the matrix, but swap values (x, y, z) -> (z, x, y)
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".direction")(-tfm_matrix.GetValue(2, 2), -tfm_matrix.GetValue(2, 0), -tfm_matrix.GetValue(2, 1)));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".gain")(intensity, intensity, intensity));
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".color")(color_r, color_g, color_b));
						scene->Parse(light_props);

						return true;
					}
					else if (light_type == 2)
					{//this is spot light
						//get cone parameter
						float cone = (float)xsi_light.GetParameterValue("LightCone", eval_time);

						luxrays::Properties light_props;
						light_props.Set(luxrays::Property("scene.lights." + light_name + ".type")("spot"));
						//set position
						XSI::MATH::CMatrix4 xsi_matrix = xsi_light.GetKinematics().GetGlobal().GetTransform().GetMatrix4();
						std::array<float, 3> position = xsi_to_lux_vector(xsi_light.GetKinematics().GetGlobal().GetTransform().GetTranslation());
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

bool update_light_object(luxcore::Scene* scene, XSI::X3DObject& xsi_object, const XSI::CTime& eval_time)
{
	if (xsi_object.GetType() == "light")
	{
		//delete the light
		std::string object_name = xsi_object_id_string(xsi_object);
		XSI::Light xsi_light(xsi_object);
		//delete both from objects and lights, because we can switch light type from are to point, and in this case we should delete from object, but now this light is not are
		scene->DeleteObject(object_name);
		scene->DeleteLight(object_name);
		
		return sync_xsi_light(scene, xsi_light, eval_time);
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