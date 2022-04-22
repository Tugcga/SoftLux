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
	XSI::CLongArray triangle_vertices;  // i1, i2, i3 for the first triangls, j1, j2, j3 for the second, ...
	xsi_acc.GetTriangleVertexIndices(triangle_vertices);
	XSI::CDoubleArray vertex_positions;  // x1, y1, z1, x2, y2, z2, ...
	xsi_acc.GetVertexPositions(vertex_positions);
	XSI::CFloatArray node_normals;  // normal coordinates for each node
	xsi_acc.GetNodeNormals(node_normals);
	XSI::CLongArray triangle_nodes;  // i1, i2, i3, for the first triangle, j1, j2, j3, for the second and so on
	xsi_acc.GetTriangleNodeIndices(triangle_nodes);

	//export the whole mesh, make split to submaterial clusters later
	Point* points = (Point*)luxcore::Scene::AllocVerticesBuffer(vertices_count);
	for (ULONG i = 0; i < vertices_count; i++)
	{
		//we sholdn't swap axis in vertex coordinates, all will be done in transform matrix
		points[i] = Point(vertex_positions[3 * i], vertex_positions[3 * i + 1], vertex_positions[3 * i + 2]);
	}

	//next triangles
	Triangle* triangles = (Triangle*)luxcore::Scene::AllocTrianglesBuffer(triangles_count);
	for (ULONG i = 0; i < triangles_count; i++)
	{
		triangles[i] = Triangle(triangle_vertices[3 * i], triangle_vertices[3 * i + 1], triangle_vertices[3 * i + 2]);
	}

	//reserve the mesh in the luxcore
	//the name of the mesh is it UniqueID
	std::string mesh_name = XSI::CString(xsi_primitive.GetObjectID()).GetAsciiString();
	std::string object_name = XSI::CString(xsi_object.GetObjectID()).GetAsciiString();
	scene->DefineMesh(mesh_name, vertices_count, triangles_count, (float*)points, (unsigned int*)triangles, NULL, NULL, NULL, NULL);

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

	//next we should split the mesh into clusters with different materials
	//export normals and colors
	//also we can export vertex and triangle aovs (can we use it from shaders?)
}