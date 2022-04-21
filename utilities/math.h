#pragma once
#include <array>
#include "xsi_kinematics.h"

double get_distance(const XSI::MATH::CVector3 &a, const XSI::MATH::CVector3& b);
std::array<float, 3> xsi_to_lux_vector(const XSI::MATH::CVector3 vector);