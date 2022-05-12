#include <vector>
#include <fstream>
#include <xsi_string.h>
#include "logs.h"
#include "export_common.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

float clamp_float(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

void write_ppm(const std::string &path, int width, int height, int components, const std::vector<float> &pixels)
{
	std::ofstream file(path);
	file << "P3\n" << width << ' ' << height << "\n255\n";
	for (int j = height - 1; j >= 0; --j)
	{
		for (int i = 0; i < width; ++i)
		{
			float r = 256 * clamp_float(pixels[(j * width + i) * components], 0.0, 0.99);
			float g = 256 * clamp_float(pixels[(j * width + i) * components + 1], 0.0, 0.99);
			float b = 256 * clamp_float(pixels[(j * width + i) * components + 2], 0.0, 0.99);

			file << static_cast<int>(r) << ' '
				 << static_cast<int>(g) << ' '
				 << static_cast<int>(b) << '\n';
		}
	}

	file.close();
}

bool write_exr(const XSI::CString &path, int width, int height, int input_components, const std::vector<float> &pixels, int output_components)
{
	//we should flip vertical coordinates of pixels
	std::vector<float> flip_pixels(pixels.size(), 0.0f);
	LONG row = 0;
	LONG column = 0;
	LONG flip_pixel = 0;
	for (LONG pixel = 0; pixel < width * height; pixel++)
	{
		row = pixel / width;
		column = pixel - row * width;
		flip_pixel = (height - row - 1) * width + column;
		//copy all input components
		for (LONG c = 0; c < input_components; c++)
		{
			flip_pixels[input_components * flip_pixel + c] = pixels[input_components * pixel + c];
		}
	}

	if (output_components == input_components)  // input_components = 4
	{
		const char* err;
		int out = SaveEXR(&flip_pixels[0], width, height, 4, 0, path.GetAsciiString(), &err);
		flip_pixels.clear();
		flip_pixels.shrink_to_fit();
		return out == 0;
	}

	//if input_components != output_components

	EXRHeader header;
	InitEXRHeader(&header);

	EXRImage image;
	InitEXRImage(&image);

	image.num_channels = output_components;
	std::vector<float> images[3];
	size_t pixels_count = static_cast<size_t>(width) * height;
	images[0].resize(pixels_count);
	images[1].resize(pixels_count);
	images[2].resize(pixels_count);

	for (int i = 0; i < width * height; i++)
	{
		images[0][i] = flip_pixels[input_components * i + 0];
		images[1][i] = flip_pixels[input_components * i + 1];
		images[2][i] = flip_pixels[input_components * i + 2];
	}

	float* image_ptr[3];
	image_ptr[0] = &(images[2].at(0)); // B
	image_ptr[1] = &(images[1].at(0)); // G
	image_ptr[2] = &(images[0].at(0)); // R

	image.images = (unsigned char**)image_ptr;
	
	image.width = width;
	image.height = height;

	header.num_channels = output_components;
	header.channels = (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels);
	// Must be BGR(A) order, since most of EXR viewers expect this channel order.
	if (output_components == 3)
	{
		strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
		strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
		strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';
	}
	else if (output_components == 2)
	{
		strncpy(header.channels[0].name, "G", 255); header.channels[0].name[strlen("G")] = '\0';
		strncpy(header.channels[1].name, "R", 255); header.channels[1].name[strlen("R")] = '\0';
	}
	else if (output_components == 1)
	{
		strncpy(header.channels[0].name, "A", 255); header.channels[0].name[strlen("A")] = '\0';
	}

	header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
	header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
	for (int i = 0; i < header.num_channels; i++) 
	{
		header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
		header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
	}

	const char* err;
	int ret = SaveEXRImageToFile(&image, &header, path.GetAsciiString(), &err);

	free(header.channels);
	free(header.pixel_types);
	free(header.requested_pixel_types);
	images[0].clear();
	images[0].shrink_to_fit();
	images[1].clear();
	images[1].shrink_to_fit();
	images[2].clear();
	images[2].shrink_to_fit();

	flip_pixels.clear();
	flip_pixels.shrink_to_fit();

	return ret == TINYEXR_SUCCESS;
}

bool write_float(const XSI::CString &path, const XSI::CString &ext, const XSI::CString &data_type, int width, int height, int components, const std::vector<float> &pixels)
{
	stbi_flip_vertically_on_write(true);
	std::string output_path = path.GetAsciiString();
	if (create_dir(output_path))
	{
		int output_components = data_type.Length();
		if (ext == "ppm")
		{
			write_ppm(output_path, width, height, components, pixels);
			return true;
		}
		if (ext == "png" || ext == "bmp" || ext == "tga" || ext == "jpg")
		{
			size_t otput_pixels_size = static_cast<size_t>(width) * height * output_components;
			unsigned char* u_pixels = new unsigned char[otput_pixels_size];
			for (size_t y = 0; y < height; y++)
			{
				for (size_t x = 0; x < width; x++)
				{
					for (size_t c = 0; c < output_components; c++)
					{
						u_pixels[(y * width + x) * output_components + c] = static_cast<unsigned char>(int(pixels[(y * width + x) * components + c] * 255.99f));
					}
				}
			}

			int out = 0;

			if (ext == "png")
			{
				out = stbi_write_png(path.GetAsciiString(), width, height, output_components, u_pixels, width * output_components);
			}
			else if (ext == "bmp")
			{
				out = stbi_write_bmp(path.GetAsciiString(), width, height, output_components, u_pixels);
			}
			else if (ext == "tga")
			{
				out = stbi_write_tga(path.GetAsciiString(), width, height, output_components, u_pixels);
			}
			else if (ext == "jpg")
			{
				out = stbi_write_jpg(path.GetAsciiString(), width, height, output_components, u_pixels, 100);
			}

			delete[]u_pixels;

			return out > 0;
		}
		else if (ext == "hdr")
		{
			//we assume that input pixels always has 4 components
			//if render engine produce image with less components, then we should extend it to 4 components and fill output_pixels buffer
			float* f_pixels = new float[pixels.size()];
			for (size_t i = 0; i < pixels.size(); i++)
			{
				f_pixels[i] = (float)pixels[i];
			}

			int out = stbi_write_hdr(path.GetAsciiString(), width, height, components, f_pixels);
			delete[]f_pixels;
			return out > 0;
		}
		else if (ext == "exr")
		{
			return write_exr(path, width, height, 4, pixels, output_components);
		}
		else
		{
			//unknow file extension
			return false;
		}
	}
	else
	{
		log_message("Fails to create dirctory for the path " + path, XSI::siWarningMsg);
		return false;
	}
}