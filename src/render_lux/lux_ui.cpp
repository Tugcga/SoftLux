#include "render_engine_lux.h"

#include <xsi_parameter.h>
#include <xsi_customproperty.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgitem.h>
#include <xsi_context.h>
#include <xsi_ppgeventcontext.h>

const XSI::siCapabilities block_mode = XSI::siReadOnly;  // siReadOnly or siNotInspectable

void build_layout(XSI::PPGLayout& layout)
{
	layout.Clear();

	//--------------------------------------------------------------
	layout.AddTab("Quality");
	XSI::CValueArray halt_condition_combo(6);
	halt_condition_combo[0] = "Samples"; halt_condition_combo[1] = 0;
	halt_condition_combo[2] = "Time"; halt_condition_combo[3] = 1;
	halt_condition_combo[4] = "Noise Level"; halt_condition_combo[5] = 2;
	layout.AddGroup("Halt Condition");
	layout.AddEnumControl("halt_condition", halt_condition_combo, "Mode", XSI::siControlCombo);
	layout.AddItem("halt_samples", "Samples");
	layout.AddItem("halt_time", "Time (in seconds)");
	layout.AddItem("halt_noise", "Noise Level");
	layout.AddItem("halt_noise_warmup", "Warmup Samples");
	layout.AddItem("halt_noise_step", "Samples Step");
	layout.AddItem("halt_noise_filter", "Filter");
	layout.EndGroup();

	//--------------------------------------------------------------
	layout.AddTab("Sampler");
	XSI::CValueArray sampler_combo(6);
	sampler_combo[0] = "Sobol"; sampler_combo[1] = 0;
	sampler_combo[2] = "Metropolis"; sampler_combo[3] = 1;
	sampler_combo[4] = "Random"; sampler_combo[5] = 2;
	layout.AddGroup("Sampler");
	layout.AddEnumControl("sampler_type", sampler_combo, "Sampler Type", XSI::siControlCombo);
	layout.AddItem("sampler_sobol_strength", "Strength");
	layout.AddItem("sampler_metropolis_largesteprate", "Large Step Rate");
	layout.AddItem("sampler_metropolis_maxconsecutivereject", "Max Reject");
	layout.AddItem("sampler_metropolis_imagemutationrate", "Mutation Rate");
	layout.EndGroup();

	layout.AddGroup("Adaptive Noise Estimation");
	layout.AddItem("noiseestimation_warmup", "Warmup Samples");
	layout.AddItem("noiseestimation_step", "Samples Step");
	layout.AddItem("noiseestimation_filter_scale", "Filter Scale");
	layout.EndGroup();

	//--------------------------------------------------------------
	layout.AddTab("Engine");
	XSI::CValueArray engine_combo(6);
	engine_combo[0] = "PATH CPU"; engine_combo[1] = 0;
	engine_combo[2] = "BIDIR CPU"; engine_combo[3] = 1;
	engine_combo[4] = "PATH OCL"; engine_combo[5] = 2;
	layout.AddGroup("Engine");
	layout.AddEnumControl("engine", engine_combo, "Engine", XSI::siControlCombo);
	layout.AddItem("cpu_threads_count", "Threads");
	layout.AddItem("pathocl_pixelatomics", "Atomic Operations");
	layout.EndGroup();

	layout.AddGroup("Max Bounces");
	layout.AddItem("path_total", "Total");
	layout.AddItem("path_diffuse", "Diffuse");
	layout.AddItem("path_glossy", "Glossy");
	layout.AddItem("path_specular", "Specular");
	layout.EndGroup();

	layout.AddGroup("Depth");
	layout.AddItem("path_maxdepth", "Path Depth");
	layout.AddItem("light_maxdepth", "Light Depth");
	layout.EndGroup();

	layout.AddGroup("Russian Roulettes");
	layout.AddItem("path_russianroulette_depth", "Depth");
	layout.AddItem("path_russianroulette_cap", "Threshold");
	layout.EndGroup();

	layout.AddGroup("Other Settings");
	layout.AddItem("path_clamping_max", "Clamping");
	layout.AddItem("path_background", "Premultiplied Alpha");
	layout.EndGroup();

	//--------------------------------------------------------------
	layout.AddTab("Filter");
	layout.AddGroup("Filter");
	XSI::CValueArray filter_combo(12);
	filter_combo[0] = "None"; filter_combo[1] = 0;
	filter_combo[2] = "Box"; filter_combo[3] = 1;
	filter_combo[4] = "Gaussian"; filter_combo[5] = 2;
	filter_combo[6] = "Mitchel"; filter_combo[7] = 3;
	filter_combo[8] = "Mitchel super-sampling"; filter_combo[9] = 4;
	filter_combo[10] = "Blcakman-Harris"; filter_combo[11] = 5;
	layout.AddEnumControl("filter_type", filter_combo, "Type", XSI::siControlCombo);
	layout.AddItem("filter_width", "Width");
	layout.AddItem("filter_gaussian_alpha", "Alpha");
	layout.AddItem("filter_mitchel_b", "B");
	layout.AddItem("filter_mitchel_c", "C");
	layout.EndGroup();

	//--------------------------------------------------------------
	layout.AddTab("Motion");
	layout.AddGroup("Objects");
	layout.AddItem("motion_objects", "Objects Motion Blur");
	layout.AddItem("motion_shutter_time", "Shutter Time");
	layout.AddItem("motion_steps", "Motion Steps");
	layout.EndGroup();

	//--------------------------------------------------------------
	layout.AddTab("Light Strategy");
	layout.AddGroup("Parameters");
	XSI::CValueArray stratagy_combo(8);
	stratagy_combo[0] = "Uniform"; stratagy_combo[1] = 0;
	stratagy_combo[2] = "Power"; stratagy_combo[3] = 1;
	stratagy_combo[4] = "Log Power"; stratagy_combo[5] = 2;
	stratagy_combo[6] = "DLS Cache"; stratagy_combo[7] = 3;
	layout.AddEnumControl("lightstrategy_mode", stratagy_combo, "Mode", XSI::siControlCombo);
	layout.EndGroup();
	layout.AddGroup("DLS Cache");
	layout.AddItem("lightstrategy_radius", "Radius");
	layout.AddItem("lightstrategy_normalangle", "Normal Angle");
	layout.AddItem("lightstrategy_maxpasses", "Max Passes");
	layout.AddItem("lightstrategy_convergencethreshold", "Threshold");
	layout.AddItem("lightstrategy_warmupsamples", "Warm Samples");
	layout.AddItem("lightstrategy_targetcachehitratio", "Hit Ration");
	layout.AddItem("lightstrategy_maxdepth", "Max Depth");
	layout.AddItem("lightstrategy_maxsamplescount", "Max Samples");
	layout.EndGroup();

	//--------------------------------------------------------------
	layout.AddTab("Service");
	layout.AddGroup("Logging");
	layout.AddItem("service_log_luxcore", "Luxcore Log");
	layout.AddItem("service_log_rendertime", "Rendertime");
	layout.EndGroup();

	layout.AddGroup("Progressive");
	layout.AddItem("service_update", "Update");
	layout.EndGroup();

	layout.AddGroup("Random");
	layout.AddItem("service_seed", "Seed");
	layout.EndGroup();

	layout.AddGroup("Accelerator");
	XSI::CValueArray accelerator_combo(8);
	accelerator_combo[0] = "Auto"; accelerator_combo[1] = 0;
	accelerator_combo[2] = "BVH"; accelerator_combo[3] = 1;
	accelerator_combo[4] = "MBVH"; accelerator_combo[5] = 2;
	accelerator_combo[6] = "Embree"; accelerator_combo[7] = 3;
	layout.AddEnumControl("service_accelerator", accelerator_combo, "Type", XSI::siControlCombo);
	layout.AddItem("service_instances", "Instances");
	layout.EndGroup();

	layout.AddGroup("Shape AOV");
	layout.AddItem("service_export_pointness", "Include Pointness");
	layout.AddItem("service_export_random_islands", "Include Island AOV");
	layout.EndGroup();
	//--------------------------------------------------------------
	layout.AddTab("Export");
	layout.AddGroup("Parameters");
	XSI::CValueArray export_mode_combo(6);
	export_mode_combo[0] = "Text-based files"; export_mode_combo[1] = 0;
	export_mode_combo[2] = "Binary file"; export_mode_combo[3] = 1;
	export_mode_combo[4] = "*.rsm file"; export_mode_combo[5] = 2;
	layout.AddEnumControl("export_mode", export_mode_combo, "Mode", XSI::siControlCombo);
	layout.EndGroup();
}

void set_halt_condition(XSI::CustomProperty& prop, const int halt_condition)
{
	XSI::CParameterRefArray prop_array = prop.GetParameters();
	XSI::Parameter halt_samples = prop_array.GetItem("halt_samples");
	halt_samples.PutCapabilityFlag(block_mode, !(halt_condition == 0));

	XSI::Parameter halt_time = prop_array.GetItem("halt_time");
	halt_time.PutCapabilityFlag(block_mode, !(halt_condition == 1));

	XSI::Parameter halt_noise = prop_array.GetItem("halt_noise");
	halt_noise.PutCapabilityFlag(block_mode, !(halt_condition == 2));

	XSI::Parameter halt_noise_warmup = prop_array.GetItem("halt_noise_warmup");
	halt_noise_warmup.PutCapabilityFlag(block_mode, !(halt_condition == 2));

	XSI::Parameter halt_noise_step = prop_array.GetItem("halt_noise_step");
	halt_noise_step.PutCapabilityFlag(block_mode, !(halt_condition == 2));

	XSI::Parameter halt_noise_filter = prop_array.GetItem("halt_noise_filter");
	halt_noise_filter.PutCapabilityFlag(block_mode, !(halt_condition == 2));
}

void set_filter_type(XSI::CustomProperty& prop, const int filter_type)
{
	XSI::CParameterRefArray prop_array = prop.GetParameters();
	XSI::Parameter filter_width = prop_array.GetItem("filter_width");
	filter_width.PutCapabilityFlag(block_mode, !(filter_type != 0));

	XSI::Parameter filter_gaussian_alpha = prop_array.GetItem("filter_gaussian_alpha");
	filter_gaussian_alpha.PutCapabilityFlag(block_mode, !(filter_type == 2));

	XSI::Parameter filter_mitchel_b = prop_array.GetItem("filter_mitchel_b");
	filter_mitchel_b.PutCapabilityFlag(block_mode, !(filter_type == 3 || filter_type == 4));

	XSI::Parameter filter_mitchel_c = prop_array.GetItem("filter_mitchel_c");
	filter_mitchel_c.PutCapabilityFlag(block_mode, !(filter_type == 3 || filter_type == 4));
}

void set_sampler_type(XSI::CustomProperty& prop, const int sampler_type)
{
	XSI::CParameterRefArray prop_array = prop.GetParameters();
	XSI::Parameter sampler_strength = prop_array.GetItem("sampler_strength");
	sampler_strength.PutCapabilityFlag(block_mode, !(sampler_type == 0 || sampler_type == 2));

	XSI::Parameter sampler_metropolis_largesteprate = prop_array.GetItem("sampler_metropolis_largesteprate");
	sampler_metropolis_largesteprate.PutCapabilityFlag(block_mode, !(sampler_type == 1));

	XSI::Parameter sampler_metropolis_maxconsecutivereject = prop_array.GetItem("sampler_metropolis_maxconsecutivereject");
	sampler_metropolis_maxconsecutivereject.PutCapabilityFlag(block_mode, !(sampler_type == 1));

	XSI::Parameter sampler_metropolis_imagemutationrate = prop_array.GetItem("sampler_metropolis_imagemutationrate");
	sampler_metropolis_imagemutationrate.PutCapabilityFlag(block_mode, !(sampler_type == 1));

	XSI::Parameter noiseestimation_warmup = prop_array.GetItem("noiseestimation_warmup");
	noiseestimation_warmup.PutCapabilityFlag(block_mode, !(sampler_type == 0 || sampler_type == 2));

	XSI::Parameter noiseestimation_step = prop_array.GetItem("noiseestimation_step");
	noiseestimation_step.PutCapabilityFlag(block_mode, !(sampler_type == 0 || sampler_type == 2));

	XSI::Parameter noiseestimation_filter_scale = prop_array.GetItem("noiseestimation_filter_scale");
	noiseestimation_filter_scale.PutCapabilityFlag(block_mode, !(sampler_type == 0 || sampler_type == 2));
}

void set_engine_type(XSI::CustomProperty& prop, const int engine_type)
{
	//0 - path cpu, 1 - bdir cpu, 2 - path ocl
	XSI::CParameterRefArray prop_array = prop.GetParameters();
	XSI::Parameter cpu_threads_count = prop_array.GetItem("cpu_threads_count");
	cpu_threads_count.PutCapabilityFlag(block_mode, !(engine_type == 0 || engine_type == 1));

	XSI::Parameter pathocl_pixelatomics = prop_array.GetItem("pathocl_pixelatomics");
	pathocl_pixelatomics.PutCapabilityFlag(block_mode, !(engine_type == 2));

	XSI::Parameter path_total = prop_array.GetItem("path_total");
	path_total.PutCapabilityFlag(block_mode, !(engine_type == 0));

	XSI::Parameter path_diffuse = prop_array.GetItem("path_diffuse");
	path_diffuse.PutCapabilityFlag(block_mode, !(engine_type == 0));

	XSI::Parameter path_glossy = prop_array.GetItem("path_glossy");
	path_glossy.PutCapabilityFlag(block_mode, !(engine_type == 0));

	XSI::Parameter path_specular = prop_array.GetItem("path_specular");
	path_specular.PutCapabilityFlag(block_mode, !(engine_type == 0));

	XSI::Parameter path_maxdepth = prop_array.GetItem("path_maxdepth");
	path_maxdepth.PutCapabilityFlag(block_mode, !(engine_type == 1));

	XSI::Parameter light_maxdepth = prop_array.GetItem("light_maxdepth");
	light_maxdepth.PutCapabilityFlag(block_mode, !(engine_type == 1));

	XSI::Parameter path_russianroulette_depth = prop_array.GetItem("path_russianroulette_depth");
	path_russianroulette_depth.PutCapabilityFlag(block_mode, !(engine_type == 0 || engine_type == 1));

	XSI::Parameter path_russianroulette_cap = prop_array.GetItem("path_russianroulette_cap");
	path_russianroulette_cap.PutCapabilityFlag(block_mode, !(engine_type == 0 || engine_type == 1));

	XSI::Parameter path_clamping_max = prop_array.GetItem("path_clamping_max");
	path_clamping_max.PutCapabilityFlag(block_mode, !(engine_type == 0));

	XSI::Parameter path_background = prop_array.GetItem("path_background");
	path_background.PutCapabilityFlag(block_mode, !(engine_type == 0));
}

void set_motion(XSI::CustomProperty& prop, const bool motion_objects)
{
	XSI::CParameterRefArray prop_array = prop.GetParameters();
	XSI::Parameter motion_shutter_time = prop_array.GetItem("motion_shutter_time");
	motion_shutter_time.PutCapabilityFlag(block_mode, !(motion_objects));

	XSI::Parameter motion_steps = prop_array.GetItem("motion_steps");
	motion_steps.PutCapabilityFlag(block_mode, !(motion_objects));
}

void set_light_strategy(XSI::CustomProperty& prop, const int light_stretagy)
{
	XSI::CParameterRefArray prop_array = prop.GetParameters();
	XSI::Parameter lightstrategy_radius = prop_array.GetItem("lightstrategy_radius");
	lightstrategy_radius.PutCapabilityFlag(block_mode, !(light_stretagy == 3));

	XSI::Parameter lightstrategy_normalangle = prop_array.GetItem("lightstrategy_normalangle");
	lightstrategy_normalangle.PutCapabilityFlag(block_mode, !(light_stretagy == 3));

	XSI::Parameter lightstrategy_maxpasses = prop_array.GetItem("lightstrategy_maxpasses");
	lightstrategy_maxpasses.PutCapabilityFlag(block_mode, !(light_stretagy == 3));

	XSI::Parameter lightstrategy_convergencethreshold = prop_array.GetItem("lightstrategy_convergencethreshold");
	lightstrategy_convergencethreshold.PutCapabilityFlag(block_mode, !(light_stretagy == 3));

	XSI::Parameter lightstrategy_warmupsamples = prop_array.GetItem("lightstrategy_warmupsamples");
	lightstrategy_warmupsamples.PutCapabilityFlag(block_mode, !(light_stretagy == 3));

	XSI::Parameter lightstrategy_targetcachehitratio = prop_array.GetItem("lightstrategy_targetcachehitratio");
	lightstrategy_targetcachehitratio.PutCapabilityFlag(block_mode, !(light_stretagy == 3));

	XSI::Parameter lightstrategy_maxdepth = prop_array.GetItem("lightstrategy_maxdepth");
	lightstrategy_maxdepth.PutCapabilityFlag(block_mode, !(light_stretagy == 3));

	XSI::Parameter lightstrategy_maxsamplescount = prop_array.GetItem("lightstrategy_maxsamplescount");
	lightstrategy_maxsamplescount.PutCapabilityFlag(block_mode, !(light_stretagy == 3));
}

XSI::CStatus RenderEngineLux::render_options_update(XSI::PPGEventContext& event_context) 
{
	XSI::PPGEventContext::PPGEvent event_id = event_context.GetEventID();
	bool is_refresh = false;
	if (event_id == XSI::PPGEventContext::siOnInit) 
	{
		XSI::CustomProperty cp_source = event_context.GetSource();

		XSI::Parameter halt_condition = cp_source.GetParameters().GetItem("halt_condition");
		set_halt_condition(cp_source, halt_condition.GetValue());

		XSI::Parameter filter_type = cp_source.GetParameters().GetItem("filter_type");
		set_filter_type(cp_source, filter_type.GetValue());

		XSI::Parameter sampler_type = cp_source.GetParameters().GetItem("sampler_type");
		set_sampler_type(cp_source, sampler_type.GetValue());

		XSI::Parameter engine = cp_source.GetParameters().GetItem("engine");
		set_engine_type(cp_source, engine.GetValue());

		XSI::Parameter lightstrategy_mode = cp_source.GetParameters().GetItem("lightstrategy_mode");
		set_light_strategy(cp_source, lightstrategy_mode.GetValue());

		XSI::Parameter motion_objects = cp_source.GetParameters().GetItem("motion_objects");
		set_motion(cp_source, motion_objects.GetValue());
	}
	else if (event_id == XSI::PPGEventContext::siParameterChange) 
	{
		XSI::Parameter changed = event_context.GetSource();
		XSI::CustomProperty prop = changed.GetParent();
		XSI::CString param_name = changed.GetScriptName();

		if (param_name == "halt_condition")
		{
			XSI::Parameter halt_condition = prop.GetParameters().GetItem("halt_condition");
			set_halt_condition(prop, halt_condition.GetValue());

			is_refresh = block_mode == XSI::siNotInspectable;
		}
		else if (param_name == "filter_type")
		{
			XSI::Parameter filter_type = prop.GetParameters().GetItem("filter_type");
			set_filter_type(prop, filter_type.GetValue());

			is_refresh = block_mode == XSI::siNotInspectable;
		}
		else if (param_name == "sampler_type")
		{
			XSI::Parameter sampler_type = prop.GetParameters().GetItem("sampler_type");
			set_sampler_type(prop, sampler_type.GetValue());

			is_refresh = block_mode == XSI::siNotInspectable;
		}
		else if (param_name == "engine")
		{
			XSI::Parameter engine = prop.GetParameters().GetItem("engine");
			set_engine_type(prop, engine.GetValue());

			is_refresh = block_mode == XSI::siNotInspectable;
		}
		else if (param_name == "lightstrategy_mode")
		{
			XSI::Parameter lightstrategy_mode = prop.GetParameters().GetItem("lightstrategy_mode");
			set_light_strategy(prop, lightstrategy_mode.GetValue());

			is_refresh = block_mode == XSI::siNotInspectable;
		}
		else if (param_name == "motion_objects")
		{
			XSI::Parameter motion_objects = prop.GetParameters().GetItem("motion_objects");
			set_motion(prop, motion_objects.GetValue());

			is_refresh = block_mode == XSI::siNotInspectable;
		}
	}

	if (is_refresh)
	{
		event_context.PutAttribute("Refresh", true);
	}

	return XSI::CStatus::OK;
}

XSI::CStatus RenderEngineLux::render_option_define_layout(XSI::Context& context)
{
	XSI::PPGLayout layout = context.GetSource();
	XSI::Parameter parameter = context.GetSource();
	XSI::CustomProperty property = parameter.GetParent();

	build_layout(layout);

	return XSI::CStatus::OK;
}

XSI::CStatus RenderEngineLux::render_option_define(XSI::CustomProperty& property)
{
	const int caps = XSI::siPersistable | XSI::siAnimatable;
	XSI::Parameter param;

	//quality tab
	property.AddParameter("halt_condition", XSI::CValue::siInt4, caps, "", "", 0, 0, 2, 0, 2, param);
	property.AddParameter("halt_samples", XSI::CValue::siInt4, caps, "", "", 4, 1, 4096, 1, 256, param);
	property.AddParameter("halt_time", XSI::CValue::siFloat, caps, "", "", 10.0, 1.0, 4096.0, 1.0, 60.0, param);  // in seconds
	property.AddParameter("halt_noise", XSI::CValue::siFloat, caps, "", "", 128.0, 1.0, 256.0, 64.0, 192.0, param);  // greater is faster
	property.AddParameter("halt_noise_warmup", XSI::CValue::siInt4, caps, "", "", 64, 1, 4096, 1, 256, param);
	property.AddParameter("halt_noise_step", XSI::CValue::siInt4, caps, "", "", 64, 1, 4096, 1, 256, param);
	property.AddParameter("halt_noise_filter", XSI::CValue::siBool, caps, "", "", true, param);

	//filter tab
	property.AddParameter("filter_type", XSI::CValue::siInt4, caps, "", "", 5, 0, 5, 0, 5, param);
	property.AddParameter("filter_width", XSI::CValue::siFloat, caps, "", "", 1.5, 0.0, FLT_MAX, 0.1, 3.0, param);
	property.AddParameter("filter_gaussian_alpha", XSI::CValue::siFloat, caps, "", "", 2.0, 0.0, FLT_MAX, 0.1, 10.0, param);
	property.AddParameter("filter_mitchel_b", XSI::CValue::siFloat, caps, "", "", 1.0/3.0, 0.0, 1.0, 0.1, 1.0, param);
	property.AddParameter("filter_mitchel_c", XSI::CValue::siFloat, caps, "", "", 1.0 / 3.0, 0.0, 1.0, 0.1, 1.0, param);

	//sampler tab
	property.AddParameter("sampler_type", XSI::CValue::siInt4, caps, "", "", 0, 0, 2, 0, 2, param);
	property.AddParameter("sampler_strength", XSI::CValue::siFloat, caps, "", "", 0.7, 0.0, 0.95, 0.5, 0.9, param);
	property.AddParameter("sampler_metropolis_largesteprate", XSI::CValue::siFloat, caps, "", "", 0.4, 0.0, 1.0, 0.0, 1.0, param);
	property.AddParameter("sampler_metropolis_maxconsecutivereject", XSI::CValue::siInt4, caps, "", "", 512, 0.0, INT_MAX, 0, 32768, param);
	property.AddParameter("sampler_metropolis_imagemutationrate", XSI::CValue::siFloat, caps, "", "", 0.1, 0.0, 1.0, 0.0, 1.0, param);

	property.AddParameter("noiseestimation_warmup", XSI::CValue::siInt4, caps, "", "", 32, 0, INT_MAX, 0, 256, param);
	property.AddParameter("noiseestimation_step", XSI::CValue::siInt4, caps, "", "", 32, 0, INT_MAX, 0, 256, param);
	property.AddParameter("noiseestimation_filter_scale", XSI::CValue::siInt4, caps, "", "", 4, 0, INT_MAX, 0, 16, param);

	//engine tab
	property.AddParameter("engine", XSI::CValue::siInt4, caps, "", "", 0, 0, 2, 0, 2, param);
	property.AddParameter("cpu_threads_count", XSI::CValue::siInt4, caps, "", "", 0, 0, INT_MAX, 0, 8, param);
	//path parameters
	property.AddParameter("path_total", XSI::CValue::siInt4, caps, "", "", 6, 0, INT_MAX, 1, 16, param);
	property.AddParameter("path_diffuse", XSI::CValue::siInt4, caps, "", "", 4, 0, INT_MAX, 1, 16, param);
	property.AddParameter("path_glossy", XSI::CValue::siInt4, caps, "", "", 4, 0, INT_MAX, 1, 16, param);
	property.AddParameter("path_specular", XSI::CValue::siInt4, caps, "", "", 6, 0, INT_MAX, 1, 16, param);
	property.AddParameter("path_russianroulette_depth", XSI::CValue::siInt4, caps, "", "", 3, 0, INT_MAX, 1, 16, param);
	property.AddParameter("path_russianroulette_cap", XSI::CValue::siFloat, caps, "", "", 0.5, 0.0, 1.0, 0.0, 1.0, param);
	property.AddParameter("path_clamping_max", XSI::CValue::siFloat, caps, "", "", 0.0, 0.0, FLT_MAX, 0.0, 8.0, param);
	property.AddParameter("path_background", XSI::CValue::siBool, caps, "", "", false, param);
	//bdir parameters
	property.AddParameter("path_maxdepth", XSI::CValue::siInt4, caps, "", "", 5, 0, INT_MAX, 1, 16, param);
	property.AddParameter("light_maxdepth", XSI::CValue::siInt4, caps, "", "", 5, 0, INT_MAX, 1, 16, param);
	//path ocl parameters
	property.AddParameter("pathocl_pixelatomics", XSI::CValue::siBool, caps, "", "", false, param);

	//motion tab
	property.AddParameter("motion_objects", XSI::CValue::siBool, caps, "", "", false, param);
	property.AddParameter("motion_shutter_time", XSI::CValue::siFloat, caps, "", "", 1.0, 0.001, FLT_MAX, 0.0, 2.0, param);
	property.AddParameter("motion_steps", XSI::CValue::siInt4, caps, "", "", 2, 2, 128, 2, 8, param);

	//light strategy tab
	property.AddParameter("lightstrategy_mode", XSI::CValue::siInt4, caps, "", "", 2, 0, 3, 0, 3, param);
	//settings for dls cache
	property.AddParameter("lightstrategy_radius", XSI::CValue::siFloat, caps, "", "", 0.0, 0.0, FLT_MAX, 0.0, 1.0, param);
	property.AddParameter("lightstrategy_normalangle", XSI::CValue::siFloat, caps, "", "", 25.0, 0.0, 360.0, 0.0, 90.0, param);
	property.AddParameter("lightstrategy_maxpasses", XSI::CValue::siInt4, caps, "", "", 1024, 0, INT_MAX, 0, 2048, param);
	property.AddParameter("lightstrategy_convergencethreshold", XSI::CValue::siFloat, caps, "", "", 0.05, 0.0, FLT_MAX, 0.0, 0.1, param);
	property.AddParameter("lightstrategy_warmupsamples", XSI::CValue::siInt4, caps, "", "", 24, 0, INT_MAX, 1, 64, param);
	property.AddParameter("lightstrategy_targetcachehitratio", XSI::CValue::siFloat, caps, "", "", 0.995, 0.0, FLT_MAX, 0.9, 1.0, param);
	property.AddParameter("lightstrategy_maxdepth", XSI::CValue::siInt4, caps, "", "", 4, 0, INT_MAX, 1, 8, param);
	property.AddParameter("lightstrategy_maxsamplescount", XSI::CValue::siInt4, caps, "", "", 10000000, 1, INT_MAX, 1, 10000000, param);

	//export tab
	property.AddParameter("export_mode", XSI::CValue::siInt4, caps, "", "", 0, 0, 2, 0, 2, param);

	//service tab
	property.AddParameter("service_log_luxcore", XSI::CValue::siBool, caps, "", "", false, param);
	property.AddParameter("service_log_rendertime", XSI::CValue::siBool, caps, "", "", true, param);
	property.AddParameter("service_update", XSI::CValue::siFloat, caps, "", "", 0.1, 0.001, FLT_MAX, 0.1, 1.0, param);
	property.AddParameter("service_seed", XSI::CValue::siInt4, caps, "", "", 1, 1, INT_MAX, 1, 16, param);
	property.AddParameter("service_accelerator", XSI::CValue::siInt4, caps, "", "", 0, 0, 3, 0, 3, param);
	property.AddParameter("service_instances", XSI::CValue::siBool, caps, "", "", true, param);

	return XSI::CStatus::OK;
}