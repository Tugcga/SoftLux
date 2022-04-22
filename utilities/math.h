#pragma once
#include <array>
#include <string>
#include <vector>

#include "xsi_kinematics.h"

double get_distance(const XSI::MATH::CVector3 &a, const XSI::MATH::CVector3& b);
std::array<float, 3> xsi_to_lux_vector(const XSI::MATH::CVector3 vector);

//convert transform matrix from xsi to luxcore
std::vector<double> xsi_to_lux_matrix(const XSI::MATH::CMatrix4& xsi_matrix);
std::string xsi_to_lux_matrix_string(const XSI::MATH::CMatrix4& xsi_matrix);