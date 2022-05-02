#include <xsi_string.h>
#include <xsi_application.h>
#include <xsi_color.h>
#include <xsi_color4f.h>
#include <xsi_vector3.h>
#include <xsi_floatarray.h>
#include <xsi_longarray.h>

#include <vector>
#include <string>

void log_message(const XSI::CString &message, XSI::siSeverityType level)
{
	XSI::Application().LogMessage("[LuxCore Renderer] " + message, level);
}

XSI::CString to_string(const XSI::CFloatArray& array)
{
	if (array.GetCount() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[" + XSI::CString(array[0]);
	for (ULONG i = 1; i < array.GetCount(); i++)
	{
		to_return += ", " + XSI::CString(array[i]);
	}
	to_return += "]";
	return to_return;
}

XSI::CString to_string(const XSI::CLongArray& array)
{
	if (array.GetCount() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[" + XSI::CString(array[0]);
	for (ULONG i = 1; i < array.GetCount(); i++)
	{
		to_return += ", " + XSI::CString(array[i]);
	}
	to_return += "]";
	return to_return;
}

XSI::CString to_string(const std::vector<std::string>& array)
{
	if (array.size() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[" + XSI::CString(array[0].c_str());
	for (ULONG i = 1; i < array.size(); i++)
	{
		to_return += ", " + XSI::CString(array[i].c_str());
	}
	to_return += "]";
	return to_return;
}

XSI::CString to_string(const std::vector<ULONG> &array)
{
	if (array.size() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[" + XSI::CString(array[0]);
	for (ULONG i = 1; i < array.size(); i++)
	{
		to_return += ", " + XSI::CString(array[i]);
	}
	to_return += "]";
	return to_return;
}

XSI::CString to_string(const std::vector<unsigned int> &array)
{
	if (array.size() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[" + XSI::CString((int)array[0]);
	for (ULONG i = 1; i < array.size(); i++)
	{
		to_return += ", " + XSI::CString((int)array[i]);
	}
	to_return += "]";
	return to_return;
}

XSI::CString to_string(const std::vector<float> &array)
{
	if (array.size() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[" + XSI::CString((float)array[0]);
	for (ULONG i = 1; i < array.size(); i++)
	{
		to_return += ", " + XSI::CString((float)array[i]);
	}
	to_return += "]";
	return to_return;
}

XSI::CString to_string(const std::vector<double>& array)
{
	if (array.size() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[" + XSI::CString((float)array[0]);
	for (ULONG i = 1; i < array.size(); i++)
	{
		to_return += ", " + XSI::CString((float)array[i]);
	}
	to_return += "]";
	return to_return;
}

XSI::CString to_string(const XSI::CColor &color)
{
	return "(" + XSI::CString(color.r) + ", " + XSI::CString(color.g) + ", " + XSI::CString(color.b) + ", " + XSI::CString(color.a) + ")";
}

XSI::CString to_string(const XSI::MATH::CVector3 &vector)
{
	return "(" + XSI::CString(vector.GetX()) + ", " + XSI::CString(vector.GetY()) + ", " + XSI::CString(vector.GetZ()) + ")";
}

XSI::CString to_string(const XSI::MATH::CColor4f &color)
{
	return "(" + XSI::CString(color.GetR()) + ", " + XSI::CString(color.GetG()) + ", " + XSI::CString(color.GetB()) + ", " + XSI::CString(color.GetA()) + ")";
}

XSI::CString remove_digits(const XSI::CString& orignal_str)
{
	XSI::CString to_return(orignal_str);
	to_return.TrimRight("0123456789");
	return to_return;
}