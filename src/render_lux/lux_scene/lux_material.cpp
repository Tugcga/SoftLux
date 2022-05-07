#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "lux_scene.h"

#include "xsi_materiallibrary.h"
#include "xsi_imageclip2.h"

void add_mapping(luxrays::Properties &texture_props, const std::string &prefix, XSI::CParameterRefArray &parameters, const XSI::CString &mapping_parameter_name, const XSI::CTime &eval_time)
{
	XSI::ShaderParameter mapping_parameter = parameters.GetItem(mapping_parameter_name);
	if (mapping_parameter.IsValid())
	{
		XSI::Shader mapping_node = get_input_node(mapping_parameter);
		if (mapping_node.IsValid())
		{
			//something connected to the mapping port
			XSI::CString prog_id = mapping_node.GetProgID();
			if (prog_id == "LUXShadersPlugin.Mapping2D.1.0")
			{
				//this is valid mapping node
				XSI::CParameterRefArray map_params = mapping_node.GetParameters();
				//in Blender the node can contains the child mapping node
				//in this case it gets basic data from the child node and then multiply it to the values of the current node
				//we will be use only one mapping node

				//set parameters to the texture property
				//get type of the 2d-mapping
				XSI::CString mapping_type = get_string_parameter_value(map_params, "mapping_type", eval_time);
				int uvindex = get_int_parameter_value(map_params, "uvindex", eval_time);
				bool is_uniform = get_bool_parameter_value(map_params, "is_uniform", eval_time);

				if (mapping_type == "uvmapping2d")
				{
					texture_props.Set(luxrays::Property(prefix + ".type")("uvmapping2d"));
					texture_props.Set(luxrays::Property(prefix + ".uvindex")(uvindex));
					float u_scale = get_float_parameter_value(map_params, "u_scale", eval_time);
					float v_scale = get_float_parameter_value(map_params, "v_scale", eval_time);
					if (is_uniform)
					{
						v_scale = u_scale;
					}
					texture_props.Set(luxrays::Property(prefix + ".uvscale")(u_scale, -v_scale));

					float rotation = get_float_parameter_value(map_params, "rotation", eval_time);
					texture_props.Set(luxrays::Property(prefix + ".rotation")(rotation));

					float u_offset = get_float_parameter_value(map_params, "u_offset", eval_time);
					float v_offset = get_float_parameter_value(map_params, "v_offset", eval_time);
					bool is_center_map = get_bool_parameter_value(map_params, "is_center_map", eval_time);
					if (is_center_map)
					{
						u_offset = u_offset + 0.5f * (1.0f - u_scale);
						v_offset = v_offset * -1.0f + 1.0f - (0.5f * (1.0f - v_scale));
					}
					else
					{
						v_offset = v_offset + 1.0f;
					}
					texture_props.Set(luxrays::Property(prefix + ".uvdelta")(u_offset, v_offset));
				}
				else
				{
					texture_props.Set(luxrays::Property(prefix + ".type")("uvrandommapping2d"));
					texture_props.Set(luxrays::Property(prefix + ".uvindex")(uvindex));
					texture_props.Set(luxrays::Property(prefix + ".rotation")(get_float_parameter_value(map_params, "rotation_min", eval_time), 
																			  get_float_parameter_value(map_params, "rotation_max", eval_time)));
					texture_props.Set(luxrays::Property(prefix + ".uvscale")(get_float_parameter_value(map_params, "u_scale_min", eval_time),
																			 get_float_parameter_value(map_params, "u_scale_max", eval_time),
																			 get_float_parameter_value(map_params, "v_scale_min", eval_time),
																			 get_float_parameter_value(map_params, "v_scale_max", eval_time)));
					texture_props.Set(luxrays::Property(prefix + ".uvscale.uniform")(is_uniform));
					texture_props.Set(luxrays::Property(prefix + ".uvdelta")(get_float_parameter_value(map_params, "u_offset_min", eval_time),
																			 get_float_parameter_value(map_params, "u_offset_max", eval_time),
																			 get_float_parameter_value(map_params, "v_offset_min", eval_time),
																			 get_float_parameter_value(map_params, "v_offset_max", eval_time)));
					XSI::CString seed_type = get_string_parameter_value(map_params, "seed_type", eval_time);
					if (seed_type == "object_id")
					{
						texture_props.Set(luxrays::Property(prefix + ".seed.type")("object_id_offset"));
						texture_props.Set(luxrays::Property(prefix + ".objectidoffset.value")(get_int_parameter_value(map_params, "id_offset", eval_time)));
					}
					else
					{
						texture_props.Set(luxrays::Property(prefix + ".seed.type")("triangle_aov"));
						texture_props.Set(luxrays::Property(prefix + ".triangleaov.index")(0));
					}
				}
			}
		}
		else
		{
			//mapping port is not connected to something, nothing to do
		}
	}
	else
	{
		//the node does not contains mapping parameter with input name
		//so, nothing to do
	}
}

void add_emission(luxcore::Scene* scene, XSI::CParameterRefArray &parameters, luxrays::Properties &lux_props, const std::string &prefix, const XSI::CTime &eval_time)
{
	XSI::ShaderParameter emission_param = parameters.GetItem("emission");
	if (emission_param.IsValid())
	{
		//get connected node
		XSI::Shader emission_node = get_input_node(emission_param);
		if (emission_node.IsValid())
		{
			XSI::CString prog_id = emission_node.GetProgID();
			if (prog_id == "LUXShadersPlugin.Emission.1.0")
			{
				XSI::CParameterRefArray emission_params = emission_node.GetParameters();
				set_material_value(scene, lux_props, "color", prefix, emission_params, eval_time);
				lux_props.Set(luxrays::Property(prefix + ".power")(0));
				lux_props.Set(luxrays::Property(prefix + ".efficency")(0));
				float gain = get_float_parameter_value(emission_params, "gain", eval_time);
				float exposure = get_float_parameter_value(emission_params, "exposure", eval_time);
				gain = gain * pow(2.0f, exposure);
				lux_props.Set(luxrays::Property(prefix + ".gain")(gain, gain, gain));
				lux_props.Set(luxrays::Property(prefix + ".normalizebycolor")(false));
				lux_props.Set(luxrays::Property(prefix + ".gain.normalizebycolor")(false));
				lux_props.Set(luxrays::Property(prefix + ".importance")(get_float_parameter_value(emission_params, "importance", eval_time)));
				lux_props.Set(luxrays::Property(prefix + ".theta")(get_float_parameter_value(emission_params, "theta", eval_time)));
				lux_props.Set(luxrays::Property(prefix + ".id")(get_int_parameter_value(emission_params, "lightgroup_id", eval_time)));
				lux_props.Set(luxrays::Property(prefix + ".directlightsampling.type")(get_string_parameter_value(emission_params, "dls_type", eval_time).GetAsciiString()));
			}
		}
		else
		{
			//there are no connections with emission parameter
		}
	}
	else
	{
		//there are no emission parameter in the node
	}
}

std::string add_texture(luxcore::Scene* scene, XSI::Shader &texture_node, const XSI::CTime &eval_time)
{
	std::string output_name = xsi_object_id_string(texture_node)[0];
	std::string prefix = "scene.textures." + output_name;
	
	XSI::CString prog_id = texture_node.GetProgID();
	XSI::CStringArray parts = prog_id.Split(".");
	luxrays::Properties texture_props;
	if (parts.GetCount() >= 2 && parts[0] == "Softimage")
	{
		//some built-in nodes
		XSI::CParameterRefArray parameters = texture_node.GetParameters();
		XSI::CString node_name = parts[1];
		if (node_name == "sib_scalar_to_color")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("constfloat1"));
			float input = get_float_parameter_value(parameters, "input", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".value")(input));
		}
		else if (node_name == "sib_color_to_scalar")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("constfloat3"));
			XSI::MATH::CColor4f input = get_color_parameter_value(parameters, "input", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".value")(input.GetR(), input.GetG(), input.GetB()));
		}
		else
		{
			//unknown built-in node
			output_name = "";
		}
	}
	else if (parts.GetCount() >= 2 && parts[0] == "LUXShadersPlugin")
	{
		XSI::CString node_name = parts[1];
		XSI::CParameterRefArray parameters = texture_node.GetParameters();
		if (node_name == "TextureFloat")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("constfloat1"));
			float value = get_float_parameter_value(parameters, "value", eval_time);

			texture_props.Set(luxrays::Property(prefix + ".value")(value));
		}
		else if (node_name == "TextureColor")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("constfloat3"));
			XSI::MATH::CColor4f value = get_color_parameter_value(parameters, "value", eval_time);

			texture_props.Set(luxrays::Property(prefix + ".value")(value.GetR(), value.GetG(), value.GetB()));
		}
		else if (node_name == "TextureImage")
		{
			//check is image is selected
			XSI::ShaderParameter file_param = parameters.GetItem("file");
			XSI::Parameter file_param_final = get_source_parameter(file_param);
			XSI::CRef file_source = file_param_final.GetSource();
			if (file_source.IsValid())
			{
				XSI::ImageClip2 clip(file_source);
				std::string file = std::string(clip.GetFileName().GetAsciiString());
				if (file.size() > 0)
				{
					//file selected, so, we can set the image
					texture_props.Set(luxrays::Property(prefix + ".type")("imagemap"));
					texture_props.Set(luxrays::Property(prefix + ".file")(file));
					XSI::CString wrap = get_string_parameter_value(parameters, "wrap", eval_time);
					bool randomizedtiling_enable = get_bool_parameter_value(parameters, "randomizedtiling_enable", eval_time);
					texture_props.Set(luxrays::Property(prefix + ".wrap")(wrap.GetAsciiString()));
					texture_props.Set(luxrays::Property(prefix + ".randomizedtiling.enable")(randomizedtiling_enable && wrap == "repeat"));
					XSI::CString filter = get_string_parameter_value(parameters, "filter", eval_time);
					texture_props.Set(luxrays::Property(prefix + ".filter")(filter.GetAsciiString()));

					bool is_normal_map = get_bool_parameter_value(parameters, "is_normal_map", eval_time);
					if (is_normal_map)
					{
						XSI::CString normalmap_orientation = get_string_parameter_value(parameters, "normalmap_orientation", eval_time);
						if (normalmap_orientation == "opengl")
						{
							texture_props.Set(luxrays::Property(prefix + ".channel")("rgb"));
						}
						else
						{
							texture_props.Set(luxrays::Property(prefix + ".channel")("directx2opengl_normalmap"));
						}
						texture_props.Set(luxrays::Property(prefix + ".gamma")(1.0));
						texture_props.Set(luxrays::Property(prefix + ".gain")(1.0));
					}
					else
					{
						XSI::CString channels = get_string_parameter_value(parameters, "channels", eval_time);
						texture_props.Set(luxrays::Property(prefix + ".channel")(channels.GetAsciiString()));
						float gamma = get_float_parameter_value(parameters, "gamma", eval_time);
						float brightness = get_float_parameter_value(parameters, "brightness", eval_time);
						texture_props.Set(luxrays::Property(prefix + ".gamma")(gamma));
						texture_props.Set(luxrays::Property(prefix + ".gain")(brightness));
					}
					add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_2d", eval_time);

					XSI::CString projection = get_string_parameter_value(parameters, "projection", eval_time);
					if (projection == "box")
					{
						//here we should add triplanar mapping
						//parse property in the scene, finish created texture
						scene->Parse(texture_props);

						std::string triplanar_prefix = "scene.textures." + output_name + "_triplanar";
						luxrays::Properties triplanar_props;
						triplanar_props.Set(luxrays::Property(triplanar_prefix + ".type")("triplanar"));
						triplanar_props.Set(luxrays::Property(triplanar_prefix + ".texture1")(output_name));
						triplanar_props.Set(luxrays::Property(triplanar_prefix + ".texture2")(output_name));
						triplanar_props.Set(luxrays::Property(triplanar_prefix + ".texture3")(output_name));
						triplanar_props.Set(luxrays::Property(triplanar_prefix + ".mapping.type")("localmapping3d"));
						texture_props = triplanar_props;
						output_name = output_name + "_triplanar";
					}

					if (is_normal_map)
					{
						//parse property in the scene, finish created texture
						scene->Parse(texture_props);

						//and then create normal texture
						std::string normal_prefix = "scene.textures." + output_name + "_normalmap";
						luxrays::Properties normal_props;
						normal_props.Set(luxrays::Property(normal_prefix + ".type")("normalmap"));
						normal_props.Set(luxrays::Property(normal_prefix + ".texture")(output_name));
						float height = get_float_parameter_value(parameters, "height", eval_time);
						normal_props.Set(luxrays::Property(normal_prefix + ".scale")(height));

						//at the end we will finish this texture
						texture_props = normal_props;
						//and return new name
						output_name = output_name + "_normalmap";
					}
				}
				else
				{
					output_name = "";
				}
			}
			else
			{
				output_name = "";
			}

		}
		else
		{
			output_name = "";
		}
	}
	else
	{
		output_name = "";
	}

	if (output_name.size() > 0)
	{
		scene->Parse(texture_props);
	}

	return output_name;
}

void set_material_value(luxcore::Scene *scene, 
	luxrays::Properties& material_props, 
	const XSI::CString &xsi_param_name, 
	const std::string &lux_param_name, 
	XSI::CParameterRefArray &parameters, 
	const XSI::CTime &eval_time,
	bool ignore_set_branch)
{
	XSI::Parameter xsi_param = parameters.GetItem(xsi_param_name);
	XSI::Parameter xsi_finall_parameter = get_source_parameter(xsi_param);
	ShaderParameterType parameter_type = get_shader_parameter_type(xsi_finall_parameter);

	//check is this parameter has connections or not
	XSI::CRef param_source = xsi_finall_parameter.GetSource();
	bool has_connection = param_source.IsValid();
	if (has_connection)
	{
		//get source node
		XSI::ShaderParameter shader_parameter(xsi_finall_parameter);
		XSI::Shader node = get_input_node(shader_parameter, true);  // we find this node by goes through default converter nodes, so, it can be this converter
		//if there is connection, try to export corresponding texture or material
		if (parameter_type == ShaderParameterType::ParameterType_Color4)
		{
			//parameter connected to material port
			//so, try to add new material
			std::string material_name = add_material(scene, node, eval_time);
			if (material_name.size() > 0)
			{
				material_props.Set(luxrays::Property(lux_param_name)(material_name));
			}
			else
			{
				has_connection = false;
			}
		}
		else if (parameter_type == ShaderParameterType::ParameterType_Float || 
				 parameter_type == ShaderParameterType::ParameterType_Color3)
		{
			//float (or color3) parameter is connected to some node
			//try to recognize the texture of this node
			std::string texture_name = add_texture(scene, node, eval_time);
			if (texture_name.size() > 0)
			{
				material_props.Set(luxrays::Property(lux_param_name)(texture_name));
			}
			else
			{
				//recognize texture fails, so, mark this connection as empty
				has_connection = false;
			}
		}
		else
		{
			//if fails, then interpret as parameter has no connections
			has_connection = false;
		}
	}

	if (!has_connection && !ignore_set_branch)
	{
		//no connections, set numeric values
		if (parameter_type == ShaderParameterType::ParameterType_Float)
		{
			material_props.Set(luxrays::Property(lux_param_name)((float)xsi_finall_parameter.GetValue(eval_time)));
		}
		else if (parameter_type == ShaderParameterType::ParameterType_Integer)
		{
			material_props.Set(luxrays::Property(lux_param_name)((int)xsi_finall_parameter.GetValue(eval_time)));
		}
		else if (parameter_type == ShaderParameterType::ParameterType_String)
		{
			material_props.Set(luxrays::Property(lux_param_name)(((XSI::CString)xsi_finall_parameter.GetValue(eval_time)).GetAsciiString()));
		}
		else if (parameter_type == ShaderParameterType::ParameterType_Boolean)
		{
			material_props.Set(luxrays::Property(lux_param_name)((bool)xsi_finall_parameter.GetValue(eval_time)));
		}
		else if (parameter_type == ShaderParameterType::ParameterType_Color3)
		{
			XSI::CParameterRefArray params_array = xsi_finall_parameter.GetParameters();
			XSI::Parameter p[3];
			p[0] = XSI::Parameter(params_array[0]);
			p[1] = XSI::Parameter(params_array[1]);
			p[2] = XSI::Parameter(params_array[2]);
			float r = p[0].GetValue(eval_time);
			float g = p[1].GetValue(eval_time);
			float b = p[2].GetValue(eval_time);
			material_props.Set(luxrays::Property(lux_param_name)(r, g, b));
		}
		else if (parameter_type == ShaderParameterType::ParameterType_Color4)
		{
			//if 4-valued color has empty connections, then we should set null material for this conenction
			//because 4-valued color is material
			//create implicit null material and connect it
			if (!scene->IsMaterialDefined("implicit_null"))
			{
				luxrays::Properties new_material;
				new_material.Set(luxrays::Property("scene.materials.implicit_null.type")("null"));
				scene->Parse(new_material);
			}
			material_props.Set(luxrays::Property(lux_param_name)("implicit_null"));
		}
	}
}

//return name of the added material
//if this method is called recursivly, then return name from id of the material node
//if this method is called from sync_material, then returned name should be overrided by the input one
//return empty string if material node is invalid
//in thie case replace this materia by null material or default material
std::string add_material(luxcore::Scene* scene, XSI::Shader &material_node, const XSI::CTime& eval_time, std::string override_name)
{
	//get shader name
	luxrays::Properties material_props;
	XSI::CString prog_id = material_node.GetProgID();
	std::string output_name;
	if (override_name.size() > 0)
	{
		output_name = override_name;
	}
	else
	{
		output_name = xsi_object_id_string(material_node)[0];  // use node id as output material name
	}
	XSI::CStringArray parts = prog_id.Split(".");
	if (parts.GetCount() >= 2 && parts[0] == "LUXShadersPlugin")
	{
		XSI::CString node_name = parts[1];
		std::string prefix = "scene.materials." + output_name;
		//get material node properties
		//this array contains as input parameters, as output ports
		//also the Name of the node
		XSI::CParameterRefArray parameters = material_node.GetParameters();
		bool setup_default = false;
		if (node_name == "ShaderMatte")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("roughmatte"));
			set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, eval_time);
			set_material_value(scene, material_props, "sigma", prefix + ".sigma", parameters, eval_time);
			setup_default = true;
		}
		else if (node_name == "ShaderMirror")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("mirror"));
			set_material_value(scene, material_props, "kr", prefix + ".kr", parameters, eval_time);
			setup_default = true;
		}
		else if (node_name == "ShaderGlass")
		{
			//glass shader is combination of several Luxcore shader types
			//so, we should get non-animatable parameters and choose proper shader type
			XSI::CString glass_mode = get_string_parameter_value(parameters, "glass_mode", eval_time);
			bool is_film = get_bool_parameter_value(parameters, "is_film", eval_time);
			bool is_anisotropic = get_bool_parameter_value(parameters, "is_anisotropic", eval_time);

			if (glass_mode == "architectural")
			{
				material_props.Set(luxrays::Property(prefix + ".type")("archglass"));
			}
			else if (glass_mode == "rough")
			{
				material_props.Set(luxrays::Property(prefix + ".type")("roughglass"));
			}
			else
			{
				material_props.Set(luxrays::Property(prefix + ".type")("glass"));
			}
			set_material_value(scene, material_props, "kr", prefix + ".kr", parameters, eval_time);
			set_material_value(scene, material_props, "kt", prefix + ".kt", parameters, eval_time);
			set_material_value(scene, material_props, "exteriorior", prefix + ".exteriorior", parameters, eval_time);
			set_material_value(scene, material_props, "interiorior", prefix + ".interiorior", parameters, eval_time);
			if (glass_mode == "default")
			{
				set_material_value(scene, material_props, "cauchyb", prefix + ".cauchyb", parameters, eval_time);
			}
			if (is_film)
			{
				set_material_value(scene, material_props, "filmthickness", prefix + ".filmthickness", parameters, eval_time);
				set_material_value(scene, material_props, "filmior", prefix + ".filmior", parameters, eval_time);
			}
			if (glass_mode == "rough")
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".uroughness", parameters, eval_time);
				if (is_anisotropic)
				{
					set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, eval_time);
				}
				else
				{
					set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, eval_time);
				}
			}
			setup_default = true;
		}
		else if (node_name == "ShaderMix")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("mix"));
			set_material_value(scene, material_props, "material1", prefix + ".material1", parameters, eval_time);
			set_material_value(scene, material_props, "material2", prefix + ".material2", parameters, eval_time);
			set_material_value(scene, material_props, "amount", prefix + ".amount", parameters, eval_time);
			setup_default = true;
		}
		else if (node_name == "ShaderNull")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("null"));
		}
		else if (node_name == "ShaderMatteTranslucent")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("roughmattetranslucent"));
			set_material_value(scene, material_props, "kr", prefix + ".kr", parameters, eval_time);
			set_material_value(scene, material_props, "kt", prefix + ".kt", parameters, eval_time);
			set_material_value(scene, material_props, "sigma", prefix + ".sigma", parameters, eval_time);
			setup_default = true;
		}
		else if (node_name == "ShaderGlossy")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("glossy2"));
			bool is_anisotropic = get_bool_parameter_value(parameters, "is_anisotropic", eval_time);
			bool multibounce = get_bool_parameter_value(parameters, "multibounce", eval_time);
			material_props.Set(luxrays::Property(prefix + ".multibounce")(multibounce));
			set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, eval_time);
			set_material_value(scene, material_props, "ks", prefix + ".ks", parameters, eval_time);
			set_material_value(scene, material_props, "ka", prefix + ".ka", parameters, eval_time);
			set_material_value(scene, material_props, "d", prefix + ".d", parameters, eval_time);
			set_material_value(scene, material_props, "index", prefix + ".index", parameters, eval_time);
			if (is_anisotropic)
			{
				set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, eval_time);
			}
			else
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, eval_time);
			}

			setup_default = true;
		}
		else if (node_name == "ShaderMetal")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("metal2"));
			bool is_anisotropic = get_bool_parameter_value(parameters, "is_anisotropic", eval_time);
			XSI::CString metal_mode = get_string_parameter_value(parameters, "metal_mode", eval_time);
			if (is_anisotropic)
			{
				set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, eval_time);
			}
			else
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, eval_time);
			}
			if (metal_mode == "color")
			{
				//create inline fresnel
				luxrays::Properties fresnel_props;
				std::string frensel_name = output_name + "_fresnel";
				fresnel_props.Set(luxrays::Property("scene.textures." + frensel_name + ".type")("fresnelcolor"));

				set_material_value(scene, fresnel_props, "kr", "scene.textures." + frensel_name + ".kr", parameters, eval_time);
				scene->Parse(fresnel_props);

				//connect generated fresnel to the material
				material_props.Set(luxrays::Property(prefix + ".fresnel")(frensel_name));
			}
			else if (metal_mode == "preset")
			{
				XSI::CString preset = get_string_parameter_value(parameters, "metal_preset", eval_time);
				material_props.Set(luxrays::Property(prefix + ".preset")(replace_symbols(preset, XSI::CString("_"), XSI::CString(" ")).GetAsciiString()));
			}
			else
			{//n, k mode
				XSI::MATH::CColor4f n_color = get_color_parameter_value(parameters, "n", eval_time);
				XSI::MATH::CColor4f k_color = get_color_parameter_value(parameters, "k", eval_time);

				material_props.Set(luxrays::Property(prefix + ".n")(n_color.GetR(), n_color.GetG(), n_color.GetB()));
				material_props.Set(luxrays::Property(prefix + ".k")(k_color.GetR(), k_color.GetG(), k_color.GetB()));
			}

			setup_default = true;
		}
		else if (node_name == "ShaderVelvet")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("velvet"));
			set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, eval_time);
			set_material_value(scene, material_props, "thickness", prefix + ".thickness", parameters, eval_time);
			bool advanced = get_bool_parameter_value(parameters, "advanced", eval_time);
			if (advanced)
			{
				set_material_value(scene, material_props, "p1", prefix + ".p1", parameters, eval_time);
				set_material_value(scene, material_props, "p2", prefix + ".p2", parameters, eval_time);
				set_material_value(scene, material_props, "p3", prefix + ".p3", parameters, eval_time);
			}
			setup_default = true;
		}
		else if (node_name == "ShaderCloth")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("cloth"));
			XSI::CString preset = get_string_parameter_value(parameters, "preset", eval_time);
			material_props.Set(luxrays::Property(prefix + ".preset")(preset.GetAsciiString()));

			float repeat_u = get_float_parameter_value(parameters, "repeat_u", eval_time);
			float repeat_v = get_float_parameter_value(parameters, "repeat_v", eval_time);
			material_props.Set(luxrays::Property(prefix + ".repeat_u")(repeat_u));
			material_props.Set(luxrays::Property(prefix + ".repeat_v")(repeat_v));

			set_material_value(scene, material_props, "weft_kd", prefix + ".weft_kd", parameters, eval_time);
			set_material_value(scene, material_props, "weft_ks", prefix + ".weft_ks", parameters, eval_time);
			set_material_value(scene, material_props, "warp_kd", prefix + ".warp_kd", parameters, eval_time);
			set_material_value(scene, material_props, "warp_ks", prefix + ".warp_ks", parameters, eval_time);

			setup_default = true;
		}
		else if (node_name == "ShaderCarpaint")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("carpaint"));
			XSI::CString preset = get_string_parameter_value(parameters, "preset", eval_time);
			if (preset == "manual")
			{
				set_material_value(scene, material_props, "ka", prefix + ".ka", parameters, eval_time);
				set_material_value(scene, material_props, "d", prefix + ".d", parameters, eval_time);
				set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, eval_time);
				set_material_value(scene, material_props, "ks1", prefix + ".ks1", parameters, eval_time);
				set_material_value(scene, material_props, "ks2", prefix + ".ks2", parameters, eval_time);
				set_material_value(scene, material_props, "ks3", prefix + ".ks3", parameters, eval_time);
				set_material_value(scene, material_props, "r1", prefix + ".r1", parameters, eval_time);
				set_material_value(scene, material_props, "r2", prefix + ".r2", parameters, eval_time);
				set_material_value(scene, material_props, "r3", prefix + ".r3", parameters, eval_time);
				set_material_value(scene, material_props, "m1", prefix + ".m1", parameters, eval_time);
				set_material_value(scene, material_props, "m2", prefix + ".m2", parameters, eval_time);
				set_material_value(scene, material_props, "m3", prefix + ".m3", parameters, eval_time);
			}
			else
			{
				material_props.Set(luxrays::Property(prefix + ".preset")(replace_symbols(preset, "_", " ").GetAsciiString()));
			}

			setup_default = true;
		}
		else if (node_name == "ShaderGlossyTranslucent")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("glossytranslucent"));
			bool multibounce = get_bool_parameter_value(parameters, "multibounce", eval_time);
			bool is_anisotropic = get_bool_parameter_value(parameters, "is_anisotropic", eval_time);
			bool is_double = get_bool_parameter_value(parameters, "is_double", eval_time);
			set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, eval_time);
			set_material_value(scene, material_props, "kt", prefix + ".kt", parameters, eval_time);
			set_material_value(scene, material_props, "ks", prefix + ".ks", parameters, eval_time);
			set_material_value(scene, material_props, is_double ? "ks_bf" : "ks", prefix + ".ks_bf", parameters, eval_time);
			set_material_value(scene, material_props, "uroughness", prefix + ".uroughness", parameters, eval_time);
			set_material_value(scene, material_props, is_double ? "uroughness_bf" : "uroughness", prefix + ".uroughness_bf", parameters, eval_time);
			if (is_anisotropic)
			{
				set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, eval_time);
				set_material_value(scene, material_props, is_double ? "vroughness_bf" : "vroughness", prefix + ".vroughness_bf", parameters, eval_time);
			}
			else
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, eval_time);
				set_material_value(scene, material_props, is_double ? "uroughness_bf" : "uroughness", prefix + ".vroughness_bf", parameters, eval_time);
			}
			set_material_value(scene, material_props, "ka", prefix + ".ka", parameters, eval_time);
			set_material_value(scene, material_props, is_double ? "ka_bf" : "ka", prefix + ".ka_bf", parameters, eval_time);
			set_material_value(scene, material_props, "d", prefix + ".d", parameters, eval_time);
			set_material_value(scene, material_props, is_double ? "d_bf" : "d", prefix + ".d_bf", parameters, eval_time);
			set_material_value(scene, material_props, "index", prefix + ".index", parameters, eval_time);
			set_material_value(scene, material_props, is_double ? "index_bf" : "index", prefix + ".index_bf", parameters, eval_time);
			material_props.Set(luxrays::Property(prefix + ".multibounce")(multibounce));
			material_props.Set(luxrays::Property(prefix + ".multibounce_bf")(multibounce));

			setup_default = true;
		}
		else if (node_name == "ShaderGlossyCoating")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("glossycoating"));
			bool multibounce = get_bool_parameter_value(parameters, "multibounce", eval_time);
			bool is_anisotropic = get_bool_parameter_value(parameters, "is_anisotropic", eval_time);
			set_material_value(scene, material_props, "base", prefix + ".base", parameters, eval_time);
			set_material_value(scene, material_props, "ks", prefix + ".ks", parameters, eval_time);
			set_material_value(scene, material_props, "ka", prefix + ".ka", parameters, eval_time);
			set_material_value(scene, material_props, "d", prefix + ".d", parameters, eval_time);
			set_material_value(scene, material_props, "uroughness", prefix + ".uroughness", parameters, eval_time);
			if (is_anisotropic)
			{
				set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, eval_time);
			}
			else
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, eval_time);
			}
			set_material_value(scene, material_props, "index", prefix + ".index", parameters, eval_time);
			material_props.Set(luxrays::Property(prefix + ".multibounce")(multibounce));

			setup_default = true;
		}
		else if (node_name == "ShaderDisney")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("disney"));
			bool is_film = get_bool_parameter_value(parameters, "is_film", eval_time);
			set_material_value(scene, material_props, "basecolor", prefix + ".basecolor", parameters, eval_time);
			set_material_value(scene, material_props, "subsurface", prefix + ".subsurface", parameters, eval_time);
			set_material_value(scene, material_props, "roughness", prefix + ".roughness", parameters, eval_time);
			set_material_value(scene, material_props, "metallic", prefix + ".metallic", parameters, eval_time);
			set_material_value(scene, material_props, "specular", prefix + ".specular", parameters, eval_time);
			set_material_value(scene, material_props, "speculartint", prefix + ".speculartint", parameters, eval_time);
			set_material_value(scene, material_props, "clearcoat", prefix + ".clearcoat", parameters, eval_time);
			set_material_value(scene, material_props, "clearcoatgloss", prefix + ".clearcoatgloss", parameters, eval_time);
			set_material_value(scene, material_props, "anisotropic", prefix + ".anisotropic", parameters, eval_time);
			set_material_value(scene, material_props, "sheen", prefix + ".sheen", parameters, eval_time);
			set_material_value(scene, material_props, "sheentint", prefix + ".sheentint", parameters, eval_time);
			if (is_film)
			{
				set_material_value(scene, material_props, "filmamount", prefix + ".filmamount", parameters, eval_time);
				set_material_value(scene, material_props, "filmthickness", prefix + ".filmthickness", parameters, eval_time);
				set_material_value(scene, material_props, "filmior", prefix + ".filmior", parameters, eval_time);
			}

			setup_default = true;
		}
		else if (node_name == "ShaderTwoSided")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("twosided"));
			set_material_value(scene, material_props, "frontmaterial", prefix + ".frontmaterial", parameters, eval_time);
			set_material_value(scene, material_props, "backmaterial", prefix + ".backmaterial", parameters, eval_time);

			setup_default = true;
		}
		else
		{
			output_name = "";
		}

		if (setup_default)
		{
			//default material values
			//any material (except null) contains these parameters
			set_material_value(scene, material_props, "transparency", prefix + ".transparency", parameters, eval_time);
			XSI::MATH::CColor4f transparency_shadow = get_color_parameter_value(parameters, "transparency_shadow", eval_time);
			bool visibility_indirect_diffuse_enable = get_bool_parameter_value(parameters, "visibility_indirect_diffuse_enable", eval_time);
			bool visibility_indirect_glossy_enable = get_bool_parameter_value(parameters, "visibility_indirect_glossy_enable", eval_time);
			bool visibility_indirect_specular_enable = get_bool_parameter_value(parameters, "visibility_indirect_specular_enable", eval_time);
			bool shadowcatcher_enable = get_bool_parameter_value(parameters, "shadowcatcher_enable", eval_time);
			bool shadowcatcher_onlyinfinitelights = get_bool_parameter_value(parameters, "shadowcatcher_onlyinfinitelights", eval_time);
			bool photongi_enable = get_bool_parameter_value(parameters, "photongi_enable", eval_time);
			bool holdout_enable = get_bool_parameter_value(parameters, "holdout_enable", eval_time);

			material_props.Set(luxrays::Property(prefix + ".transparency.shadow")(transparency_shadow.GetR(), transparency_shadow.GetG(), transparency_shadow.GetB()));
			material_props.Set(luxrays::Property(prefix + ".visibility.indirect.diffuse.enable")(visibility_indirect_diffuse_enable));
			material_props.Set(luxrays::Property(prefix + ".visibility.indirect.glossy.enable")(visibility_indirect_glossy_enable));
			material_props.Set(luxrays::Property(prefix + ".visibility.indirect.specular.enable")(visibility_indirect_specular_enable));
			material_props.Set(luxrays::Property(prefix + ".shadowcatcher.enable")(shadowcatcher_enable));
			material_props.Set(luxrays::Property(prefix + ".shadowcatcher.onlyinfinitelights")(shadowcatcher_onlyinfinitelights));
			material_props.Set(luxrays::Property(prefix + ".photongi.enable")(photongi_enable));
			material_props.Set(luxrays::Property(prefix + ".holdout.enable")(holdout_enable));

			//try to add emission
			add_emission(scene, parameters, material_props, prefix + ".emission", eval_time);
			//next bump
			set_material_value(scene, material_props, "bump", prefix + ".bumptex", parameters, eval_time, true);
			//and normal
			set_material_value(scene, material_props, "normal", prefix + ".normaltex", parameters, eval_time, true);
		}
	}
	else
	{
		output_name = "";
	}

	if (output_name.size() > 0)
	{
		scene->Parse(material_props);
	}
	return output_name;
}

void sync_material(luxcore::Scene* scene, XSI::Material &xsi_material, std::set<ULONG>& xsi_materials_in_lux, const XSI::CTime& eval_time)
{
	//we should set the name of material equal to UniqueID of the object
	//next we should export some basic material
	//and assign it to the polygon meshes

	std::string material_name = xsi_object_id_string(xsi_material)[0];

	XSI::CRefArray first_level_shaders = xsi_material.GetShaders();
	std::vector<XSI::ShaderParameter> surface_ports = get_root_shader_parameter(first_level_shaders, GRSPM_ParameterName, "surface");
	if (surface_ports.size() == 0)
	{//no connections to the surface port
		//set material to null
		luxrays::Properties material_props;
		material_props.Set(luxrays::Property("scene.materials." + material_name + ".type")("null"));
		//set default id
		material_props.Set(luxrays::Property("scene.materials." + material_name + ".id")((int)xsi_material.GetObjectID()));

		scene->Parse(material_props);
	}
	else
	{
		//get the material node
		XSI::Shader material_node = get_input_node(surface_ports[0]);
		//next we should add material from the selected node
		std::string add_name = add_material(scene, material_node, eval_time, material_name);
		if (add_name.size() == 0)
		{
			//this is unknown material node
			//in this case set default material
			luxrays::Properties material_props;
			material_props.Set(luxrays::Property("scene.materials." + material_name + ".type")("matte"));
			material_props.Set(luxrays::Property("scene.materials." + material_name + ".kd")(0.8, 0.8, 0.8));
			material_props.Set(luxrays::Property("scene.materials." + material_name + ".id")((int)xsi_material.GetObjectID()));
			scene->Parse(material_props);
		}
	}

	xsi_materials_in_lux.insert(xsi_material.GetObjectID());
}

void override_material(luxcore::Scene* scene, XSI::X3DObject &xsi_object, const std::string material_name)
{
	//get object materials
	XSI::CRefArray xsi_materials = xsi_object.GetMaterials();

	std::vector<std::string> object_names = xsi_object_id_string(xsi_object);
	for (ULONG i = 0; i < object_names.size(); i++)
	{
		scene->UpdateObjectMaterial(object_names[i], material_name);
	}
}

void sync_default_material(luxcore::Scene* scene)
{
	//export default material
	luxrays::Properties default_props;
	default_props.Set(luxrays::Property("scene.materials.default_material.type")("matte"));
	default_props.Set(luxrays::Property("scene.materials.default_material.kd")(0.8, 0.8, 0.8));
	default_props.Set(luxrays::Property("scene.materials.default_material.id")(0));
	scene->Parse(default_props);
}

void sync_materials(luxcore::Scene *scene, const XSI::Scene &xsi_scene, std::set<ULONG>& xsi_materials_in_lux, const XSI::CTime &eval_time)
{
	if (!scene->IsMaterialDefined("default_material"))
	{
		sync_default_material(scene);
	}

	XSI::CRefArray material_libraries = xsi_scene.GetMaterialLibraries();
	for (LONG lib_index = 0; lib_index < material_libraries.GetCount(); lib_index++)
	{
		XSI::MaterialLibrary libray = material_libraries.GetItem(lib_index);
		XSI::CRefArray materials = libray.GetItems();
		for (LONG mat_index = 0; mat_index < materials.GetCount(); mat_index++)
		{
			XSI::Material xsi_material = materials.GetItem(mat_index);
			XSI::CRefArray used_objects = xsi_material.GetUsedBy();
			if (used_objects.GetCount() > 0)
			{
				sync_material(scene, xsi_material, xsi_materials_in_lux, eval_time);
			}
		}
	}
}

void reassign_all_materials(luxcore::Scene* scene, const XSI::Scene& xsi_scene, std::set<ULONG>& xsi_materials_in_lux, std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map, const XSI::CTime &eval_time)
{
	XSI::CRefArray material_libraries = xsi_scene.GetMaterialLibraries();
	for (LONG lib_index = 0; lib_index < material_libraries.GetCount(); lib_index++)
	{
		XSI::MaterialLibrary libray = material_libraries.GetItem(lib_index);
		XSI::CRefArray materials = libray.GetItems();
		for (LONG mat_index = 0; mat_index < materials.GetCount(); mat_index++)
		{
			XSI::Material xsi_material = materials.GetItem(mat_index);
			std::string lux_material_name = xsi_object_id_string(xsi_material)[0];
			ULONG material_id = xsi_material.GetObjectID();
			XSI::CRefArray used_objects = xsi_material.GetUsedBy();
			for (ULONG obj_index = 0; obj_index < used_objects.GetCount(); obj_index++)
			{
				XSI::CRef object_ref = used_objects[obj_index];
				XSI::siClassID object_class = object_ref.GetClassID();
				if (object_class == XSI::siX3DObjectID)
				{
					XSI::X3DObject object(object_ref);
					ULONG object_id = object.GetObjectID();
					if (!scene->IsMaterialDefined(lux_material_name))
					{
						sync_material(scene, xsi_material, xsi_materials_in_lux, eval_time);
					}

					if(xsi_materials_in_lux.contains(material_id) && xsi_id_to_lux_names_map.contains(object_id))
					{
						std::vector<std::string> object_names = xsi_id_to_lux_names_map[object_id];
						for (ULONG i = 0; i < object_names.size(); i++)
						{
							scene->UpdateObjectMaterial(object_names[i], lux_material_name);
						}
					}
					else
					{
						
					}
				}
			}
		}
	}
}