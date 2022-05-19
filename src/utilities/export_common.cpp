#include "export_common.h"
#include "arrays.h"
#include "logs.h"

#include <xsi_x3dobject.h>
#include <xsi_time.h>
#include <xsi_property.h>
#include <xsi_application.h>
#include <xsi_utils.h>
#include <xsi_project.h>
#include <xsi_material.h>
#include <xsi_model.h>
#include <xsi_geometry.h>
#include <xsi_primitive.h>
#include <xsi_iceattribute.h>
#include <xsi_iceattributedataarray2D.h>

#include <filesystem>
#include <fstream>
#include <windows.h>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <limits>

bool is_xsi_object_visible(const XSI::CTime &eval_time, XSI::X3DObject &xsi_object)
{
	XSI::Property visibility_prop;
	xsi_object.GetPropertyFromName("visibility", visibility_prop);
	if (visibility_prop.IsValid())
	{
		return visibility_prop.GetParameterValue("rendvis", eval_time);
	}
	return false;
}

bool is_pointcloud_strands(XSI::X3DObject &xsi_object, const XSI::CTime &eval_time)
{
	XSI::Geometry geometry = xsi_object.GetActivePrimitive(eval_time).GetGeometry(eval_time);
	XSI::ICEAttribute sp_attr = geometry.GetICEAttributeFromName("StrandPosition");
	XSI::ICEAttribute pp_attr = geometry.GetICEAttributeFromName("PointPosition");
	if (sp_attr.IsDefined() && sp_attr.GetElementCount() > 0 && pp_attr.GetElementCount() > 0)
	{
		return true;
	}
	return false;
}

std::vector<std::string> xsi_object_id_string(XSI::ProjectItem& xsi_item)
{
	if (xsi_item.GetClassID() == XSI::siX3DObjectID)
	{
		XSI::X3DObject xsi_object(xsi_item);

		if (xsi_item.GetType() == "polymsh")
		{
			XSI::CRefArray xsi_materials = xsi_object.GetMaterials();  // this list can contains the same material several times
			std::string object_id = XSI::CString(xsi_object.GetObjectID()).GetAsciiString();

			std::vector<std::string> to_return;
			for (ULONG i = 0; i < xsi_materials.GetCount(); i++)
			{
				XSI::Material m(xsi_materials[i]);
				std::string m_name = object_id + "_" + XSI::CString(m.GetObjectID()).GetAsciiString();
				if (!is_contains(to_return, m_name))
				{
					to_return.push_back(m_name);
				}
			}
			return to_return;
		}
		else
		{
			return std::vector<std::string> {XSI::CString(xsi_item.GetObjectID()).GetAsciiString()};
		}
	}
	else if (xsi_item.GetClassID() == XSI::siModelID)
	{
		XSI::Model xsi_model(xsi_item);
		XSI::siModelKind model_kind = xsi_model.GetModelKind();
		if (model_kind == XSI::siModelKind_Instance)
		{
			//for the instance return array of the form modelID_subobjectName
			ULONG xsi_id = xsi_model.GetObjectID();
			XSI::Model master = xsi_model.GetInstanceMaster();
			XSI::CStringArray str_families_subobject;
			XSI::CRefArray children = master.FindChildren2("", "", str_families_subobject);
			std::vector<std::string> to_return;
			for (ULONG i = 0; i < children.GetCount(); i++)
			{
				XSI::X3DObject object(children[i]);
				XSI::CString object_type = object.GetType();
				//TODO: add another types when exporter will supports it
				if (object_type == "polymsh" || object_type == "hair")
				{
					std::vector<std::string> names = xsi_object_id_string(object);
					for (ULONG j = 0; j < names.size(); j++)
					{
						to_return.push_back(std::to_string(xsi_id) + "_" + names[j]);
					}
				}
			}

			return to_return;
		}
		else
		{
			//for non-instance model return object id
			return std::vector<std::string> {XSI::CString(xsi_model.GetObjectID()).GetAsciiString()};
		}
	}
	else
	{
		return std::vector<std::string> {XSI::CString(xsi_item.GetObjectID()).GetAsciiString()};
	}
}

XSI::CString resolve_path(const XSI::CString &input_path)
{
	if (input_path.Length() > 0 && !XSI::CUtils::IsAbsolutePath(input_path))
	{
		XSI::CString project_path = XSI::Application().GetActiveProject().GetPath();
		XSI::CString to_return = XSI::CUtils::ResolvePath(XSI::CUtils::BuildPath(project_path, input_path));
		if (std::filesystem::exists(to_return.GetAsciiString()))
		{
			return to_return;
		}
		else
		{
			return "";
		}
	}


	if (std::filesystem::exists(input_path.GetAsciiString()))
	{
		return input_path;
	}
	else
	{
		return "";
	}
}

bool create_dir(const std::string& file_path)
{
	const size_t last_slash = file_path.find_last_of("/\\");
	std::string folder_path = file_path.substr(0, last_slash);
	std::string file_name = file_path.substr(last_slash + 1, file_path.length());
	if (file_name.length() > 0 && file_name[0] == ':')//unsupported file start
	{
		return false;
	}
	while (CreateDirectory(folder_path.c_str(), NULL) == FALSE)
	{
		if (ERROR_ALREADY_EXISTS == GetLastError())
		{
			return true;
		}
		TCHAR s_temp[MAX_PATH];
		int k = folder_path.length();
		strcpy(s_temp, folder_path.c_str());

		while (CreateDirectory(s_temp, NULL) != TRUE)
		{
			while (s_temp[--k] != '\\')
			{
				if (k <= 1)
				{
					return false;
				}
				s_temp[k] = NULL;
			}
		}
	}

	return true;
}

bool is_extension_ldr(const XSI::CString& ext)
{
	return ext == "png" ||
		ext == "jpg" ||
		ext == "bmp" ||
		ext == "tga" ||
		ext == "ppm";
}

bool is_extension_hdr(const XSI::CString& ext)
{
	return ext == "exr" ||
		ext == "hdr";
}

XSI::CString get_file_extension(const XSI::CString& file_path)
{
	ULONG i = file_path.ReverseFindString(".");
	return file_path.GetSubString(i + 1);
}

bool is_file_exists(const XSI::CString& file_path)
{
	return std::filesystem::exists(file_path.GetAsciiString());
}

int get_key_by_value(std::unordered_map<ULONG, std::vector<std::string>>& map, const std::string& value)
{
	for (const auto& [key, map_value] : map)
	{
		if (std::find(map_value.begin(), map_value.end(), value) != map_value.end()) 
		{
			return key;
		}
	}
	return -1;
}

bool is_isolation_list_contaons_object(const XSI::CRefArray& xsi_isolation_list, const XSI::X3DObject& xsi_object)
{
	ULONG xsi_id = xsi_object.GetObjectID();
	for (ULONG i = 0; i < xsi_isolation_list.GetCount(); i++)
	{
		XSI::X3DObject in_list_object = XSI::X3DObject(xsi_isolation_list[i]);
		if (in_list_object.GetObjectID() == xsi_id)
		{
			return true;
		}
	}

	return false;
}