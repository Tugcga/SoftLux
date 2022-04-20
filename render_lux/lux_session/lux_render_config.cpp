#include "lux_session.h"
#include "../../utilities/logs.h"

void sync_render_config(luxcore::RenderConfig* config, const XSI::Property& render_property, const XSI::CTime& eval_time,
	const int image_x_start, const int image_x_end, const int image_y_start, const int image_y_end,
	const int image_width, const int image_height)
{
	config->Parse(
		luxrays::Property("renderengine.type")("PATHCPU") <<
		luxrays::Property("sampler.type")("SOBOL") <<
		luxrays::Property("batch.haltspp")(4) <<
		//luxrays::Property("batch.halttime")(1.0f) <<
		luxrays::Property("film.width")(image_width) <<
		luxrays::Property("film.height")(image_height) <<
		luxrays::Property("film.subregion")(image_x_start, image_x_end, image_y_start, image_y_end) <<
		luxrays::Property("film.outputs.1.type")("RGB_TONEMAPPED") <<
		luxrays::Property("film.outputs.1.index")(0) <<
		luxrays::Property("film.outputs.1.filename")("image.png") <<
		luxrays::Property("film.imagepipeline.0.type")("TONEMAP_LINEAR"));
}
