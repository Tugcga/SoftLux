#pragma once
#include <array>
#include <string>
#include <vector>

#include "xsi_kinematics.h"

#define M_PI 3.14159265358979323846
#define DEG2RADF(_deg) ((_deg) * (float)(M_PI / 180.0))
#define RAD2DEGF(_rad) ((_rad) * (float)(180.0 / M_PI))

double get_distance(const XSI::MATH::CVector3 &a, const XSI::MATH::CVector3& b);
std::array<float, 3> xsi_to_lux_vector(const XSI::MATH::CVector3 vector);

//convert transform matrix from xsi to luxcore
std::vector<double> xsi_to_lux_matrix(const XSI::MATH::CMatrix4& xsi_matrix, bool swap_axis = true);
std::vector<float> xsi_to_lux_matrix_float(const XSI::MATH::CMatrix4& xsi_matrix);
std::string xsi_to_lux_matrix_string(const XSI::MATH::CMatrix4& xsi_matrix);

float get_distant_light_normalization_factor(float theta);

//cionver string to array of floats
//split the string by "," and trip spaces for each part
//then convert to float
std::vector<float> string_to_array(const XSI::CString &input);