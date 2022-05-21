#include "lux_session.h"
#include "../../utilities/logs.h"
#include "../../utilities/xsi_shaders.h"
#include "../../utilities/export_common.h"

#include "../lux_scene/lux_scene.h"

#include <set>

//return true if the output must be ldr
bool is_lux_output_ldr(luxcore::Film::FilmOutputType output_type)
{
	return output_type == luxcore::Film::FilmOutputType::OUTPUT_MATERIAL_ID ||
		   output_type == luxcore::Film::FilmOutputType::OUTPUT_OBJECT_ID ||
		   output_type == luxcore::Film::FilmOutputType::OUTPUT_SAMPLECOUNT;
}

//return true if the output must be hdr
bool is_lux_output_hdr(luxcore::Film::FilmOutputType output_type)
{
	return output_type == luxcore::Film::FilmOutputType::OUTPUT_RGB ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_RGBA ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_DEPTH ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_POSITION ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_GEOMETRY_NORMAL ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_SHADING_NORMAL ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_DIFFUSE ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_DIFFUSE_REFLECT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_DIFFUSE_TRANSMIT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_GLOSSY ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_GLOSSY_REFLECT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_GLOSSY_TRANSMIT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_EMISSION ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_DIFFUSE ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_DIFFUSE_REFLECT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_DIFFUSE_TRANSMIT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_GLOSSY ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_GLOSSY_REFLECT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_GLOSSY_TRANSMIT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_SPECULAR ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_SPECULAR_REFLECT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_SPECULAR_TRANSMIT ||
		output_type == luxcore::Film::FilmOutputType::OUTPUT_CAUSTIC;
}

luxcore::RenderSession* sync_render_config(luxcore::Scene* scene, const RenderType render_type, const XSI::Property& render_property, const XSI::CTime& eval_time,
	luxcore::Film::FilmOutputType lux_visual_output_type, const XSI::CStringArray& output_channels, const XSI::CStringArray &output_paths, const XSI::CString &archive_folder, const XSI::CString &scene_name,
	const int image_x_start, const int image_x_end, const int image_y_start, const int image_y_end,
	const int image_width, const int image_height)
{
	XSI::CParameterRefArray render_params = render_property.GetParameters();
	luxrays::Properties render_props;
	//render settings
	int engine = render_params.GetValue("engine", eval_time);
	//setup settings for shaderball
	if (render_type == RenderType_Shaderball)
	{
		render_props.Set(luxrays::Property("batch.halttime")(5.0));  // hardcode the render time
		render_props.Set(luxrays::Property("renderengine.type")("PATHCPU"));  // engine
		render_props.Set(luxrays::Property("sampler.type")("SOBOL"));  // and sampler
		//all other parameters are default
	}
	else
	{
		//seed
		render_props.Set(luxrays::Property("renderengine.seed")((int)render_params.GetValue("service_seed", eval_time)));

		//render engine
		int cpu_count = render_params.GetValue("cpu_threads_count", eval_time);
		if (engine == 2)
		{//path ocl
			render_props.Set(luxrays::Property("renderengine.type")("PATHOCL"));
			render_props.Set(luxrays::Property("pathocl.pixelatomics.enable")((bool)render_params.GetValue("pathocl_pixelatomics", eval_time)));
		}
		else if (engine == 1)
		{//bidir
			render_props.Set(luxrays::Property("renderengine.type")("BIDIRCPU"));
			if (cpu_count > 0) { render_props.Set(luxrays::Property("native.threads.count")(cpu_count)); }
			render_props.Set(luxrays::Property("path.maxdepth")((int)render_params.GetValue("path_maxdepth", eval_time)));
			render_props.Set(luxrays::Property("light.maxdepth")((int)render_params.GetValue("light_maxdepth", eval_time)));
			render_props.Set(luxrays::Property("path.russianroulette.depth")((int)render_params.GetValue("path_russianroulette_depth", eval_time)));
			render_props.Set(luxrays::Property("path.russianroulette.cap")((float)render_params.GetValue("path_russianroulette_cap", eval_time)));
		}
		else
		{
			render_props.Set(luxrays::Property("renderengine.type")("PATHCPU"));
			if (cpu_count > 0) { render_props.Set(luxrays::Property("native.threads.count")(cpu_count)); }
			render_props.Set(luxrays::Property("path.pathdepth.total")((int)render_params.GetValue("path_total", eval_time)));
			render_props.Set(luxrays::Property("path.pathdepth.diffuse")((int)render_params.GetValue("path_diffuse", eval_time)));
			render_props.Set(luxrays::Property("path.pathdepth.glossy")((int)render_params.GetValue("path_glossy", eval_time)));
			render_props.Set(luxrays::Property("path.pathdepth.specular")((int)render_params.GetValue("path_specular", eval_time)));
			render_props.Set(luxrays::Property("path.russianroulette.depth")((int)render_params.GetValue("path_russianroulette_depth", eval_time)));
			render_props.Set(luxrays::Property("path.russianroulette.cap")((float)render_params.GetValue("path_russianroulette_cap", eval_time)));
			render_props.Set(luxrays::Property("path.clamping.variance.maxvalue")((float)render_params.GetValue("path_clamping_max", eval_time)));
			render_props.Set(luxrays::Property("path.forceblackbackground.enable")((bool)render_params.GetValue("path_background", eval_time)));
		}

		//sampler
		int sampler_type = render_params.GetValue("sampler_type", eval_time);
		if (sampler_type == 1)
		{
			render_props.Set(luxrays::Property("sampler.type")("METROPOLIS"));
			render_props.Set(luxrays::Property("sampler.metropolis.largesteprate")((float)render_params.GetValue("sampler_metropolis_largesteprate", eval_time)));
			render_props.Set(luxrays::Property("sampler.metropolis.maxconsecutivereject")((int)render_params.GetValue("sampler_metropolis_maxconsecutivereject", eval_time)));
			render_props.Set(luxrays::Property("sampler.metropolis.imagemutationrate")((float)render_params.GetValue("sampler_metropolis_imagemutationrate", eval_time)));
		}
		else if (sampler_type == 2)
		{
			render_props.Set(luxrays::Property("sampler.type")("RANDOM"));
			render_props.Set(luxrays::Property("sampler.random.adaptive.strength")((float)render_params.GetValue("sampler_strength", eval_time)));
		}
		else
		{
			render_props.Set(luxrays::Property("sampler.type")("SOBOL"));
			render_props.Set(luxrays::Property("sampler.sobol.adaptive.strength")((float)render_params.GetValue("sampler_strength", eval_time)));
		}

		//filter
		int filter_type = render_params.GetValue("filter_type", eval_time);
		if (filter_type == 1)
		{
			render_props.Set(luxrays::Property("film.filter.type")("BOX"));
			render_props.Set(luxrays::Property("film.filter.width")((float)render_params.GetValue("filter_width", eval_time)));
		}
		else if (filter_type == 2)
		{
			render_props.Set(luxrays::Property("film.filter.type")("GAUSSIAN"));
			render_props.Set(luxrays::Property("film.filter.width")((float)render_params.GetValue("filter_width", eval_time)));
			render_props.Set(luxrays::Property("film.filter.gaussian.alpha")((float)render_params.GetValue("filter_gaussian_alpha", eval_time)));
		}
		else if (filter_type == 3)
		{
			render_props.Set(luxrays::Property("film.filter.type")("MITCHELL"));
			render_props.Set(luxrays::Property("film.filter.width")((float)render_params.GetValue("filter_width", eval_time)));
			render_props.Set(luxrays::Property("film.filter.mitchell.b")((float)render_params.GetValue("filter_mitchel_b", eval_time)));
			render_props.Set(luxrays::Property("film.filter.mitchell.c")((float)render_params.GetValue("filter_mitchel_c", eval_time)));
		}
		else if (filter_type == 4)
		{
			render_props.Set(luxrays::Property("film.filter.type")("MITCHELL_SS"));
			render_props.Set(luxrays::Property("film.filter.width")((float)render_params.GetValue("filter_width", eval_time)));
			render_props.Set(luxrays::Property("film.filter.mitchellss.b")((float)render_params.GetValue("filter_mitchel_b", eval_time)));
			render_props.Set(luxrays::Property("film.filter.mitchellss.c")((float)render_params.GetValue("filter_mitchel_c", eval_time)));
		}
		else if (filter_type == 5)
		{
			render_props.Set(luxrays::Property("film.filter.type")("BLACKMANHARRIS"));
			render_props.Set(luxrays::Property("film.filter.width")((float)render_params.GetValue("filter_width", eval_time)));
		}
		else
		{
			render_props.Set(luxrays::Property("film.filter.type")("NONE"));
		}

		//light strategy
		int lightstrategy_mode = render_params.GetValue("lightstrategy_mode", eval_time);
		if (lightstrategy_mode == 1)
		{
			render_props.Set(luxrays::Property("lightstrategy.type")("POWER"));
		}
		else if (lightstrategy_mode == 2)
		{
			render_props.Set(luxrays::Property("lightstrategy.type")("LOG_POWER"));
		}
		else if (lightstrategy_mode == 3)
		{
			render_props.Set(luxrays::Property("lightstrategy.type")("DLS_CACHE"));

			render_props.Set(luxrays::Property("lightstrategy.entry.radius")((float)render_params.GetValue("lightstrategy_radius", eval_time)));
			render_props.Set(luxrays::Property("lightstrategy.entry.normalangle")((float)render_params.GetValue("lightstrategy_normalangle", eval_time)));
			render_props.Set(luxrays::Property("lightstrategy.entry.maxpasses")((int)render_params.GetValue("lightstrategy_maxpasses", eval_time)));
			render_props.Set(luxrays::Property("lightstrategy.entry.convergencethreshold")((float)render_params.GetValue("lightstrategy_convergencethreshold", eval_time)));
			render_props.Set(luxrays::Property("lightstrategy.entry.warmupsamples")((int)render_params.GetValue("lightstrategy_warmupsamples", eval_time)));
			render_props.Set(luxrays::Property("lightstrategy.targetcachehitratio")((float)render_params.GetValue("lightstrategy_targetcachehitratio", eval_time)));
			render_props.Set(luxrays::Property("lightstrategy.maxdepth")((int)render_params.GetValue("lightstrategy_maxdepth", eval_time)));
			render_props.Set(luxrays::Property("lightstrategy.maxsamplescount")((int)render_params.GetValue("lightstrategy_maxsamplescount", eval_time)));
		}
		else
		{
			render_props.Set(luxrays::Property("lightstrategy.type")("UNIFORM"));
		}

		//accelerator
		int service_accelerator = render_params.GetValue("service_accelerator", eval_time);
		if (service_accelerator == 1)
		{
			render_props.Set(luxrays::Property("accelerator.type")("BVH"));
		}
		else if (service_accelerator == 2)
		{
			render_props.Set(luxrays::Property("accelerator.type")("MBVH"));
		}
		else if (service_accelerator == 3)
		{
			render_props.Set(luxrays::Property("accelerator.type")("EMBREE"));
		}
		else
		{
			render_props.Set(luxrays::Property("accelerator.type")("AUTO"));
		}

		//noise estimation
		render_props.Set(luxrays::Property("film.noiseestimation.warmup")((int)render_params.GetValue("noiseestimation_warmup", eval_time)));
		render_props.Set(luxrays::Property("film.noiseestimation.step")((int)render_params.GetValue("noiseestimation_step", eval_time)));
		render_props.Set(luxrays::Property("film.noiseestimation.filter.scale")((int)render_params.GetValue("noiseestimation_filter_scale", eval_time)));

		//halt condition
		int halt_condition = render_params.GetValue("halt_condition", eval_time);
		if (halt_condition == 1)
		{//time
			render_props.Set(luxrays::Property("batch.halttime")((float)render_params.GetValue("halt_time", eval_time)));
			render_props.Set(luxrays::Property("batch.haltnoisethreshold")(-1.0f));
			render_props.Set(luxrays::Property("batch.haltspp")(0));
		}
		else if (halt_condition == 2)
		{//noise level
			render_props.Set(luxrays::Property("batch.haltnoisethreshold")((float)render_params.GetValue("halt_noise", eval_time)));
			render_props.Set(luxrays::Property("batch.haltnoisethreshold.warmup")((int)render_params.GetValue("halt_noise_warmup", eval_time)));
			render_props.Set(luxrays::Property("batch.haltnoisethreshold.step")((int)render_params.GetValue("halt_noise_step", eval_time)));
			render_props.Set(luxrays::Property("batch.haltnoisethreshold.filter.enable")((bool)render_params.GetValue("halt_noise_filter", eval_time)));
			render_props.Set(luxrays::Property("batch.halttime")(0.0f));
			render_props.Set(luxrays::Property("batch.haltspp")(0));
		}
		else
		{//samples
			render_props.Set(luxrays::Property("batch.haltspp")((int)render_params.GetValue("halt_samples", eval_time)));
			render_props.Set(luxrays::Property("batch.haltnoisethreshold")(-1.0f));
			render_props.Set(luxrays::Property("batch.halttime")(0.0f));
		}
	}
	
	//image size
	render_props.Set(luxrays::Property("film.width")(image_width));
	render_props.Set(luxrays::Property("film.height")(image_height));
	render_props.Set(luxrays::Property("film.subregion")(image_x_start, image_x_end, image_y_start, image_y_end));

	//if we export the scene, then we should reassign render engine to filesaver
	if (render_type == RenderType_Export)
	{
		int export_mode = render_params.GetValue("export_mode", eval_time);
		if (export_mode == 0 || export_mode == 1)
		{
			render_props.Set(luxrays::Property("renderengine.type")("FILESAVER"));
			//set engine from the render settings
			if (engine == 2)
			{
				render_props.Set(luxrays::Property("filesaver.renderengine.type")("PATHOCL"));
			}
			else if (engine == 1)
			{
				render_props.Set(luxrays::Property("filesaver.renderengine.type")("BIDIRCPU"));
			}
			else
			{
				render_props.Set(luxrays::Property("filesaver.renderengine.type")("PATHCPU"));
			}
			if (export_mode == 0)
			{//text-based mode
				render_props.Set(luxrays::Property("filesaver.format")("TXT"));
				render_props.Set(luxrays::Property("filesaver.directory")(archive_folder.GetAsciiString()));
			}
			else
			{//binary
				render_props.Set(luxrays::Property("filesaver.format")("BIN"));
				render_props.Set(luxrays::Property("filesaver.filename")((archive_folder + "\\" + scene_name + ".bcf").GetAsciiString()));
			}
		}
		//for rsm mode we does not use filesaver, but export resume file for regular render
	}
 
	//image piplines
	if (render_type != RenderType_Shaderball)
	{
		sync_imagepipline(render_props, eval_time);
	}
	else
	{
		sync_shaderball_imagepipline(render_props, eval_time);
	}
	
	//outputs
	if (render_type == RenderType_Export)
	{//setup outputs for the export
		//use only render passes without visual and so on
		//use output_paths input array
		for (ULONG i = 0; i < output_channels.GetCount(); i++)
		{
			luxcore::Film::FilmOutputType output_type = output_string_prime_to_lux(output_channels[i]);
			//for the other modes we use RGB (without alpha) and ALPHA channels
			//here we should output actual RGBA channels if RGB is selected
			if (output_type == luxcore::Film::OUTPUT_RGB)
			{
				output_type = luxcore::Film::OUTPUT_RGBA;
			}
			else if (output_type == luxcore::Film::OUTPUT_RGB_IMAGEPIPELINE)
			{
				output_type = luxcore::Film::OUTPUT_RGBA_IMAGEPIPELINE;
			}
			std::string channel_index_str = std::to_string(i);
			std::string channel_output_name = output_type_to_string(output_type);
			render_props.Set(luxrays::Property("film.outputs." + channel_index_str + ".type")(channel_output_name));
			//if the output is not tonemaped, then it should be saved into hdr
			//so, we should check is the channel must be ldr, hdr or can be both
			XSI::CString ext = get_file_extension(output_paths[i]);
			XSI::CString output_path = output_paths[i];
			if (is_lux_output_ldr(output_type) && !is_extension_ldr(ext))
			{//output must be ldr, but extension is not
				log_message("Output channel " + output_channels[i] + " must be LDR, but output file is HDR. Change output extension to *.png", XSI::siWarningMsg);

				ULONG p = output_path.ReverseFindString(".");
				output_path = output_path.GetSubString(0, p) + ".png";
			}
			if (is_lux_output_hdr(output_type) && !is_extension_hdr(ext))
			{//output must be hdr, but extension is not
				log_message("Output channel " + output_channels[i] + " must be HDR, but output file is LDR. Change output extension to *.exr", XSI::siWarningMsg);

				ULONG p = output_path.ReverseFindString(".");
				output_path = output_path.GetSubString(0, p) + ".exr";
			}
			render_props.Set(luxrays::Property("film.outputs." + channel_index_str + ".filename")(output_path.GetAsciiString()));
		}
	}
	else
	{
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
	}
	
	luxcore::RenderConfig* render_config = luxcore::RenderConfig::Create(render_props, scene);
	luxcore::RenderSession *session = luxcore::RenderSession::Create(render_config);
	return session;
}
