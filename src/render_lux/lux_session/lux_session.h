#pragma once
#include "../../render_base/render_visual_buffer.h"

#include <luxcore/luxcore.h>

#include "xsi_property.h"
#include "xsi_time.h"

static std::vector<std::string> pipline_names = {
		"PassTonemapLinear" ,
		"PassTonemapReinhard",
		"PassTonemapAutoLinear",
		"PassTonemapLuxLinear",
		"PassGammaCorrection",
		"PassNOP",
		"PassOutputSwitcher",
		"PassGaussianFilter",
		"PassCameraResponse",
		"PassContourLines",
		"PassBackgroundImage",
		"PassBloom",
		"PassObjectIDMask",
		"PassVignetting",
		"PassColorAberration",
		"PassPremultiplyAlpha",
		"PassMist",
		"PassBCDDenoiser",
		"PassPatterns",
		"PassIntelOIDN",
		"PassWhiteBalance",
		"PassBakeMapMargin",
		"PassColorLUT",
		"PassOptixDenoiser"
		//"PassTonemapOpenColorIO"
};

void sync_imagepipline(luxrays::Properties& render_props, const XSI::CTime& eval_time);
luxcore::RenderSession* sync_render_config(luxcore::Scene* scene, const XSI::Property &render_property, const XSI::CTime &eval_time,
	const int image_x_start, const int image_x_end, const int image_y_start, const int image_y_end,
	const int image_width, const int image_height);

void read_visual_buffer(luxcore::Film &film, RenderVisualBuffer &buffer);