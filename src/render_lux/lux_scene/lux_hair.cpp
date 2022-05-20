#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "lux_scene.h"

#include "xsi_hairprimitive.h"
#include "xsi_floatarray.h"

bool sync_hair(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object, 
	const XSI::CParameterRefArray& render_params,
	std::set<ULONG>& xsi_materials_in_lux,
	std::unordered_map<std::string, std::string>& object_name_to_shape_name,
	std::unordered_map<std::string, std::string>& object_name_to_material_name,
	const XSI::CTime& eval_time)
{
	XSI::HairPrimitive hair_prim(xsi_object.GetActivePrimitive(eval_time));

	LONG hairs_count = hair_prim.GetParameterValue("TotalHairs");
	LONG strand_multiplicity = hair_prim.GetParameterValue("StrandMult");
	if (strand_multiplicity <= 1)
	{
		strand_multiplicity = 1;
	}
	hairs_count = hairs_count * strand_multiplicity;
	XSI::CRenderHairAccessor rha = hair_prim.GetRenderHairAccessor(hairs_count);
	LONG num_uvs = rha.GetUVCount();
	LONG num_colors = rha.GetVertexColorCount();
	LONG i = 0;

	//Get keys and curves data
	ULONG num_keys = 0;
	ULONG num_curves = 0;
	while (rha.Next())
	{
		XSI::CLongArray vertices_count_array;
		rha.GetVerticesCount(vertices_count_array);
		ULONG strands_count = vertices_count_array.GetCount();
		for (i = 0; i < strands_count; i++)
		{
			ULONG n_count = vertices_count_array[i];
			num_keys += n_count;
		}
		num_curves += strands_count;
	}
	rha.Reset();

	//create Luxcore strands object
	luxrays::cyHairFile lux_hairs;
	lux_hairs.SetHairCount(num_curves);
	lux_hairs.SetPointCount(num_keys);

	lux_hairs.SetDefaultTransparency(0.0f);

	int flags = CY_HAIR_FILE_POINTS_BIT;
	flags |= CY_HAIR_FILE_SEGMENTS_BIT;
	flags |= CY_HAIR_FILE_THICKNESS_BIT;
	if (num_colors > 0)
	{
		flags |= CY_HAIR_FILE_COLORS_BIT;
	}
	else
	{
		lux_hairs.SetDefaultColor(1.0f, 1.0f, 1.0f);
	}
	if (num_uvs > 0)
	{
		flags |= CY_HAIR_FILE_UVS_BIT;
	}
	lux_hairs.SetArrays(flags);

	std::vector<float> points(num_keys * 3, 0.0f);
	std::vector<unsigned short> segments(num_curves, 0);
	std::vector<float> thickness(num_keys, 0.0f);
	std::vector<float> colors;
	if (num_colors > 0)
	{
		colors.resize(num_keys * 3);
	}
	std::vector<float> uvs;
	if (num_uvs > 0)
	{
		uvs.resize(num_keys * 2);
	}
	

	//Set hair data
	num_keys = 0;
	while (rha.Next())
	{
		XSI::CLongArray vertices_count_array;  // size of each hair strand
		rha.GetVerticesCount(vertices_count_array);
		LONG strands_count = vertices_count_array.GetCount();
		XSI::CFloatArray position_values;
		rha.GetVertexPositions(position_values);  // actual vertex positions for all hairs
		XSI::CFloatArray radius_values;
		rha.GetVertexRadiusValues(radius_values);  // point radius
		ULONG pos_k = 0;
		ULONG radius_k = 0;

		//iterate by strands
		for (i = 0; i < strands_count; i++)
		{
			ULONG n_count = vertices_count_array[i];  // get the number of points in the selected strand
			for (ULONG j = 0; j < n_count; j++)
			{
				points[pos_k] = position_values[pos_k];
				points[pos_k + 1] = position_values[pos_k + 1];
				points[pos_k + 2] = position_values[pos_k + 2];

				thickness[radius_k] = radius_values[radius_k];

				pos_k = pos_k + 3;
				radius_k = radius_k + 1;
			}

			segments[i] = n_count - 1;
		}

		//fill uv
		if (num_uvs > 0)
		{
			XSI::CFloatArray uv_values;
			rha.GetUVValues(0, uv_values);  // use only the first uvs
			ULONG uv_values_count = uv_values.GetCount();
			ULONG uv_index = 0;
			for (ULONG j = 0; j < uv_values_count; j += 3)
			{
				float u = uv_values[j];
				float v = uv_values[j + 1];
				//we should set the same values to all points in the strand
				ULONG n_count = vertices_count_array[j];
				for (ULONG k = 0; k < n_count; k++)
				{
					uvs[uv_index] = u;
					uvs[uv_index + 1] = v;
					uv_index = uv_index + 2;
				}
			}
		}

		//fill vertex color
		if (num_colors > 0)
		{
			XSI::CFloatArray color_values;
			rha.GetVertexColorValues(0, color_values);  // also use only the first vertex color property
			ULONG colors_count = color_values.GetCount();
			ULONG color_index = 0;
			for (ULONG j = 0; j < colors_count; j += 4)
			{
				float r = color_values[j];
				float g = color_values[j + 1];
				float b = color_values[j + 2];
				ULONG n_count = vertices_count_array[j];
				for (ULONG k = 0; k < n_count; k++)
				{
					colors[color_index] = r;
					colors[color_index + 1] = g;
					colors[color_index + 2] = b;
					color_index = color_index + 3;
				}
			}
		}
	}
	std::move(points.begin(), points.end(), lux_hairs.GetPointsArray());
	std::move(thickness.begin(), thickness.end(), lux_hairs.GetThicknessArray());
	std::move(segments.begin(), segments.end(), lux_hairs.GetSegmentsArray());
	if (num_colors > 0)
	{
		std::move(colors.begin(), colors.end(), lux_hairs.GetColorsArray());
	}
	if (num_uvs > 0)
	{
		std::move(uvs.begin(), uvs.end(), lux_hairs.GetUVsArray());
	}

	XSI::Primitive xsi_primitive = xsi_object.GetActivePrimitive(eval_time);
	std::string shape_name = xsi_object_id_string(xsi_primitive)[0];
	int service_strands_type = render_params.GetValue("service_strands_type", eval_time);
	bool service_strands_use_camera = render_params.GetValue("service_strands_use_camera", eval_time);
	int service_strands_sidecount = render_params.GetValue("service_strands_sidecount", eval_time);
	bool service_strands_cap_top = render_params.GetValue("service_strands_cap_top", eval_time);
	bool service_strands_cap_bottom = render_params.GetValue("service_strands_cap_bottom", eval_time);
	int service_strands_adaptive_maxdepth = render_params.GetValue("service_strands_adaptive_maxdepth", eval_time);
	float service_strands_adaptive_error = render_params.GetValue("service_strands_adaptive_error", eval_time);
	luxcore::Scene::StrandsTessellationType tessel_type = 
		service_strands_type == 0 ? luxcore::Scene::TESSEL_RIBBON :
		(service_strands_type == 1 ? luxcore::Scene::TESSEL_RIBBON_ADAPTIVE : 
		(service_strands_type == 2 ? luxcore::Scene::TESSEL_SOLID : luxcore::Scene::TESSEL_SOLID_ADAPTIVE));

	scene->DefineStrands(shape_name, lux_hairs, tessel_type, service_strands_adaptive_maxdepth, service_strands_adaptive_error, service_strands_sidecount, service_strands_cap_bottom, service_strands_cap_top, service_strands_use_camera);

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
	segments.clear();
	segments.shrink_to_fit();
	uvs.clear();
	uvs.shrink_to_fit();
	colors.clear();
	colors.shrink_to_fit();

	return true;
}