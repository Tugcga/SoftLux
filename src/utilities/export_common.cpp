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
		return XSI::CUtils::ResolvePath(XSI::CUtils::BuildPath(project_path, input_path));
	}

	return input_path;
}