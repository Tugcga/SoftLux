#include "math.h"
#include <math.h>

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

std::vector<double> xsi_to_lux_matrix(const XSI::MATH::CMatrix4& xsi_matrix)
{
	std::vector<double> to_return = {
		xsi_matrix.GetValue(0, 2), xsi_matrix.GetValue(0, 0), xsi_matrix.GetValue(0, 1), xsi_matrix.GetValue(0, 3),
		xsi_matrix.GetValue(1, 2), xsi_matrix.GetValue(1, 0), xsi_matrix.GetValue(1, 1), xsi_matrix.GetValue(1, 3),
		xsi_matrix.GetValue(2, 2), xsi_matrix.GetValue(2, 0), xsi_matrix.GetValue(2, 1), xsi_matrix.GetValue(2, 3),
		xsi_matrix.GetValue(3, 2), xsi_matrix.GetValue(3, 0), xsi_matrix.GetValue(3, 1), xsi_matrix.GetValue(3, 3)
	};

	return to_return;
}

std::vector<float> xsi_to_lux_matrix_float(const XSI::MATH::CMatrix4& xsi_matrix)
{
	std::vector<float> to_return = {
		(float)xsi_matrix.GetValue(0, 2), (float)xsi_matrix.GetValue(0, 0), (float)xsi_matrix.GetValue(0, 1), (float)xsi_matrix.GetValue(0, 3),
		(float)xsi_matrix.GetValue(1, 2), (float)xsi_matrix.GetValue(1, 0), (float)xsi_matrix.GetValue(1, 1), (float)xsi_matrix.GetValue(1, 3),
		(float)xsi_matrix.GetValue(2, 2), (float)xsi_matrix.GetValue(2, 0), (float)xsi_matrix.GetValue(2, 1), (float)xsi_matrix.GetValue(2, 3),
		(float)xsi_matrix.GetValue(3, 2), (float)xsi_matrix.GetValue(3, 0), (float)xsi_matrix.GetValue(3, 1), (float)xsi_matrix.GetValue(3, 3)
	};

	return to_return;
}

std::string xsi_to_lux_matrix_string(const XSI::MATH::CMatrix4 &xsi_matrix)
{
	return std::to_string(xsi_matrix.GetValue(0, 2)) + " " + std::to_string(xsi_matrix.GetValue(0, 0)) + " " + std::to_string(xsi_matrix.GetValue(0, 1)) + " " + std::to_string(xsi_matrix.GetValue(0, 3)) +
		   std::to_string(xsi_matrix.GetValue(1, 2)) + " " + std::to_string(xsi_matrix.GetValue(1, 0)) + " " + std::to_string(xsi_matrix.GetValue(1, 1)) + " " + std::to_string(xsi_matrix.GetValue(1, 3)) +
		   std::to_string(xsi_matrix.GetValue(2, 2)) + " " + std::to_string(xsi_matrix.GetValue(2, 0)) + " " + std::to_string(xsi_matrix.GetValue(2, 1)) + " " + std::to_string(xsi_matrix.GetValue(2, 3)) +
		   std::to_string(xsi_matrix.GetValue(3, 2)) + " " + std::to_string(xsi_matrix.GetValue(3, 0)) + " " + std::to_string(xsi_matrix.GetValue(3, 1)) + " " + std::to_string(xsi_matrix.GetValue(3, 3));
}

//from luxcore for Blender source
float get_distant_light_normalization_factor(float theta)
{
	float epsilon = 1e-9;
	float cos_theta_max = std::min((float)cos(DEG2RADF(theta)), 1 - epsilon);
	return 1.0f / (2.0f * M_PI * (1.0f - cos_theta_max));
}