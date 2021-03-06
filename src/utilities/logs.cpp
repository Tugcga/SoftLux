#include <xsi_string.h>
#include <xsi_application.h>
#include <xsi_color.h>
#include <xsi_color4f.h>
#include <xsi_vector3.h>
#include <xsi_floatarray.h>
#include <xsi_longarray.h>
#include <xsi_doublearray.h>

#include <vector>
#include <string>
#include <set>

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

XSI::CString to_string(const XSI::CDoubleArray& array)
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

XSI::CString to_string(const std::set<ULONG>& array)
{
	if (array.size() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[";
	for (auto it = array.begin(); it != array.end(); ++it)
	{
		to_return += XSI::CString(*it) + ", ";
	}
	to_return = to_return.GetSubString(0, to_return.Length() - 2);
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

XSI::CString to_string(const std::vector<unsigned short>& array)
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

XSI::CString to_string(const XSI::CStringArray& array)
{
	if (array.GetCount() == 0)
	{
		return "[]";
	}

	XSI::CString to_return = "[" + array[0];
	for (ULONG i = 1; i < array.GetCount(); i++)
	{
		to_return += ", " + array[i];
	}
	to_return += "]";
	return to_return;
}

XSI::CString remove_digits(const XSI::CString& orignal_str)
{
	XSI::CString to_return(orignal_str);
	to_return.TrimRight("0123456789");
	return to_return;
}

XSI::CString replace_symbols(const XSI::CString& input_string, const XSI::CString& splitter, const XSI::CString& replacer)
{
	XSI::CStringArray parts = input_string.Split(splitter);
	XSI::CString to_return = parts[0];
	for (ULONG i = 1; i < parts.GetCount(); i++)
	{
		to_return += replacer + parts[i];
	}

	return to_return;
}