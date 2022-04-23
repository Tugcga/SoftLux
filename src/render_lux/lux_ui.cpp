#include "render_engine_lux.h"

#include <xsi_parameter.h>
#include <xsi_customproperty.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgitem.h>
#include <xsi_context.h>
#include <xsi_ppgeventcontext.h>

void build_layout(XSI::PPGLayout& layout)
{
	layout.Clear();
}

XSI::CStatus RenderEngineLux::render_options_update(XSI::PPGEventContext& event_context) 
{
	XSI::PPGEventContext::PPGEvent event_id = event_context.GetEventID();
	if (event_id == XSI::PPGEventContext::siOnInit) 
	{

	}
	else if (event_id == XSI::PPGEventContext::siParameterChange) 
	{

	}

	return XSI::CStatus::OK;
}

XSI::CStatus RenderEngineLux::render_option_define_layout(XSI::Context& context)
{
	XSI::PPGLayout layout = context.GetSource();
	XSI::Parameter parameter = context.GetSource();
	XSI::CustomProperty property = parameter.GetParent();

	build_layout(layout);

	//set_sun_sky_mode(property, false);
	//set_renderer(property, 0);

	return XSI::CStatus::OK;
}

XSI::CStatus RenderEngineLux::render_option_define(XSI::CustomProperty& property)
{
	const int caps = XSI::siPersistable | XSI::siAnimatable;
	XSI::Parameter param;

	return XSI::CStatus::OK;
}