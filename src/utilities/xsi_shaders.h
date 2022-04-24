#pragma once
#include "xsi_shaderparameter.h"
#include "xsi_shader.h"

#include <vector>

//return shader parameter of the material root node, which has root_parameter_name name and connected to first-level node in the tree
//if there is no port with the name, return empty array
std::vector<XSI::ShaderParameter> get_root_shader_parameter(const XSI::CRefArray& first_level_shaders, const XSI::CString& root_parameter_name, bool check_substring = false);

//return true if input shader node is compound
bool is_shader_compound(const XSI::Shader& shader);

//return shader node (may be inside compound), connected to the parameter
//parameter is a right port, shader node at the left
//*node*------>*parameter*
XSI::Shader get_input_node(const XSI::ShaderParameter& parameter);