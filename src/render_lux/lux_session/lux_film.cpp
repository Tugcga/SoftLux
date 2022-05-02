#include "lux_session.h"
#include "../../utilities/logs.h"

luxcore::Film::FilmOutputType output_string_to_lux(const XSI::CString &name)
{
	if (name == "Luxcore RGBA") { return luxcore::Film::OUTPUT_RGB; }
	else if (name == "Luxcore RGBA Image Pipline" || name == "Main") { return luxcore::Film::OUTPUT_RGB_IMAGEPIPELINE; }
	else if (name == "Luxcore Alpha") { return luxcore::Film::OUTPUT_ALPHA; }
	else if (name == "Luxcore Depth") { return luxcore::Film::OUTPUT_DEPTH; }
	else if (name == "Luxcore Position") { return luxcore::Film::OUTPUT_POSITION; }
	else if (name == "Luxcore Geometry Normal") { return luxcore::Film::OUTPUT_GEOMETRY_NORMAL; }
	else if (name == "Luxcore Shading Normal") { return luxcore::Film::OUTPUT_SHADING_NORMAL; }
	else if (name == "Luxcore Direct Diffuse") { return luxcore::Film::OUTPUT_DIRECT_DIFFUSE; }
	else if (name == "Luxcore Direct Glossy") { return luxcore::Film::OUTPUT_DIRECT_GLOSSY; }
	else if (name == "Luxcore Emission") { return luxcore::Film::OUTPUT_EMISSION; }
	else if (name == "Luxcore Indirect Diffuse") { return luxcore::Film::OUTPUT_INDIRECT_DIFFUSE; }
	else if (name == "Luxcore Indirect Glossy") { return luxcore::Film::OUTPUT_INDIRECT_GLOSSY; }
	else if (name == "Luxcore Indirect Specular") { return luxcore::Film::OUTPUT_INDIRECT_SPECULAR; }
	else if (name == "Luxcore Material ID Mask") { return luxcore::Film::OUTPUT_MATERIAL_ID_MASK; }
	else if (name == "Luxcore Direct Shadow Mask") { return luxcore::Film::OUTPUT_DIRECT_SHADOW_MASK; }
	else if (name == "Luxcore Indirect Shadow Mask") { return luxcore::Film::OUTPUT_INDIRECT_SHADOW_MASK; }
	else if (name == "Luxcore Radiance Group") { return luxcore::Film::OUTPUT_RADIANCE_GROUP; }
	else if (name == "Luxcore UV") { return luxcore::Film::OUTPUT_UV; }
	else if (name == "Luxcore Raycount") { return luxcore::Film::OUTPUT_RAYCOUNT; }
	else if (name == "Luxcore By Material ID") { return luxcore::Film::OUTPUT_BY_MATERIAL_ID; }
	else if (name == "Luxcore Material ID") { return luxcore::Film::OUTPUT_MATERIAL_ID; }
	else if (name == "Luxcore Irradiance") { return luxcore::Film::OUTPUT_IRRADIANCE; }
	else if (name == "Luxcore Object ID") { return luxcore::Film::OUTPUT_OBJECT_ID; }
	else if (name == "Luxcore Object ID Mask") { return luxcore::Film::OUTPUT_OBJECT_ID_MASK; }
	else if (name == "Luxcore By Object ID") { return luxcore::Film::OUTPUT_BY_OBJECT_ID; }
	else if (name == "Luxcore Sample Count") { return luxcore::Film::OUTPUT_SAMPLECOUNT; }
	else if (name == "Luxcore Convergence") { return luxcore::Film::OUTPUT_CONVERGENCE; }
	else if (name == "Luxcore Noise") { return luxcore::Film::OUTPUT_NOISE; }
	else if (name == "Luxcore Material ID Color") { return luxcore::Film::OUTPUT_MATERIAL_ID_COLOR; }
	else { log_message("Unknown channel " + name + ", fallback to RGBA Image Pipline.", XSI::siWarningMsg); return luxcore::Film::OUTPUT_RGB_IMAGEPIPELINE; }
}

luxcore::Film::FilmOutputType output_string_prime_to_lux(const XSI::CString& name)
{
	XSI::CStringArray name_parts = name.Split("_");
	//join back by spaces
	XSI::CString channel_name = name_parts[0];
	for (ULONG j = 1; j < name_parts.GetCount(); j++)
	{
		channel_name += " " + name_parts[j];
	}
	return output_string_to_lux(channel_name);
}

std::string output_type_to_string(luxcore::Film::FilmOutputType type)
{
	if (type == luxcore::Film::FilmOutputType::OUTPUT_RGB) { return "RGB"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_RGBA) { return "RGBA"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_RGB_IMAGEPIPELINE) { return "RGB_IMAGEPIPELINE"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_RGBA_IMAGEPIPELINE) { return "RGBA_IMAGEPIPELINE"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_ALPHA) { return "ALPHA"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_DEPTH) { return "DEPTH"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_POSITION) { return "POSITION"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_GEOMETRY_NORMAL) { return "GEOMETRY_NORMAL"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_SHADING_NORMAL) { return "SHADING_NORMAL"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_MATERIAL_ID) { return "MATERIAL_ID"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_DIFFUSE) { return "DIRECT_DIFFUSE"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_DIFFUSE_REFLECT) { return "DIRECT_DIFFUSE_REFLECT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_DIFFUSE_TRANSMIT) { return "DIRECT_DIFFUSE_TRANSMIT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_GLOSSY) { return "DIRECT_GLOSSY"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_GLOSSY_REFLECT) { return "DIRECT_GLOSSY_REFLECT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_GLOSSY_TRANSMIT) { return "DIRECT_GLOSSY_TRANSMIT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_EMISSION) { return "EMISSION"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_DIFFUSE) { return "INDIRECT_DIFFUSE"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_DIFFUSE_REFLECT) { return "INDIRECT_DIFFUSE_REFLECT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_DIFFUSE_TRANSMIT) { return "INDIRECT_DIFFUSE_TRANSMIT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_GLOSSY) { return "INDIRECT_GLOSSY"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_GLOSSY_REFLECT) { return "INDIRECT_GLOSSY_REFLECT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_GLOSSY_TRANSMIT) { return "INDIRECT_GLOSSY_TRANSMIT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_SPECULAR) { return "INDIRECT_SPECULAR"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_SPECULAR_REFLECT) { return "INDIRECT_SPECULAR_REFLECT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_SPECULAR_TRANSMIT) { return "INDIRECT_SPECULAR_TRANSMIT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_MATERIAL_ID_MASK) { return "MATERIAL_ID_MASK"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_DIRECT_SHADOW_MASK) { return "DIRECT_SHADOW_MASK"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_INDIRECT_SHADOW_MASK) { return "INDIRECT_SHADOW_MASK"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_RADIANCE_GROUP) { return "RADIANCE_GROUP"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_UV) { return "UV"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_RAYCOUNT) { return "RAYCOUNT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_BY_MATERIAL_ID) { return "BY_MATERIAL_ID"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_IRRADIANCE) { return "IRRADIANCE"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_OBJECT_ID) { return "OBJECT_ID"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_OBJECT_ID_MASK) { return "OBJECT_ID_MASK"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_BY_OBJECT_ID) { return "BY_OBJECT_ID"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_SAMPLECOUNT) { return "SAMPLECOUNT"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_CONVERGENCE) { return "CONVERGENCE"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_SERIALIZED_FILM) { return "SERIALIZED_FILM"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_MATERIAL_ID_COLOR) { return "MATERIAL_ID_COLOR"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_ALBEDO) { return "ALBEDO"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_AVG_SHADING_NORMAL) { return "AVG_SHADING_NORMAL"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_NOISE) { return "NOISE"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_USER_IMPORTANCE) { return "USER_IMPORTANCE"; }
	else if (type == luxcore::Film::FilmOutputType::OUTPUT_CAUSTIC) { return "CAUSTIC"; }
	else { return "RGB_IMAGEPIPELINE"; }
}

void get_film_pixels(luxcore::Film& film, luxcore::Film::FilmOutputType output_type, std::vector<float> &film_pixels)
{
	if (is_lux_output_ldr(output_type))
	{
		//for these outputs get unsigned int values
		std::vector<unsigned int> int_film_pixels(film.GetOutputSize(output_type));
		film.GetOutput<unsigned int>(output_type, int_film_pixels.data(), 0, true);
		//copy values to float
		for (size_t i = 0; i < int_film_pixels.size(); i++)
		{
			film_pixels[i] = int_film_pixels[i];
		}
	}
	else
	{
		film.GetOutput<float>(output_type, film_pixels.data(), 0, true);
	}
}

void copy_pixels(std::vector<float> &src, std::vector<float>& src_alpha, std::vector<float>& dst, unsigned int dst_shift,
	unsigned int src_width, unsigned int src_height, unsigned int src_channels,
	unsigned int dst_width, unsigned int dst_height,
	unsigned int dst_corner_x, unsigned int dst_corner_y)
{
	for (size_t y = 0; y < dst_height; y++)
	{
		for (size_t x = 0; x < dst_width; x++)
		{
			for (int c = 0; c < src_channels; c++)
			{
				dst[(y * dst_width + x) * 4 + c + dst_shift] = src[((y + dst_corner_y) * src_width + (x + dst_corner_x)) * src_channels + c];
			}
			//all other channels fill by the first one
			for (int c = src_channels; c < 3; c++)
			{
				//for uv (2 channels), fill the third channel by 0.0
				if (src_channels == 2)
				{
					dst[(y * dst_width + x) * 4 + c + dst_shift] = 0.0;
				}
				else
				{
					dst[(y * dst_width + x) * 4 + c + dst_shift] = src[((y + dst_corner_y) * src_width + (x + dst_corner_x)) * src_channels];
				}
			}

			//next fill the alpha
			dst[(y * dst_width + x) * 4 + 3 + dst_shift] = src_alpha[(y + dst_corner_y) * src_width + (x + dst_corner_x)];
		}
	}
}

void read_visual_buffer(luxcore::Film &film, luxcore::Film::FilmOutputType visual_type, RenderVisualBuffer& buffer)
{
	unsigned int width = film.GetWidth();
	unsigned int height = film.GetHeight();
	unsigned int pixels_count = width * height;
	//these two aov should be selected in the render config
	std::vector<float> film_pixels(film.GetOutputSize(visual_type));
	std::vector<float> alpha_pixels(film.GetOutputSize(luxcore::Film::OUTPUT_ALPHA));

	unsigned int film_channels_count = film_pixels.size() / pixels_count;

	//in fact only RGBA and RGBA_IP has 4 channels
	//we does not use it
	//UV has 2 channels
	//all other outputs has 1 or 3 channels
	//alpha output is always has 1 channel
	get_film_pixels(film, visual_type, film_pixels);
	film.GetOutput<float>(luxcore::Film::OUTPUT_ALPHA, alpha_pixels.data(), 0, true);
	//next we should copy pixels for selected rectangle
	copy_pixels(film_pixels, alpha_pixels, buffer.pixels, 0,
		buffer.full_width, buffer.full_height, film_channels_count, 
		buffer.width, buffer.height,
		buffer.corner_x, buffer.corner_y);

	film_pixels.clear();
	alpha_pixels.clear();
	film_pixels.shrink_to_fit();
	alpha_pixels.shrink_to_fit();
}

void copy_film_to_output_pixels(luxcore::Film& film, std::vector<float> &output_pixels, const XSI::CStringArray &output_channels)
{
	unsigned int width = film.GetWidth();
	unsigned int height = film.GetHeight();
	unsigned int pixels_count = width * height;
	std::vector<float> alpha_pixels(film.GetOutputSize(luxcore::Film::OUTPUT_ALPHA));
	film.GetOutput<float>(luxcore::Film::OUTPUT_ALPHA, alpha_pixels.data(), 0, true);

	for (ULONG i = 0; i < output_channels.GetCount(); i++)
	{
		//we should save to output pixels always 4-channels per pixel
		//initial buffer can contains less pixels (4(RGBA), 3, 2 (UV) or 1)
		//most outputs is float, but some of them are unsigned int
		luxcore::Film::FilmOutputType lux_output_type = output_string_prime_to_lux(output_channels[i]);
		std::vector<float> film_pixels(film.GetOutputSize(lux_output_type));
		get_film_pixels(film, lux_output_type, film_pixels);
		unsigned int film_channels_count = film_pixels.size() / pixels_count;
		copy_pixels(film_pixels, alpha_pixels, output_pixels, i * pixels_count * 4,
			width, height, film_channels_count,
			width, height, 0, 0);

		film_pixels.clear();
		film_pixels.shrink_to_fit();
	}

	alpha_pixels.clear();
	alpha_pixels.shrink_to_fit();
}