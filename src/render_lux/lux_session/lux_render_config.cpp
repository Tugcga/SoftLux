#include "lux_session.h"
#include "../../utilities/logs.h"
#include "../../utilities/xsi_shaders.h"

luxcore::RenderSession* sync_render_config(luxcore::Scene* scene, const RenderType render_type, const XSI::Property& render_property, const XSI::CTime& eval_time,
	const int image_x_start, const int image_x_end, const int image_y_start, const int image_y_end,
	const int image_width, const int image_height)
{
	luxrays::Properties render_props;
	//render settings
	render_props.Set(luxrays::Property("renderengine.type")("PATHCPU"));
	render_props.Set(luxrays::Property("sampler.type")("SOBOL"));
	render_props.Set(luxrays::Property("batch.haltspp")(4));
	render_props.Set(luxrays::Property("film.width")(image_width));
	render_props.Set(luxrays::Property("film.height")(image_height));
	render_props.Set(luxrays::Property("film.subregion")(image_x_start, image_x_end, image_y_start, image_y_end));
 
	//image piplines
	if (render_type != RenderType_Shaderball)
	{
		sync_imagepipline(render_props, eval_time);
	}
	
	//outputs
	render_props.Set(luxrays::Property("film.outputs.0.type")("RGB_IMAGEPIPELINE"));
	render_props.Set(luxrays::Property("film.outputs.0.filename")(""));
	render_props.Set(luxrays::Property("film.outputs.1.type")("ALPHA"));

	luxcore::RenderConfig* render_config = luxcore::RenderConfig::Create(render_props, scene);
	luxcore::RenderSession *session = luxcore::RenderSession::Create(render_config);
	return session;
}
