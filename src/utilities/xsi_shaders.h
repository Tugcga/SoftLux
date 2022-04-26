#pragma once
#include "xsi_shaderparameter.h"
#include "xsi_shader.h"
#include "xsi_time.h"

#include <vector>

enum GetRootShaderParameterMode
{
	GRSPM_ParameterName,  // find by the name of the root parameter
	GRSPM_NodeName  // find by the name of the shader node, connected to the returned parameter
};

float get_float_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time);
int get_int_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time);
bool get_bool_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time);
XSI::CString get_string_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time);
XSI::MATH::CColor4f get_color_parameter_value(const XSI::CParameterRefArray& all_parameters, const XSI::CString& parameter_name, const XSI::CTime& eval_time);

//return shader parameter of the material root node, which has root_parameter_name name and connected to first-level node in the tree
//if there is no port with the name, return empty array
std::vector<XSI::ShaderParameter> get_root_shader_parameter(const XSI::CRefArray& first_level_shaders, GetRootShaderParameterMode mode, const XSI::CString& root_parameter_name = "", bool check_substring = false, const XSI::CString& plugin_name = "", const XSI::CString& node_name = "");

//return true if input shader node is compound
bool is_shader_compound(const XSI::Shader& shader);

//return shader node (may be inside compound), connected to the parameter
//parameter is a right port, shader node at the left
//*node*------>*parameter*
XSI::Shader get_input_node(const XSI::ShaderParameter& parameter);

//return parameter, connected to the input parameter of the shader node
//if it connect to the primitive node, then return the last parameter before this connection
//if it connect to compound input, then return this input
//go deeper through passthrough nodes
XSI::Parameter get_source_parameter(XSI::Parameter& parameter);