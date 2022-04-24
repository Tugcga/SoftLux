#pragma once

#include <xsi_x3dobject.h>
#include <xsi_time.h>
#include "xsi_projectitem.h"

#include <string>

//return true if object have active render visibility
bool is_xsi_object_visible(const XSI::CTime &eval_time, XSI::X3DObject &xsi_object);

//conver unique id of the object to string
std::string xsi_object_id_string(const XSI::ProjectItem& xsi_item);