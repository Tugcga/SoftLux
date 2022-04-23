#include "../../utilities/logs.h"

#include "lux_scene.h"

#include "xsi_primitive.h"
#include "xsi_polygonmesh.h"
#include "xsi_geometryaccessor.h"
#include "xsi_kinematics.h"

class UV 
{
public:
	UV(float _u = 0.0f, float _v = 0.0f)
		: u(_u), v(_v) { }

	float u, v;
};

class Point {
public:
	Point(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
		: x(_x), y(_y), z(_z) { }

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

void sync_polymesh(luxcore::Scene* scene, XSI::X3DObject &xsi_object, const XSI::CTime& eval_time)
{
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
	XSI::PolygonMesh xsi_polygonmesh = xsi_primitive.GetGeometry(eval_time, XSI::siConstructionModeSecondaryShape);
	XSI::CGeometryAccessor xsi_acc = xsi_polygonmesh.GetGeometryAccessor(XSI::siConstructionModeSecondaryShape, XSI::siCatmullClark, subdivs, false, ga_use_angle, ga_angle);

	LONG vertices_count = xsi_acc.GetVertexCount();
	LONG triangles_count = xsi_acc.GetTriangleCount();
	XSI::CLongArray triangle_vertices;  // i1, i2, i3 for the first triangl, j1, j2, j3 for the second, ...
	xsi_acc.GetTriangleVertexIndices(triangle_vertices);
	XSI::CDoubleArray vertex_positions;  // x1, y1, z1, x2, y2, z2, ...
	xsi_acc.GetVertexPositions(vertex_positions);
	XSI::CFloatArray node_normals;  // normal coordinates for each node
	xsi_acc.GetNodeNormals(node_normals);
	XSI::CLongArray triangle_nodes;  // i1, i2, i3, for the first triangle, j1, j2, j3, for the second and so on
	xsi_acc.GetTriangleNodeIndices(triangle_nodes);

	//for simplicity we will export each triangle as separate triangle with three vertices and tree normals
	//yes, the vertex count will be greater than in original mesh, but luxcore store normals, uvs, colors and so on only per-vertex
	Point* points = (Point*)luxcore::Scene::AllocVerticesBuffer(triangles_count * 3);
	Triangle* triangles = (Triangle*)luxcore::Scene::AllocTrianglesBuffer(triangles_count);
	Normal* normals = new Normal[3 * triangles_count];
	//TODO: export uvs and colors
	for (ULONG i = 0; i < triangles_count; i++)
	{
		//for each triangle register vertex positions
		LONG v0 = triangle_vertices[3 * i];
		LONG v1 = triangle_vertices[3 * i + 1];
		LONG v2 = triangle_vertices[3 * i + 2];
		points[3 * i] = Point(vertex_positions[3*v0], vertex_positions[3*v0 + 1], vertex_positions[3*v0 + 2]);
		points[3 * i + 1] = Point(vertex_positions[3 * v1], vertex_positions[3 * v1 + 1], vertex_positions[3 * v1 + 2]);
		points[3 * i + 2] = Point(vertex_positions[3 * v2], vertex_positions[3 * v2 + 1], vertex_positions[3 * v2 + 2]);

		//next normals
		LONG n0 = triangle_nodes[3 * i];
		LONG n1 = triangle_nodes[3 * i + 1];
		LONG n2 = triangle_nodes[3 * i + 2];
		normals[3 * i] = Normal(node_normals[3 * n0], node_normals[3 * n0 + 1], node_normals[3 * n0 + 2]);
		normals[3 * i + 1] = Normal(node_normals[3 * n1], node_normals[3 * n1 + 1], node_normals[3 * n1 + 2]);
		normals[3 * i + 2] = Normal(node_normals[3 * n2], node_normals[3 * n2 + 1], node_normals[3 * n2 + 2]);

		//finally tringle
		triangles[i] = Triangle(3*i, 3*i + 1, 3*i + 2);
	}

	//reserve the mesh in the luxcore
	//the name of the mesh is it UniqueID
	std::string mesh_name = XSI::CString(xsi_primitive.GetObjectID()).GetAsciiString();
	std::string object_name = XSI::CString(xsi_object.GetObjectID()).GetAsciiString();
	scene->DefineMesh(mesh_name, triangles_count * 3, triangles_count, (float*)points, (unsigned int*)triangles, (float*)normals, NULL, NULL, NULL);

	//add mesh to the scene
	luxrays::Properties polymesh_props;
	polymesh_props.Set(luxrays::Property("scene.objects." + object_name + ".shape")(mesh_name));
	polymesh_props.Set(luxrays::Property("scene.objects." + object_name + ".material")("default_material"));
	polymesh_props.Set(luxrays::Property("scene.objects." + object_name + ".id")(static_cast<unsigned int>(xsi_object.GetObjectID())));
	polymesh_props.Set(luxrays::Property("scene.objects." + object_name + ".camerainvisible")(false));

	//set transform
	XSI::MATH::CMatrix4 xsi_matrix = xsi_object.GetKinematics().GetGlobal().GetTransform().GetMatrix4();
	std::vector<double> lux_matrix = xsi_to_lux_matrix(xsi_matrix);
	polymesh_props.Set(luxrays::Property("scene.objects." + object_name + ".transformation")(lux_matrix));

	scene->Parse(polymesh_props);

	//TODO: split the mesh into clusters with different materials
	//export normals and colors
	//also we can export vertex and triangle aovs (can we use it from shaders?)
}