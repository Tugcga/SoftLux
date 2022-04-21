#pragma once
#include <luxcore/luxcore.h>

#include "xsi_camera.h"
#include "xsi_time.h"

void sync_camera_scene(luxcore::Scene* scene, const XSI::Camera& xsi_camera, const XSI::CTime& eval_time);
void sync_camera_shaderball(luxcore::Scene* scene);