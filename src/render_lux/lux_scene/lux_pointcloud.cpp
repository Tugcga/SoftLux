#include "../../utilities/logs.h"

#include "lux_scene.h"

#include "xsi_primitive.h"
#include "xsi_iceattribute.h"
#include "xsi_geometry.h"
#include "xsi_iceattributedataarray.h"

bool sync_pointcloud(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object, 
	MotionParameters& motion,
	std::unordered_map<ULONG, std::vector<std::string>>& xsi_id_to_lux_names_map,
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<ULONG, std::vector<ULONG>>& master_to_instance_map,
	std::unordered_map<ULONG, std::set<ULONG>>& material_with_shape_to_polymesh_map,
	std::set<std::string>& names_to_delete,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time)
{
	ULONG xsi_id = xsi_object.GetObjectID();

	XSI::MATH::CTransformation particles_tfm = xsi_object.GetKinematics().GetGlobal().GetTransform();

	XSI::Primitive pc_primitive = xsi_object.GetActivePrimitive(eval_time);
	XSI::Geometry pc_geometry = pc_primitive.GetGeometry(eval_time);

	XSI::ICEAttribute position_attr = pc_geometry.GetICEAttributeFromName("PointPosition");
	if (position_attr.GetElementCount() > 0)
	{
		XSI::CICEAttributeDataArrayVector3f position_data;
		position_attr.GetDataArray(position_data);

		XSI::ICEAttribute shape_attr = pc_geometry.GetICEAttributeFromName("Shape");
		XSI::CICEAttributeDataArrayShape shape_data;
		shape_attr.GetDataArray(shape_data);

		XSI::ICEAttribute orientation_attr = pc_geometry.GetICEAttributeFromName("Orientation");
		XSI::CICEAttributeDataArrayRotationf rotation_data;
		orientation_attr.GetDataArray(rotation_data);

		XSI::ICEAttribute size_attr = pc_geometry.GetICEAttributeFromName("Size");
		XSI::CICEAttributeDataArrayFloat size_data;
		size_attr.GetDataArray(size_data);

		XSI::ICEAttribute scale_attr = pc_geometry.GetICEAttributeFromName("Scale");
		XSI::CICEAttributeDataArrayVector3f scale_data;
		scale_attr.GetDataArray(scale_data);

		int scale_count = scale_attr.GetElementCount();
		bool is_scale_define = scale_attr.IsDefined();
		ULONG shape_data_count = shape_data.GetCount();
		ULONG position_data_count = position_data.GetCount();

		//next read motion SRT
		std::vector<XSI::MATH::CTransformation> time_transforms = build_time_points_transforms(xsi_object, motion, eval_time);
		for (ULONG i = 0; i < shape_data_count; i++)
		{
			XSI::MATH::CVector3f position = position_data[i];
			XSI::MATH::CRotationf rotation = rotation_data[i];
			float scale_x = 1.0f;
			float scale_y = 1.0f;
			float scale_z = 1.0f;
			if (is_scale_define && i < scale_count)
			{
				scale_x = scale_data[i].GetX();
				scale_y = scale_data[i].GetY();
				scale_z = scale_data[i].GetZ();
			}
			XSI::MATH::CVector3f size(scale_x * size_data[i], scale_y * size_data[i], scale_z * size_data[i]);

			if (shape_data[i].GetType() == XSI::siICEShapeReference)
			{
				XSI::MATH::CShape shape = shape_data[i];
				bool is_branch_selected = shape.IsBranchSelected();  // if true, then we should export the whole hierarchy, if false - then only the root object
				ULONG shape_ref_id = shape.GetReferenceID();
				XSI::X3DObject master_root = (XSI::X3DObject)XSI::Application().GetObjectFromID(shape_ref_id);

				//generate transform of the particle
				XSI::MATH::CTransformation point_tfm;
				point_tfm.SetIdentity();
				point_tfm.SetTranslationFromValues(position.GetX(), position.GetY(), position.GetZ());
				float rot_x; float rot_y; float rot_z;
				rotation.GetXYZAngles(rot_x, rot_y, rot_z);
				point_tfm.SetRotationFromXYZAnglesValues(rot_x, rot_y, rot_z);
				point_tfm.SetScalingFromValues(size.GetX(), size.GetY(), size.GetZ());
				point_tfm.MulInPlace(particles_tfm);

				std::string point_name = std::to_string(xsi_id) + "_" + std::to_string(i);

				//here we should create transforms from point data in different times
				sync_instance(scene, xsi_id, point_name, master_root, motion, point_tfm, xsi_id_to_lux_names_map, xsi_materials_in_lux, master_to_instance_map, material_with_shape_to_polymesh_map, names_to_delete, object_name_to_shape_name, object_name_to_material_name, eval_time, true, is_branch_selected, time_transforms, motion.motion_objects ? i * motion.motion_steps : 0);
			}
		}
	}
	return true;
}