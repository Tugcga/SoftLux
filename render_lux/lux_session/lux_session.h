#pragma once
#include "../../render_base/render_visual_buffer.h"

#include <luxcore/luxcore.h>

#include "xsi_property.h"
#include "xsi_time.h"

luxcore::RenderSession* sync_render_config(luxcore::Scene* scene, const XSI::Property &render_property, const XSI::CTime &eval_time,
	const int image_x_start, const int image_x_end, const int image_y_start, const int image_y_end,
	const int image_width, const int image_height);

void read_visual_buffer(luxcore::Film &film, RenderVisualBuffer &buffer);