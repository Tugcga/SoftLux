#include "lux_session.h"
#include "../../utilities/xsi_shaders.h"
#include "../../utilities/logs.h"
#include "../../utilities/export_common.h"

#include "xsi_project.h"
#include "xsi_scene.h"
#include "xsi_pass.h"
#include "xsi_shaderparameter.h"
#include "xsi_application.h"
#include "xsi_imageclip2.h"
#include "xsi_image.h"
#include "xsi_utils.h"

#include <map>

void update_pipline_map(std::map<int, XSI::Shader>& pipline_map, const XSI::CRefArray& pass_shaders, std::vector<XSI::ShaderParameter>& pipline_shader, const XSI::CString& name)
{
	pipline_shader = get_root_shader_parameter(pass_shaders, GRSPM_NodeName, "", false, "LUXShadersPlugin", name);
	if (pipline_shader.size() > 0)
	{//find the port, connected to the shader
		XSI::CString port_name = pipline_shader[0].GetName();
		//get shader
		XSI::Shader node = get_input_node(pipline_shader[0]);

		//conver port name to the index
		ULONG p = port_name.FindString("Item");
		if (p < UINT_MAX)
		{
			int index = 0;
			if (port_name != "Item")
			{
				XSI::CString number_str = port_name.GetSubString(p + 5, port_name.Length() - p - 6);
				if (number_str.Length() > 0)
				{//convert number to string
					index = std::stoi(number_str.GetAsciiString());
				}
			}
			if (index >= 0)
			{
				pipline_map[index] = node;
			}
		}
	}
}

void sync_imagepipline(luxrays::Properties &render_props, bool &is_contour_lines, const XSI::CTime& eval_time)
{
	XSI::Project xsi_project = XSI::Application().GetActiveProject();
	XSI::Scene xsi_scene = xsi_project.GetActiveScene();
	XSI::Pass xsi_pass = xsi_scene.GetActivePass();
	XSI::CRefArray pass_shaders = xsi_pass.GetAllShaders();
	//next we should try to find all available output pass shaders
	std::vector<XSI::ShaderParameter> pipline_shader;
	std::map<int, XSI::Shader> pipline_map;  // key - index in thelist, value - link to the shader node
	
	if(pass_shaders.GetCount() > 0)
	{
		for (size_t i = 0; i < pipline_names.size(); i++)
		{
			update_pipline_map(pipline_map, pass_shaders, pipline_shader, XSI::CString(pipline_names[i].c_str()));
		}
	}
	
	int pipline_index = 0;
	if (pipline_map.size() > 0)
	{
		for (const auto& [key, value] : pipline_map)
		{
			std::string index_str = "0." + std::to_string(pipline_index);
			std::string prefix = "film.imagepipelines." + index_str;
			XSI::CString name = value.GetProgID().Split(".")[1];
			XSI::CParameterRefArray all_parameters = value.GetParameters();
			bool enable = get_bool_parameter_value(all_parameters, "enable", eval_time);
			if (enable)
			{
				if (name == "PassTonemapLinear")
				{
					float scale = get_float_parameter_value(all_parameters, "scale", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("TONEMAP_LINEAR"));
					render_props.Set(luxrays::Property(prefix + ".scale")(scale));
				}
				else if (name == "PassTonemapReinhard")
				{
					float prescale = get_float_parameter_value(all_parameters, "prescale", eval_time);
					float postscale = get_float_parameter_value(all_parameters, "postscale", eval_time);
					float burn = get_float_parameter_value(all_parameters, "burn", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("TONEMAP_REINHARD02"));
					render_props.Set(luxrays::Property(prefix + ".prescale")(prescale));
					render_props.Set(luxrays::Property(prefix + ".postscale")(postscale));
					render_props.Set(luxrays::Property(prefix + ".burn")(burn));
				}
				else if (name == "PassTonemapAutoLinear")
				{
					render_props.Set(luxrays::Property(prefix + ".type")("TONEMAP_AUTOLINEAR"));
				}
				else if (name == "PassTonemapLuxLinear")
				{
					float sensitivity = get_float_parameter_value(all_parameters, "sensitivity", eval_time);
					float exposure = get_float_parameter_value(all_parameters, "exposure", eval_time);
					float fstop = get_float_parameter_value(all_parameters, "fstop", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("TONEMAP_LUXLINEAR"));
					render_props.Set(luxrays::Property(prefix + ".sensitivity")(sensitivity));
					render_props.Set(luxrays::Property(prefix + ".exposure")(exposure));
					render_props.Set(luxrays::Property(prefix + ".fstop")(fstop));
				}
				else if (name == "PassGammaCorrection")
				{
					float value = get_float_parameter_value(all_parameters, "value", eval_time);
					int table_size = get_int_parameter_value(all_parameters, "table_size", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("GAMMA_CORRECTION"));
					render_props.Set(luxrays::Property(prefix + ".value")(value));
					render_props.Set(luxrays::Property(prefix + ".table.size")(table_size));
				}
				else if (name == "PassNOP")
				{
					render_props.Set(luxrays::Property(prefix + ".type")("NOP"));
				}
				else if (name == "PassOutputSwitcher")
				{
					std::string channel = std::string(get_string_parameter_value(all_parameters, "channel", eval_time).GetAsciiString());
					int index = get_int_parameter_value(all_parameters, "index", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("OUTPUT_SWITCHER"));
					render_props.Set(luxrays::Property(prefix + ".channel")(channel));
					render_props.Set(luxrays::Property(prefix + ".index")(index));
				}
				else if (name == "PassGaussianFilter")
				{
					float weight = get_float_parameter_value(all_parameters, "weight", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("GAUSSIANFILTER_3x3"));
					render_props.Set(luxrays::Property(prefix + ".weight")(weight));
				}
				else if (name == "PassCameraResponse")
				{
					std::string name = std::string(get_string_parameter_value(all_parameters, "camera_name", eval_time).GetAsciiString());

					render_props.Set(luxrays::Property(prefix + ".type")("CAMERA_RESPONSE_FUNC"));
					render_props.Set(luxrays::Property(prefix + ".name")(name));
				}
				else if (name == "PassContourLines")
				{
					float scale = get_float_parameter_value(all_parameters, "scale", eval_time);
					float range = get_float_parameter_value(all_parameters, "range", eval_time);
					int steps = get_int_parameter_value(all_parameters, "steps", eval_time);
					int zerogridsize = get_int_parameter_value(all_parameters, "zerogridsize", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("CONTOUR_LINES"));
					render_props.Set(luxrays::Property(prefix + ".scale")(scale));
					render_props.Set(luxrays::Property(prefix + ".range")(range));
					render_props.Set(luxrays::Property(prefix + ".steps")(steps));
					render_props.Set(luxrays::Property(prefix + ".zerogridsize")(zerogridsize));

					is_contour_lines = true;
				}
				else if (name == "PassBackgroundImage")
				{
					float gamma = get_float_parameter_value(all_parameters, "gamma", eval_time);
					XSI::ShaderParameter file_param = all_parameters.GetItem("file");
					XSI::Parameter file_param_final = get_source_parameter(file_param);
					XSI::CRef file_source = file_param_final.GetSource();
					if (file_source.IsValid())
					{
						XSI::ImageClip2 clip(file_source);
						std::string file = std::string(clip.GetFileName().GetAsciiString());
						if (file.size() > 0)
						{
							render_props.Set(luxrays::Property(prefix + ".type")("BACKGROUND_IMG"));
							render_props.Set(luxrays::Property(prefix + ".file")(file));
							render_props.Set(luxrays::Property(prefix + ".gamma")(gamma));
						}
						else
						{
							pipline_index--;
						}
					}
					else
					{
						pipline_index--;
					}
				}
				else if (name == "PassBloom")
				{
					float radius = get_float_parameter_value(all_parameters, "radius", eval_time);
					float weight = get_float_parameter_value(all_parameters, "weight", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("BLOOM"));
					render_props.Set(luxrays::Property(prefix + ".radius")(radius));
					render_props.Set(luxrays::Property(prefix + ".weight")(weight));
				}
				else if (name == "PassObjectIDMask")
				{
					int id = get_int_parameter_value(all_parameters, "id", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("OBJECT_ID_MASK"));
					render_props.Set(luxrays::Property(prefix + ".id")(id));
				}
				else if (name == "PassVignetting")
				{
					float scale = get_float_parameter_value(all_parameters, "scale", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("VIGNETTING"));
					render_props.Set(luxrays::Property(prefix + ".scale")(scale));
				}
				else if (name == "PassColorAberration")
				{
					float amount = get_float_parameter_value(all_parameters, "amount", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("COLOR_ABERRATION"));
					render_props.Set(luxrays::Property(prefix + ".amount")(amount));
				}
				else if (name == "PassPremultiplyAlpha")
				{
					render_props.Set(luxrays::Property(prefix + ".type")("PREMULTIPLY_ALPHA"));
				}
				else if (name == "PassMist")
				{
					XSI::MATH::CColor4f color = get_color_parameter_value(all_parameters, "color", eval_time);
					float amount = get_float_parameter_value(all_parameters, "amount", eval_time);
					float startdistance = get_float_parameter_value(all_parameters, "startdistance", eval_time);
					float enddistance = get_float_parameter_value(all_parameters, "enddistance", eval_time);
					bool excludebackground = get_bool_parameter_value(all_parameters, "excludebackground", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("MIST"));
					render_props.Set(luxrays::Property(prefix + ".color")(color.GetR(), color.GetG(), color.GetB()));
					render_props.Set(luxrays::Property(prefix + ".amount")(amount));
					render_props.Set(luxrays::Property(prefix + ".startdistance")(startdistance));
					render_props.Set(luxrays::Property(prefix + ".enddistance")(enddistance));
					render_props.Set(luxrays::Property(prefix + ".excludebackground")(excludebackground));
				}
				else if (name == "PassBCDDenoiser")
				{
					float warmupspp = get_float_parameter_value(all_parameters, "warmupspp", eval_time);
					float histdistthresh = get_float_parameter_value(all_parameters, "histdistthresh", eval_time);
					int patchradius = get_int_parameter_value(all_parameters, "patchradius", eval_time);
					int searchwindowradius = get_int_parameter_value(all_parameters, "searchwindowradius", eval_time);
					float mineigenvalue = get_float_parameter_value(all_parameters, "mineigenvalue", eval_time);
					bool userandompixelorder = get_bool_parameter_value(all_parameters, "userandompixelorder", eval_time);
					float markedpixelsskippingprobability = get_float_parameter_value(all_parameters, "markedpixelsskippingprobability", eval_time);
					int threadcount = get_int_parameter_value(all_parameters, "threadcount", eval_time);
					int scales = get_int_parameter_value(all_parameters, "scales", eval_time);
					bool filterspikes = get_bool_parameter_value(all_parameters, "filterspikes", eval_time);
					bool applydenoise = get_bool_parameter_value(all_parameters, "applydenoise", eval_time);
					float spikestddev = get_float_parameter_value(all_parameters, "spikestddev", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("BCD_DENOISER"));
					render_props.Set(luxrays::Property(prefix + ".warmupspp")(warmupspp));
					render_props.Set(luxrays::Property(prefix + ".histdistthresh")(histdistthresh));
					render_props.Set(luxrays::Property(prefix + ".patchradius")(patchradius));
					render_props.Set(luxrays::Property(prefix + ".searchwindowradius")(searchwindowradius));
					render_props.Set(luxrays::Property(prefix + ".mineigenvalue")(mineigenvalue));
					render_props.Set(luxrays::Property(prefix + ".userandompixelorder")(userandompixelorder));
					render_props.Set(luxrays::Property(prefix + ".markedpixelsskippingprobability")(markedpixelsskippingprobability));
					render_props.Set(luxrays::Property(prefix + ".threadcount")(threadcount));
					render_props.Set(luxrays::Property(prefix + ".scales")(scales));
					render_props.Set(luxrays::Property(prefix + ".filterspikes")(filterspikes));
					render_props.Set(luxrays::Property(prefix + ".applydenoise")(applydenoise));
					render_props.Set(luxrays::Property(prefix + ".spikestddev")(spikestddev));
				}
				else if (name == "PassPatterns")
				{
					int index = get_int_parameter_value(all_parameters, "index", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("PATTERNS"));
					render_props.Set(luxrays::Property(prefix + ".index")(index));
				}
				else if (name == "PassIntelOIDN")
				{
					std::string filter_type = std::string(get_string_parameter_value(all_parameters, "filter_type", eval_time).GetAsciiString());
					int oidnmemory = get_int_parameter_value(all_parameters, "oidnmemory", eval_time);
					float sharpness = get_float_parameter_value(all_parameters, "sharpness", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("INTEL_OIDN"));
					render_props.Set(luxrays::Property(prefix + ".filter.type")(filter_type));
					render_props.Set(luxrays::Property(prefix + ".oidnmemory")(oidnmemory));
					render_props.Set(luxrays::Property(prefix + ".sharpness")(sharpness));
				}
				else if (name == "PassWhiteBalance")
				{
					float temperature = get_float_parameter_value(all_parameters, "temperature", eval_time);
					bool reverse = get_bool_parameter_value(all_parameters, "reverse", eval_time);
					bool normalize = get_bool_parameter_value(all_parameters, "normalize", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("WHITE_BALANCE"));
					render_props.Set(luxrays::Property(prefix + ".temperature")(temperature));
					render_props.Set(luxrays::Property(prefix + ".reverse")(reverse));
					render_props.Set(luxrays::Property(prefix + ".normalize")(normalize));
				}
				else if (name == "PassBakeMapMargin")
				{
					int margin = get_int_parameter_value(all_parameters, "margin", eval_time);
					float samplesthreshold = get_float_parameter_value(all_parameters, "samplesthreshold", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("BAKEMAP_MARGIN"));
					render_props.Set(luxrays::Property(prefix + ".margin")(margin));
					render_props.Set(luxrays::Property(prefix + ".samplesthreshold")(samplesthreshold));
				}
				else if (name == "PassColorLUT")
				{
					XSI::CString xsi_file = resolve_path(get_string_parameter_value(all_parameters, "file", eval_time));
					float strength = get_float_parameter_value(all_parameters, "strength", eval_time);

					if (xsi_file.Length() > 0)
					{
						render_props.Set(luxrays::Property(prefix + ".type")("COLOR_LUT"));
						render_props.Set(luxrays::Property(prefix + ".file")(std::string(xsi_file.GetAsciiString())));
						render_props.Set(luxrays::Property(prefix + ".strength")(strength));
					}
					else
					{
						pipline_index--;
					}
				}
				else if (name == "PassOptixDenoiser")
				{
					float sharpness = get_float_parameter_value(all_parameters, "sharpness", eval_time);
					int minspp = get_int_parameter_value(all_parameters, "minspp", eval_time);

					render_props.Set(luxrays::Property(prefix + ".type")("OPTIX_DENOISER"));
					render_props.Set(luxrays::Property(prefix + ".sharpness")(sharpness));
					render_props.Set(luxrays::Property(prefix + ".minspp")(minspp));
				}
				/*else if (name == "PassTonemapOpenColorIO")
				{

				}*/

				pipline_index++;
			}
		}
	}

	if (pipline_index == 0)
	{//there are no pipline in the pass
		//add the default one
		render_props.Set(luxrays::Property("film.imagepipelines.0.0.type")("TONEMAP_LINEAR"));
		render_props.Set(luxrays::Property("film.imagepipelines.0.1.type")("GAMMA_CORRECTION"));
	}
}