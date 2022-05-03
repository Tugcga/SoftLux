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
		return XSI::CUtils::ResolvePath(XSI::CUtils::BuildPath(project_path, input_path));
	}

	return input_path;
}