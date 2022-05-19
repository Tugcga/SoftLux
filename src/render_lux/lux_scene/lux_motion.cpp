#include "lux_scene.h"

#include "../../utilities/logs.h"

#include "xsi_primitive.h"
#include "xsi_iceattribute.h"
#include "xsi_geometry.h"
#include "xsi_iceattributedataarray.h"

bool sync_motion(luxrays::Properties &lux_props,
	const std::string &prefix,
	const MotionParameters &motion,
	const XSI::KinematicState &xsi_kinematic,
	const XSI::CTime &eval_time,
	const bool set_only_position)
{
	//if set_only_position is true, then use identity transform materix and set only position (but also swap axis)
	if (motion.motion_objects)
	{
		//camera is open from 0 to shutter time
		//so, we should devide the interval of the length = shutter_time into steps and find transforms of the object at each time point
		float one_step_time = motion.motion_shutter_time / (float)(motion.motion_steps - 1);
		const float shift_time = motion.motion_shutter_time / 2;
		for (ULONG step = 0; step < motion.motion_steps; step++)
		{
			float s_time = step * one_step_time;
			lux_props.Set(luxrays::Property(prefix + ".motion." + std::to_string(step) + ".time")(s_time));
			//and next transformation
			//but at first we should get time inside the Softimeage, where we should evaluate the transform
			const float xsi_time = eval_time.GetTime() - shift_time + step * one_step_time;
			//get transform at the time point
			XSI::MATH::CTransformation xsi_time_tfm = xsi_kinematic.GetTransform(xsi_time);
			if (set_only_position)
			{
				//set transform only with position
				double x; double y; double z;
				xsi_time_tfm.GetTranslationValues(x, y, z);
				std::vector<double> lux_matrix = 
				{
					1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					z, x, y, 1.0
				};
				lux_props.Set(luxrays::Property(prefix + ".motion." + std::to_string(step) + ".transformation")(lux_matrix));
			}
			else
			{
				//set full transform
				XSI::MATH::CMatrix4 xsi_matrix = xsi_time_tfm.GetMatrix4();
				std::vector<double> lux_matrix = xsi_to_lux_matrix(xsi_matrix);
				lux_props.Set(luxrays::Property(prefix + ".motion." + std::to_string(step) + ".transformation")(lux_matrix));
			}
		}

		return true;
	}

	return false;
}

bool sync_instance_motion(luxrays::Properties& lux_props,
	const std::string& prefix,
	const MotionParameters& motion,
	const std::vector<XSI::MATH::CTransformation>& time_transforms,
	const ULONG time_start_index,
	const XSI::KinematicState &master_root_kine,
	const XSI::KinematicState& master_object_kine,
	const XSI::CTime& eval_time)
{
	if (motion.motion_objects)
	{
		float one_step_time = motion.motion_shutter_time / (float)(motion.motion_steps - 1);
		const float shift_time = motion.motion_shutter_time / 2;
		for (ULONG step = 0; step < motion.motion_steps; step++)
		{
			float s_time = step * one_step_time;
			lux_props.Set(luxrays::Property(prefix + ".motion." + std::to_string(step) + ".time")(s_time));

			const float xsi_time = eval_time.GetTime() - shift_time + step * one_step_time;
			XSI::MATH::CTransformation root_tfm = master_root_kine.GetTransform(xsi_time);
			XSI::MATH::CTransformation object_tfm = master_object_kine.GetTransform(xsi_time);

			XSI::MATH::CTransformation root_tfm_inverse;
			root_tfm_inverse.Invert(root_tfm);
			XSI::MATH::CTransformation object_root_tfm;
			object_root_tfm.Mul(object_tfm, root_tfm_inverse);
			XSI::MATH::CTransformation object_instance_tfm;
			object_instance_tfm.Mul(object_root_tfm, time_transforms[time_start_index + step]);

			XSI::MATH::CMatrix4 xsi_matrix = object_instance_tfm.GetMatrix4();
			std::vector<double> lux_matrix = xsi_to_lux_matrix(xsi_matrix);
			lux_props.Set(luxrays::Property(prefix + ".motion." + std::to_string(step) + ".transformation")(lux_matrix));
		}

		return true;
	}
	else
	{
		return false;
	}
}

std::vector<XSI::MATH::CTransformation> build_time_transforms(const XSI::KinematicState &xsi_kine,
	const MotionParameters &motion,
	const XSI::CTime &eval_time)
{
	if (motion.motion_objects)
	{
		std::vector<XSI::MATH::CTransformation> to_return(motion.motion_steps);
		float one_step_time = motion.motion_shutter_time / (float)(motion.motion_steps - 1);
		const float shift_time = motion.motion_shutter_time / 2;
		for (ULONG step = 0; step < motion.motion_steps; step++)
		{
			const float xsi_time = eval_time.GetTime() - shift_time + step * one_step_time;
			XSI::MATH::CTransformation xsi_time_tfm = xsi_kine.GetTransform(xsi_time);
			to_return[step] = xsi_time_tfm;
		}

		return to_return;
	}
	else
	{
		return std::vector<XSI::MATH::CTransformation>(0);
	}
}

void put_transform(const XSI::MATH::CVector3f &position, 
	const XSI::MATH::CRotationf &rotation, 
	const ULONG p_index, 
	const bool is_scale_define, 
	const ULONG scale_count,
	const XSI::MATH::CVector3f &scale,
	const float size,
	const XSI::MATH::CTransformation &root_tfm,
	const ULONG step,
	const ULONG shape_data_count,
	const ULONG motion_steps,
	std::vector<XSI::MATH::CTransformation> &time_transforms)
{
	//create point SRT
	
	float scale_x = 1.0f;
	float scale_y = 1.0f;
	float scale_z = 1.0f;
	if (is_scale_define && p_index < scale_count)
	{
		scale_x = scale.GetX();
		scale_y = scale.GetY();
		scale_z = scale.GetZ();
	}
	XSI::MATH::CVector3f finall_size(scale_x * size, scale_y * size, scale_z * size);

	//create transform
	XSI::MATH::CTransformation point_tfm;
	point_tfm.SetIdentity();
	point_tfm.SetTranslationFromValues(position.GetX(), position.GetY(), position.GetZ());
	float rot_x; float rot_y; float rot_z;
	rotation.GetXYZAngles(rot_x, rot_y, rot_z);
	point_tfm.SetRotationFromXYZAnglesValues(rot_x, rot_y, rot_z);
	point_tfm.SetScalingFromValues(finall_size.GetX(), finall_size.GetY(), finall_size.GetZ());
	point_tfm.MulInPlace(root_tfm);

	time_transforms[step + p_index * motion_steps] = point_tfm;
}

std::vector<XSI::MATH::CTransformation> build_time_points_transforms(const XSI::X3DObject& particles,
	const MotionParameters& motion,
	const XSI::CTime& eval_time)
{
	XSI::Primitive pc_primitive = particles.GetActivePrimitive();
	XSI::KinematicState particles_kine = particles.GetKinematics().GetGlobal();
	XSI::Geometry pc_geometry = pc_primitive.GetGeometry(eval_time);
	XSI::ICEAttribute shape_attr = pc_geometry.GetICEAttributeFromName("Shape");
	XSI::CICEAttributeDataArrayShape shape_data;
	shape_attr.GetDataArray(shape_data);
	ULONG shape_data_count = shape_data.GetCount();
	//we should find point indexes with valid shapes
	//for now we consider only instance shapes
	std::vector<ULONG> point_valid_indices;
	point_valid_indices.reserve(shape_data_count);
	for (ULONG i = 0; i < shape_data_count; i++)
	{
		if (shape_data[i].GetType() == XSI::siICEShapeReference)
		{
			point_valid_indices.push_back(i);
		}
	}
	ULONG valid_points_count = point_valid_indices.size();

	XSI::ICEAttribute orientation_attr = pc_geometry.GetICEAttributeFromName("Orientation");
	XSI::CICEAttributeDataArrayRotationf rotation_data;
	orientation_attr.GetDataArray(rotation_data);

	XSI::ICEAttribute size_attr = pc_geometry.GetICEAttributeFromName("Size");
	XSI::CICEAttributeDataArrayFloat size_data;
	size_attr.GetDataArray(size_data);

	XSI::ICEAttribute scale_attr = pc_geometry.GetICEAttributeFromName("Scale");
	XSI::CICEAttributeDataArrayVector3f scale_data;
	scale_attr.GetDataArray(scale_data);

	XSI::ICEAttribute position_attr = pc_geometry.GetICEAttributeFromName("PointPosition");
	XSI::CICEAttributeDataArrayVector3f position_data;
	position_attr.GetDataArray(position_data);

	int scale_count = scale_attr.GetElementCount();
	bool is_scale_define = scale_attr.IsDefined();

	std::vector<XSI::MATH::CTransformation> time_transforms;
	if (motion.motion_objects && shape_data_count > 0 && valid_points_count > 0)
	{
		//we should save transforms of all points in the particle
		//save all transformations for the first point, then for the second and so on
		//so, the size of this array is steps * points
		time_transforms.resize(motion.motion_steps * valid_points_count);  // we should ignore point type and other types of particles

		//next read all these transforms
		float one_step_time = motion.motion_shutter_time / (float)(motion.motion_steps - 1);
		const float shift_time = motion.motion_shutter_time / 2;
		for (ULONG step = 0; step < motion.motion_steps; step++)
		{
			const float xsi_time = eval_time.GetTime() - shift_time + step * one_step_time;
			XSI::MATH::CTransformation pc_time_tfm = particles_kine.GetTransform(xsi_time);
			//next get attributes at time
			XSI::Geometry time_geometry = pc_primitive.GetGeometry(xsi_time);
			XSI::ICEAttribute time_position_attribute = time_geometry.GetICEAttributeFromName("PointPosition");
			XSI::ICEAttribute time_orientation_attribute = time_geometry.GetICEAttributeFromName("Orientation");
			XSI::ICEAttribute time_size_attribute = time_geometry.GetICEAttributeFromName("Size");
			XSI::ICEAttribute time_scale_attribute = time_geometry.GetICEAttributeFromName("Scale");
			XSI::ICEAttribute time_shape_attr = time_geometry.GetICEAttributeFromName("Shape");
			
			if (time_position_attribute.GetElementCount() > 0 && time_orientation_attribute.GetElementCount() > 0 && time_size_attribute.GetElementCount() > 0)
			{
				//read attributes data
				XSI::CICEAttributeDataArrayVector3f time_position_data;
				XSI::CICEAttributeDataArrayRotationf time_orientation_data;
				XSI::CICEAttributeDataArrayFloat time_size_data;
				XSI::CICEAttributeDataArrayVector3f time_scale_data;
				XSI::CICEAttributeDataArrayShape time_shape_data;
				time_position_attribute.GetDataArray(time_position_data);
				time_orientation_attribute.GetDataArray(time_orientation_data);
				time_size_attribute.GetDataArray(time_size_data);
				time_scale_attribute.GetDataArray(time_scale_data);
				time_shape_attr.GetDataArray(time_shape_data);
				int scale_count = time_scale_attribute.GetElementCount();
				bool is_scale_define = time_scale_attribute.IsDefined();
				//save data per object
				for (size_t i = 0; i < valid_points_count; i++)
				{
					ULONG p_index = point_valid_indices[i];
					put_transform(time_position_data[p_index], time_orientation_data[p_index], i, is_scale_define, scale_count, time_scale_data[p_index], time_size_data[p_index], pc_time_tfm, step, shape_data_count, motion.motion_steps, time_transforms);
				}
			}
			else
			{//for invalid attribute save current particle transforms
				for (size_t i = 0; i < valid_points_count; i++)
				{
					ULONG p_index = point_valid_indices[i];
					put_transform(position_data[p_index], rotation_data[p_index], i, is_scale_define, scale_count, scale_data[p_index], size_data[p_index], pc_time_tfm, step, shape_data_count, motion.motion_steps, time_transforms);
				}
			}
		}
	}
	return time_transforms;
}