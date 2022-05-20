#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "lux_scene.h"

#include "xsi_iceattribute.h"
#include "xsi_geometry.h"
#include "xsi_iceattributedataarray.h"
#include "xsi_iceattributedataarray2D.h"

bool sync_pointcloud_strands(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object,
	const XSI::CParameterRefArray& render_params,
	std::set<ULONG>& xsi_materials_in_lux, 
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime &eval_time)
{
	//get attributes
	XSI::Geometry xsi_geometry = xsi_object.GetActivePrimitive(eval_time).GetGeometry(eval_time);
	XSI::ICEAttribute pp_attr = xsi_geometry.GetICEAttributeFromName("PointPosition");
	XSI::ICEAttribute sp_attr = xsi_geometry.GetICEAttributeFromName("StrandPosition");
	XSI::ICEAttribute size_attr = xsi_geometry.GetICEAttributeFromName("Size");
	XSI::ICEAttribute color_attr = xsi_geometry.GetICEAttributeFromName("Color");

	//fill the data
	XSI::CICEAttributeDataArrayVector3f pp_data;
	pp_attr.GetDataArray(pp_data);

	XSI::CICEAttributeDataArray2DVector3f sp_data;
	sp_attr.GetDataArray2D(sp_data);

	XSI::CICEAttributeDataArrayFloat size_data;
	size_attr.GetDataArray(size_data);

	XSI::CICEAttributeDataArrayVector3f one_strand_data;
	sp_data.GetSubArray(0, one_strand_data);

	XSI::CICEAttributeDataArrayColor4f color_data;
	color_attr.GetDataArray(color_data);

	ULONG curves_count = pp_data.GetCount();
	ULONG strand_length = one_strand_data.GetCount();

	//create Luxcore strands object
	luxrays::cyHairFile strands;
	strands.SetHairCount(curves_count);
	strands.SetPointCount(curves_count * (strand_length + 1));

	//ICE strands always have constant size, segments and so on
	strands.SetDefaultSegmentCount(strand_length);
	strands.SetDefaultTransparency(0.0f);

	int flags = CY_HAIR_FILE_POINTS_BIT;
	flags |= CY_HAIR_FILE_THICKNESS_BIT;
	flags |= CY_HAIR_FILE_COLORS_BIT;
	strands.SetArrays(flags);

	ULONG point_count = (strand_length + 1) * curves_count;
	ULONG points_count_x3 = 3 * point_count;
	std::vector<float> points(points_count_x3, 0.0f);
	std::vector<float> thickness(point_count, 0.0f);
	std::vector<float> colors(points_count_x3, 1.0f);

	//fill buffers
	ULONG points_in_strand = strand_length + 1;
	ULONG point_index = 0;
	ULONG thickness_index = 0;
	for (ULONG curve_index = 0; curve_index < curves_count; curve_index++)
	{
		XSI::MATH::CVector3f start_point = pp_data[curve_index];
		XSI::MATH::CColor4f point_color = color_data[curve_index];
		points[point_index] = start_point.GetX();
		colors[point_index] = point_color.GetR();
		point_index++;
		points[point_index] = start_point.GetY();
		colors[point_index] = point_color.GetG();
		point_index++;
		points[point_index] = start_point.GetZ();
		colors[point_index] = point_color.GetB();
		point_index++;

		thickness[thickness_index] = size_data[curve_index];
		thickness_index++;

		sp_data.GetSubArray(curve_index, one_strand_data);
		for (ULONG k_index = 0; k_index < one_strand_data.GetCount(); k_index++)
		{
			XSI::MATH::CVector3f middle_point = one_strand_data[k_index];
			points[point_index] = middle_point.GetX();
			colors[point_index] = point_color.GetR();
			point_index++;
			points[point_index] = middle_point.GetY();
			colors[point_index] = point_color.GetG();
			point_index++;
			points[point_index] = middle_point.GetZ();
			colors[point_index] = point_color.GetB();
			point_index++;

			//use the same value for the whole strand
			thickness[thickness_index] = size_data[curve_index];
			thickness_index++;
		}
	}

	std::move(points.begin(), points.end(), strands.GetPointsArray());
	std::move(thickness.begin(), thickness.end(), strands.GetThicknessArray());
	std::move(colors.begin(), colors.end(), strands.GetColorsArray());

	XSI::Primitive xsi_primitive = xsi_object.GetActivePrimitive(eval_time);
	std::string shape_name = xsi_object_id_string(xsi_primitive)[0];
	scene->DefineStrands(shape_name, strands, luxcore::Scene::TESSEL_SOLID, 1, 0.1f, 12, true, true, true);
	
	luxrays::Properties strands_props;
	std::string object_name = xsi_object_id_string(xsi_object)[0];
	strands_props.Set(luxrays::Property("scene.objects." + object_name + ".shape")(shape_name));
	XSI::Material xsi_material = xsi_object.GetMaterial();
	std::string mat_name = xsi_object_id_string(xsi_material)[0];
	if (!scene->IsMaterialDefined(mat_name))
	{
		sync_material(scene, xsi_material, xsi_materials_in_lux, eval_time);
	}
	strands_props.Set(luxrays::Property("scene.objects." + object_name + ".material")(mat_name));

	//set transform
	XSI::MATH::CMatrix4 xsi_matrix = xsi_object.GetKinematics().GetGlobal().GetTransform().GetMatrix4();
	std::vector<double> lux_matrix = xsi_to_lux_matrix(xsi_matrix);
	strands_props.Set(luxrays::Property("scene.objects." + object_name + ".transformation")(lux_matrix));
	
	bool is_motion = sync_motion(strands_props, "scene.objects." + object_name, render_params, xsi_object.GetKinematics().GetGlobal(), eval_time);

	scene->Parse(strands_props);

	object_name_to_shape_name[object_name] = shape_name;
	object_name_to_material_name[object_name] = mat_name;

	points.clear();
	points.shrink_to_fit();
	thickness.clear();
	thickness.shrink_to_fit();
	colors.clear();
	colors.shrink_to_fit();

	return true;
}