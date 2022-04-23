#pragma once
#include "../../render_base/render_engine_base.h"
#include "../../utilities/math.h"

#include <luxcore/luxcore.h>

#include "xsi_camera.h"
#include "xsi_time.h"
#include "xsi_renderercontext.h"
#include "xsi_scene.h"
#include "xsi_transformation.h"
#include "xsi_application.h"
#include "xsi_primitive.h"
#include "xsi_light.h"

//return true if we add the object to the scene, and false in other case (unsupported object, for example)
bool sync_object(luxcore::Scene* scene, XSI::X3DObject& xsi_object, const XSI::CTime& eval_time);
void sync_scene_objects(luxcore::Scene* scene, XSI::RendererContext & xsi_render_context, const RenderType render_type, std::vector<ULONG> &xsi_objects_in_lux, const XSI::CTime &eval_time, const ULONG override_material = 0);
void sync_shaderball_back_material(luxcore::Scene* scene);
void sync_material(luxcore::Scene* scene, const XSI::Material& xsi_material, std::vector<ULONG>& xsi_materials_in_lux, const XSI::CTime& eval_time);
void sync_materials(luxcore::Scene* scene, const XSI::Scene& xsi_scene, std::vector<ULONG>& xsi_materials_in_lux, const XSI::CTime& eval_time);

bool sync_polymesh(luxcore::Scene* scene, XSI::X3DObject& xsi_object, const XSI::CTime& eval_time);
bool sync_light(luxcore::Scene* scene, XSI::Light& xsi_light, const XSI::CTime& eval_time);
bool update_light_object(luxcore::Scene* scene, XSI::X3DObject& xsi_object, const XSI::CTime& eval_time);
void sync_camera_scene(luxcore::Scene* scene, const XSI::Camera& xsi_camera, const XSI::CTime& eval_time);
void sync_camera_shaderball(luxcore::Scene* scene);
void sync_transform(luxcore::Scene* scene, const ULONG xsi_id, const XSI::MATH::CTransformation& xsi_tfm, const XSI::CTime& eval_time);