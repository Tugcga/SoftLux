#pragma once
#include "../../render_base/render_visual_buffer.h"
#include "../../render_base/render_engine_base.h"

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

luxcore::Film::FilmOutputType output_string_to_lux(const XSI::CString& name);
//the same as before, but input is name with _ instead of spaces
luxcore::Film::FilmOutputType output_string_prime_to_lux(const XSI::CString& name);;
std::string output_type_to_string(luxcore::Film::FilmOutputType type);

bool is_lux_output_ldr(luxcore::Film::FilmOutputType output_type);

void sync_imagepipline(luxrays::Properties& render_props, const XSI::CTime& eval_time);
luxcore::RenderSession* sync_render_config(luxcore::Scene* scene, const RenderType render_type, const XSI::Property &render_property, const XSI::CTime &eval_time,
	luxcore::Film::FilmOutputType lux_visual_output_type, const XSI::CStringArray& output_channels, const XSI::CStringArray& output_paths,
	const int image_x_start, const int image_x_end, const int image_y_start, const int image_y_end,
	const int image_width, const int image_height);

void read_visual_buffer(luxcore::Film &film, luxcore::Film::FilmOutputType visual_type, RenderVisualBuffer &buffer);
void copy_film_to_output_pixels(luxcore::Film& film, std::vector<float>& output_pixels, const XSI::CStringArray& output_channels);

void export_scene(luxcore::RenderSession* session, XSI::RendererContext& render_context, const XSI::CString& archive_folder);