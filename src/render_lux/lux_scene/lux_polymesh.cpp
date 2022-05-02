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
bool sync_polymesh(luxcore::Scene* scene, XSI::X3DObject& xsi_object, std::set<ULONG>& xsi_materials_in_lux, const XSI::CTime& eval_time, const ULONG override_material, const bool use_default_material)
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
	XSI::CVertexRefArray vertices = mesh_polygonMesh.GetVertices();
	XSI::CPolygonNodeRefArray nodes = mesh_polygonMesh.GetNodes();
	XSI::CPolygonFaceRefArray polygons = mesh_polygonMesh.GetPolygons();
	XSI::CFloatArray node_normals;  // normal coordinates for each node
	xsi_acc.GetNodeNormals(node_normals);
	ULONG nodes_count = nodes.GetCount();  // the number of nodes in the mesh
	ULONG polygons_count = polygons.GetCount();
	XSI::CLongArray polygon_sizes;
	xsi_acc.GetPolygonVerticesCount(polygon_sizes);  //array contains sizes of all polygons

	XSI::CRefArray uv_refs = xsi_acc.GetUVs();
	ULONG uv_count = uv_refs.GetCount();
	XSI::CFloatArray uv_values;
	XSI::ClusterProperty uv_prop;

	//get node positions and form node_to_vertex map
	std::vector<Point> points(nodes_count);
	std::vector<ULONG> node_to_vertex(nodes_count);
	for (ULONG i = 0; i < vertex_count; i++)
	{
		XSI::Vertex vertex = vertices[i];
		XSI::MATH::CVector3 vertex_position = vertex.GetPosition();
		XSI::CPolygonNodeRefArray vertex_nodes = vertex.GetNodes();
		ULONG vertex_nodes_count = vertex_nodes.GetCount();
		for (ULONG node_index = 0; node_index < vertex_nodes_count; node_index++)
		{
			XSI::PolygonNode node(vertex_nodes[node_index]);
			points[node.GetIndex()] = Point(vertex_position.GetX(), vertex_position.GetY(), vertex_position.GetZ());
			node_to_vertex[node.GetIndex()] = i;
		}
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
	//reserve array
	std::vector<float> xsi_colors(vertex_colors_array_count * nodes_count * 3);  // save 3 color channels per node
	std::vector<float> xsi_alphas(vertex_colors_array_count * nodes_count);
	for (ULONG vc_index = 0; vc_index < vertex_colors_array_count; vc_index++)
	{
		XSI::ClusterProperty vertex_color_prop(vertex_colors_array[vc_index]);
		XSI::CFloatArray values;
		vertex_color_prop.GetValues(values);

		for (ULONG face_index = 0; face_index < polygons_count; face_index++)
		{
			XSI::PolygonFace face(polygons[face_index]);
			XSI::CPolygonNodeRefArray face_nodes = face.GetNodes();
			ULONG face_nodes_count = face_nodes.GetCount();
			for (ULONG node_index = 0; node_index < face_nodes_count; node_index++)
			{
				XSI::PolygonNode node(face_nodes[node_index]);
				ULONG n = node.GetIndex();
				xsi_colors[3 * nodes_count * vc_index + 3 * n] = values[4 * n];
				xsi_colors[3 * nodes_count * vc_index + 3 * n + 1] = values[4 * n + 1];
				xsi_colors[3 * nodes_count * vc_index + 3 * n + 2] = values[4 * n + 2];
				xsi_alphas[nodes_count * vc_index + n] = values[4 * n + 3];
			}
		}
	}

	XSI::CLongArray polygon_material_indices;
	xsi_acc.GetPolygonMaterialIndices(polygon_material_indices);
	XSI::CRefArray xsi_materials = xsi_object.GetMaterials();
	std::vector<std::string> xsi_material_names;
	std::map<ULONG, ULONG> material_index_to_submesh;  // key - index in the material list, value - index in names list (this list is shorter)
	for (ULONG i = 0; i < xsi_materials.GetCount(); i++)
	{
		XSI::Material m(xsi_materials[i]);
		std::string new_name = xsi_object_id_string(m)[0];
		int index = get_index_in_array(xsi_material_names, new_name);
		if (index == -1)
		{
			xsi_material_names.push_back(new_name);
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

		submesh_colors[i] = std::vector<Color*>(vertex_colors_array_count);
		submesh_alphas[i] = std::vector<Alpha*>(vertex_colors_array_count);
		for (ULONG j = 0; j < vertex_colors_array_count; j++)
		{
			submesh_colors[i][j] = new Color[submesh_vertices_count[i]];
			submesh_alphas[i][j] = new Alpha[submesh_vertices_count[i]];
		}
	}

	//fill the data
	std::vector<ULONG> submesh_point_index(submeshes_count, 0);  // store here actual submesh maximal point index
	std::vector<ULONG> submesh_triangle_index(submeshes_count, 0);
	for (ULONG i = 0; i < polygons_count; i++)
	{
		ULONG submesh_index = material_index_to_submesh[polygon_material_indices[i]];
		XSI::PolygonFace polygon(polygons[i]);
		XSI::CPolygonNodeRefArray polygon_nodes = polygon.GetNodes();
		ULONG polygon_nodes_count = polygon_nodes.GetCount();

		for (ULONG j = 0; j < polygon_nodes_count; j++)
		{
			XSI::PolygonNode node(polygon_nodes[j]);
			ULONG n = node.GetIndex();

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
			for (ULONG k = 0; k < vertex_colors_array_count; k++)
			{
				submesh_colors[submesh_index][k][point_pointer] = Color(xsi_colors[3 * nodes_count * k + 3 * n], xsi_colors[3 * nodes_count * k + 3 * n + 1], xsi_colors[3 * nodes_count * k + 3 * n + 2]);
				submesh_alphas[submesh_index][k][point_pointer] = Alpha(xsi_alphas[nodes_count * k + n]);
			}
		}
		XSI::CLongArray polygon_trianglulation = polygon.GetTriangleSubIndexArray();
		ULONG v = submesh_point_index[submesh_index];
		ULONG polygon_triangles_count = polygon_trianglulation.GetCount() / 3;
		for (ULONG t = 0; t < polygon_triangles_count; t++)
		{
			submesh_triangles[submesh_index][submesh_triangle_index[submesh_index] + t] = Triangle(v + polygon_trianglulation[3*t], v + polygon_trianglulation[3 * t + 1], v + polygon_trianglulation[3 * t + 2]);
		}
		submesh_point_index[submesh_index] += polygon_nodes_count;
		submesh_triangle_index[submesh_index] += polygon_triangles_count;
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
		for (ULONG j = 0; j < std::min(vertex_colors_array_count, (ULONG)LC_MESH_MAX_DATA_COUNT); j++)
		{
			colors[j] = (float*)&submesh_colors[i][j][0];
			alphas[j] = (float*)&submesh_alphas[i][j][0];
		}
		scene->DefineMeshExt(submesh_name, submesh_vertices_count[i], submesh_triangles_count[i], 
			(float*)submesh_points[i], 
			(unsigned int*)submesh_triangles[i], 
			(float*)submesh_normals[i],
			uv_count > 0 ? &uvs : NULL,
			vertex_colors_array_count > 0 ? &colors : NULL,
			vertex_colors_array_count > 0 ? &alphas : NULL);


		//add mesh to the scene
		luxrays::Properties submobject_props;
		submobject_props.Set(luxrays::Property("scene.objects." + subobject_name + ".shape")(submesh_name));
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