#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "lux_scene.h"

#include "xsi_materiallibrary.h"
#include "xsi_imageclip2.h"

#include <unordered_map>

//return true if we add some mapping to the texture
bool add_mapping(luxrays::Properties &texture_props, const std::string &prefix, XSI::CParameterRefArray &parameters, const XSI::CString &mapping_parameter_name, const XSI::CTime &eval_time, const bool force_3dmapping = false)
{
	bool to_return = false;
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
				to_return = true;
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
					texture_props.Set(luxrays::Property(prefix + ".uvscale")(u_scale, v_scale));

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
			else if (prog_id == "LUXShadersPlugin.Mapping3D.1.0")
			{
				to_return = true;
				XSI::CParameterRefArray map_params = mapping_node.GetParameters();
				XSI::CString mapping_type = get_string_parameter_value(map_params, "mapping_type", eval_time);
				int uvindex = get_int_parameter_value(map_params, "uvindex", eval_time);
				bool is_uniform = get_bool_parameter_value(map_params, "is_uniform", eval_time);
				if (mapping_type == "globalmapping3d" || mapping_type == "localmapping3d" || mapping_type == "uvmapping3d")
				{
					//get SRT values
					float scale_x = get_float_parameter_value(map_params, "scale_x", eval_time);
					float scale_y = get_float_parameter_value(map_params, "scale_y", eval_time);
					float scale_z = get_float_parameter_value(map_params, "scale_z", eval_time);
					if (is_uniform)
					{
						scale_y = scale_x;
						scale_z = scale_x;
					}
					if (abs(scale_x) < 0.000001f) { scale_x = 0.000001f; }
					if (abs(scale_y) < 0.000001f) { scale_y = 0.000001f; }
					if (abs(scale_z) < 0.000001f) { scale_z = 0.000001f; }
					//we should convert rotation angles to radians
					float rotate_x = DEG2RADF(get_float_parameter_value(map_params, "rotate_x", eval_time));
					float rotate_y = DEG2RADF(get_float_parameter_value(map_params, "rotate_y", eval_time));
					float rotate_z = DEG2RADF(get_float_parameter_value(map_params, "rotate_z", eval_time));
					float translate_x = get_float_parameter_value(map_params, "translate_x", eval_time);
					float translate_y = get_float_parameter_value(map_params, "translate_y", eval_time);
					float translate_z = get_float_parameter_value(map_params, "translate_z", eval_time);

					XSI::MATH::CTransformation tfm_translate;
					tfm_translate.SetIdentity();
					tfm_translate.SetTranslationFromValues(translate_x, translate_y, translate_z);
					XSI::MATH::CTransformation tfm_rotate;
					tfm_rotate.SetIdentity();
					tfm_rotate.SetRotationFromXYZAnglesValues(rotate_x, rotate_y, rotate_z);
					XSI::MATH::CTransformation tfm_scale;
					tfm_scale.SetIdentity();
					tfm_scale.SetScalingFromValues(scale_x, scale_y, scale_z);
					XSI::MATH::CTransformation tfm;
					tfm.SetIdentity();
					tfm.MulInPlace(tfm_scale);
					tfm.MulInPlace(tfm_rotate);
					tfm.MulInPlace(tfm_translate);

					texture_props.Set(luxrays::Property(prefix + ".type")(mapping_type.GetAsciiString()));
					texture_props.Set(luxrays::Property(prefix + ".uvindex")(uvindex));
					texture_props.Set(luxrays::Property(prefix + ".transformation")(xsi_to_lux_matrix(tfm.GetMatrix4(), false)));
				}
				else if(mapping_type == "localrandommapping3d")
				{
					//random mapping
					texture_props.Set(luxrays::Property(prefix + ".type")("localrandommapping3d"));
					texture_props.Set(luxrays::Property(prefix + ".uvindex")(uvindex));
					float rotate_x = DEG2RADF(get_float_parameter_value(map_params, "rotate_x", eval_time));
					float rotate_y = DEG2RADF(get_float_parameter_value(map_params, "rotate_y", eval_time));
					float rotate_z = DEG2RADF(get_float_parameter_value(map_params, "rotate_z", eval_time));
					float rotate_max_x = DEG2RADF(get_float_parameter_value(map_params, "rotate_max_x", eval_time));
					float rotate_max_y = DEG2RADF(get_float_parameter_value(map_params, "rotate_max_y", eval_time));
					float rotate_max_z = DEG2RADF(get_float_parameter_value(map_params, "rotate_max_z", eval_time));
					bool is_random_rotation = get_bool_parameter_value(map_params, "is_random_rotation", eval_time);
					if (!is_random_rotation)
					{
						rotate_max_x = rotate_x;
						rotate_max_y = rotate_y;
						rotate_max_z = rotate_z;
					}
					texture_props.Set(luxrays::Property(prefix + ".xrotation")(rotate_z, rotate_max_x));
					texture_props.Set(luxrays::Property(prefix + ".yrotation")(rotate_y, rotate_max_y));
					texture_props.Set(luxrays::Property(prefix + ".zrotation")(rotate_z, rotate_max_z));

					float translate_x = get_float_parameter_value(map_params, "translate_x", eval_time);
					float translate_y = get_float_parameter_value(map_params, "translate_y", eval_time);
					float translate_z = get_float_parameter_value(map_params, "translate_z", eval_time);
					float translate_max_x = get_float_parameter_value(map_params, "translate_max_x", eval_time);
					float translate_max_y = get_float_parameter_value(map_params, "translate_max_y", eval_time);
					float translate_max_z = get_float_parameter_value(map_params, "translate_max_z", eval_time);
					bool is_random_translation = get_bool_parameter_value(map_params, "is_random_translation", eval_time);
					if (!is_random_translation)
					{
						translate_max_x = translate_x;
						translate_max_y = translate_y;
						translate_max_z = translate_z;
					}
					texture_props.Set(luxrays::Property(prefix + ".xtranslate")(translate_x, translate_max_x));
					texture_props.Set(luxrays::Property(prefix + ".ytranslate")(translate_y, translate_max_y));
					texture_props.Set(luxrays::Property(prefix + ".ztranslate")(translate_z, translate_max_z));

					texture_props.Set(luxrays::Property(prefix + ".xyzscale.uniform")(is_uniform));
					float scale_x = get_float_parameter_value(map_params, "scale_x", eval_time);
					float scale_y = get_float_parameter_value(map_params, "scale_y", eval_time);
					float scale_z = get_float_parameter_value(map_params, "scale_z", eval_time);
					float scale_max_x = get_float_parameter_value(map_params, "scale_max_x", eval_time);
					float scale_max_y = get_float_parameter_value(map_params, "scale_max_x", eval_time);
					float scale_max_z = get_float_parameter_value(map_params, "scale_max_z", eval_time);
					bool is_random_scale = get_bool_parameter_value(map_params, "is_random_scale", eval_time);
					if (!is_random_scale)
					{
						scale_max_x = scale_x;
						scale_max_y = scale_y;
						scale_max_z = scale_z;
					}
					if (is_uniform)
					{
						scale_y = scale_x;
						scale_z = scale_x;
						scale_max_y = scale_max_x;
						scale_max_z = scale_max_x;
					}
					texture_props.Set(luxrays::Property(prefix + ".xscale")(scale_x, scale_max_x));
					texture_props.Set(luxrays::Property(prefix + ".yscale")(scale_y, scale_max_y));
					texture_props.Set(luxrays::Property(prefix + ".zscale")(scale_z, scale_max_z));

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
			if (force_3dmapping)
			{
				texture_props.Set(luxrays::Property(prefix + ".type")("globalmapping3d"));
				XSI::MATH::CMatrix4 m;
				m.SetIdentity();
				texture_props.Set(luxrays::Property(prefix + ".transformation")(xsi_to_lux_matrix(m, false)));
			}
		}
	}
	else
	{
		//the node does not contains mapping parameter with input name
		//so, nothing to do
	}

	return to_return;
}

void add_emission(luxcore::Scene* scene, XSI::CParameterRefArray &parameters, luxrays::Properties &lux_props, const std::string &prefix, std::unordered_map<ULONG, std::string>& exported_nodes_map, const XSI::CTime &eval_time)
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
				set_material_value(scene, lux_props, "color", prefix, emission_params, exported_nodes_map, eval_time);
				float spread_angle = get_float_parameter_value(emission_params, "theta", eval_time);

				XSI::CString unit = get_string_parameter_value(emission_params, "unit", eval_time);
				float power = get_float_parameter_value(emission_params, "power", eval_time);
				float efficency = get_float_parameter_value(emission_params, "efficency", eval_time);
				bool normalized = get_bool_parameter_value(emission_params, "normalized", eval_time);
				if (unit == "power")
				{
					lux_props.Set(luxrays::Property(prefix + ".power")(power / (2.0f * M_PI * (1.0f - cos(DEG2RADF(spread_angle) / 2.0f)))));
					lux_props.Set(luxrays::Property(prefix + ".efficency")(efficency));
					lux_props.Set(luxrays::Property(prefix + ".normalizebycolor")(normalized));

					if (power < 0.0001f || efficency < 0.0001f){ lux_props.Set(luxrays::Property(prefix + ".gain")(0.0, 0.0, 0.0)); }
					else{ lux_props.Set(luxrays::Property(prefix + ".gain")(1.0, 1.0, 1.0)); }
				}
				else if (unit == "lumen")
				{
					float lumen = get_float_parameter_value(emission_params, "lumen", eval_time);
					lux_props.Set(luxrays::Property(prefix + ".power")(lumen / (2.0f * M_PI * (1.0f - cos(DEG2RADF(spread_angle) / 2.0f)))));
					lux_props.Set(luxrays::Property(prefix + ".efficency")(1.0));
					lux_props.Set(luxrays::Property(prefix + ".normalizebycolor")(normalized));
					if (lumen < 0.0001f) { lux_props.Set(luxrays::Property(prefix + ".gain")(0.0, 0.0, 0.0)); }
					else { lux_props.Set(luxrays::Property(prefix + ".gain")(1.0, 1.0, 1.0)); }
				}
				else if (unit == "candela")
				{
					bool per_square_meter = get_bool_parameter_value(emission_params, "per_square_meter", eval_time);
					float candela = get_float_parameter_value(emission_params, "candela", eval_time);
					if (per_square_meter)
					{
						lux_props.Set(luxrays::Property(prefix + ".power")(0));
						lux_props.Set(luxrays::Property(prefix + ".efficency")(0));
						lux_props.Set(luxrays::Property(prefix + ".gain")(candela, candela, candela));
						lux_props.Set(luxrays::Property(prefix + ".normalizebycolor")(normalized));
					}
					else
					{
						lux_props.Set(luxrays::Property(prefix + ".power")(candela * M_PI));
						lux_props.Set(luxrays::Property(prefix + ".efficency")(1.0f));
						lux_props.Set(luxrays::Property(prefix + ".normalizebycolor")(normalized));
						if (candela < 0.0001f) { lux_props.Set(luxrays::Property(prefix + ".gain")(0.0, 0.0, 0.0)); }
						else { lux_props.Set(luxrays::Property(prefix + ".gain")(1.0, 1.0, 1.0)); }
					}
				}
				else
				{
					lux_props.Set(luxrays::Property(prefix + ".power")(0));
					lux_props.Set(luxrays::Property(prefix + ".efficency")(0));
					float gain = get_float_parameter_value(emission_params, "gain", eval_time);
					float exposure = get_float_parameter_value(emission_params, "exposure", eval_time);
					gain = gain * pow(2.0f, exposure);
					lux_props.Set(luxrays::Property(prefix + ".gain")(gain, gain, gain));
					lux_props.Set(luxrays::Property(prefix + ".normalizebycolor")(false));
					lux_props.Set(luxrays::Property(prefix + ".gain.normalizebycolor")(false));
				}

				lux_props.Set(luxrays::Property(prefix + ".importance")(get_float_parameter_value(emission_params, "importance", eval_time)));
				lux_props.Set(luxrays::Property(prefix + ".theta")(spread_angle));
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

void add_inline_bump(luxcore::Scene *scene, std::string prefix, std::string suffix, XSI::CParameterRefArray &parameters, std::unordered_map<ULONG, std::string>& exported_nodes_map, const XSI::CTime &eval_time)
{
	luxrays::Properties bump_props;
	std::string bump_prefix = prefix + suffix;
	bump_props.Set(luxrays::Property(bump_prefix + ".type")("scale"));
	set_material_value(scene, bump_props, "value" + XSI::CString(suffix.c_str()), bump_prefix + ".texture1", parameters, exported_nodes_map, eval_time);
	set_material_value(scene, bump_props, "height" + XSI::CString(suffix.c_str()), bump_prefix + ".texture2", parameters, exported_nodes_map, eval_time);
	scene->Parse(bump_props);
}

std::string add_texture(luxcore::Scene* scene, XSI::Shader &texture_node, std::unordered_map<ULONG, std::string>& exported_nodes_map, const XSI::CTime &eval_time)
{
	//we will parse texture parameters in the scene at the end of the method
	ULONG node_id = texture_node.GetObjectID();
	if (exported_nodes_map.contains(node_id))
	{
		return exported_nodes_map[node_id];
	}

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
		else if (node_name == "sib_color_to_scalar" || node_name == "sib_color_to_vector")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("constfloat3"));
			XSI::MATH::CColor4f input = get_color_parameter_value(parameters, "input", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".value")(input.GetR(), input.GetG(), input.GetB()));
		}
		else if (node_name == "sib_vector_to_color")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("constfloat3"));
			XSI::ShaderParameter p(parameters.GetItem("input"));
			XSI::MATH::CVector3 input = get_vector_parameter_value(parameters, "input", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".value")(input.GetX(), input.GetY(), input.GetZ()));
		}
		else if (node_name == "txt2d-image-explicit")
		{
			XSI::Parameter tex_param = parameters.GetItem("tex");
			XSI::Parameter tex_param_finall = get_source_parameter(tex_param);
			XSI::Parameter repeats_param = parameters.GetItem("repeats");
			XSI::Parameter repeats_param_finall = get_source_parameter(repeats_param);
						
			XSI::CRef tex_source = tex_param_finall.GetSource();
			if (tex_source.IsValid() && repeats_param_finall.IsValid())
			{
				XSI::ImageClip2 clip(tex_source);
				XSI::CString file_path = clip.GetFileName();

				XSI::CParameterRefArray repeats_params = repeats_param_finall.GetParameters();
				float repeat_x = ((XSI::Parameter)repeats_params[0]).GetValue();
				float repeat_y = ((XSI::Parameter)repeats_params[1]).GetValue();
				float repeat_z = ((XSI::Parameter)repeats_params[2]).GetValue();

				//we will use only the first uv, because there is no way to define the uv index from tspace_id property value (this property contains the name, but not the index of the uv)
				//the same material can used for different object, which can contains one or several uvs
				texture_props.Set(luxrays::Property(prefix + ".type")("imagemap"));
				texture_props.Set(luxrays::Property(prefix + ".file")(file_path.GetAsciiString()));
				texture_props.Set(luxrays::Property(prefix + ".wrap")("repeat"));
				texture_props.Set(luxrays::Property(prefix + ".gamma")(2.2f));
				//also set the mapping
				texture_props.Set(luxrays::Property(prefix + ".mapping.type")("uvmapping2d"));
				texture_props.Set(luxrays::Property(prefix + ".mapping.uvindex")(0));
				texture_props.Set(luxrays::Property(prefix + ".mapping.uvscale")(repeat_x, repeat_y));
			}
			else
			{
				output_name = "";
			}
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
		else if (node_name == "TextureMath")
		{
			XSI::CString mode = get_string_parameter_value(parameters, "type", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".type")(mode.GetAsciiString()));

			if (mode == "abs")
			{
				set_material_value(scene, texture_props, "value_1", prefix + ".texture", parameters, exported_nodes_map, eval_time);
			}
			else if (mode == "clamp")
			{
				set_material_value(scene, texture_props, "value_1", prefix + ".texture", parameters, exported_nodes_map, eval_time);
				texture_props.Set(luxrays::Property(prefix + ".min")(get_float_parameter_value(parameters, "clamp_min", eval_time)));
				texture_props.Set(luxrays::Property(prefix + ".max")(get_float_parameter_value(parameters, "clamp_max", eval_time)));
			}
			else if (mode == "mix")
			{
				set_material_value(scene, texture_props, "value_1", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "value_2", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "fac", prefix + ".amount", parameters, exported_nodes_map, eval_time);
			}
			else if (mode == "power")
			{
				set_material_value(scene, texture_props, "value_1", prefix + ".base", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "value_2", prefix + ".exponent", parameters, exported_nodes_map, eval_time);
			}
			else if (mode == "rounding")
			{
				set_material_value(scene, texture_props, "value_1", prefix + ".texture", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "value_2", prefix + ".increment", parameters, exported_nodes_map, eval_time);
			}
			else if (mode == "modulo")
			{
				set_material_value(scene, texture_props, "value_1", prefix + ".texture", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "value_2", prefix + ".modulo", parameters, exported_nodes_map, eval_time);
			}
			else
			{
				set_material_value(scene, texture_props, "value_1", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "value_2", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
			}

			bool is_clamp = get_bool_parameter_value(parameters, "is_clamp", eval_time);
			if (is_clamp && mode != "clamp")
			{
				scene->Parse(texture_props);
				std::string clamp_prefix = "scene.textures." + output_name + "_clamp";
				luxrays::Properties clamp_props;
				clamp_props.Set(luxrays::Property(clamp_prefix + ".type")("clamp"));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".texture")(output_name));
				float clamp_min = get_float_parameter_value(parameters, "clamp_min", eval_time);
				float clamp_max = get_float_parameter_value(parameters, "clamp_max", eval_time);
				clamp_props.Set(luxrays::Property(clamp_prefix + ".min")(0.0f));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".max")(1.0f));

				//at the end we will finish this texture
				texture_props = clamp_props;
				//and return new name
				output_name = output_name + "_clamp";
			}
		}
		else if (node_name == "TextureColorMath")
		{
			XSI::CString mode = get_string_parameter_value(parameters, "type", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".type")(mode.GetAsciiString()));
			if (mode == "abs")
			{
				set_material_value(scene, texture_props, "color_1", prefix + ".texture", parameters, exported_nodes_map, eval_time);
			}
			else if (mode == "clamp")
			{
				set_material_value(scene, texture_props, "color_1", prefix + ".texture", parameters, exported_nodes_map, eval_time);
				texture_props.Set(luxrays::Property(prefix + ".min")(get_float_parameter_value(parameters, "clamp_min", eval_time)));
				texture_props.Set(luxrays::Property(prefix + ".max")(get_float_parameter_value(parameters, "clamp_max", eval_time)));
			}
			else
			{
				set_material_value(scene, texture_props, "color_1", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_2", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
				if (mode == "mix")
				{
					set_material_value(scene, texture_props, "fac", prefix + ".amount", parameters, exported_nodes_map, eval_time);
				}
			}
			bool is_clamp = get_bool_parameter_value(parameters, "is_clamp", eval_time);
			if (is_clamp && mode != "clamp")
			{
				scene->Parse(texture_props);
				std::string clamp_prefix = "scene.textures." + output_name + "_clamp";
				luxrays::Properties clamp_props;
				clamp_props.Set(luxrays::Property(clamp_prefix + ".type")("clamp"));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".texture")(output_name));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".min")(0.0f));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".max")(1.0f));

				//at the end we will finish this texture
				texture_props = clamp_props;
				//and return new name
				output_name = output_name + "_clamp";
			}
		}
		else if (node_name == "TextureVectorMath")
		{
			XSI::CString mode = get_string_parameter_value(parameters, "type", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".type")(mode.GetAsciiString()));
			if (mode == "abs")
			{
				set_material_value(scene, texture_props, "vector_1", prefix + ".texture", parameters, exported_nodes_map, eval_time);
			}
			else if (mode == "clamp")
			{
				set_material_value(scene, texture_props, "vector_1", prefix + ".texture", parameters, exported_nodes_map, eval_time);
				float clamp_min = get_float_parameter_value(parameters, "clamp_min", eval_time);
				float clamp_max = get_float_parameter_value(parameters, "clamp_max", eval_time);
				texture_props.Set(luxrays::Property(prefix + ".min")(clamp_min));
				texture_props.Set(luxrays::Property(prefix + ".max")(clamp_max));
			}
			else
			{
				set_material_value(scene, texture_props, "vector_1", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "vector_2", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
				if (mode == "mix")
				{
					set_material_value(scene, texture_props, "fac", prefix + ".amount", parameters, exported_nodes_map, eval_time);
				}
			}
		}
		else if (node_name == "TextureDotProduct")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("dotproduct"));
			set_material_value(scene, texture_props, "vector_1", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "vector_2", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureCheckerboard2D")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("checkerboard2d"));
			set_material_value(scene, texture_props, "color_1", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "color_2", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_2d", eval_time);
		}
		else if (node_name == "TextureCheckerboard3D")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("checkerboard3d"));
			set_material_value(scene, texture_props, "color_1", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "color_2", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time);
		}
		else if (node_name == "TriplanarMapping")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("triplanar"));
			bool multiple_textures = get_bool_parameter_value(parameters, "multiple_textures", eval_time);
			if (multiple_textures)
			{
				set_material_value(scene, texture_props, "color_x", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_y", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_z", prefix + ".texture3", parameters, exported_nodes_map, eval_time);
			}
			else
			{
				set_material_value(scene, texture_props, "color_x", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_x", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_x", prefix + ".texture3", parameters, exported_nodes_map, eval_time);
			}
			bool is_add = add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time);
			if (!is_add)
			{
				texture_props.Set(luxrays::Property(prefix + ".mapping.type")("localmapping3d"));
			}
		}
		else if (node_name == "TriplanarBumpMapping")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("triplanar"));
			bool multiple_textures = get_bool_parameter_value(parameters, "multiple_textures", eval_time);
			//create inline bump texture
			add_inline_bump(scene, prefix, "_x", parameters, exported_nodes_map, eval_time);
			texture_props.Set(luxrays::Property(prefix + ".texture1")(output_name + "_x"));
			if (multiple_textures)
			{
				add_inline_bump(scene, prefix, "_y", parameters, exported_nodes_map, eval_time);
				texture_props.Set(luxrays::Property(prefix + ".texture2")(output_name + "_y"));
				add_inline_bump(scene, prefix, "_z", parameters, exported_nodes_map, eval_time);
				texture_props.Set(luxrays::Property(prefix + ".texture3")(output_name + "_z"));
			}
			else
			{
				texture_props.Set(luxrays::Property(prefix + ".texture2")(output_name + "_x"));
				texture_props.Set(luxrays::Property(prefix + ".texture3")(output_name + "_x"));
			}
			bool is_add = add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time);
			if (!is_add)
			{
				texture_props.Set(luxrays::Property(prefix + ".mapping.type")("localmapping3d"));
			}
		}
		else if (node_name == "TriplanarNormalMapping")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("triplanar"));
			bool multiple_textures = get_bool_parameter_value(parameters, "multiple_textures", eval_time);
			if (multiple_textures)
			{
				set_material_value(scene, texture_props, "color_x", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_y", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_z", prefix + ".texture3", parameters, exported_nodes_map, eval_time);
			}
			else
			{
				set_material_value(scene, texture_props, "color_x", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_x", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, texture_props, "color_x", prefix + ".texture3", parameters, exported_nodes_map, eval_time);
			}

			bool is_add = add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time);
			if (!is_add)
			{
				texture_props.Set(luxrays::Property(prefix + ".mapping.type")("localmapping3d"));
			}

			//instert normalmap
			scene->Parse(texture_props);
			std::string normal_prefix = "scene.textures." + output_name + "_normalmap";
			luxrays::Properties normal_props;
			normal_props.Set(luxrays::Property(normal_prefix + ".type")("normalmap"));
			normal_props.Set(luxrays::Property(normal_prefix + ".texture")(output_name));
			normal_props.Set(luxrays::Property(normal_prefix + ".scale")(get_float_parameter_value(parameters, "height", eval_time)));
			texture_props = normal_props;
			//and return new name
			output_name = output_name + "_normalmap";
		}
		else if (node_name == "TextureBlackbody")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("blackbody"));
			texture_props.Set(luxrays::Property(prefix + ".temperature")(get_float_parameter_value(parameters, "temperature", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".normalize")(get_bool_parameter_value(parameters, "normalize", eval_time)));
		}
		else if (node_name == "TextureIrregularData")
		{
			//before create property, check that data is correct
			//each string field should contains the same number of float values, the number of values >= 2
			XSI::CString wavelengths = get_string_parameter_value(parameters, "wavelengths", eval_time);
			XSI::CString data = get_string_parameter_value(parameters, "data", eval_time);
			std::vector wavelengths_vector = string_to_array(wavelengths);
			std::vector data_vector = string_to_array(data);
			if (wavelengths_vector.size() > 1 && data_vector.size() > 1 && wavelengths_vector.size() == data_vector.size())
			{
				texture_props.Set(luxrays::Property(prefix + ".type")("irregulardata"));
				texture_props.Set(luxrays::Property(prefix + ".wavelengths")(wavelengths_vector));
				texture_props.Set(luxrays::Property(prefix + ".data")(data_vector));
			}
			else
			{
				output_name = "";
			}
		}
		else if (node_name == "TextureLampSpectrum")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("lampspectrum"));
			XSI::CString lamp_category = get_string_parameter_value(parameters, "lamp_category", eval_time);
			if (lamp_category == "Natural") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_natural", eval_time).GetAsciiString())); }
			if (lamp_category == "Incandescent") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_incandescent", eval_time).GetAsciiString())); }
			if (lamp_category == "Fluorescent") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_flourescent", eval_time).GetAsciiString())); }
			if (lamp_category == "High_Pressure_Mercury") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_high_pressure", eval_time).GetAsciiString())); }
			if (lamp_category == "Sodium_Discharge") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_sodium_discharge", eval_time).GetAsciiString())); }
			if (lamp_category == "Metal_Halide") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_metal_halide", eval_time).GetAsciiString())); }
			if (lamp_category == "Diode") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_diode", eval_time).GetAsciiString())); }
			if (lamp_category == "Spectral") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_spectral", eval_time).GetAsciiString())); }
			if (lamp_category == "Glow_Discharge") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_glow_discharge", eval_time).GetAsciiString())); }
			if (lamp_category == "Molecular") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_molecular", eval_time).GetAsciiString())); }
			if (lamp_category == "Fluorescence") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_fluorescence", eval_time).GetAsciiString())); }
			if (lamp_category == "Various") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_various", eval_time).GetAsciiString())); }
			if (lamp_category == "BlacklightUV") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_blacklight", eval_time).GetAsciiString())); }
			if (lamp_category == "Mercury_UV") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_mercury_uv", eval_time).GetAsciiString())); }
			if (lamp_category == "AbsorptionMixed") { texture_props.Set(luxrays::Property(prefix + ".name")(get_string_parameter_value(parameters, "lamp_absorption", eval_time).GetAsciiString())); }
		}
		else if (node_name == "TextureBump")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("scale"));
			set_material_value(scene, texture_props, "value", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "height", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureColorRamp")
		{
			XSI::Parameter gp = parameters.GetItem("gradient");
			XSI::Parameter gradient_param = get_source_parameter(gp);
			if (gradient_param.IsValid())
			{
				texture_props.Set(luxrays::Property(prefix + ".type")("band"));
				XSI::CParameterRefArray g_params = gradient_param.GetParameters();
				XSI::Parameter markers_parameter = g_params.GetItem("markers");
				int interpolation = get_int_parameter_value(g_params, "interpolation", eval_time);
				texture_props.Set(luxrays::Property(prefix + ".interpolation")(interpolation == 0 ? "linear" : "cubic"));
				XSI::CParameterRefArray m_params = markers_parameter.GetParameters();
				//std::map<float, XSI::MATH::CVector3f> positions;
				std::map<float, std::tuple<float, float, float>> positions;
				for (ULONG i = 0; i < m_params.GetCount(); i++)
				{
					XSI::ShaderParameter p = m_params[i];
					float pos = p.GetParameterValue("pos");
					float mid = p.GetParameterValue("mid");
					float r = p.GetParameterValue("red");
					float g = p.GetParameterValue("green");
					float b = p.GetParameterValue("blue");
					float a = p.GetParameterValue("alpha");
					if (pos > -1.0f)
					{
						positions.emplace(std::piecewise_construct, std::forward_as_tuple(pos),std::forward_as_tuple(r, g, b));
						
					}
				}
				set_material_value(scene, texture_props, "amount", prefix + ".amount", parameters, exported_nodes_map, eval_time);
				//next write positions to the texture
				ULONG index = 0;
				for (const auto& [key, value] : positions)
				{
					texture_props.Set(luxrays::Property(prefix + ".offset" + std::to_string(index))(key));
					texture_props.Set(luxrays::Property(prefix + ".value" + std::to_string(index))(std::get<0>(value), std::get<1>(value), std::get<2>(value)));
					index++;
				}

				positions.clear();
			}
		}
		else if (node_name == "TextureDistort")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("distort"));
			texture_props.Set(luxrays::Property(prefix + ".strength")(get_float_parameter_value(parameters, "strength", eval_time)));
			set_material_value(scene, texture_props, "texture", prefix + ".texture", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "offset", prefix + ".offset", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureHSV")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("hsv"));
			set_material_value(scene, texture_props, "texture", prefix + ".texture", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "hue", prefix + ".hue", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "saturation", prefix + ".saturation", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "value", prefix + ".value", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureBrightContrast")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("brightcontrast"));
			set_material_value(scene, texture_props, "texture", prefix + ".texture", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "brightness", prefix + ".brightness", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "contrast", prefix + ".contrast", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureInvert")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("subtract"));
			set_material_value(scene, texture_props, "maximum", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "value", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TexturePointNormal")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("shadingnormal"));
		}
		else if (node_name == "TexturePointPosition")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("position"));
		}
		else if (node_name == "TexturePointness")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("hitpointalpha"));
			XSI::CString mode = get_string_parameter_value(parameters, "mode", eval_time);

			if (mode == "both")
			{
				scene->Parse(texture_props);
				std::string abs_prefix = "scene.textures." + output_name + "_abs";
				luxrays::Properties abs_props;
				abs_props.Set(luxrays::Property(abs_prefix + ".type")("abs"));
				abs_props.Set(luxrays::Property(abs_prefix + ".texture")(output_name));
				texture_props = abs_props;
				output_name = output_name + "_abs";
			}
			else if (mode == "concave")
			{
				scene->Parse(texture_props);
				std::string clamp_prefix = "scene.textures." + output_name + "_clamp";
				luxrays::Properties clamp_props;
				clamp_props.Set(luxrays::Property(clamp_prefix + ".type")("clamp"));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".texture")(output_name));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".min")(0.0f));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".max")(1.0f));
				texture_props = clamp_props;
				output_name = output_name + "_clamp";
			}
			else if (mode == "convex")
			{
				scene->Parse(texture_props);
				std::string flip_prefix = "scene.textures." + output_name + "_flip";
				luxrays::Properties flip_props;
				flip_props.Set(luxrays::Property(flip_prefix + ".type")("scale"));
				flip_props.Set(luxrays::Property(flip_prefix + ".texture1")(output_name));
				flip_props.Set(luxrays::Property(flip_prefix + ".texture2")(-1.0f));
				texture_props = flip_props;
				output_name = output_name + "_flip";

				scene->Parse(texture_props);
				std::string clamp_prefix = "scene.textures." + output_name + "_clamp";
				luxrays::Properties clamp_props;
				clamp_props.Set(luxrays::Property(clamp_prefix + ".type")("clamp"));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".texture")(output_name));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".min")(0.0f));
				clamp_props.Set(luxrays::Property(clamp_prefix + ".max")(1.0f));
				texture_props = clamp_props;
				output_name = output_name + "_clamp";
			}
			
			scene->Parse(texture_props);
			std::string multiplier_prefix = "scene.textures." + output_name + "_multiplier";
			luxrays::Properties multiplier_props;
			multiplier_props.Set(luxrays::Property(multiplier_prefix + ".type")("scale"));
			multiplier_props.Set(luxrays::Property(multiplier_prefix + ".texture1")(output_name));
			set_material_value(scene, multiplier_props, "multiplier", multiplier_prefix + ".texture2", parameters, exported_nodes_map, eval_time);
			texture_props = multiplier_props;
			output_name = output_name + "_multiplier";
		}
		else if (node_name == "TextureObjectID")
		{
			XSI::CString mode = get_string_parameter_value(parameters, "mode", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".type")(mode.GetAsciiString()));
		}
		else if (node_name == "TextureRandomPerIsland")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("hitpointtriangleaov"));
			texture_props.Set(luxrays::Property(prefix + ".dataindex")(1));
		}
		else if (node_name == "TextureTimeInfo")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("constfloat1"));
			texture_props.Set(luxrays::Property(prefix + ".value")(eval_time.GetTime()));
		}
		else if (node_name == "TextureUV")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("uv"));
			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_2d", eval_time);
		}
		else if (node_name == "TextureRandom")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("random"));
			texture_props.Set(luxrays::Property(prefix + ".seed")(get_int_parameter_value(parameters, "seed", eval_time)));
			set_material_value(scene, texture_props, "value", prefix + ".texture", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureBombing")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("bombing"));
			float random_scale = get_float_parameter_value(parameters, "random_scale", eval_time);
			bool use_random_rotation = get_bool_parameter_value(parameters, "use_random_rotation", eval_time);

			texture_props.Set(luxrays::Property(prefix + ".bullet.randomscale.range")(random_scale * 5.0f));
			texture_props.Set(luxrays::Property(prefix + ".bullet.randomrotation.enable")(use_random_rotation));
			set_material_value(scene, texture_props, "background", prefix + ".background", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "bullet", prefix + ".bullet", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "mask", prefix + ".bullet.mask", parameters, exported_nodes_map, eval_time);

			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_2d", eval_time);
		}
		else if (node_name == "TextureBrick")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("brick"));
			texture_props.Set(luxrays::Property(prefix + ".brickbond")(replace_symbols(get_string_parameter_value(parameters, "type", eval_time), "_", " ").GetAsciiString()));
			texture_props.Set(luxrays::Property(prefix + ".brickmodbias")(get_float_parameter_value(parameters, "brickmodbias", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".brickwidth")(get_float_parameter_value(parameters, "brickwidth", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".brickheight")(get_float_parameter_value(parameters, "brickheight", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".brickdepth")(get_float_parameter_value(parameters, "brickdepth", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".mortarsize")(get_float_parameter_value(parameters, "mortarsize", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".brickrun")(get_float_parameter_value(parameters, "brickrun", eval_time) / 100.0f));
			set_material_value(scene, texture_props, "color_1", prefix + ".bricktex", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "color_2", prefix + ".brickmodtex", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "mortar", prefix + ".mortartex", parameters, exported_nodes_map, eval_time);
			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time);
		}
		else if (node_name == "TextureWireframe")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("wireframe"));
			texture_props.Set(luxrays::Property(prefix + ".width")(get_float_parameter_value(parameters, "width", eval_time)));
			set_material_value(scene, texture_props, "border", prefix + ".border", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "inside", prefix + ".inside", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureDots")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("dots"));
			set_material_value(scene, texture_props, "inside", prefix + ".inside", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "outside", prefix + ".outside", parameters, exported_nodes_map, eval_time);
			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_2d", eval_time);
		}
		else if (node_name == "TextureFBM")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("fbm"));
			texture_props.Set(luxrays::Property(prefix + ".octaves")(get_int_parameter_value(parameters, "octaves", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".roughness")(get_float_parameter_value(parameters, "roughness", eval_time)));
			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time);
		}
		else if (node_name == "TextureMarble")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("marble"));
			texture_props.Set(luxrays::Property(prefix + ".octaves")(get_int_parameter_value(parameters, "octaves", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".roughness")(get_float_parameter_value(parameters, "roughness", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".scale")(get_float_parameter_value(parameters, "scale", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".variation")(get_float_parameter_value(parameters, "variation", eval_time)));
			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time);
		}
		else if (node_name == "TextureWrinkled")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("wrinkled"));
			texture_props.Set(luxrays::Property(prefix + ".octaves")(get_int_parameter_value(parameters, "octaves", eval_time)));
			texture_props.Set(luxrays::Property(prefix + ".roughness")(get_float_parameter_value(parameters, "roughness", eval_time)));
			add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time);
		}
		else if (node_name == "TextureVertexColor")
		{
			XSI::CString mode = get_string_parameter_value(parameters, "mode", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".type")(mode.GetAsciiString()));
			if (mode == "hitpointgrey")
			{
				XSI::CString channel = get_string_parameter_value(parameters, "channel", eval_time);
				texture_props.Set(luxrays::Property(prefix + ".channel")(channel.GetAsciiString()));
			}
			texture_props.Set(luxrays::Property(prefix + ".dataindex")(get_int_parameter_value(parameters, "index", eval_time)));
		}
		else if (node_name == "TextureVertexAlpha")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("hitpointalpha"));
			texture_props.Set(luxrays::Property(prefix + ".dataindex")(get_int_parameter_value(parameters, "index", eval_time)));
		}
		else if (node_name == "TextureFresnel")
		{
			XSI::CString type = get_string_parameter_value(parameters, "type", eval_time);
			if (type == "color")
			{
				texture_props.Set(luxrays::Property(prefix + ".type")("fresnelcolor"));
				set_material_value(scene, texture_props, "color", prefix + ".kr", parameters, exported_nodes_map, eval_time);
			}
			else if (type == "preset")
			{
				texture_props.Set(luxrays::Property(prefix + ".type")("fresnelpreset"));
				XSI::CString preset = replace_symbols(get_string_parameter_value(parameters, "preset", eval_time), "_", " ");
				texture_props.Set(luxrays::Property(prefix + ".name")(preset.GetAsciiString()));
			}
			else if (type == "nk")
			{
				XSI::CString xsi_file = resolve_path(get_string_parameter_value(parameters, "file", eval_time));
				if (xsi_file.Length() > 0)
				{
					XSI::CString file_type = get_string_parameter_value(parameters, "file_type", eval_time);
					if (file_type == "luxpop")
					{
						texture_props.Set(luxrays::Property(prefix + ".type")("fresnelluxpop"));
					}
					else
					{
						texture_props.Set(luxrays::Property(prefix + ".type")("fresnelsopra"));
					}
					texture_props.Set(luxrays::Property(prefix + ".file")(xsi_file.GetAsciiString()));
				}
				else
				{
					texture_props.Set(luxrays::Property(prefix + ".type")("fresnelcolor"));
					texture_props.Set(luxrays::Property(prefix + ".kr")(0.0f, 0.0f, 0.0f));
				}
			}
			else if (type == "custom_nk")
			{
				texture_props.Set(luxrays::Property(prefix + ".type")("fresnelconst"));
				XSI::MATH::CColor4f color_n = get_color_parameter_value(parameters, "color_n", eval_time);
				XSI::MATH::CColor4f color_k = get_color_parameter_value(parameters, "color_k", eval_time);
				std::vector<float> array_n{color_n.GetR(), color_n.GetG(), color_n.GetB()};
				std::vector<float> array_k{ color_k.GetR(), color_k.GetG(), color_k.GetB() };
				texture_props.Set(luxrays::Property(prefix + ".n")(array_n));
				texture_props.Set(luxrays::Property(prefix + ".k")(array_k));
			}
		}
		else if (node_name == "TextureSplitRGB")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("splitfloat3"));
			set_material_value(scene, texture_props, "color", prefix + ".texture", parameters, exported_nodes_map, eval_time);
			XSI::CString channel = get_string_parameter_value(parameters, "channel", eval_time);
			texture_props.Set(luxrays::Property(prefix + ".channel")(channel == "r" ? 0 : (channel == "g" ? 1 : 2)));
		}
		else if (node_name == "TextureCombineRGB")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("makefloat3"));
			set_material_value(scene, texture_props, "r", prefix + ".texture1", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "g", prefix + ".texture2", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "b", prefix + ".texture3", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureRemap")
		{
			texture_props.Set(luxrays::Property(prefix + ".type")("remap"));
			set_material_value(scene, texture_props, "value", prefix + ".value", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "sourcemin", prefix + ".sourcemin", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "sourcemax", prefix + ".sourcemax", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "targetmin", prefix + ".targetmin", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, texture_props, "targetmax", prefix + ".targetmax", parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "TextureOpenVDB")
		{
			XSI::CString xsi_file = resolve_path(get_string_parameter_value(parameters, "file", eval_time));
			XSI::CString grid = get_string_parameter_value(parameters, "grid", eval_time);
			int nx = get_int_parameter_value(parameters, "nx", eval_time);
			int ny = get_int_parameter_value(parameters, "ny", eval_time);
			int nz = get_int_parameter_value(parameters, "nz", eval_time);
			//here we does not check correctness of the grid name
			if (xsi_file.Length() > 0 && grid.Length() > 0 && nx > 0 && ny > 0 && nz > 0)
			{
				texture_props.Set(luxrays::Property(prefix + ".type")("densitygrid"));
				texture_props.Set(luxrays::Property(prefix + ".wrap")("black"));
				texture_props.Set(luxrays::Property(prefix + ".storage")(get_string_parameter_value(parameters, "precision", eval_time).GetAsciiString()));
				texture_props.Set(luxrays::Property(prefix + ".nx")(nx));
				texture_props.Set(luxrays::Property(prefix + ".ny")(ny));
				texture_props.Set(luxrays::Property(prefix + ".nz")(nz));
				texture_props.Set(luxrays::Property(prefix + ".openvdb.file")(xsi_file.GetAsciiString()));
				texture_props.Set(luxrays::Property(prefix + ".openvdb.grid")(grid.GetAsciiString()));

				add_mapping(texture_props, prefix + ".mapping", parameters, "mapping_3d", eval_time, true);
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
		exported_nodes_map[node_id] = output_name;
	}

	return output_name;
}

void set_material_value(luxcore::Scene *scene, 
	luxrays::Properties& material_props, 
	const XSI::CString &xsi_param_name, 
	const std::string &lux_param_name, 
	XSI::CParameterRefArray &parameters, 
	std::unordered_map<ULONG, std::string>& exported_nodes_map,
	const XSI::CTime &eval_time,
	bool ignore_set_branch)
{
	XSI::Parameter xsi_param = parameters.GetItem(xsi_param_name);
	XSI::Parameter xsi_finall_parameter = get_source_parameter(xsi_param);
	XSI::ShaderParameter shader_finall_parameter(xsi_finall_parameter);
	ShaderParameterType parameter_type = get_shader_parameter_type(shader_finall_parameter);

	//check is this parameter has connections or not
	XSI::CRef param_source = xsi_finall_parameter.GetSource();
	bool has_connection = param_source.IsValid();
	if (has_connection)
	{
		//get source node
		XSI::ShaderParameter shader_parameter(xsi_finall_parameter);
		XSI::Parameter source_param(param_source);
		XSI::Shader node = get_input_node(shader_parameter, true);  // we find this node by goes through default converter nodes, so, it can be this converter
		//if there is connection, try to export corresponding texture or material
		if (parameter_type == ShaderParameterType::ParameterType_Color4)
		{
			//parameter connected to material port
			//so, try to add new material
			luxrays::Properties new_material_props;
			std::string material_name = add_material(scene, new_material_props, node, exported_nodes_map, eval_time);
			if (material_name.size() > 0)
			{
				scene->Parse(new_material_props);

				material_props.Set(luxrays::Property(lux_param_name)(material_name));
			}
			else
			{
				has_connection = false;
			}
		}
		else if (parameter_type == ShaderParameterType::ParameterType_Float || 
				 parameter_type == ShaderParameterType::ParameterType_Color3 ||
				 parameter_type == ShaderParameterType::ParameterType_Vector3)
		{
			//float (or color3, vector3) parameter is connected to some node
			//try to recognize the texture of this node
			std::string texture_name = add_texture(scene, node, exported_nodes_map, eval_time);
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
		else if (parameter_type == ShaderParameterType::ParameterType_Color3 || parameter_type == ShaderParameterType::ParameterType_Vector3)
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
std::string add_material(luxcore::Scene* scene, luxrays::Properties &material_props, XSI::Shader &material_node, std::unordered_map<ULONG, std::string>& exported_nodes_map, const XSI::CTime& eval_time, std::string override_name)
{
	//we does not parse property in the scene in this method
	ULONG node_id = material_node.GetObjectID();
	if (exported_nodes_map.contains(node_id))
	{
		return exported_nodes_map[node_id];
	}

	//get shader name
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
			set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "sigma", prefix + ".sigma", parameters, exported_nodes_map, eval_time);
			setup_default = true;
		}
		else if (node_name == "ShaderMirror")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("mirror"));
			set_material_value(scene, material_props, "kr", prefix + ".kr", parameters, exported_nodes_map, eval_time);
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
			set_material_value(scene, material_props, "kr", prefix + ".kr", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "kt", prefix + ".kt", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "exteriorior", prefix + ".exteriorior", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "interiorior", prefix + ".interiorior", parameters, exported_nodes_map, eval_time);
			if (glass_mode == "default")
			{
				set_material_value(scene, material_props, "cauchyb", prefix + ".cauchyb", parameters, exported_nodes_map, eval_time);
			}
			if (is_film)
			{
				set_material_value(scene, material_props, "filmthickness", prefix + ".filmthickness", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "filmior", prefix + ".filmior", parameters, exported_nodes_map, eval_time);
			}
			if (glass_mode == "rough")
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".uroughness", parameters, exported_nodes_map, eval_time);
				if (is_anisotropic)
				{
					set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
				}
				else
				{
					set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
				}
			}
			setup_default = true;
		}
		else if (node_name == "ShaderMix")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("mix"));
			set_material_value(scene, material_props, "material1", prefix + ".material1", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "material2", prefix + ".material2", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "amount", prefix + ".amount", parameters, exported_nodes_map, eval_time);
			setup_default = true;
		}
		else if (node_name == "ShaderNull")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("null"));
		}
		else if (node_name == "ShaderMatteTranslucent")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("roughmattetranslucent"));
			set_material_value(scene, material_props, "kr", prefix + ".kr", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "kt", prefix + ".kt", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "sigma", prefix + ".sigma", parameters, exported_nodes_map, eval_time);
			setup_default = true;
		}
		else if (node_name == "ShaderGlossy")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("glossy2"));
			bool is_anisotropic = get_bool_parameter_value(parameters, "is_anisotropic", eval_time);
			bool multibounce = get_bool_parameter_value(parameters, "multibounce", eval_time);
			material_props.Set(luxrays::Property(prefix + ".multibounce")(multibounce));
			set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "ks", prefix + ".ks", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "ka", prefix + ".ka", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "d", prefix + ".d", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "index", prefix + ".index", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "uroughness", prefix + ".uroughness", parameters, exported_nodes_map, eval_time);
			if (is_anisotropic)
			{
				set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
			}
			else
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
			}

			setup_default = true;
		}
		else if (node_name == "ShaderMetal")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("metal2"));
			bool is_anisotropic = get_bool_parameter_value(parameters, "is_anisotropic", eval_time);
			XSI::CString metal_mode = get_string_parameter_value(parameters, "metal_mode", eval_time);
			set_material_value(scene, material_props, "uroughness", prefix + ".uroughness", parameters, exported_nodes_map, eval_time);
			if (is_anisotropic)
			{
				set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
			}
			else
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
			}

			//get connection of the fresnel port
			XSI::Parameter fp = parameters.GetItem("fresnel");
			XSI::Parameter fp_finell = get_source_parameter(fp);
			XSI::ShaderParameter fresnel_parameter(fp);
			XSI::Shader fresnel_node = get_input_node(fresnel_parameter);
			
			bool is_fresnel = false;
			if (fresnel_node.IsValid())
			{
				XSI::CString fresnel_node_prog_id = fresnel_node.GetProgID();
				if (fresnel_node_prog_id == "LUXShadersPlugin.TextureFresnel.1.0")
				{
					std::string fresnel_name = add_texture(scene, fresnel_node, exported_nodes_map, eval_time);
					if (fresnel_name.size() > 0)
					{
						material_props.Set(luxrays::Property(prefix + ".fresnel")(fresnel_name));
						is_fresnel = true;
					}
					else
					{
						is_fresnel = false;
					}
				}
			}

			if (!is_fresnel)
			{
				//create inline fresnel
				luxrays::Properties fresnel_props;
				std::string frensel_name = output_name + "_fresnel";
				fresnel_props.Set(luxrays::Property("scene.textures." + frensel_name + ".type")("fresnelcolor"));
				fresnel_props.Set(luxrays::Property("scene.textures." + frensel_name + ".kr")(1.0f, 1.0f, 1.0f));
				scene->Parse(fresnel_props);

				//connect generated fresnel to the material
				material_props.Set(luxrays::Property(prefix + ".fresnel")(frensel_name));
			}

			setup_default = true;
		}
		else if (node_name == "ShaderVelvet")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("velvet"));
			set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "thickness", prefix + ".thickness", parameters, exported_nodes_map, eval_time);
			bool advanced = get_bool_parameter_value(parameters, "advanced", eval_time);
			if (advanced)
			{
				set_material_value(scene, material_props, "p1", prefix + ".p1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "p2", prefix + ".p2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "p3", prefix + ".p3", parameters, exported_nodes_map, eval_time);
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

			set_material_value(scene, material_props, "weft_kd", prefix + ".weft_kd", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "weft_ks", prefix + ".weft_ks", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "warp_kd", prefix + ".warp_kd", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "warp_ks", prefix + ".warp_ks", parameters, exported_nodes_map, eval_time);

			setup_default = true;
		}
		else if (node_name == "ShaderCarpaint")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("carpaint"));
			XSI::CString preset = get_string_parameter_value(parameters, "preset", eval_time);
			if (preset == "manual")
			{
				set_material_value(scene, material_props, "ka", prefix + ".ka", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "d", prefix + ".d", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "ks1", prefix + ".ks1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "ks2", prefix + ".ks2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "ks3", prefix + ".ks3", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "r1", prefix + ".r1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "r2", prefix + ".r2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "r3", prefix + ".r3", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "m1", prefix + ".m1", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "m2", prefix + ".m2", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "m3", prefix + ".m3", parameters, exported_nodes_map, eval_time);
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
			set_material_value(scene, material_props, "kd", prefix + ".kd", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "kt", prefix + ".kt", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "ks", prefix + ".ks", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, is_double ? "ks_bf" : "ks", prefix + ".ks_bf", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "uroughness", prefix + ".uroughness", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, is_double ? "uroughness_bf" : "uroughness", prefix + ".uroughness_bf", parameters, exported_nodes_map, eval_time);
			if (is_anisotropic)
			{
				set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, is_double ? "vroughness_bf" : "vroughness", prefix + ".vroughness_bf", parameters, exported_nodes_map, eval_time);
			}
			else
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, is_double ? "uroughness_bf" : "uroughness", prefix + ".vroughness_bf", parameters, exported_nodes_map, eval_time);
			}
			set_material_value(scene, material_props, "ka", prefix + ".ka", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, is_double ? "ka_bf" : "ka", prefix + ".ka_bf", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "d", prefix + ".d", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, is_double ? "d_bf" : "d", prefix + ".d_bf", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "index", prefix + ".index", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, is_double ? "index_bf" : "index", prefix + ".index_bf", parameters, exported_nodes_map, eval_time);
			material_props.Set(luxrays::Property(prefix + ".multibounce")(multibounce));
			material_props.Set(luxrays::Property(prefix + ".multibounce_bf")(multibounce));

			setup_default = true;
		}
		else if (node_name == "ShaderGlossyCoating")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("glossycoating"));
			bool multibounce = get_bool_parameter_value(parameters, "multibounce", eval_time);
			bool is_anisotropic = get_bool_parameter_value(parameters, "is_anisotropic", eval_time);
			set_material_value(scene, material_props, "base", prefix + ".base", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "ks", prefix + ".ks", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "ka", prefix + ".ka", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "d", prefix + ".d", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "uroughness", prefix + ".uroughness", parameters, exported_nodes_map, eval_time);
			if (is_anisotropic)
			{
				set_material_value(scene, material_props, "vroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
			}
			else
			{
				set_material_value(scene, material_props, "uroughness", prefix + ".vroughness", parameters, exported_nodes_map, eval_time);
			}
			set_material_value(scene, material_props, "index", prefix + ".index", parameters, exported_nodes_map, eval_time);
			material_props.Set(luxrays::Property(prefix + ".multibounce")(multibounce));

			setup_default = true;
		}
		else if (node_name == "ShaderDisney")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("disney"));
			bool is_film = get_bool_parameter_value(parameters, "is_film", eval_time);
			set_material_value(scene, material_props, "basecolor", prefix + ".basecolor", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "subsurface", prefix + ".subsurface", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "roughness", prefix + ".roughness", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "metallic", prefix + ".metallic", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "specular", prefix + ".specular", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "speculartint", prefix + ".speculartint", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "clearcoat", prefix + ".clearcoat", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "clearcoatgloss", prefix + ".clearcoatgloss", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "anisotropic", prefix + ".anisotropic", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "sheen", prefix + ".sheen", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "sheentint", prefix + ".sheentint", parameters, exported_nodes_map, eval_time);
			if (is_film)
			{
				set_material_value(scene, material_props, "filmamount", prefix + ".filmamount", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "filmthickness", prefix + ".filmthickness", parameters, exported_nodes_map, eval_time);
				set_material_value(scene, material_props, "filmior", prefix + ".filmior", parameters, exported_nodes_map, eval_time);
			}

			setup_default = true;
		}
		else if (node_name == "ShaderTwoSided")
		{
			material_props.Set(luxrays::Property(prefix + ".type")("twosided"));
			set_material_value(scene, material_props, "frontmaterial", prefix + ".frontmaterial", parameters, exported_nodes_map, eval_time);
			set_material_value(scene, material_props, "backmaterial", prefix + ".backmaterial", parameters, exported_nodes_map, eval_time);

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
			set_material_value(scene, material_props, "transparency", prefix + ".transparency", parameters, exported_nodes_map, eval_time);
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
			add_emission(scene, parameters, material_props, prefix + ".emission", exported_nodes_map, eval_time);
			//next bump
			set_material_value(scene, material_props, "bump", prefix + ".bumptex", parameters, exported_nodes_map, eval_time, true);
			//and normal
			//set_material_value(scene, material_props, "normal", prefix + ".normaltex", parameters, eval_time, true);
		}
	}
	else
	{
		output_name = "";
	}
	if (output_name.size() > 0)
	{
		exported_nodes_map[node_id] = output_name;
	}

	return output_name;
}

void setup_default_volume(luxcore::Scene* scene, luxrays::Properties &volume_props, std::string &prefix, std::string &volume_name, XSI::CParameterRefArray &parameters, std::unordered_map<ULONG, std::string>& exported_nodes_map, const XSI::CTime& eval_time)
{
	set_material_value(scene, volume_props, "ior", prefix + ".ior", parameters, exported_nodes_map, eval_time);
	int priority = get_int_parameter_value(parameters, "priority", eval_time);
	volume_props.Set(luxrays::Property(prefix + ".priority")(priority));
	set_material_value(scene, volume_props, "emission", prefix + ".emission", parameters, exported_nodes_map, eval_time);
	int emission_id = get_int_parameter_value(parameters, "emission_id", eval_time);
	volume_props.Set(luxrays::Property(prefix + ".emission.id")(emission_id));
	float abs_depth = get_float_parameter_value(parameters, "absorption_depth", eval_time);

	//create implicit texture for absorption color and depth
	std::string abs_name = volume_name + "_colordepth";
	luxrays::Properties abs_props;
	std::string abs_prefix = "scene.textures." + abs_name;
	abs_props.Set(luxrays::Property(abs_prefix + ".type")("colordepth"));
	set_material_value(scene, abs_props, "absorption", abs_prefix + ".kt", parameters, exported_nodes_map, eval_time);
	abs_props.Set(luxrays::Property(abs_prefix + ".depth")(abs_depth));
	scene->Parse(abs_props);

	//set name of the created texture to the volume node
	volume_props.Set(luxrays::Property(prefix + ".absorption")(abs_name));
}

void setup_scattering_volume(luxcore::Scene* scene, luxrays::Properties& volume_props, std::string& prefix, std::string& volume_name, XSI::CParameterRefArray& parameters, std::unordered_map<ULONG, std::string>& exported_nodes_map, const XSI::CTime& eval_time)
{
	std::string scat_name = volume_name + "_scale";
	luxrays::Properties scat_props;
	std::string scat_prefix = "scene.textures." + scat_name;

	scat_props.Set(luxrays::Property(scat_prefix + ".type")("scale"));
	set_material_value(scene, scat_props, "scattering_scale", scat_prefix + ".texture1", parameters, exported_nodes_map, eval_time);
	set_material_value(scene, scat_props, "scattering", scat_prefix + ".texture2", parameters, exported_nodes_map, eval_time);
	scene->Parse(scat_props);

	volume_props.Set(luxrays::Property(prefix + ".scattering")(scat_name));
}

std::string add_volume_node(luxcore::Scene* scene, XSI::Shader &node, std::unordered_map<ULONG, std::string> &exported_nodes_map, const XSI::CTime& eval_time)
{
	ULONG node_id = node.GetObjectID();
	if (exported_nodes_map.contains(node_id))
	{
		return exported_nodes_map[node_id];
	}

	XSI::CString prog_id = node.GetProgID();
	std::string output_name = xsi_object_id_string(node)[0];
	XSI::CStringArray parts = prog_id.Split(".");
	std::string prefix = "scene.volumes." + output_name;
	luxrays::Properties volume_props;
	if (parts.GetCount() > 1 && parts[0] == "LUXShadersPlugin")
	{
		XSI::CString node_name = parts[1];
		XSI::CParameterRefArray parameters = node.GetParameters();
		if (node_name == "VolumeClear")
		{
			volume_props.Set(luxrays::Property(prefix + ".type")("clear"));
			setup_default_volume(scene, volume_props, prefix, output_name, parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "VolumeHeterogeneous")
		{
			volume_props.Set(luxrays::Property(prefix + ".type")("heterogeneous"));
			set_material_value(scene, volume_props, "asymmetry", prefix + ".asymmetry", parameters, exported_nodes_map, eval_time);
			bool multiscattering = get_bool_parameter_value(parameters, "multiscattering", eval_time);
			volume_props.Set(luxrays::Property(prefix + ".multiscattering")(multiscattering));

			float step_size = get_float_parameter_value(parameters, "step_size", eval_time);
			int max_steps = get_int_parameter_value(parameters, "max_steps", eval_time);
			volume_props.Set(luxrays::Property(prefix + ".steps.size")(step_size));
			volume_props.Set(luxrays::Property(prefix + ".steps.maxcount")(max_steps));

			setup_default_volume(scene, volume_props, prefix, output_name, parameters, exported_nodes_map, eval_time);
			setup_scattering_volume(scene, volume_props, prefix, output_name, parameters, exported_nodes_map, eval_time);
		}
		else if (node_name == "VolumeHomogeneous")
		{
			volume_props.Set(luxrays::Property(prefix + ".type")("homogeneous"));
			set_material_value(scene, volume_props, "asymmetry", prefix + ".asymmetry", parameters, exported_nodes_map, eval_time);
			bool multiscattering = get_bool_parameter_value(parameters, "multiscattering", eval_time);
			volume_props.Set(luxrays::Property(prefix + ".multiscattering")(multiscattering));

			setup_default_volume(scene, volume_props, prefix, output_name, parameters, exported_nodes_map, eval_time);
			setup_scattering_volume(scene, volume_props, prefix, output_name, parameters, exported_nodes_map, eval_time);
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
		scene->Parse(volume_props);
		exported_nodes_map[node_id] = output_name;
	}
	return output_name;
}

void add_volume(luxcore::Scene* scene, luxrays::Properties &material_props, const std::string &material_name, XSI::Material& xsi_material, std::unordered_map<ULONG, std::string>& exported_nodes_map, const XSI::CTime &eval_time)
{
	XSI::CRefArray first_level_shaders = xsi_material.GetShaders();
	std::vector<XSI::ShaderParameter> volume_ports = get_root_shader_parameter(first_level_shaders, GRSPM_ParameterName, "volume");
	std::string volume_name = "";
	if (volume_ports.size() > 0)
	{
		XSI::Shader volume_node = get_input_node(volume_ports[0]);
		volume_name = add_volume_node(scene, volume_node, exported_nodes_map, eval_time);
	}

	if (volume_name.size() > 0)
	{
		material_props.Set(luxrays::Property("scene.materials." + material_name + ".volume.interior")(volume_name));
	}
}

void sync_material(luxcore::Scene* scene, XSI::Material &xsi_material, std::set<ULONG>& xsi_materials_in_lux, const XSI::CTime& eval_time)
{
	//we should set the name of material equal to UniqueID of the object
	//next we should export some basic material
	//and assign it to the polygon meshes

	std::string material_name = xsi_object_id_string(xsi_material)[0];
	std::unordered_map<ULONG, std::string> exported_nodes_map;  // key - id of the rendertree node, value - name in the textures or materials list

	XSI::CRefArray first_level_shaders = xsi_material.GetShaders();
	std::vector<XSI::ShaderParameter> surface_ports = get_root_shader_parameter(first_level_shaders, GRSPM_ParameterName, "surface");
	if (surface_ports.size() == 0)
	{//no connections to the surface port
		//set material to null
		luxrays::Properties material_props;
		material_props.Set(luxrays::Property("scene.materials." + material_name + ".type")("null"));
		//set default id
		material_props.Set(luxrays::Property("scene.materials." + material_name + ".id")((int)xsi_material.GetObjectID()));

		//next try to add the volume
		add_volume(scene, material_props, material_name, xsi_material, exported_nodes_map, eval_time);

		scene->Parse(material_props);
	}
	else
	{
		//get the material node
		XSI::Shader material_node = get_input_node(surface_ports[0]);
		//next we should add material from the selected node
		luxrays::Properties material_props;
		std::string add_name = add_material(scene, material_props, material_node, exported_nodes_map, eval_time, material_name);
		if (add_name.size() == 0)
		{
			//this is unknown material node
			//in this case set default material
			material_props.Clear();
			material_props.Set(luxrays::Property("scene.materials." + material_name + ".type")("matte"));
			material_props.Set(luxrays::Property("scene.materials." + material_name + ".kd")(0.8, 0.8, 0.8));
			material_props.Set(luxrays::Property("scene.materials." + material_name + ".id")((int)xsi_material.GetObjectID()));
		}
		add_volume(scene, material_props, material_name, xsi_material, exported_nodes_map, eval_time);
		scene->Parse(material_props);
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

std::string iterate_shape(luxcore::Scene* scene, std::string& input_shape_name, XSI::CParameterRefArray &parameters, std::unordered_map<ULONG, std::string>& exported_nodes_map, const bool ignore_subdivision, const XSI::CTime& eval_time)
{
	//get shape input of the node
	XSI::ShaderParameter shape_parameter(parameters.GetItem("in_shape"));
	if (shape_parameter.IsValid())
	{
		//get source node of the parameter
		XSI::Shader source_node = get_input_node(shape_parameter);
		if (source_node.IsValid())
		{
			return add_shape(scene, input_shape_name, source_node, exported_nodes_map, ignore_subdivision, eval_time);
		}
		else
		{
			//no connection
			return input_shape_name;
		}
	}
	else
	{
		return input_shape_name;
	}
}

std::string add_shape(luxcore::Scene* scene, std::string& input_shape_name, XSI::Shader &node, std::unordered_map<ULONG, std::string>& exported_nodes_map, const bool ignore_subdivision, const XSI::CTime& eval_time)
{
	XSI::CString prog_id = node.GetProgID();
	XSI::CParameterRefArray parameters = node.GetParameters();
	luxrays::Properties shape_props;
	if (prog_id == "LUXShadersPlugin.ShapeSubdivision.1.0")
	{
		if (ignore_subdivision)
		{
			log_message("Subdivision shape is not supported for the mesh with vertex color. Skip subdividing.", XSI::siWarningMsg);
			return input_shape_name;
		}

		std::string subshape_name = iterate_shape(scene, input_shape_name, parameters, exported_nodes_map, ignore_subdivision, eval_time);
		std::string shape_name = subshape_name + "_subdivision";
		std::string prefix = "scene.shapes." + shape_name;
		shape_props.Set(luxrays::Property(prefix + ".type")("subdiv"));
		shape_props.Set(luxrays::Property(prefix + ".source")(subshape_name));

		int maxlevel = get_int_parameter_value(parameters, "maxlevel", eval_time);
		float maxedgescreensize = get_int_parameter_value(parameters, "maxedgescreensize", eval_time);
		shape_props.Set(luxrays::Property(prefix + ".maxlevel")(maxlevel));
		shape_props.Set(luxrays::Property(prefix + ".maxedgescreensize")(maxedgescreensize));

		scene->Parse(shape_props);
		return shape_name;
	}
	else if (prog_id == "LUXShadersPlugin.ShapeHeightDisplacement.1.0")
	{
		std::string subshape_name = iterate_shape(scene, input_shape_name, parameters, exported_nodes_map, ignore_subdivision, eval_time);
		std::string shape_name = subshape_name + "_height_displacement";
		std::string prefix = "scene.shapes." + shape_name;
		shape_props.Set(luxrays::Property(prefix + ".type")("displacement"));
		shape_props.Set(luxrays::Property(prefix + ".source")(subshape_name));

		shape_props.Set(luxrays::Property(prefix + ".map.type")("height"));
		float scale = get_float_parameter_value(parameters, "scale", eval_time);
		float offset = get_float_parameter_value(parameters, "offset", eval_time);
		bool normalsmooth = get_bool_parameter_value(parameters, "normalsmooth", eval_time);
		shape_props.Set(luxrays::Property(prefix + ".scale")(scale));
		shape_props.Set(luxrays::Property(prefix + ".offset")(offset));
		shape_props.Set(luxrays::Property(prefix + ".normalsmooth")(normalsmooth));
		set_material_value(scene, shape_props, "height", prefix + ".map", parameters, exported_nodes_map, ignore_subdivision, eval_time);

		scene->Parse(shape_props);
		return shape_name;
	}
	else if (prog_id == "LUXShadersPlugin.ShapeVectorDisplacement.1.0")
	{
		std::string subshape_name = iterate_shape(scene, input_shape_name, parameters, exported_nodes_map, ignore_subdivision, eval_time);
		std::string shape_name = subshape_name + "_vector_displacement";
		std::string prefix = "scene.shapes." + shape_name;
		shape_props.Set(luxrays::Property(prefix + ".type")("displacement"));
		shape_props.Set(luxrays::Property(prefix + ".source")(subshape_name));

		shape_props.Set(luxrays::Property(prefix + ".map.type")("vector"));
		float scale = get_float_parameter_value(parameters, "scale", eval_time);
		float offset = get_float_parameter_value(parameters, "offset", eval_time);
		bool normalsmooth = get_bool_parameter_value(parameters, "normalsmooth", eval_time);
		shape_props.Set(luxrays::Property(prefix + ".scale")(scale));
		shape_props.Set(luxrays::Property(prefix + ".offset")(offset));
		shape_props.Set(luxrays::Property(prefix + ".normalsmooth")(normalsmooth));
		set_material_value(scene, shape_props, "vector", prefix + ".map", parameters, exported_nodes_map, ignore_subdivision, eval_time);
		XSI::CString channels = get_string_parameter_value(parameters, "channels", eval_time);
		//parse symbols in channels string
		std::vector<int> a(3);
		for (ULONG i = 0; i < 3; i++)
		{
			char v = channels.GetAt(i);
			if (v == '0') { a[i] = 0; }
			else if (v == '1') { a[i] = 1; }
			else { a[i] = 2; }
		}
		shape_props.Set(luxrays::Property(prefix + ".map.channels")(a[0], a[1], a[2]));

		scene->Parse(shape_props);
		return shape_name;
	}
	else if (prog_id == "LUXShadersPlugin.ShapeHarlequin.1.0")
	{
		std::string subshape_name = iterate_shape(scene, input_shape_name, parameters, exported_nodes_map, ignore_subdivision, eval_time);
		std::string shape_name = subshape_name + "_harlequin";
		std::string prefix = "scene.shapes." + shape_name;
		shape_props.Set(luxrays::Property(prefix + ".type")("harlequin"));
		shape_props.Set(luxrays::Property(prefix + ".source")(subshape_name));

		scene->Parse(shape_props);
		return shape_name;
	}
	else if (prog_id == "LUXShadersPlugin.ShapeSimplify.1.0")
	{
		std::string subshape_name = iterate_shape(scene, input_shape_name, parameters, exported_nodes_map, ignore_subdivision, eval_time);
		std::string shape_name = subshape_name + "_simplify";
		std::string prefix = "scene.shapes." + shape_name;
		shape_props.Set(luxrays::Property(prefix + ".type")("simplify"));
		shape_props.Set(luxrays::Property(prefix + ".source")(subshape_name));

		float edgescreensize = get_float_parameter_value(parameters, "edgescreensize", eval_time);
		float target = get_float_parameter_value(parameters, "target", eval_time);
		bool preserveborder = get_bool_parameter_value(parameters, "preserveborder", eval_time);
		shape_props.Set(luxrays::Property(prefix + ".edgescreensize")(edgescreensize));
		shape_props.Set(luxrays::Property(prefix + ".preserveborder")(preserveborder));
		shape_props.Set(luxrays::Property(prefix + ".target")(target / 100.0f));

		scene->Parse(shape_props);
		return shape_name;
	}
	else if (prog_id == "LUXShadersPlugin.ShapeEdgeDetectorAOV.1.0")
	{
		std::string subshape_name = iterate_shape(scene, input_shape_name, parameters, exported_nodes_map, ignore_subdivision, eval_time);
		std::string shape_name = subshape_name + "_edgedetector";
		std::string prefix = "scene.shapes." + shape_name;
		shape_props.Set(luxrays::Property(prefix + ".type")("edgedetectoraov"));
		shape_props.Set(luxrays::Property(prefix + ".source")(subshape_name));

		scene->Parse(shape_props);
		return shape_name;
	}
	else if (prog_id == "LUXShadersPlugin.ShapeBevel.1.0")
	{
		std::string subshape_name = iterate_shape(scene, input_shape_name, parameters, exported_nodes_map, ignore_subdivision, eval_time);
		std::string shape_name = subshape_name + "_bevel";
		std::string prefix = "scene.shapes." + shape_name;
		shape_props.Set(luxrays::Property(prefix + ".type")("bevel"));
		shape_props.Set(luxrays::Property(prefix + ".source")(subshape_name));

		float radius = get_float_parameter_value(parameters, "radius", eval_time);
		shape_props.Set(luxrays::Property(prefix + ".bevel.radius")(radius));

		scene->Parse(shape_props);
		return shape_name;
	}
	else
	{
		return input_shape_name;
	}
}

std::string sync_polymesh_shapes(luxcore::Scene* scene, std::string &input_shape_name, XSI::Material & xsi_material, const bool ignore_subdivision, const XSI::CTime &eval_time)
{
	XSI::CRefArray first_level_shaders = xsi_material.GetShaders();
	std::unordered_map<ULONG, std::string> exported_nodes_map;
	std::vector<XSI::ShaderParameter> contour_ports = get_root_shader_parameter(first_level_shaders, GRSPM_ParameterName, "contour");
	std::string output_shape_name = input_shape_name;
	if (contour_ports.size() > 0)
	{
		//something connected to the port
		//get this node
		XSI::Shader shape_node = get_input_node(contour_ports[0]);
		output_shape_name = add_shape(scene, input_shape_name, shape_node, exported_nodes_map, ignore_subdivision, eval_time);
	}
	
	return output_shape_name;
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