#include "math.h"

double get_distance(const XSI::MATH::CVector3& a, const XSI::MATH::CVector3& b)
{
	double dx = a.GetX() - b.GetX();
	double dy = a.GetY() - b.GetY();
	double dz = a.GetZ() - b.GetZ();
	return sqrt(dx * dx + dy * dy + dz * dz);
}

std::array<float, 3> xsi_to_lux_vector(const XSI::MATH::CVector3 vector)
{
	std::array<float, 3> lux_vector{vector.GetZ(), vector.GetX(), vector.GetY()};

	return lux_vector;
}