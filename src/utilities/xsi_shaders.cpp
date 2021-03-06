#include "xsi_shaders.h"
#include "logs.h"

float get_float_parameter_value(const XSI::CParameterRefArray &all_parameters, const XSI::CString &parameter_name, const XSI::CTime &eval_time)
{
	XSI::Parameter param = all_parameters.GetItem(parameter_name);
	XSI::Parameter param_final = get_source_parameter(param);

	return (float)param_final.GetValue(eval_time);
}

int get_int_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time)
{
	XSI::Parameter param = all_parameters.GetItem(parameter_name);
	XSI::Parameter param_final = get_source_parameter(param);

	return (int)param_final.GetValue(eval_time);
}

bool get_bool_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time)
{
	XSI::Parameter param = all_parameters.GetItem(parameter_name);
	XSI::Parameter param_final = get_source_parameter(param);

	return (bool)param_final.GetValue(eval_time);
}

XSI::CString get_string_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time)
{
	XSI::Parameter param = all_parameters.GetItem(parameter_name);
	XSI::Parameter param_final = get_source_parameter(param);

	return (XSI::CString)param_final.GetValue(eval_time);
}

XSI::MATH::CColor4f get_color_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time)
{
	XSI::Parameter param = all_parameters.GetItem(parameter_name);
	XSI::Parameter param_final = get_source_parameter(param);

	return (XSI::MATH::CColor4f)param_final.GetValue(eval_time);
}

XSI::MATH::CVector3 get_vector_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time)
{
	XSI::Parameter param = all_parameters.GetItem(parameter_name);
	XSI::Parameter param_final = get_source_parameter(param);
	XSI::CParameterRefArray v_params = param_final.GetParameters();
	XSI::Parameter p[3];
	p[0] = XSI::Parameter(v_params[0]);
	p[1] = XSI::Parameter(v_params[1]);
	p[2] = XSI::Parameter(v_params[2]);

	return XSI::MATH::CVector3(p[0].GetValue(eval_time), p[1].GetValue(eval_time), p[2].GetValue(eval_time));
}

ShaderParameterType get_shader_parameter_type(XSI::ShaderParameter &parameter)
{
	XSI::siShaderParameterDataType xsi_type = parameter.GetDataType();
	if (xsi_type == XSI::siShaderDataTypeColor3)
	{
		return ShaderParameterType::ParameterType_Color3;
	}
	else if (xsi_type == XSI::siShaderDataTypeColor4)
	{
		return ShaderParameterType::ParameterType_Color4;
	}
	else if (xsi_type == XSI::siShaderDataTypeInteger)
	{
		return ShaderParameterType::ParameterType_Integer;
	}
	else if (xsi_type == XSI::siShaderDataTypeScalar)
	{
		return ShaderParameterType::ParameterType_Float;
	}
	else if (xsi_type == XSI::siShaderDataTypeString)
	{
		return ShaderParameterType::ParameterType_String;
	}
	else if (xsi_type == XSI::siShaderDataTypeBoolean)
	{
		return ShaderParameterType::ParameterType_Boolean;
	}
	else if (xsi_type == XSI::siShaderDataTypeVector3)
	{
		return ShaderParameterType::ParameterType_Vector3;
	}
	else
	{
		return ShaderParameterType::ParameterType_Unknown;
	}
}

std::vector<XSI::ShaderParameter> get_root_shader_parameter(const XSI::CRefArray& first_level_shaders, 
															GetRootShaderParameterMode mode, 
															const XSI::CString& root_parameter_name, 
															bool check_substring, 
															const XSI::CString& plugin_name,
															const XSI::CString& node_name)
{
	std::vector<XSI::ShaderParameter> to_return;
	for (ULONG i = 0; i < first_level_shaders.GetCount(); i++)
	{
		XSI::Shader shader(first_level_shaders[i]);
		XSI::CRefArray shader_params = XSI::CRefArray(shader.GetParameters());
		for (ULONG j = 0; j < shader_params.GetCount(); j++)
		{
			XSI::Parameter parameter(shader_params.GetItem(j));
			XSI::CString parameter_name = parameter.GetName();
			bool is_input;
			XSI::siShaderParameterType param_type = shader.GetShaderParameterType(parameter_name, is_input);
			//here there is a bug in Softimage: output parameter does not visible or interpreter as input, when it is a port, created from inside of the compound
			if (!is_input)
			{
				//this is output shader parameter
				XSI::CRefArray targets = shader.GetShaderParameterTargets(parameter_name);
				for (LONG k = 0; k < targets.GetCount(); k++)
				{
					XSI::ShaderParameter p = targets.GetItem(k);
					if (mode == GRSPM_ParameterName)
					{
						if ((!check_substring && p.GetName() == root_parameter_name) || (check_substring && p.GetName().FindString(root_parameter_name) != UINT_MAX))
						{
							to_return.push_back(p);
						}
					}
					else if (mode == GRSPM_NodeName)
					{
						//get the node
						XSI::Shader connected_node = get_input_node(p);
						XSI::CStringArray name_parts = connected_node.GetProgID().Split(".");
						if (name_parts.GetCount() >= 2)
						{
							if (name_parts[0] == plugin_name && name_parts[1] == node_name)
							{
								to_return.push_back(p);
								return to_return;
							}
						}
					}
				}
			}
		}
	}
	return to_return;
}

bool is_shader_compound(const XSI::Shader& shader)
{
	XSI::CRefArray sub_shaders = shader.GetAllShaders();
	return sub_shaders.GetCount() > 0;
}

XSI::Shader get_input_node(const XSI::ShaderParameter& parameter, bool ignore_converters)
{
	XSI::CRef source = parameter.GetSource();
	if (source.IsValid())
	{
		XSI::ShaderParameter source_param(source);
		XSI::Shader source_shader(source_param.GetParent());
		if (is_shader_compound(source_shader))
		{
			return get_input_node(source_param);
		}
		else
		{
			if (!ignore_converters)
			{
				return source_shader;
			}
			else
			{
				XSI::CString prog_id = source_shader.GetProgID();
				if (prog_id == "Softimage.sib_scalar_to_color.1.0" || prog_id == "Softimage.sib_color_to_scalar.1.0" || prog_id == "Softimage.sib_color_to_vector.1.0" || prog_id == "Softimage.sib_vector_to_color.1.0")
				{
					XSI::Parameter input_param = source_shader.GetParameter("input");
					XSI::ShaderParameter shader_input_param(input_param);
					return get_input_node(shader_input_param, ignore_converters);
				}
				else
				{
					//finish by this node
					return source_shader;
				}
			}
		}
	}
	else
	{
		if (!ignore_converters)
		{
			return XSI::Shader();
		}
		else
		{
			//get parent node
			XSI::Shader parent_node = parameter.GetParent();
			XSI::CString prog_id = parent_node.GetProgID();
			if (prog_id == "Softimage.sib_scalar_to_color.1.0" || prog_id == "Softimage.sib_color_to_scalar.1.0" || prog_id == "Softimage.sib_color_to_vector.1.0" || prog_id == "Softimage.sib_vector_to_color.1.0")
			{
				return parent_node;
			}
			else
			{
				return XSI::Shader();
			}
		}
	}
}

XSI::Parameter get_source_parameter(XSI::Parameter &parameter)
{
	XSI::CRef source = parameter.GetSource();
	if (source.IsValid())
	{
		//source is valid
		//this means that parameter is connected to something
		if (source.GetClassID() == XSI::siShaderParameterID)
		{
			//get the parent node of the shader parameter
			XSI::ShaderParameter source_param(source);
			XSI::Shader source_node = source_param.GetParent();
			XSI::CString source_prog_id = source_node.GetProgID();
			if (source_prog_id.GetSubString(0, 13) == "XSIRTCOMPOUND")
			{
				//parameter connected to the compound port, try to find next connection
				return get_source_parameter(source_param);
			}
			else
			{
				//may be the source node is passthrough node, then go deeper
				XSI::CStringArray name_parts = source_prog_id.Split(".");
				if (name_parts[0] == "SIUtilityShaders")
				{
					if (name_parts[1].ReverseFindString("Passthrough") < UINT_MAX)
					{
						//find substring, this is passtrouhg node
						//get input parameter of the node
						XSI::Parameter p = source_node.GetParameter("input");
						return get_source_parameter(p);
					}
					else
					{
						return parameter;
					}
				}
				else
				{
					//this is any shader node, so, our parameter connected to something, return itself
					return parameter;
				}
			}
		}
		else
		{
			//parameter connect not to the shader parameter, this is wron, so, return the input parameter
			return parameter;
		}
	}
	else
	{
		//if source of the parameter is not valid, then this parameter does not connect to anything, so, this is the final parameter
		return parameter;
	}
}