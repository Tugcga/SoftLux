#pragma once

#include <xsi_x3dobject.h>
#include <xsi_time.h>
#include "xsi_projectitem.h"

#include <string>
#include <vector>

//return true if object have active render visibility
bool is_xsi_object_visible(const XSI::CTime &eval_time, XSI::X3DObject &xsi_object);

//return true, if pointcloud should be interpret as strands
bool is_pointcloud_strands(XSI::X3DObject& xsi_object, const XSI::CTime& eval_time);

//conver unique id of the object to string
std::vector<std::string> xsi_object_id_string(XSI::ProjectItem& xsi_item);

//convert path from local (with respect to project path) to global
//or return the input path, if it already global
XSI::CString resolve_path(const XSI::CString& input_path);

bool create_dir(const std::string& file_path);

bool is_extension_ldr(const XSI::CString &ext);
bool is_extension_hdr(const XSI::CString& ext);
XSI::CString get_file_extension(const XSI::CString &file_path);