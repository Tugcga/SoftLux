#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"
#include "../../utilities/arrays.h"

#include "lux_scene.h"

#include "xsi_primitive.h"
#include "xsi_polygonmesh.h"
#include "xsi_geometryaccessor.h"
#include "xsi_kinematics.h"
#include "xsi_vertex.h"
#include "xsi_polygonnode.h"
#include "xsi_polygonface.h"

#include <unordered_map>

class UV 
{
public:
	UV(float _u = 0.0f, float _v = 0.0f)
		: u(_u), v(_v) { }

	XSI::CString to_string()
	{
		return "<" + XSI::CString(u) + ", " + XSI::CString(v) + ">";
	}

	float u, v;
};

class Color
{
public:
	Color(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f)
		: r(_r), g(_g), b(_b) { }

	XSI::CString to_string()
	{
		return "{" + XSI::CString(r) + ", " + XSI::CString(g) + ", " + XSI::CString(b) + "}";
	}

	float r, g, b;
};

class Alpha
{
public:
	Alpha(float _a = 0.0f)
		: a(_a) { }

	XSI::CString to_string()
	{
		return "(" + XSI::CString(a) + ")";
	}

	float a;
};

class Point {
public:
	Point(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
		: x(_x), y(_y), z(_z) { }

	XSI::CString to_string()
	{
		return "(" + XSI::CString(x) + ", " + XSI::CString(y) + ", " + XSI::CString(z) + ")";
	}

	float x, y, z;
};

class Triangle {
public:
	Triangle(const unsigned int v0, const unsigned int v1, const unsigned int v2) 
	{
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
	}

	XSI::CString to_string()
	{
		return "[" + XSI::CString((int)v[0]) + ", " + XSI::CString((int)v[1]) + ", " + XSI::CString((int)v[2]) + "]";
	}

	unsigned int v[3];
};

class Normal
{
public:
	Normal(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
		: x(_x), y(_y), z(_z) { }

	XSI::CString to_string()
	{
		return "(" + XSI::CString(x) + ", " + XSI::CString(y) + ", " + XSI::CString(z) + ")";
	}

	float x, y, z;
};

void define_area_light_mesh(luxcore::Scene* scene, const std::string &shape_name)
{
	Point* points = (Point*)luxcore::Scene::AllocVerticesBuffer(4);
	Triangle* triangles = (Triangle*)luxcore::Scene::AllocTrianglesBuffer(2);
	UV* uvs = new UV[4];
	points[0] = Point(-1.0, -1.0, 0.0);
	points[1] = Point(1.0, -1.0, 0.0);
	points[2] = Point(1.0, 1.0, 0.0);
	points[3] = Point(-1.0, 1.0, 0.0);
	triangles[0] = Triangle(2, 1, 0);
	triangles[1] = Triangle(2, 0, 3);
	uvs[0] = UV(0.0, 0.0);
	uvs[1] = UV(1.0, 0.0);
	uvs[2] = UV(1.0, 1.0);
	uvs[3] = UV(0.0, 1.0);

	scene->DefineMesh(shape_name, 4, 2, (float*)points, (unsigned int*)triangles, NULL, (float*)uvs, NULL, NULL);
}

//use override_material for shaderball main object (this is id of the material)
//use use_default_material for background of the shaderball
bool sync_polymesh(luxcore::Scene* scene, 
	XSI::X3DObject& xsi_object, 
	std::set<ULONG>& xsi_materials_in_lux, 
	std::unordered_map<ULONG, std::set<ULONG>> &material_with_shape_to_polymesh_map,
	const XSI::CTime& eval_time, 
	const ULONG override_material, 
	const bool use_default_material)
{
	//the main principle: each node is a separate vertex in the Luxcore mesh
	//get polygonmesh geometry properties
	int subdivs = 0;
	float ga_angle = 60.0;
	bool ga_use_angle = true;
	//we should get these parameters from geometry approximation property
	XSI::Property ga_property;
	xsi_object.GetPropertyFromName("geomapprox", ga_property);
	if (ga_property.IsValid())
	{
		subdivs = ga_property.GetParameterValue("gapproxmordrsl", eval_time);
		ga_angle = ga_property.GetParameterValue("gapproxmoan", eval_time);
		ga_use_angle = ga_property.GetParameterValue("gapproxmoad", eval_time);
	}

	XSI::Primitive xsi_primitive = xsi_object.GetActivePrimitive(eval_time);
	XSI::PolygonMesh mesh_polygonMesh = xsi_primitive.GetGeometry(eval_time, XSI::siConstructionModeSecondaryShape);
	XSI::CGeometryAccessor xsi_acc = mesh_polygonMesh.GetGeometryAccessor(XSI::siConstructionModeSecondaryShape, XSI::siCatmullClark, subdivs, false, ga_use_angle, ga_angle);
	XSI::CLongArray triangle_nodes;  // array of node indeces of the triangle vertices (i1, i2, i3, j1, j2, j3, ...)
	xsi_acc.GetTriangleNodeIndices(triangle_nodes);
	ULONG triangles_count = triangle_nodes.GetCount() / 3;  // the number of triangles in the mesh
	ULONG vertex_count = xsi_acc.GetVertexCount();
	XSI::CDoubleArray vertex_positions;
	xsi_acc.GetVertexPositions(vertex_positions);
	XSI::CFloatArray node_normals;  // normal coordinates for each node
	xsi_acc.GetNodeNormals(node_normals);
	ULONG nodes_count = xsi_acc.GetNodeCount();
	ULONG polygons_count = xsi_acc.GetPolygonCount();
	XSI::CLongArray polygon_sizes;
	xsi_acc.GetPolygonVerticesCount(polygon_sizes);  //array contains sizes of all polygons

	XSI::CRefArray uv_refs = xsi_acc.GetUVs();
	ULONG uv_count = uv_refs.GetCount();
	XSI::CFloatArray uv_values;
	XSI::ClusterProperty uv_prop;

	//at first we should generate node_to_vertex map, and then we can use it to find node positions
	std::vector<Point> points(nodes_count);
	std::vector<ULONG> node_to_vertex(nodes_count);

	//to find the map we use polygon indices (for vertices and nodes)
	XSI::CLongArray vertex_indices;
	xsi_acc.GetVertexIndices(vertex_indices);
	XSI::CLongArray node_indices;
	xsi_acc.GetNodeIndices(node_indices);
	for (ULONG i = 0; i < vertex_indices.GetCount(); i++)
	{
		node_to_vertex[node_indices[i]] = vertex_indices[i];
	}

	//now we can find node positions
	for (ULONG i = 0; i < nodes_count; i++)
	{
		ULONG vertex_index = node_to_vertex[i];
		points[i] = Point(vertex_positions[3* vertex_index], vertex_positions[3 * vertex_index + 1], vertex_positions[3 * vertex_index + 2]);
	}

	//reserve the array of uv-values
	std::vector<float> xsi_uvs(uv_count * nodes_count * 2);  // each node has two coordinates
	//write uv values
	for (ULONG i = 0; i < uv_count; i++)
	{
		uv_prop = XSI::ClusterProperty(uv_refs[i]);
		uv_prop.GetValues(uv_values);  // uv_values has 3 values per node
		//copy values
		for (ULONG j = 0; j < nodes_count; j++)
		{
			xsi_uvs[2 * nodes_count * i + 2 * j] = uv_values[3 * j];
			xsi_uvs[2 * nodes_count * i + 2 * j + 1] = uv_values[3 * j + 1];
		}
	}

	//read vertex colors
	XSI::CRefArray vertex_colors_array = xsi_acc.GetVertexColors();
	ULONG vertex_colors_array_count = vertex_colors_array.GetCount();
	//if the mesh contains colors, then write it from the index = 1
	//and use index = 0 for generated data
	//if there are no colors in the mesh, then does not create empty color channel
	ULONG shift_vc_index = vertex_colors_array_count > 0 ? 1 : 0;
	//we will fill it by the procedural data (pointenss and harlequin)
	//reserve array
	std::vector<float> xsi_colors((vertex_colors_array_count + shift_vc_index) * nodes_count * 3);  // save 3 color channels per node
	std::vector<float> xsi_alphas((vertex_colors_array_count + shift_vc_index) * nodes_count);
	//fill the first channels by zeros
	for (ULONG vc_index = 0; vc_index < shift_vc_index; vc_index++)
	{
		for (ULONG node_index = 0; node_index < nodes_count; node_index++)
		{
			xsi_colors[3 * nodes_count * vc_index + 3 * node_index] = 0.0f;
			xsi_colors[3 * nodes_count * vc_index + 3 * node_index + 1] = 0.0f;
			xsi_colors[3 * nodes_count * vc_index + 3 * node_index + 2] = 0.0f;
			xsi_alphas[nodes_count * vc_index + node_index] = 0.0f;
		}
	}
	
	//copy colors to other channels
	for (ULONG vc_index = 0; vc_index < vertex_colors_array_count; vc_index++)
	{
		XSI::ClusterProperty vertex_color_prop(vertex_colors_array[vc_index]);
		XSI::CFloatArray values;
		vertex_color_prop.GetValues(values);
		for (ULONG node_index = 0; node_index < nodes_count; node_index++)
		{
			xsi_colors[3 * nodes_count * (vc_index + shift_vc_index) + 3 * node_index] = values[4 * node_index];
			xsi_colors[3 * nodes_count * (vc_index + shift_vc_index) + 3 * node_index + 1] = values[4 * node_index + 1];
			xsi_colors[3 * nodes_count * (vc_index + shift_vc_index) + 3 * node_index + 2] = values[4 * node_index + 2];
			xsi_alphas[nodes_count * (vc_index + shift_vc_index) + node_index] = values[4 * node_index + 3];
		}
	}
		
	XSI::CLongArray polygon_material_indices;
	xsi_acc.GetPolygonMaterialIndices(polygon_material_indices);
	XSI::CRefArray xsi_materials = xsi_object.GetMaterials();
	std::vector<std::string> xsi_material_names;
	std::vector<XSI::Material> xsi_material_materials;
	std::map<ULONG, ULONG> material_index_to_submesh;  // key - index in the material list, value - index in names list (this list is shorter)
	for (ULONG i = 0; i < xsi_materials.GetCount(); i++)
	{
		XSI::Material m(xsi_materials[i]);
		std::string new_name = xsi_object_id_string(m)[0];
		int index = get_index_in_array(xsi_material_names, new_name);
		if (index == -1)
		{
			xsi_material_names.push_back(new_name);
			xsi_material_materials.push_back(m);
			material_index_to_submesh[i] = xsi_material_names.size() - 1;
		}
		else
		{
			material_index_to_submesh[i] = index;
		}

		//check created materials only if we does not override material of the mesh
		if (override_material == 0 && !use_default_material)
		{
			if (!scene->IsMaterialDefined(new_name))
			{
				sync_material(scene, m, xsi_materials_in_lux, eval_time);
			}
		}
	}

	//the size of xsi_material_names array define how many meshes we should export as separate objects
	//because in Luxcore each mesh can use only one material
	//so, clusters with different materials are different objects
	ULONG submeshes_count = xsi_material_names.size();
	//for each submesh we should get the number of vertices (in fact nodes)  and triangles
	std::vector<ULONG> submesh_vertices_count(submeshes_count, 0);
	std::vector<ULONG> submesh_triangles_count(submeshes_count, 0);
	//enumerate polygons
	for (ULONG i = 0; i < polygons_count; i++)
	{
		//get polygon material index
		LONG m_index = material_index_to_submesh[polygon_material_indices[i]];
		//add data to the corresponding submesh
		submesh_vertices_count[m_index] += polygon_sizes[i];
		submesh_triangles_count[m_index] += polygon_sizes[i] - 2;
	}

	//now we can reserve Luxcore meshes
	std::vector<Point*> submesh_points(submeshes_count);
	std::vector<Triangle*> submesh_triangles(submeshes_count);
	std::vector<Normal*> submesh_normals(submeshes_count);
	std::vector<std::vector<UV*>> submesh_uvs(submeshes_count);
	std::vector<std::vector<Color*>> submesh_colors(submeshes_count);
	std::vector<std::vector<Alpha*>> submesh_alphas(submeshes_count);
	for (ULONG i = 0; i < submeshes_count; i++)
	{
		submesh_points[i] = (Point*)luxcore::Scene::AllocVerticesBuffer(submesh_vertices_count[i]);
		submesh_triangles[i] = (Triangle*)luxcore::Scene::AllocTrianglesBuffer(submesh_triangles_count[i]);
		submesh_normals[i] = new Normal[submesh_vertices_count[i]];

		submesh_uvs[i] = std::vector<UV*>(uv_count);
		for (ULONG j = 0; j < uv_count; j++)
		{
			submesh_uvs[i][j] = new UV[submesh_vertices_count[i]];
		}

		submesh_colors[i] = std::vector<Color*>(vertex_colors_array_count + shift_vc_index);
		submesh_alphas[i] = std::vector<Alpha*>(vertex_colors_array_count + shift_vc_index);
		for (ULONG j = 0; j < vertex_colors_array_count + shift_vc_index; j++)
		{
			submesh_colors[i][j] = new Color[submesh_vertices_count[i]];
			submesh_alphas[i][j] = new Alpha[submesh_vertices_count[i]];
		}
	}
	//fill the data
	std::vector<ULONG> submesh_point_index(submeshes_count, 0);  // store here actual submesh maximal point index
	std::vector<ULONG> submesh_triangle_index(submeshes_count, 0);
	ULONG node_index = 0;
	for (ULONG i = 0; i < polygons_count; i++)
	{
		ULONG submesh_index = material_index_to_submesh[polygon_material_indices[i]];
		ULONG polygon_nodes_count = polygon_sizes[i];

		for (ULONG j = 0; j < polygon_nodes_count; j++)
		{
			ULONG n = node_indices[node_index];
			node_index++;

			ULONG point_pointer = submesh_point_index[submesh_index] + j;
			//set positions
			submesh_points[submesh_index][point_pointer] = points[n];
			//set normal
			submesh_normals[submesh_index][point_pointer] = Normal(node_normals[3 * n], node_normals[3 * n + 1], node_normals[3 * n + 2]);

			//set uvs
			for (ULONG k = 0; k < uv_count; k++)
			{
				//we should set k-th uv coordinates of the node with index n
				submesh_uvs[submesh_index][k][point_pointer] = UV(xsi_uvs[2 * nodes_count * k + 2 * n], xsi_uvs[2 * nodes_count * k + 2 * n + 1]);
			}

			//set colors
			for (ULONG k = 0; k < vertex_colors_array_count + shift_vc_index; k++)
			{
				submesh_colors[submesh_index][k][point_pointer] = Color(xsi_colors[3 * nodes_count * k + 3 * n], xsi_colors[3 * nodes_count * k + 3 * n + 1], xsi_colors[3 * nodes_count * k + 3 * n + 2]);
				submesh_alphas[submesh_index][k][point_pointer] = Alpha(xsi_alphas[nodes_count * k + n]);
			}
		}
		//use manual truiangulation of the polygon
		//this works ok if all polygons are convex
		//for concave polygons it can create wrong triangulation
		//but it is to slow to find proper triangulation of each polygon
		//GeometryAccessor does not allow to get triangulation of the selected polygon
		//in fact we need local indices of nodes, which form triangulation of the selected polygon
		ULONG v = submesh_point_index[submesh_index];  // get luxcore point index, created in the sumbesh
		for (ULONG t = 0; t < polygon_sizes[i] - 2; t++)
		{
			submesh_triangles[submesh_index][submesh_triangle_index[submesh_index] + t] = Triangle(v, v + t + 1, v + t + 2);
		}

		submesh_point_index[submesh_index] += polygon_nodes_count;  // increase the number of luxcore points in the submesh
		submesh_triangle_index[submesh_index] += polygon_sizes[i] - 2;
	}

	//define Luxcore meshes
	std::string mesh_name = xsi_object_id_string(xsi_primitive)[0];
	std::vector<std::string> object_names = xsi_object_id_string(xsi_object);
	for (ULONG i = 0; i < submeshes_count; i++)
	{
		std::string submesh_name = mesh_name + "_" + xsi_material_names[i];
		std::string subobject_name = object_names[i];
		
		std::array<float*, LC_MESH_MAX_DATA_COUNT> uvs;
		fill(uvs.begin(), uvs.end(), nullptr);
		for (ULONG j = 0; j < std::min(uv_count, (ULONG)LC_MESH_MAX_DATA_COUNT); j++)
		{
			uvs[j] = (float*)&submesh_uvs[i][j][0];
		}
		std::array<float*, LC_MESH_MAX_DATA_COUNT> colors;
		std::array<float*, LC_MESH_MAX_DATA_COUNT> alphas;
		fill(colors.begin(), colors.end(), nullptr);
		fill(alphas.begin(), alphas.end(), nullptr);
		for (ULONG j = 0; j < std::min(vertex_colors_array_count + shift_vc_index, (ULONG)LC_MESH_MAX_DATA_COUNT); j++)
		{
			colors[j] = (float*)&submesh_colors[i][j][0];
			alphas[j] = (float*)&submesh_alphas[i][j][0];
		}
		scene->DefineMeshExt(submesh_name, submesh_vertices_count[i], submesh_triangles_count[i], 
			(float*)submesh_points[i], 
			(unsigned int*)submesh_triangles[i], 
			(float*)submesh_normals[i],
			uv_count > 0 ? &uvs : NULL,
			(vertex_colors_array_count + shift_vc_index) > 0 ? &colors : NULL,
			(vertex_colors_array_count + shift_vc_index) > 0 ? &alphas : NULL);

		//form shape modificators
		std::string shape_name = submesh_name;
		if (!use_default_material && override_material == 0)
		{
			//may be this is a bug of the LuxCore, but if the shape contains vertex color, then it crashes in the subdivision process
			//so, to fix this, ignore subdivision if there ara colors in the mesh
			//vertex_colors_array_count > 0 is true if there are colors
			//in this case say exporter to ignore subdivision
			//but if the mesh contains harequin shape, then it generate color to the 0-th index correctly
			//so, the problem with user-defined colors
			shape_name = sync_polymesh_shapes(scene, submesh_name, xsi_material_materials[i], vertex_colors_array_count > 0, eval_time);
			if (submesh_name != shape_name)
			{
				//in this case we should remember, that after changing material we should also change the mesh
				ULONG material_id = xsi_material_materials[i].GetObjectID();
				if (!material_with_shape_to_polymesh_map.contains(material_id))
				{
					material_with_shape_to_polymesh_map[material_id] = std::set<ULONG>();
				}
				material_with_shape_to_polymesh_map[material_id].insert(xsi_object.GetObjectID());
			}

			//if we select this in the render settings, then add default shapes
			//add pointness
			std::string p_shape_name = shape_name + "_pointness";
			std::string p_prefix = "scene.shapes." + p_shape_name;
			luxrays::Properties p_shape_props;
			p_shape_props.Set(luxrays::Property(p_prefix + ".type")("pointiness"));
			p_shape_props.Set(luxrays::Property(p_prefix + ".source")(shape_name));
			scene->Parse(p_shape_props);

			//also add islandaov
			std::string island_shape_name = p_shape_name + "_island_aov";
			std::string island_prefix = "scene.shapes." + island_shape_name;
			luxrays::Properties island_shape_props;
			island_shape_props.Set(luxrays::Property(island_prefix + ".type")("islandaov"));
			island_shape_props.Set(luxrays::Property(island_prefix + ".source")(p_shape_name));
			island_shape_props.Set(luxrays::Property(island_prefix + ".dataindex")(0));
			scene->Parse(island_shape_props);

			//and randomtriangleaov
			std::string random_shape_name = island_shape_name + "_random_tri_aov_shape";
			std::string random_prefix = "scene.shapes." + random_shape_name;
			luxrays::Properties random_shape_props;
			random_shape_props.Set(luxrays::Property(random_prefix + ".type")("randomtriangleaov"));
			random_shape_props.Set(luxrays::Property(random_prefix + ".source")(island_shape_name));
			random_shape_props.Set(luxrays::Property(random_prefix + ".srcdataindex")(0));
			random_shape_props.Set(luxrays::Property(random_prefix + ".dstdataindex")(1));
			scene->Parse(random_shape_props);

			shape_name = random_shape_name;
		}

		//add mesh to the scene
		luxrays::Properties submobject_props;
		submobject_props.Set(luxrays::Property("scene.objects." + subobject_name + ".shape")(shape_name));
		//get mesh material
		std::string material_name = use_default_material ? "default_material" : (override_material == 0 ? xsi_material_names[i] : std::string(XSI::CString(override_material).GetAsciiString()));
		submobject_props.Set(luxrays::Property("scene.objects." + subobject_name + ".material")(material_name));
		submobject_props.Set(luxrays::Property("scene.objects." + subobject_name + ".id")(static_cast<unsigned int>(xsi_object.GetObjectID())));
		submobject_props.Set(luxrays::Property("scene.objects." + subobject_name + ".camerainvisible")(false));

		//set transform
		XSI::MATH::CMatrix4 xsi_matrix = xsi_object.GetKinematics().GetGlobal().GetTransform().GetMatrix4();
		std::vector<double> lux_matrix = xsi_to_lux_matrix(xsi_matrix);
		submobject_props.Set(luxrays::Property("scene.objects." + subobject_name + ".transformation")(lux_matrix));

		scene->Parse(submobject_props);
	}

	//clear temporary arrays
	points.clear();
	points.shrink_to_fit();

	submesh_points.clear();
	submesh_triangles.clear();
	submesh_normals.clear();

	node_to_vertex.clear();
	node_to_vertex.shrink_to_fit();

	xsi_uvs.clear();
	xsi_uvs.shrink_to_fit();

	xsi_colors.clear();
	xsi_colors.shrink_to_fit();

	xsi_alphas.clear();
	xsi_alphas.shrink_to_fit();

	xsi_material_names.clear();
	xsi_material_names.shrink_to_fit();

	xsi_material_materials.clear();
	xsi_material_materials.shrink_to_fit();

	submesh_vertices_count.clear();
	submesh_vertices_count.shrink_to_fit();

	submesh_triangles_count.clear();
	submesh_triangles_count.shrink_to_fit();

	submesh_point_index.clear();
	submesh_point_index.shrink_to_fit();

	submesh_triangle_index.clear();
	submesh_triangle_index.shrink_to_fit();

	object_names.clear();
	object_names.shrink_to_fit();

	submesh_uvs.clear();
	submesh_uvs.shrink_to_fit();

	submesh_colors.clear();
	submesh_colors.shrink_to_fit();

	submesh_alphas.clear();
	submesh_alphas.shrink_to_fit();

	return true;
}