#include "lux_session.h"
#include "../../utilities/logs.h"

void read_visual_buffer(luxcore::Film &film, RenderVisualBuffer& buffer)
{
	//these two aov should be selected in the render config
	std::vector<float> film_pixels(film.GetOutputSize(luxcore::Film::OUTPUT_RGB));
	std::vector<float> alpha_pixels(film.GetOutputSize(luxcore::Film::OUTPUT_ALPHA));
	film.GetOutput<float>(luxcore::Film::OUTPUT_RGB_IMAGEPIPELINE, film_pixels.data(), 0, true);
	film.GetOutput<float>(luxcore::Film::OUTPUT_ALPHA, alpha_pixels.data(), 0, true);
	//next we should copy pixels for selected rectangle
	for (size_t y = 0; y < buffer.height; y++)
	{
		for (size_t x = 0; x < buffer.width; x++)
		{
			for (int c = 0; c < 3; c++)
			{
				buffer.pixels[(y * buffer.width + x) * 4 + c] = film_pixels[((y + buffer.corner_y) * buffer.full_width + (x + buffer.corner_x)) * 3 + c] * 1.0;
			}
			buffer.pixels[(y * buffer.width + x) * 4 + 3] = alpha_pixels[(y + buffer.corner_y) * buffer.full_width + (x + buffer.corner_x)];
		}
	}

	film_pixels.clear();
	alpha_pixels.clear();
	film_pixels.shrink_to_fit();
	alpha_pixels.shrink_to_fit();
}