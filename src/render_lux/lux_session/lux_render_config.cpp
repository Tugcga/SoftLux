#include "lux_session.h"
#include "../../utilities/logs.h"
#include "../../utilities/xsi_shaders.h"

#include <set>

bool is_lux_output_ldr(luxcore::Film::FilmOutputType output_type)
{
	return output_type == luxcore::Film::FilmOutputType::OUTPUT_MATERIAL_ID ||
		   output_type == luxcore::Film::FilmOutputType::OUTPUT_OBJECT_ID ||
		   output_type == luxcore::Film::FilmOutputType::OUTPUT_SAMPLECOUNT;
}

luxcore::RenderSession* sync_render_config(luxcore::Scene* scene, const RenderType render_type, const XSI::Property& render_property, const XSI::CTime& eval_time,
	luxcore::Film::FilmOutputType lux_visual_output_type, const XSI::CStringArray& output_channels,
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
	//set the visual output
	std::string visual_output_name = output_type_to_string(lux_visual_output_type);
	render_props.Set(luxrays::Property("film.outputs.0.type")(visual_output_name));
	render_props.Set(luxrays::Property("film.outputs.0.filename")(visual_output_name + (is_lux_output_ldr(lux_visual_output_type) ? ".png" : ".exr")));
	render_props.Set(luxrays::Property("film.outputs.1.type")("ALPHA"));
	render_props.Set(luxrays::Property("film.outputs.1.filename")("ALPHA.exr"));

	std::set<luxcore::Film::FilmOutputType> channels_set;
	channels_set.insert(lux_visual_output_type);
	channels_set.insert(luxcore::Film::OUTPUT_ALPHA);

	int channel_index = 2;
	//also we should set output channels
	for (ULONG i = 0; i < output_channels.GetCount(); i++)
	{
		luxcore::Film::FilmOutputType output_type = output_string_prime_to_lux(output_channels[i]);
		if (!channels_set.contains(output_type))
		{
			std::string channel_index_str = std::to_string(channel_index);
			std::string channel_output_name = output_type_to_string(output_type);
			render_props.Set(luxrays::Property("film.outputs." + channel_index_str + ".type")(channel_output_name));
			render_props.Set(luxrays::Property("film.outputs." + channel_index_str + ".filename")(channel_output_name + (is_lux_output_ldr(output_type) ? ".png" : ".exr")));
			channels_set.insert(output_type);
			channel_index++;
		}
	}

	luxcore::RenderConfig* render_config = luxcore::RenderConfig::Create(render_props, scene);
	luxcore::RenderSession *session = luxcore::RenderSession::Create(render_config);
	return session;
}
