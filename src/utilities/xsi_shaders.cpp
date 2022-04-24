#include "xsi_shaders.h"

std::vector<XSI::ShaderParameter> get_root_shader_parameter(const XSI::CRefArray& first_level_shaders, const XSI::CString& root_parameter_name, bool check_substring)
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
					if ((!check_substring && p.GetName() == root_parameter_name) || (check_substring && p.GetName().FindString(root_parameter_name) != UINT_MAX))
					{
						to_return.push_back(p);
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

XSI::Shader get_input_node(const XSI::ShaderParameter& parameter)
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
			return source_shader;
		}
	}
	else
	{
		return XSI::Shader();
	}
}