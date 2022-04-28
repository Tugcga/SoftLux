#include "export_common.h"

#include <xsi_x3dobject.h>
#include <xsi_time.h>
#include <xsi_property.h>
#include <xsi_application.h>
#include <xsi_utils.h>
#include <xsi_project.h>

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

std::string xsi_object_id_string(const XSI::ProjectItem& xsi_item)
{
	return XSI::CString(xsi_item.GetObjectID()).GetAsciiString();
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