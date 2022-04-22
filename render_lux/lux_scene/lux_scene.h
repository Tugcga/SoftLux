#pragma once
#include "../../render_base/render_engine_base.h"
#include "../../utilities/math.h"

#include <luxcore/luxcore.h>

#include "xsi_camera.h"
#include "xsi_time.h"
#include "xsi_renderercontext.h"
#include "xsi_scene.h"

void sync_scene_objects(luxcore::Scene* scene, const XSI::RendererContext & xsi_render_context, const RenderType render_type, const XSI::CTime &eval_time);
void sync_materials(luxcore::Scene* scene, const XSI::Scene& xsi_scene, const XSI::CTime& eval_time);

void sync_polymesh(luxcore::Scene* scene, XSI::X3DObject& xsi_object, const XSI::CTime& eval_time);
void sync_camera_scene(luxcore::Scene* scene, const XSI::Camera& xsi_camera, const XSI::CTime& eval_time);
void sync_camera_shaderball(luxcore::Scene* scene);