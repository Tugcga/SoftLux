#include "lux_session.h"
#include "../../utilities/logs.h"

luxcore::RenderSession* sync_render_config(luxcore::Scene* scene, const XSI::Property& render_property, const XSI::CTime& eval_time,
	const int image_x_start, const int image_x_end, const int image_y_start, const int image_y_end,
	const int image_width, const int image_height)
{
	luxrays::Properties render_props(
		luxrays::Property("renderengine.type")("PATHCPU") <<
		luxrays::Property("sampler.type")("SOBOL") <<
		luxrays::Property("batch.haltspp")(4) <<
		luxrays::Property("film.width")(image_width) <<
		luxrays::Property("film.height")(image_height) <<
		luxrays::Property("film.subregion")(image_x_start, image_x_end, image_y_start, image_y_end) <<
		//----------------------------
		luxrays::Property("film.imagepipeline.0.type")("TONEMAP_LINEAR") <<
		//luxrays::Property("film.imagepipeline.0.scale")(0.0001) <<
		luxrays::Property("film.imagepipeline.1.type")("GAMMA_CORRECTION") <<
		luxrays::Property("film.imagepipeline.1.value")(2.2) <<
		//----------------------------
		luxrays::Property("film.outputs.0.type")("RGB_IMAGEPIPELINE") <<
		luxrays::Property("film.outputs.0.index")(0) <<
		luxrays::Property("film.outputs.0.filename")("image.png") <<
		luxrays::Property("film.outputs.1.type")("ALPHA")  // add alpha for preview output
	);

	luxcore::RenderConfig* render_config = luxcore::RenderConfig::Create(render_props, scene);

	luxcore::RenderSession *session = luxcore::RenderSession::Create(render_config);
	return session;
}
