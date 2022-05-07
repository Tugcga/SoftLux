import win32com.client
from win32com.client import constants as c

null = None
false = 0
true = 1
app = Application

distribution_type_enum = [
    "None", "NONE",
    "Uniform", "UNIFORM",
    "Exponential", "EXPONENTIAL",
    "Inverse Exponential", "INVERSEEXPONENTIAL",
    "Gaussian", "GAUSSIAN",
    "Inverse Gaussian", "INVERSEGAUSSIAN",
    "Triangular", "TRIANGULAR"  # skip CUSTOM distribution type
]

output_switcher_channel_enum = [
    "Radiance Per Pixel Normalized", "RADIANCE_PER_PIXEL_NORMALIZED",
    "Radiance Per Screen Normalized", "RADIANCE_PER_SCREEN_NORMALIZED",
    "Alpha", "ALPHA",
    "Depth", "DEPTH",
    "Position", "POSITION",
    "Geometry Normal", "GEOMETRY_NORMAL",
    "Shading Normal", "SHADING_NORMAL",
    "Material Id", "MATERIAL_ID",
    "Direct Diffuse", "DIRECT_DIFFUSE",
    "Direct Diffuse Reflect", "DIRECT_DIFFUSE_REFLECT",
    "Direct Diffuse Transmit", "DIRECT_DIFFUSE_TRANSMIT",
    "Direct Glossy", "DIRECT_GLOSSY",
    "Direct Glossy Reflect", "DIRECT_GLOSSY_REFLECT",
    "Direct Glossy Transmit", "DIRECT_GLOSSY_TRANSMIT",
    "Emission", "EMISSION",
    "Indirect Diffuse", "INDIRECT_DIFFUSE",
    "Indirect Diffuse Reflect", "INDIRECT_DIFFUSE_REFLECT",
    "Indirect Diffuse Transmit", "INDIRECT_DIFFUSE_TRANSMIT",
    "Indirect Glossy", "INDIRECT_GLOSSY",
    "Indirect Glossy Reflect", "INDIRECT_GLOSSY_REFLECT",
    "Indirect Glossy Transmit", "INDIRECT_GLOSSY_TRANSMIT",
    "Indirect Specular", "INDIRECT_SPECULAR",
    "Indirect Specular Reflect", "INDIRECT_SPECULAR_REFLECT",
    "Indirect Specular Transmit", "INDIRECT_SPECULAR_TRANSMIT",
    "Material Id Mask", "MATERIAL_ID_MASK",
    "Direct Shadow Mask", "DIRECT_SHADOW_MASK",
    "Indirect Shadow Mask", "INDIRECT_SHADOW_MASK",
    "UV", "UV",
    "Raycount", "RAYCOUNT",
    "By Material Id", "BY_MATERIAL_ID",
    "Irradiance", "IRRADIANCE",
    "Object Id", "OBJECT_ID",
    "Object Id Mask", "OBJECT_ID_MASK",
    "By Object Id", "BY_OBJECT_ID",
    "Samplecount", "SAMPLECOUNT",
    "Convergence", "CONVERGENCE",
    "Material Id Color", "MATERIAL_ID_COLOR",
    "Albedo", "ALBEDO",
    "Avg Shading Normal", "AVG_SHADING_NORMAL",
    "Noise", "NOISE",
    "User Importance", "USER_IMPORTANCE",
    "Caustic", "CAUSTIC"
]

camera_function_name_enum = [
    "Advantix 100CD", "Advantix_100CD",
    "Advantix 200CD", "Advantix_200CD",
    "Advantix 400CD", "Advantix_400CD",
    "Agfachrome ctpecisa 200CD", "Agfachrome_ctpecisa_200CD",
    "Agfachrome ctprecisa 100CD", "Agfachrome_ctprecisa_100CD",
    "Agfachrome rsx2 050CD", "Agfachrome_rsx2_050CD",
    "Agfachrome rsx2 100CD", "Agfachrome_rsx2_100CD",
    "Agfachrome rsx2 200CD", "Agfachrome_rsx2_200CD",
    "Agfacolor futura 100CD", "Agfacolor_futura_100CD",
    "Agfacolor futura 200CD", "Agfacolor_futura_200CD",
    "Agfacolor futura 400CD", "Agfacolor_futura_400CD",
    "Agfacolor futuraII 100CD", "Agfacolor_futuraII_100CD",
    "Agfacolor futuraII 200CD", "Agfacolor_futuraII_200CD",
    "Agfacolor futuraII 400CD", "Agfacolor_futuraII_400CD",
    "Agfacolor hdc 100 plusCD", "Agfacolor_hdc_100_plusCD",
    "Agfacolor hdc 200 plusCD", "Agfacolor_hdc_200_plusCD",
    "Agfacolor hdc 400 plusCD", "Agfacolor_hdc_400_plusCD",
    "Agfacolor optimaII 100CD", "Agfacolor_optimaII_100CD",
    "Agfacolor optimaII 200CD", "Agfacolor_optimaII_200CD",
    "Agfacolor ultra 050 CD", "Agfacolor_ultra_050_CD",
    "Agfacolor vista 100CD", "Agfacolor_vista_100CD",
    "Agfacolor vista 200CD", "Agfacolor_vista_200CD",
    "Agfacolor vista 400CD", "Agfacolor_vista_400CD",
    "Agfacolor vista 800CD", "Agfacolor_vista_800CD",
    "Agfapan apx 025CD", "Agfapan_apx_025CD",
    "Agfapan apx 100CD", "Agfapan_apx_100CD",
    "Agfapan apx 400CD", "Agfapan_apx_400CD",
    "Ektachrome 100 plusCD", "Ektachrome_100_plusCD",
    "Ektachrome 100CD", "Ektachrome_100CD",
    "Ektachrome 320TCD", "Ektachrome_320TCD",
    "Ektachrome 400XCD", "Ektachrome_400XCD",
    "Ektachrome 64CD", "Ektachrome_64CD",
    "Ektachrome 64TCD", "Ektachrome_64TCD",
    "Ektachrome E100SCD", "Ektachrome_E100SCD",
    "F125CD", "F125CD",
    "F250CD", "F250CD",
    "F400CD", "F400CD",
    "FCICD", "FCICD",
    "Gold 100CD", "Gold_100CD",
    "Gold 200CD", "Gold_200CD",
    "Kodachrome 200CD", "Kodachrome_200CD",
    "Kodachrome 25CD", "Kodachrome_25CD",
    "Kodachrome 64CD", "Kodachrome_64CD",
    "Max Zoom 800CD", "Max_Zoom_800CD",
    "Portra 100TCD", "Portra_100TCD",
    "Portra 160NCCD", "Portra_160NCCD",
    "Portra 160VCCD", "Portra_160VCCD",
    "Portra 400NCCD", "Portra_400NCCD",
    "Portra 400VCCD", "Portra_400VCCD",
    "Portra 800CD", "Portra_800CD"
]

glass_type_enum = [
    "Rough", "rough",
    "Default", "default",
    "Architectural", "architectural"
]

metal_mode_enum = [
    "Color", "color",
    "Preset", "preset",
    "Custom n, k Values", "custom"
]

metal_presets_enum = [
    "Amorphous carbon", "amorphous_carbon",
    "Copper", "copper",
    "Gold", "gold",
    "Silver", "silver",
    "Aluminium", "aluminium"
]

cloth_preset_enum = [
    "Denim", "denim",
    "Silk Charmeuse", "silk_charmeuse",
    "Silk Shantung", "silk_shantung",
    "Cotton Twill", "cotton_twill",
    "Wool Gabardine", "wool_gabardine",
    "Polyester Lining Cloth", "polyester_lining_cloth"
]

carpaint_preset_enum = [
    "Manual Settings", "manual",
    "2K Aacrylack", "2k_acrylack",
    "Blue", "blue",
    "Blue Matte", "blue_matte",
    "BMW 339", "bmw_339",
    "Ford F8", "ford_f8",
    "Opel Titan", "opel_titan",
    "Polaris Silber", "polaris_silber",
    "White", "white"
]

image_channels_enum = [
    "Default Channels", "default",
    "RGB", "rgb",
    "Red", "red",
    "Green", "green",
    "Blue", "blue",
    "Alpha", "alpha",
    "Mean (Average)", "mean",
    "Mean (Luminance)", "colored_mean"
]

image_filter_enum = [
    "Linear", "linear",
    "Nearest", "nearest"
]

image_wrap_enum = [
    "Repeat", "repeat",
    "Clamp", "clamp",
    "Black", "black",
    "White", "white"
]

image_projection_enum = [
    "Flat", "flat",
    "Box", "box"
]

image_normalmap_orientation_enum = [
    "OpenGL", "opengl",
    "DirectX", "directx"
]

emission_dlstypes_enum = [
    "Auto", "AUTO",
    "Enabled", "ENABLED",
    "Disabled", "DISABLED"
]

mapping2d_type_enum = [
    "UV", "uvmapping2d",
    "Randomized UV", "uvrandommapping2d"
]

mapping2d_seed_enum = [
    "Object ID", "object_id",
    "Mesh Islands", "mesh_islands"
]


def XSILoadPlugin(in_reg):
    in_reg.Author = "Shekn Itrch"
    in_reg.Name = "LUXShadersPlugin"
    in_reg.Major = 1
    in_reg.Minor = 0

    #RegistrationInsertionPoint - do not remove this line
    # rendertree shader nodes
    in_reg.RegisterShader("ShaderMatte", 1, 0)  # matte, roughmatte
    in_reg.RegisterShader("ShaderMirror", 1, 0)  # mirror
    in_reg.RegisterShader("ShaderGlass", 1, 0)  # glass, archglass, roughglass
    in_reg.RegisterShader("ShaderMix", 1, 0)  # mix
    in_reg.RegisterShader("ShaderNull", 1, 0)  # null
    in_reg.RegisterShader("ShaderMatteTranslucent", 1, 0)  # mattetranslucent, roughmattetranslucent
    in_reg.RegisterShader("ShaderGlossy", 1, 0)  # glossy2
    in_reg.RegisterShader("ShaderMetal", 1, 0)  # metal2
    in_reg.RegisterShader("ShaderVelvet", 1, 0)  # velvet
    in_reg.RegisterShader("ShaderCloth", 1, 0)  # cloth
    in_reg.RegisterShader("ShaderCarpaint", 1, 0)  # carpaint
    in_reg.RegisterShader("ShaderGlossyTranslucent", 1, 0)  # glossytranslucent
    in_reg.RegisterShader("ShaderGlossyCoating", 1, 0)  # glossycoating
    in_reg.RegisterShader("ShaderDisney", 1, 0)  # disney
    in_reg.RegisterShader("ShaderTwoSided", 1, 0)  # twosided
    # rendertree textures
    in_reg.RegisterShader("TextureImage", 1, 0)
    in_reg.RegisterShader("TextureFloat", 1, 0)
    in_reg.RegisterShader("TextureColor", 1, 0)
    in_reg.RegisterShader("Emission", 1, 0)
    in_reg.RegisterShader("Mapping2D", 1, 0)
    # camera lense shader nodes
    in_reg.RegisterShader("LensePanoramic", 1, 0)
    in_reg.RegisterShader("LenseBokeh", 1, 0)
    # pass output shaders (for image piplines)
    in_reg.RegisterShader("PassTonemapLinear", 1, 0)
    in_reg.RegisterShader("PassTonemapReinhard", 1, 0)
    in_reg.RegisterShader("PassTonemapAutoLinear", 1, 0)
    in_reg.RegisterShader("PassTonemapLuxLinear", 1, 0)
    in_reg.RegisterShader("PassGammaCorrection", 1, 0)
    in_reg.RegisterShader("PassNOP", 1, 0)
    in_reg.RegisterShader("PassOutputSwitcher", 1, 0)
    in_reg.RegisterShader("PassGaussianFilter", 1, 0)
    in_reg.RegisterShader("PassCameraResponse", 1, 0)
    in_reg.RegisterShader("PassContourLines", 1, 0)
    in_reg.RegisterShader("PassBackgroundImage", 1, 0)
    in_reg.RegisterShader("PassBloom", 1, 0)
    in_reg.RegisterShader("PassObjectIDMask", 1, 0)
    in_reg.RegisterShader("PassVignetting", 1, 0)
    in_reg.RegisterShader("PassColorAberration", 1, 0)
    in_reg.RegisterShader("PassPremultiplyAlpha", 1, 0)
    in_reg.RegisterShader("PassMist", 1, 0)
    in_reg.RegisterShader("PassBCDDenoiser", 1, 0)
    in_reg.RegisterShader("PassPatterns", 1, 0)
    in_reg.RegisterShader("PassIntelOIDN", 1, 0)
    in_reg.RegisterShader("PassWhiteBalance", 1, 0)
    in_reg.RegisterShader("PassBakeMapMargin", 1, 0)
    in_reg.RegisterShader("PassColorLUT", 1, 0)
    in_reg.RegisterShader("PassOptixDenoiser", 1, 0)
    # TODO: implement ColorIO parameters
    # we should recognize available parameters for selected *.lut file
    # and output it to PPG
    # in_reg.RegisterShader("PassTonemapOpenColorIO", 1, 0)

    # pass environment shaders
    in_reg.RegisterShader("PassInfinite", 1, 0)
    in_reg.RegisterShader("PassSky", 1, 0)
    in_reg.RegisterShader("PassSun", 1, 0)

    return true


def XSIUnloadPlugin(in_reg):
    strPluginName = in_reg.Name
    app.LogMessage(str(strPluginName) + str(" has been unloaded."), c.siVerbose)
    return true


def add_output_closure(shader_def, name="closure"):
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetLongName(name)
    params = shader_def.OutputParamDefs
    param_def = params.AddParamDef2(name, c.siShaderDataTypeColor4, param_options)
    param_def.MainPort = False


def add_output_texture(shader_def, name="texture"):
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetLongName(name)
    params = shader_def.OutputParamDefs
    param_def = params.AddParamDef2(name, c.siShaderDataTypeColor3, param_options)
    param_def.MainPort = False


def add_output_float(shader_def, name="float"):
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetLongName(name)
    params = shader_def.OutputParamDefs
    param_def = params.AddParamDef2(name, c.siShaderDataTypeScalar, param_options)
    param_def.MainPort = False


def add_output_2dmapping(shader_def, name="mapping_2d"):
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetLongName(name)
    params = shader_def.OutputParamDefs
    param_def = params.AddParamDef2(name, c.siShaderDataTypeMatrix33, param_options)
    param_def.MainPort = False


def add_output_3dmapping(shader_def, name="mapping_3d"):
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetLongName(name)
    params = shader_def.OutputParamDefs
    param_def = params.AddParamDef2(name, c.siShaderDataTypeMatrix44, param_options)
    param_def.MainPort = False


def add_output_emission(shader_def, name="emission"):
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetLongName(name)
    param_options.SetAttribute(c.siCustomTypeNameAttribute, "emission")
    params = shader_def.OutputParamDefs
    param_def = params.AddParamDef2(name, c.siShaderDataTypeCustom, param_options)
    param_def.MainPort = False


def add_input_material(param_options, params, default_value=1, name="material"):
    param_options.SetDefaultValue(default_value)
    params.AddParamDef(name, c.siShaderDataTypeColor4, param_options)


def add_input_color(param_options, params, default_value=1, name="color"):
    param_options.SetDefaultValue(default_value)
    params.AddParamDef(name, c.siShaderDataTypeColor3, param_options)


def add_input_float(param_options, params, default_value=0, name="float", vis_min=None, vis_max=None):
    param_options.SetDefaultValue(default_value)
    if vis_min is not None and vis_max is not None:
        param_options.SetSoftLimit(vis_min, vis_max)
    params.AddParamDef(name, c.siShaderDataTypeScalar, param_options)


def add_input_integer(param_options, params, default_value=0, name="integer", vis_min=None, vis_max=None):
    param_options.SetDefaultValue(default_value)
    if vis_min is not None and vis_max is not None:
        param_options.SetSoftLimit(vis_min, vis_max)
    params.AddParamDef(name, c.siShaderDataTypeInteger, param_options)


def add_input_boolean(param_options, params, default_value=True, name="boolean"):
    param_options.SetDefaultValue(default_value)
    params.AddParamDef(name, c.siShaderDataTypeBoolean, param_options)


def add_input_vector(param_options, params, default_value=0, name="vector", min_visible=None, max_visible=None):
    param_options.SetDefaultValue(default_value)
    if min_visible is not None and max_visible is not None:
        param_options.SetSoftLimit(min_visible, max_visible)
    params.AddParamDef(name, c.siShaderDataTypeVector3, param_options)


def add_input_string(param_options, params, default_value=0, name="string"):
    param_options.SetDefaultValue(default_value)
    params.AddParamDef(name, c.siShaderDataTypeString, param_options)


def add_input_image(param_options, params, default_value="", name="image"):
    param_options.SetDefaultValue(default_value)
    params.AddParamDef(name, c.siShaderDataTypeImage, param_options)


def add_input_emission(param_options, params, default_value="", name="emission"):
    param_options.SetDefaultValue(default_value)
    param_options.SetAttribute(c.siCustomTypeNameAttribute, "emission")
    params.AddParamDef(name, c.siShaderDataTypeCustom, param_options)


def add_input_2dmapping(param_options, params, default_value="", name="2d mapping"):
    param_options.SetDefaultValue(default_value)
    params.AddParamDef(name, c.siShaderDataTypeMatrix33, param_options)


def add_input_3dmapping(param_options, params, default_value="", name="3d mapping"):
    param_options.SetDefaultValue(default_value)
    params.AddParamDef(name, c.siShaderDataTypeMatrix44, param_options)


def standart_param_options():
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetAnimatable(True)
    param_options.SetTexturable(True)
    param_options.SetReadOnly(False)
    param_options.SetInspectable(True)
    return param_options

def nonport_param_options():
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetAnimatable(True)
    param_options.SetTexturable(False)
    param_options.SetReadOnly(False)
    param_options.SetInspectable(True)
    return param_options

#------------------------------------------------------------
#--------------------Lense Shader nodes----------------------

#--------------------LensePanoramic--------------------------

def LUXShadersPlugin_LensePanoramic_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Lense")
    in_ctxt.SetAttribute("DisplayName", "luxPanoramic")
    return True


def LUXShadersPlugin_LensePanoramic_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyLens)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 360.0, "degrees", 0.0, 360.0)

    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enabled")
    ppg_layout.AddItem("degrees", "Degrees")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("degrees").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "LensePanoramic"

    return True

#--------------------LenseBokeh--------------------------

def LUXShadersPlugin_LenseBokeh_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Lense")
    in_ctxt.SetAttribute("DisplayName", "luxBokeh")
    return True


def LUXShadersPlugin_LenseBokeh_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyLens)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 0.0, "lensradius", 0.0, 12.0)
    add_input_integer(nonport_param_options(), params, 0, "blades", 0, 6)
    add_input_integer(nonport_param_options(), params, 3, "power", 1, 12)
    add_input_float(nonport_param_options(), params, 1.0, "scale_x", 0.1, 4.0)
    add_input_float(nonport_param_options(), params, 1.0, "scale_y", 0.1, 4.0)
    add_input_string(nonport_param_options(), params, "NONE", "distribution_type")

    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enabled")
    ppg_layout.AddItem("lensradius", "Lens Radius")
    ppg_layout.AddItem("blades", "Blades")
    ppg_layout.AddItem("power", "Power")
    ppg_layout.AddRow()
    ppg_layout.AddItem("scale_x", "Scale X")
    ppg_layout.AddItem("scale_y", "Scale Y")
    ppg_layout.EndRow()
    ppg_layout.AddEnumControl("distribution_type", distribution_type_enum, "Type")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("lensradius").ReadOnly = not enable
    prop.Parameters("blades").ReadOnly = not enable
    prop.Parameters("power").ReadOnly = not enable
    prop.Parameters("scale_x").ReadOnly = not enable
    prop.Parameters("scale_y").ReadOnly = not enable
    prop.Parameters("distribution_type").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "LenseBokeh"

    return True

#------------------------------------------------------------
#--------------------Pass Shader nodes-----------------------

#--------------------PassTonemapLinear-----------------------

def LUXShadersPlugin_PassTonemapLinear_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapLinear")
    return True


def LUXShadersPlugin_PassTonemapLinear_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 1.0, "scale", 0.0, 2.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("scale", "Scale")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("scale").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassTonemapLinear"

    return True

#--------------------PassTonemapReinhard-----------------------

def LUXShadersPlugin_PassTonemapReinhard_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapReinhard")
    return True


def LUXShadersPlugin_PassTonemapReinhard_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 1.0, "prescale", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 1.2, "postscale", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 3.75, "burn", 1.0, 8.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("prescale", "Pre-Scale")
    ppg_layout.AddItem("postscale", "Post-Scale")
    ppg_layout.AddItem("burn", "Burn")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("prescale").ReadOnly = not enable
    prop.Parameters("postscale").ReadOnly = not enable
    prop.Parameters("burn").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassTonemapReinhard"

    return True

#--------------------PassTonemapAutoLinear-----------------------

def LUXShadersPlugin_PassTonemapAutoLinear_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapAutoLinear")
    return True


def LUXShadersPlugin_PassTonemapAutoLinear_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassTonemapAutoLinear"

    return True

#--------------------PassTonemapLuxLinear-----------------------

def LUXShadersPlugin_PassTonemapLuxLinear_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapLuxLinear")
    return True


def LUXShadersPlugin_PassTonemapLuxLinear_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 100.0, "sensitivity", 1.0, 1024.0)
    add_input_float(nonport_param_options(), params, 1.0 / 1000.0, "exposure", 0.0, 1.0)
    add_input_float(nonport_param_options(), params, 2.8, "fstop", 1.0, 4.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("sensitivity", "Sensitivity")
    ppg_layout.AddItem("exposure", "Exposure")
    ppg_layout.AddItem("fstop", "FStop")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("sensitivity").ReadOnly = not enable
    prop.Parameters("exposure").ReadOnly = not enable
    prop.Parameters("fstop").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassTonemapLuxLinear"

    return True

#--------------------PassGammaCorrection-----------------------

def LUXShadersPlugin_PassGammaCorrection_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxGammaCorrection")
    return True


def LUXShadersPlugin_PassGammaCorrection_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 2.2, "value", 1.0, 4.0)
    add_input_integer(nonport_param_options(), params, 4096, "table_size", 2048, 8192)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("value", "Value")
    ppg_layout.AddItem("table_size", "Table Size")
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassGammaCorrection"

    return True

#--------------------PassNOP-----------------------

def LUXShadersPlugin_PassNOP_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxNOP")
    return True


def LUXShadersPlugin_PassNOP_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassNOP"

    return True

#--------------------PassOutputSwitcher-----------------------

def LUXShadersPlugin_PassOutputSwitcher_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Data")
    in_ctxt.SetAttribute("DisplayName", "luxOutputSwitcher")
    return True


def LUXShadersPlugin_PassOutputSwitcher_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_string(nonport_param_options(), params, "DEPTH", "channel")
    add_input_integer(nonport_param_options(), params, 0, "index", 0, 4)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddEnumControl("channel", output_switcher_channel_enum, "Channel")
    ppg_layout.AddItem("index", "Index")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("channel").ReadOnly = not enable
    prop.Parameters("index").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassOutputSwitcher"

    return True

#--------------------PassGaussianFilter-----------------------

def LUXShadersPlugin_PassGaussianFilter_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxGaussianFilter")
    return True


def LUXShadersPlugin_PassGaussianFilter_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 0.15, "weight", 0.0, 1.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("weight", "Weight")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("weight").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassGaussianFilter"

    return True

#--------------------PassCameraResponse-----------------------

def LUXShadersPlugin_PassCameraResponse_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxCameraResponse")
    return True


def LUXShadersPlugin_PassCameraResponse_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_string(nonport_param_options(), params, "Advantix_100CD", "camera_name")  # parameter name is different from corresponding parameter in luxcore
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddEnumControl("camera_name", camera_function_name_enum, "Name")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    print("!23")
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("camera_name").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassCameraResponse"

    return True

#--------------------PassContourLines-----------------------

def LUXShadersPlugin_PassContourLines_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Generate")
    in_ctxt.SetAttribute("DisplayName", "luxContourLines")
    return True


def LUXShadersPlugin_PassContourLines_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 179.0, "scale", 1.0, 512.0)
    add_input_float(nonport_param_options(), params, 100.0, "range", 1.0, 512.0)
    add_input_integer(nonport_param_options(), params, 8, "steps", 1, 16)
    add_input_integer(nonport_param_options(), params, 8, "zerogridsize", 1, 16)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("scale", "Scale")
    ppg_layout.AddItem("range", "Range")
    ppg_layout.AddItem("steps", "Steps")
    ppg_layout.AddItem("zerogridsize", "Zero Grid Size")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("scale").ReadOnly = not enable
    prop.Parameters("range").ReadOnly = not enable
    prop.Parameters("steps").ReadOnly = not enable
    prop.Parameters("zerogridsize").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassContourLines"

    return True

#--------------------PassBackgroundImage-----------------------

def LUXShadersPlugin_PassBackgroundImage_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxBackgroundImage")
    return True


def LUXShadersPlugin_PassBackgroundImage_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_image(standart_param_options(), params, "", "file")
    add_input_float(nonport_param_options(), params, 2.2, "gamma", 1.0, 4.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("gamma", "Gamma")
    ppg_layout.AddItem("file", "File")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("gamma").ReadOnly = not enable
    prop.Parameters("file").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassBackgroundImage"

    return True

#--------------------PassBloom-----------------------

def LUXShadersPlugin_PassBloom_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxBloom")
    return True


def LUXShadersPlugin_PassBloom_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 0.07, "radius", 0.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.25, "weight", 0.0, 1.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("radius", "Radius")
    ppg_layout.AddItem("weight", "Weight")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("radius").ReadOnly = not enable
    prop.Parameters("weight").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassBloom"

    return True

#--------------------PassObjectIDMask-----------------------

def LUXShadersPlugin_PassObjectIDMask_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Data")
    in_ctxt.SetAttribute("DisplayName", "luxObjectIDMask")
    return True


def LUXShadersPlugin_PassObjectIDMask_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_integer(nonport_param_options(), params, 0, "id", 0, 12)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("id", "ID")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("id").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassObjectIDMask"

    return True

#--------------------PassVignetting-----------------------

def LUXShadersPlugin_PassVignetting_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxVignetting")
    return True


def LUXShadersPlugin_PassVignetting_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 0.4, "scale", 0.0, 1.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("scale", "Scale")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("scale").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassVignetting"

    return True

#--------------------PassColorAberration-----------------------

def LUXShadersPlugin_PassColorAberration_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxColorAberration")
    return True


def LUXShadersPlugin_PassColorAberration_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 0.005, "amount", 0.001, 0.01)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("amount", "Amount")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("amount").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassColorAberration"

    return True

#--------------------PassPremultiplyAlpha-----------------------

def LUXShadersPlugin_PassPremultiplyAlpha_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxPremultiplyAlpha")
    return True


def LUXShadersPlugin_PassPremultiplyAlpha_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassPremultiplyAlpha"

    return True

#--------------------PassMist-----------------------

def LUXShadersPlugin_PassMist_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxMist")
    return True


def LUXShadersPlugin_PassMist_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_color(nonport_param_options(), params, [1.0, 1.0, 1.0], "color")
    add_input_float(nonport_param_options(), params, 0.005, "amount", 0.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.0, "startdistance", 0.0, 12.0)
    add_input_float(nonport_param_options(), params, 10000.0, "enddistance", 10000.0, 60000.0)
    add_input_boolean(nonport_param_options(), params, False, "excludebackground")
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("color", "Color")
    ppg_layout.AddItem("amount", "Amount")
    ppg_layout.AddItem("startdistance", "Start Distance")
    ppg_layout.AddItem("enddistance", "End Distance")
    ppg_layout.AddItem("excludebackground", "Exclude Background")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("color").ReadOnly = not enable
    prop.Parameters("amount").ReadOnly = not enable
    prop.Parameters("startdistance").ReadOnly = not enable
    prop.Parameters("enddistance").ReadOnly = not enable
    prop.Parameters("excludebackground").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassMist"

    return True

#--------------------PassBCDDenoiser-----------------------

def LUXShadersPlugin_PassBCDDenoiser_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Denoise")
    in_ctxt.SetAttribute("DisplayName", "luxBCDDenoiser")
    return True


def LUXShadersPlugin_PassBCDDenoiser_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 2.0, "warmupspp", 0.0, 4.0)
    add_input_float(nonport_param_options(), params, 1.0, "histdistthresh", 0.0, 4.0)
    add_input_integer(nonport_param_options(), params, 1, "patchradius", 1, 8)
    add_input_integer(nonport_param_options(), params, 6, "searchwindowradius", 1, 12)
    add_input_float(nonport_param_options(), params, 0.00000001, "mineigenvalue")
    add_input_boolean(nonport_param_options(), params, True, "userandompixelorder")
    add_input_float(nonport_param_options(), params, 1.0, "markedpixelsskippingprobability", 0.0, 1.0)
    add_input_integer(nonport_param_options(), params, 0, "threadcount", 0, 12)
    add_input_integer(nonport_param_options(), params, 3, "scales", 1, 6)
    add_input_boolean(nonport_param_options(), params, False, "filterspikes")
    add_input_boolean(nonport_param_options(), params, True, "applydenoise")
    add_input_float(nonport_param_options(), params, 2.0, "spikestddev", 0.0, 4.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("warmupspp", "Warm Up SPP")
    ppg_layout.AddItem("histdistthresh", "Histogram DT")
    ppg_layout.AddItem("patchradius", "Patch Radius")
    ppg_layout.AddItem("searchwindowradius", "Search Radius")
    ppg_layout.AddItem("mineigenvalue", "Min Eigenvalue")
    ppg_layout.AddItem("userandompixelorder", "Use Random")
    ppg_layout.AddItem("markedpixelsskippingprobability", "Probability")
    ppg_layout.AddItem("threadcount", "Thread Count")
    ppg_layout.AddItem("scales", "Scales")
    ppg_layout.AddItem("filterspikes", "Filter Spikes")
    ppg_layout.AddItem("applydenoise", "Apply Denoise")
    ppg_layout.AddItem("spikestddev", "Prefilter Factor")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("warmupspp").ReadOnly = not enable
    prop.Parameters("histdistthresh").ReadOnly = not enable
    prop.Parameters("patchradius").ReadOnly = not enable
    prop.Parameters("searchwindowradius").ReadOnly = not enable
    prop.Parameters("mineigenvalue").ReadOnly = not enable
    prop.Parameters("userandompixelorder").ReadOnly = not enable
    prop.Parameters("markedpixelsskippingprobability").ReadOnly = not enable
    prop.Parameters("threadcount").ReadOnly = not enable
    prop.Parameters("scales").ReadOnly = not enable
    prop.Parameters("filterspikes").ReadOnly = not enable
    prop.Parameters("applydenoise").ReadOnly = not enable
    prop.Parameters("spikestddev").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassBCDDenoiser"

    return True

#--------------------PassPatterns-----------------------

def LUXShadersPlugin_PassPatterns_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Generate")
    in_ctxt.SetAttribute("DisplayName", "luxPatterns")
    return True


def LUXShadersPlugin_PassPatterns_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_integer(nonport_param_options(), params, 0, "index")
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("index", "Index")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("index").ReadOnly = True

def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("index").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassPatterns"

    return True

#--------------------PassIntelOIDN-----------------------

def LUXShadersPlugin_PassIntelOIDN_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Denoise")
    in_ctxt.SetAttribute("DisplayName", "luxIntelOIDN")
    return True


def LUXShadersPlugin_PassIntelOIDN_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_string(nonport_param_options(), params, "RT", "filter_type")
    add_input_integer(nonport_param_options(), params, 6000, "oidnmemory", 512, 8192)
    add_input_float(nonport_param_options(), params, 1.0, "sharpness", 0.0, 1.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("filter_type", "Filter Type")
    ppg_layout.AddItem("oidnmemory", "OIDN Memory")
    ppg_layout.AddItem("sharpness", "Sharpness")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("filter_type").ReadOnly = True

def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("oidnmemory").ReadOnly = not enable
    prop.Parameters("sharpness").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassIntelOIDN"

    return True

#--------------------PassWhiteBalance-----------------------

def LUXShadersPlugin_PassWhiteBalance_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxWhiteBalance")
    return True


def LUXShadersPlugin_PassWhiteBalance_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 6500.0, "temperature", 1000.0, 40000.0)
    add_input_boolean(nonport_param_options(), params, True, "reverse")
    add_input_boolean(nonport_param_options(), params, True, "normalize")
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("temperature", "Temperature")
    ppg_layout.AddItem("reverse", "Reverse")
    ppg_layout.AddItem("normalize", "Normalize")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("temperature").ReadOnly = not enable
    prop.Parameters("reverse").ReadOnly = not enable
    prop.Parameters("normalize").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassWhiteBalance"

    return True

#--------------------PassBakeMapMargin-----------------------

def LUXShadersPlugin_PassBakeMapMargin_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Bake")
    in_ctxt.SetAttribute("DisplayName", "luxBakeMapMargin")
    return True


def LUXShadersPlugin_PassBakeMapMargin_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_integer(nonport_param_options(), params, 2, "margin", 1, 8)
    add_input_float(nonport_param_options(), params, 0.0, "samplesthreshold", 0.0, 1.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("margin", "Margin")
    ppg_layout.AddItem("samplesthreshold", "Samples Threshold")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("margin").ReadOnly = not enable
    prop.Parameters("samplesthreshold").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassBakeMapMargin"

    return True

#--------------------PassColorLUT-----------------------

def LUXShadersPlugin_PassColorLUT_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxColorLUT")
    return True


def LUXShadersPlugin_PassColorLUT_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_string(nonport_param_options(),params, "", "file")
    add_input_float(nonport_param_options(), params, 1.0, "strength", 0.0, 1.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    file_item = ppg_layout.AddItem("file", "File", c.siControlFilePath)
    file_item.SetAttribute(c.siUIOpenFile, True)
    file_item.SetAttribute(c.siUIFileFilter, "LUT file (*.cube)|*.cube|")
    file_item.SetAttribute(c.siUIFileMustExist, True)
    ppg_layout.AddItem("strength", "Strength")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("file").ReadOnly = not enable
    prop.Parameters("strength").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassColorLUT"

    return True

#--------------------PassOptixDenoiser-----------------------

def LUXShadersPlugin_PassOptixDenoiser_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Denoise")
    in_ctxt.SetAttribute("DisplayName", "luxOptixDenoiser")
    return True


def LUXShadersPlugin_PassOptixDenoiser_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_float(nonport_param_options(), params, 0.1, "sharpness", 0.0, 1.0)
    add_input_integer(nonport_param_options(), params, 0, "minspp", 0, 12)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("sharpness", "Sharpness")
    ppg_layout.AddItem("minspp", "Min SPP")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("sharpness").ReadOnly = not enable
    prop.Parameters("minspp").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassOptixDenoiser"

    return True

#--------------------PassTonemapOpenColorIO-----------------------

def LUXShadersPlugin_PassTonemapOpenColorIO_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Output/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapOpenColorIO")
    return True


def LUXShadersPlugin_PassTonemapOpenColorIO_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassTonemapOpenColorIO"

    return True

#------------------------------------------------------------
#--------------------Pass Environment nodes------------------

#--------------------PassInfinite-----------------------

def LUXShadersPlugin_PassInfinite_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Environment")
    in_ctxt.SetAttribute("DisplayName", "luxInfinite")
    return True


def LUXShadersPlugin_PassInfinite_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyEnvironment)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_color(nonport_param_options(), params, [1.0, 1.0, 1.0], "gain")
    add_input_image(standart_param_options(), params, "", "file")
    add_input_float(nonport_param_options(), params, 2.2, "gamma", 1.0, 3.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_x", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_y", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_z", 0.0, 360.0)
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_diffuse_enable")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_glossy_enable")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_specular_enable")
    add_input_boolean(nonport_param_options(), params, False, "sampleupperhemisphereonly")
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("gain", "Gain")
    ppg_layout.AddItem("file", "File")
    ppg_layout.AddItem("gamma", "Gamma")
    ppg_layout.AddGroup("Rotation")
    ppg_layout.AddRow()
    ppg_layout.AddItem("rotation_x", "X")
    ppg_layout.AddItem("rotation_y", "Y")
    ppg_layout.AddItem("rotation_z", "Z")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddItem("sampleupperhemisphereonly", "Clip Ground")
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Indirect Visibility")
    ppg_layout.AddItem("visibility_indirect_diffuse_enable", "Diffuse")
    ppg_layout.AddItem("visibility_indirect_glossy_enable", "Glossy")
    ppg_layout.AddItem("visibility_indirect_specular_enable", "Specular")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("gain").ReadOnly = not enable
    prop.Parameters("file").ReadOnly = not enable
    prop.Parameters("gamma").ReadOnly = not enable
    prop.Parameters("rotation_x").ReadOnly = not enable
    prop.Parameters("rotation_y").ReadOnly = not enable
    prop.Parameters("rotation_z").ReadOnly = not enable
    prop.Parameters("sampleupperhemisphereonly").ReadOnly = not enable
    prop.Parameters("visibility_indirect_diffuse_enable").ReadOnly = not enable
    prop.Parameters("visibility_indirect_glossy_enable").ReadOnly = not enable
    prop.Parameters("visibility_indirect_specular_enable").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassInfinite"

    return True

#--------------------PassSky-----------------------

def LUXShadersPlugin_PassSky_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Environment")
    in_ctxt.SetAttribute("DisplayName", "luxSky")
    return True


def LUXShadersPlugin_PassSky_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyEnvironment)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_color(nonport_param_options(), params, [0.5, 0.5, 0.5], "gain")
    add_input_float(nonport_param_options(), params, 2.2, "turbidity", 1.0, 12.0)
    add_input_color(nonport_param_options(), params, [0.5, 0.5, 0.5], "groundalbedo")
    add_input_boolean(nonport_param_options(), params, False, "ground_enable")
    add_input_boolean(nonport_param_options(), params, True, "ground_autoscale")
    add_input_color(nonport_param_options(), params, [0.75, 0.75, 0.75], "ground_color")
    add_input_float(nonport_param_options(), params, 0.0, "dir_x", -1.0, 1.0)
    add_input_float(nonport_param_options(), params, 1.0, "dir_y", -1.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.0, "dir_z", -1.0, 1.0)
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_diffuse_enable")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_glossy_enable")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_specular_enable")
    add_input_integer(nonport_param_options(), params, 512, "distribution_width", 256, 1024)
    add_input_integer(nonport_param_options(), params, 256, "distribution_height", 128, 512)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_x", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_y", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_z", 0.0, 360.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("gain", "Gain")
    ppg_layout.AddItem("turbidity", "Turbidity")
    # ignore direction for the sky, but use it for the sun
    ppg_layout.AddGroup("Direction")
    ppg_layout.AddRow()
    ppg_layout.AddItem("dir_x", "X")
    ppg_layout.AddItem("dir_y", "Y")
    ppg_layout.AddItem("dir_z", "Z")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Ground")
    ppg_layout.AddItem("groundalbedo", "Ground Albedo")
    ppg_layout.AddItem("ground_enable", "Ground Enable")
    ppg_layout.AddItem("ground_autoscale", "Ground Autoscale")
    ppg_layout.AddItem("ground_color", "Ground Color")
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Map")
    ppg_layout.AddGroup("Size")
    ppg_layout.AddRow()
    ppg_layout.AddItem("distribution_width", "Width")
    ppg_layout.AddItem("distribution_height", "Height")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Rotation")
    ppg_layout.AddRow()
    ppg_layout.AddItem("rotation_x", "X")
    ppg_layout.AddItem("rotation_y", "Y")
    ppg_layout.AddItem("rotation_z", "Z")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Indirect Visibility")
    ppg_layout.AddItem("visibility_indirect_diffuse_enable", "Diffuse")
    ppg_layout.AddItem("visibility_indirect_glossy_enable", "Glossy")
    ppg_layout.AddItem("visibility_indirect_specular_enable", "Specular")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("gain").ReadOnly = not enable
    prop.Parameters("turbidity").ReadOnly = not enable
    prop.Parameters("groundalbedo").ReadOnly = not enable
    prop.Parameters("ground_enable").ReadOnly = not enable
    prop.Parameters("ground_autoscale").ReadOnly = not enable
    prop.Parameters("ground_color").ReadOnly = not enable
    prop.Parameters("dir_x").ReadOnly = not enable
    prop.Parameters("dir_y").ReadOnly = not enable
    prop.Parameters("dir_z").ReadOnly = not enable
    prop.Parameters("visibility_indirect_diffuse_enable").ReadOnly = not enable
    prop.Parameters("visibility_indirect_glossy_enable").ReadOnly = not enable
    prop.Parameters("visibility_indirect_specular_enable").ReadOnly = not enable
    prop.Parameters("distribution_width").ReadOnly = not enable
    prop.Parameters("distribution_height").ReadOnly = not enable
    prop.Parameters("rotation_x").ReadOnly = not enable
    prop.Parameters("rotation_y").ReadOnly = not enable
    prop.Parameters("rotation_z").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassSky"

    return True

#--------------------PassSun-----------------------

def LUXShadersPlugin_PassSun_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Environment")
    in_ctxt.SetAttribute("DisplayName", "luxSun")
    return True


def LUXShadersPlugin_PassSun_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyEnvironment)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_param_options(), params, True, "enable")
    add_input_color(nonport_param_options(), params, [0.5, 0.5, 0.5], "gain")
    add_input_float(nonport_param_options(), params, 2.2, "turbidity", 1.0, 12.0)
    add_input_float(nonport_param_options(), params, 1.0, "relsize", 0.1, 2.0)
    add_input_float(nonport_param_options(), params, 0.0, "dir_x", -1.0, 1.0)
    add_input_float(nonport_param_options(), params, 1.0, "dir_y", -1.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.0, "dir_z", -1.0, 1.0)
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_diffuse_enable")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_glossy_enable")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_specular_enable")
    add_input_float(nonport_param_options(), params, 0.0, "rotation_x", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_y", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_z", 0.0, 360.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("gain", "Gain")
    ppg_layout.AddItem("turbidity", "Turbidity")
    ppg_layout.AddItem("relsize", "Relative Size")
    ppg_layout.AddGroup("Direction")
    ppg_layout.AddRow()
    ppg_layout.AddItem("dir_x", "X")
    ppg_layout.AddItem("dir_y", "Y")
    ppg_layout.AddItem("dir_z", "Z")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Rotation")
    ppg_layout.AddRow()
    ppg_layout.AddItem("rotation_x", "X")
    ppg_layout.AddItem("rotation_y", "Y")
    ppg_layout.AddItem("rotation_z", "Z")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Indirect Visibility")
    ppg_layout.AddItem("visibility_indirect_diffuse_enable", "Diffuse")
    ppg_layout.AddItem("visibility_indirect_glossy_enable", "Glossy")
    ppg_layout.AddItem("visibility_indirect_specular_enable", "Specular")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def enable_OnChanged():
    prop = PPG.Inspected(0)
    enable = prop.Parameters("enable").Value
    prop.Parameters("gain").ReadOnly = not enable
    prop.Parameters("turbidity").ReadOnly = not enable
    prop.Parameters("relsize").ReadOnly = not enable
    prop.Parameters("dir_x").ReadOnly = not enable
    prop.Parameters("dir_y").ReadOnly = not enable
    prop.Parameters("dir_z").ReadOnly = not enable
    prop.Parameters("rotation_x").ReadOnly = not enable
    prop.Parameters("rotation_y").ReadOnly = not enable
    prop.Parameters("rotation_z").ReadOnly = not enable
    prop.Parameters("visibility_indirect_diffuse_enable").ReadOnly = not enable
    prop.Parameters("visibility_indirect_glossy_enable").ReadOnly = not enable
    prop.Parameters("visibility_indirect_specular_enable").ReadOnly = not enable
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "PassSun"

    return True

#------------------------------------------------------------
#--------------------RT Shader nodes-------------------------

def setup_defaul_material_parameters(params):
    add_input_float(standart_param_options(), params, 1.0, "transparency", 0.0, 1.0)
    add_input_color(nonport_param_options(), params, [0.0, 0.0, 0.0], "transparency_shadow")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_diffuse_enable")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_glossy_enable")
    add_input_boolean(nonport_param_options(), params, True, "visibility_indirect_specular_enable")
    add_input_boolean(nonport_param_options(), params, False, "shadowcatcher_enable")
    add_input_boolean(nonport_param_options(), params, False, "shadowcatcher_onlyinfinitelights")
    add_input_boolean(nonport_param_options(), params, True, "photongi_enable")
    add_input_boolean(nonport_param_options(), params, False, "holdout_enable")

    # for these three parameters we use only ports
    add_input_float(standart_param_options(), params, [0.0, 0.0, 0.0], "bump")
    add_input_vector(standart_param_options(), params, [0.0, 0.0, 0.0], "normal")
    add_input_emission(standart_param_options(), params, [0.0, 0.0, 0.0], "emission")

def setup_default_material_ppg(ppg_layout):
    ppg_layout.AddTab("Visibility")
    ppg_layout.AddGroup("Transparency")
    ppg_layout.AddItem("transparency", "Opacity")
    ppg_layout.AddItem("transparency_shadow", "Shadow")
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Indirect")
    ppg_layout.AddItem("visibility_indirect_diffuse_enable", "Indirect Diffuse")
    ppg_layout.AddItem("visibility_indirect_glossy_enable", "Indirect Glossy")
    ppg_layout.AddItem("visibility_indirect_specular_enable", "Indirect Specular")
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Shadow Catcher")
    ppg_layout.AddItem("shadowcatcher_enable", "Enable Shadow Catcher")
    ppg_layout.AddItem("shadowcatcher_onlyinfinitelights", "Only Finite Lights")
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("photongi_enable", "Photon GI")
    ppg_layout.AddItem("holdout_enable", "Holdout")
    ppg_layout.EndGroup()

#---------------------matte, roughmatte----------------------------------
def LUXShadersPlugin_ShaderMatte_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxMatte")
    return True


def LUXShadersPlugin_ShaderMatte_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_color(standart_param_options(), params, 0.8, "kd")
    add_input_float(standart_param_options(), params, 0.0, "sigma", 0.0, 1.0)
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("kd", "Diffuse Color")
    ppg_layout.AddItem("sigma", "Sigma")
    ppg_layout.EndGroup()
    # default parameters
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderMatte"

    return True

#---------------------mirror----------------------------------
def LUXShadersPlugin_ShaderMirror_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxMirror")
    return True


def LUXShadersPlugin_ShaderMirror_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_color(standart_param_options(), params, [1.0, 1.0, 1.0], "kr")
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("kr", "Reflection Color")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderMirror"

    return True

#---------------------glass, archglass, roughglassrror----------------------------------
def LUXShadersPlugin_ShaderGlass_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxGlass")
    return True


def LUXShadersPlugin_ShaderGlass_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_string(nonport_param_options(), params, "default", "glass_mode");
    add_input_boolean(nonport_param_options(), params, False, "is_film")
    add_input_boolean(nonport_param_options(), params, False, "is_anisotropic")
    add_input_color(standart_param_options(), params, [1.0, 1.0, 1.0], "kr")
    add_input_color(standart_param_options(), params, [1.0, 1.0, 1.0], "kt")
    add_input_float(standart_param_options(), params, 1.0, "exteriorior", 0.5, 2.0)
    add_input_float(standart_param_options(), params, 1.5, "interiorior", 0.5, 2.0)
    add_input_float(standart_param_options(), params, 0.0, "filmthickness", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 1.5, "filmior", 0.5, 2.0)
    add_input_float(standart_param_options(), params, 0.0, "cauchyb", 0.0, 0.05)
    add_input_float(standart_param_options(), params, 0.1, "uroughness", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "vroughness", 0.0, 1.0)
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("is_film", "This Film Coating")
    ppg_layout.AddItem("is_anisotropic", "Anisotropic Roughness")
    ppg_layout.AddEnumControl("glass_mode", glass_type_enum, "Mode")
    ppg_layout.AddItem("kr", "Reflection Color")
    ppg_layout.AddItem("kt", "Transmission Color")
    ppg_layout.AddItem("exteriorior", "Exterior IOR")
    ppg_layout.AddItem("interiorior", "Interior IOR")
    ppg_layout.AddItem("cauchyb", "Dispersion")
    ppg_layout.AddItem("filmthickness", "Film Thickness")
    ppg_layout.AddItem("filmior", "Film IOR")
    ppg_layout.AddItem("uroughness", "U Roughness")
    ppg_layout.AddItem("vroughness", "V Roughness")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
    is_film = prop.Parameters("is_film").Value
    if is_film:
        prop.Parameters("filmthickness").ReadOnly = False
        prop.Parameters("filmior").ReadOnly = False
    else:
        prop.Parameters("filmthickness").ReadOnly = True
        prop.Parameters("filmior").ReadOnly = True
    glass_mode = prop.Parameters("glass_mode").Value
    if glass_mode == "default":
        prop.Parameters("cauchyb").ReadOnly = False
    else:
        prop.Parameters("cauchyb").ReadOnly = True
    if glass_mode != "rough":
        prop.Parameters("is_anisotropic").ReadOnly = True
        prop.Parameters("uroughness").ReadOnly = True
        prop.Parameters("vroughness").ReadOnly = True
    else:
        prop.Parameters("is_anisotropic").ReadOnly = False
        prop.Parameters("uroughness").ReadOnly = False
        is_anisotropic = prop.Parameters("is_anisotropic").Value
        if is_anisotropic:
            prop.Parameters("vroughness").ReadOnly = False
        else:
            prop.Parameters("vroughness").ReadOnly = True

def is_film_OnChanged():
    prop = PPG.Inspected(0)
    is_film = prop.Parameters("is_film").Value
    if is_film:
        prop.Parameters("filmthickness").ReadOnly = False
        prop.Parameters("filmior").ReadOnly = False
    else:
        prop.Parameters("filmthickness").ReadOnly = True
        prop.Parameters("filmior").ReadOnly = True

def glass_mode_OnChanged():
    prop = PPG.Inspected(0)
    glass_mode = prop.Parameters("glass_mode").Value
    if glass_mode == "default":
        prop.Parameters("cauchyb").ReadOnly = False
    else:
        prop.Parameters("cauchyb").ReadOnly = True
    if glass_mode != "rough":
        prop.Parameters("is_anisotropic").ReadOnly = True
        prop.Parameters("uroughness").ReadOnly = True
        prop.Parameters("vroughness").ReadOnly = True
    else:
        prop.Parameters("is_anisotropic").ReadOnly = False
        prop.Parameters("uroughness").ReadOnly = False
        is_anisotropic = prop.Parameters("is_anisotropic").Value
        if is_anisotropic:
            prop.Parameters("vroughness").ReadOnly = False
        else:
            prop.Parameters("vroughness").ReadOnly = True

def is_anisotropic_OnChanged():
    prop = PPG.Inspected(0)
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderGlass"

    return True

#---------------------mix----------------------------------
def LUXShadersPlugin_ShaderMix_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxMix")
    return True


def LUXShadersPlugin_ShaderMix_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_material(standart_param_options(), params, [0.0, 0.0, 0.0], "material1")
    add_input_material(standart_param_options(), params, [0.0, 0.0, 0.0], "material2")
    add_input_float(standart_param_options(), params, 0.5, "amount", 0.0, 1.0)
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("amount", "Amount")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderMix"

    return True

#---------------------null----------------------------------
def LUXShadersPlugin_ShaderNull_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxNull")
    return True


def LUXShadersPlugin_ShaderNull_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    # default parameters
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderNull"

    return True

#---------------------mattetranslucent, roughmattetranslucent----------------------------------
def LUXShadersPlugin_ShaderMatteTranslucent_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxMatteTranslucent")
    return True


def LUXShadersPlugin_ShaderMatteTranslucent_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_color(standart_param_options(), params, [0.5, 0.5, 0.5], "kr")
    add_input_color(standart_param_options(), params, [0.5, 0.5, 0.5], "kt")
    add_input_float(standart_param_options(), params, 0.0, "sigma", 0.0, 1.0)
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("kr", "Reflection Color")
    ppg_layout.AddItem("kt", "Transmission Color")
    ppg_layout.AddItem("sigma", "Sigma")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderMatteTranslucent"

    return True

#---------------------glossy2----------------------------------
def LUXShadersPlugin_ShaderGlossy_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxGlossy")
    return True


def LUXShadersPlugin_ShaderGlossy_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, False, "is_anisotropic")
    add_input_boolean(nonport_param_options(), params, False, "multibounce")
    add_input_color(standart_param_options(), params, [0.8, 0.8, 0.8], "kd")
    add_input_color(standart_param_options(), params, [0.1, 0.1, 0.1], "ks")
    add_input_float(standart_param_options(), params, 0.1, "uroughness", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "vroughness", 0.0, 1.0)
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "ka")
    add_input_float(standart_param_options(), params, 0.0, "d", 0.0, 1.0)
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "index")
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("multibounce", "Multibounce")
    ppg_layout.AddItem("is_anisotropic", "Anisotropic Roughness")
    ppg_layout.AddItem("kd", "Diffuse Color")
    ppg_layout.AddItem("ks", "Specular Color")
    ppg_layout.AddItem("ka", "Absorption Color")
    ppg_layout.AddItem("d", "Absorption Depth")
    ppg_layout.AddItem("uroughness", "U Roughness")
    ppg_layout.AddItem("vroughness", "V Roughness")
    ppg_layout.AddItem("index", "Index")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True

def is_anisotropic_OnChanged():
    prop = PPG.Inspected(0)
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderGlossy"

    return True

#---------------------metal2----------------------------------
def LUXShadersPlugin_ShaderMetal_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxMetal")
    return True


def LUXShadersPlugin_ShaderMetal_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, False, "is_anisotropic")
    add_input_string(nonport_param_options(), params, "color", "metal_mode")
    add_input_string(nonport_param_options(), params, "aluminium", "metal_preset")
    add_input_color(standart_param_options(), params, [0.8, 0.8, 0.8], "kr")  # create implicit fresnel texture with this color at the export
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "n")
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "k")
    add_input_float(standart_param_options(), params, 0.1, "uroughness", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "vroughness", 0.0, 1.0)
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddEnumControl("metal_mode", metal_mode_enum, "Mode")
    ppg_layout.AddEnumControl("metal_preset", metal_presets_enum, "Preset")
    ppg_layout.AddItem("is_anisotropic", "Anisotropic Roughness")
    ppg_layout.AddItem("kr", "Color")
    ppg_layout.AddItem("n", "N")
    ppg_layout.AddItem("k", "K")
    ppg_layout.AddItem("uroughness", "U Roughness")
    ppg_layout.AddItem("vroughness", "U Roughness")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True
    metal_mode = prop.Parameters("metal_mode").Value
    if metal_mode == "color":
        prop.Parameters("metal_preset").ReadOnly = True
        prop.Parameters("n").ReadOnly = True
        prop.Parameters("k").ReadOnly = True
        prop.Parameters("kr").ReadOnly = False
    elif metal_mode == "preset":
        prop.Parameters("metal_preset").ReadOnly = False
        prop.Parameters("n").ReadOnly = True
        prop.Parameters("k").ReadOnly = True
        prop.Parameters("kr").ReadOnly = True
    elif metal_mode == "custom":
        prop.Parameters("metal_preset").ReadOnly = True
        prop.Parameters("n").ReadOnly = False
        prop.Parameters("k").ReadOnly = False
        prop.Parameters("kr").ReadOnly = True

def metal_mode_OnChanged():
    prop = PPG.Inspected(0)
    metal_mode = prop.Parameters("metal_mode").Value
    if metal_mode == "color":
        prop.Parameters("metal_preset").ReadOnly = True
        prop.Parameters("n").ReadOnly = True
        prop.Parameters("k").ReadOnly = True
        prop.Parameters("kr").ReadOnly = False
    elif metal_mode == "preset":
        prop.Parameters("metal_preset").ReadOnly = False
        prop.Parameters("n").ReadOnly = True
        prop.Parameters("k").ReadOnly = True
        prop.Parameters("kr").ReadOnly = True
    elif metal_mode == "custom":
        prop.Parameters("metal_preset").ReadOnly = True
        prop.Parameters("n").ReadOnly = False
        prop.Parameters("k").ReadOnly = False
        prop.Parameters("kr").ReadOnly = True

def is_anisotropic_OnChanged():
    prop = PPG.Inspected(0)
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderMetal"

    return True

#---------------------velvet----------------------------------
def LUXShadersPlugin_ShaderVelvet_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxVelvet")
    return True


def LUXShadersPlugin_ShaderVelvet_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, False, "advanced")
    add_input_color(standart_param_options(), params, [0.8, 0.8, 0.8], "kd")
    add_input_float(standart_param_options(), params, -2.0, "p1", -5.0, 5.0)
    add_input_float(standart_param_options(), params, 20.0, "p2", 0.0, 40.0)
    add_input_float(standart_param_options(), params, 2.0, "p3", 0.0, 4.0)
    add_input_float(standart_param_options(), params, 0.1, "thickness", 0.0, 1.0)
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("advanced", "Advanced Options")
    ppg_layout.AddItem("kd", "Diffuse Color")
    ppg_layout.AddItem("p1", "P1")
    ppg_layout.AddItem("p2", "P2")
    ppg_layout.AddItem("p3", "P3")
    ppg_layout.AddItem("thickness", "Thickness")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
    advanced = prop.Parameters("advanced").Value
    if advanced:
        prop.Parameters("p1").ReadOnly = False
        prop.Parameters("p2").ReadOnly = False
        prop.Parameters("p3").ReadOnly = False
    else:
        prop.Parameters("p1").ReadOnly = True
        prop.Parameters("p2").ReadOnly = True
        prop.Parameters("p3").ReadOnly = True

def advanced_OnChanged():
    prop = PPG.Inspected(0)
    advanced = prop.Parameters("advanced").Value
    if advanced:
        prop.Parameters("p1").ReadOnly = False
        prop.Parameters("p2").ReadOnly = False
        prop.Parameters("p3").ReadOnly = False
    else:
        prop.Parameters("p1").ReadOnly = True
        prop.Parameters("p2").ReadOnly = True
        prop.Parameters("p3").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderVelvet"

    return True

#---------------------cloth----------------------------------
def LUXShadersPlugin_ShaderCloth_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxCloth")
    return True


def LUXShadersPlugin_ShaderCloth_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_float(nonport_param_options(), params, 100.0, "repeat_u", 1.0, 256.0)
    add_input_float(nonport_param_options(), params, 100.0, "repeat_v", 1.0, 256.0)
    add_input_string(nonport_param_options(), params, "denim", "preset")
    add_input_color(standart_param_options(), params, [0.8, 0.8, 0.8], "weft_kd")
    add_input_color(standart_param_options(), params, [0.1, 0.1, 0.1], "weft_ks")
    add_input_color(standart_param_options(), params, [0.7, 0.1, 0.1], "warp_kd")
    add_input_color(standart_param_options(), params, [0.1, 0.1, 0.1], "warp_ks")
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("repeat_u", "Repeat U")
    ppg_layout.AddItem("repeat_v", "Repeat V")
    ppg_layout.AddEnumControl("preset", cloth_preset_enum, "Preset")
    ppg_layout.AddItem("warp_kd", "Warp Diffuse Color")
    ppg_layout.AddItem("warp_ks", "Warp Specular Color")
    ppg_layout.AddItem("weft_kd", "Waft Diffuse Color")
    ppg_layout.AddItem("weft_ks", "Waft Specular Color")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderCloth"

    return True

#---------------------carpaint----------------------------------
def LUXShadersPlugin_ShaderCarpaint_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxCarpaint")
    return True


def LUXShadersPlugin_ShaderCarpaint_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_string(nonport_param_options(), params, "manual", "preset")
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "ka")
    add_input_float(standart_param_options(), params, 0.0, "d", 0.0, 1.0)

    add_input_color(standart_param_options(), params, [0.5, 0.5, 0.5], "kd")
    add_input_color(standart_param_options(), params, [1.0, 1.0, 1.0], "ks1")
    add_input_color(standart_param_options(), params, [1.0, 1.0, 1.0], "ks2")
    add_input_color(standart_param_options(), params, [1.0, 1.0, 1.0], "ks3")

    add_input_float(standart_param_options(), params, 0.95, "r1", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.9, "r2", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.7, "r3", 0.0, 1.0)

    add_input_float(standart_param_options(), params, 0.25, "m1", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "m2", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.015, "m3", 0.0, 1.0)
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddEnumControl("preset", carpaint_preset_enum, "Preset")
    ppg_layout.AddItem("kd", "Diffuse Color")
    ppg_layout.AddItem("ks1", "Specular Color 1")
    ppg_layout.AddItem("r1", "R1")
    ppg_layout.AddItem("m1", "M1")
    ppg_layout.AddItem("ks2", "Specular Color 2")
    ppg_layout.AddItem("r2", "R2")
    ppg_layout.AddItem("m2", "M2")
    ppg_layout.AddItem("ks3", "Specular Color 3")
    ppg_layout.AddItem("r3", "R3")
    ppg_layout.AddItem("m3", "M3")
    ppg_layout.AddItem("ka", "Absorption Color")
    ppg_layout.AddItem("d", "Absorption Depth")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
    preset = prop.Parameters("preset").Value
    if preset == "manual":
        prop.Parameters("kd").ReadOnly = False
        prop.Parameters("ks1").ReadOnly = False
        prop.Parameters("r1").ReadOnly = False
        prop.Parameters("m1").ReadOnly = False
        prop.Parameters("ks2").ReadOnly = False
        prop.Parameters("r2").ReadOnly = False
        prop.Parameters("m2").ReadOnly = False
        prop.Parameters("ks3").ReadOnly = False
        prop.Parameters("r3").ReadOnly = False
        prop.Parameters("m3").ReadOnly = False
    else:
        prop.Parameters("kd").ReadOnly = True
        prop.Parameters("ks1").ReadOnly = True
        prop.Parameters("r1").ReadOnly = True
        prop.Parameters("m1").ReadOnly = True
        prop.Parameters("ks2").ReadOnly = True
        prop.Parameters("r2").ReadOnly = True
        prop.Parameters("m2").ReadOnly = True
        prop.Parameters("ks3").ReadOnly = True
        prop.Parameters("r3").ReadOnly = True
        prop.Parameters("m3").ReadOnly = True

def preset_OnChanged():
    prop = PPG.Inspected(0)
    preset = prop.Parameters("preset").Value
    if preset == "manual":
        prop.Parameters("kd").ReadOnly = False
        prop.Parameters("ks1").ReadOnly = False
        prop.Parameters("r1").ReadOnly = False
        prop.Parameters("m1").ReadOnly = False
        prop.Parameters("ks2").ReadOnly = False
        prop.Parameters("r2").ReadOnly = False
        prop.Parameters("m2").ReadOnly = False
        prop.Parameters("ks3").ReadOnly = False
        prop.Parameters("r3").ReadOnly = False
        prop.Parameters("m3").ReadOnly = False
    else:
        prop.Parameters("kd").ReadOnly = True
        prop.Parameters("ks1").ReadOnly = True
        prop.Parameters("r1").ReadOnly = True
        prop.Parameters("m1").ReadOnly = True
        prop.Parameters("ks2").ReadOnly = True
        prop.Parameters("r2").ReadOnly = True
        prop.Parameters("m2").ReadOnly = True
        prop.Parameters("ks3").ReadOnly = True
        prop.Parameters("r3").ReadOnly = True
        prop.Parameters("m3").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderCarpaint"

    return True

#---------------------glossytranslucent----------------------------------
def LUXShadersPlugin_ShaderGlossyTranslucent_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxGlossyTranslucent")
    return True


def LUXShadersPlugin_ShaderGlossyTranslucent_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, False, "multibounce")
    add_input_boolean(nonport_param_options(), params, False, "is_anisotropic")
    add_input_boolean(nonport_param_options(), params, False, "is_double")
    add_input_color(standart_param_options(), params, [0.5, 0.5, 0.5], "kd")
    add_input_color(standart_param_options(), params, [0.5, 0.5, 0.5], "kt")
    add_input_color(standart_param_options(), params, [0.1, 0.1, 0.1], "ks")
    add_input_color(standart_param_options(), params, [0.1, 0.1, 0.1], "ks_bf")
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "ka")
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "ka_bf")
    add_input_float(standart_param_options(), params, 0.0, "d", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.0, "d_bf", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "uroughness", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "vroughness", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "uroughness_bf", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "vroughness_bf", 0.0, 1.0)
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "index")
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "index_bf")
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("multibounce", "Multibounce")
    ppg_layout.AddItem("is_anisotropic", "Anisotropic Roughness")
    ppg_layout.AddItem("is_double", "Double Sided")
    ppg_layout.AddItem("kd", "Diffuse Color")
    ppg_layout.AddItem("kt", "Transmission Color")
    ppg_layout.AddItem("ks", "Specular Color")
    ppg_layout.AddItem("ka", "Absorption Color")
    ppg_layout.AddItem("d", "Absorption Depth")
    ppg_layout.AddItem("uroughness", "U Roughness")
    ppg_layout.AddItem("vroughness", "V Roughness")
    ppg_layout.AddItem("index", "Index")
    ppg_layout.AddItem("ks_bf", "BF Specular Color")
    ppg_layout.AddItem("ka_bf", "BF Absorption Color")
    ppg_layout.AddItem("d_bf", "BF Absorption Depth")
    ppg_layout.AddItem("uroughness_bf", "BF U Roughness")
    ppg_layout.AddItem("vroughness_bf", "BF V Roughness")
    ppg_layout.AddItem("index_bf", "BF Index")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    is_double = prop.Parameters("is_double").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
        if is_double:
            prop.Parameters("vroughness_bf").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True
        prop.Parameters("vroughness_bf").ReadOnly = True
    if is_double:
        prop.Parameters("ks_bf").ReadOnly = False
        prop.Parameters("ka_bf").ReadOnly = False
        prop.Parameters("d_bf").ReadOnly = False
        prop.Parameters("uroughness_bf").ReadOnly = False
        if is_anisotropic:
            prop.Parameters("vroughness_bf").ReadOnly = False
        prop.Parameters("index_bf").ReadOnly = False
    else:
        prop.Parameters("ks_bf").ReadOnly = True
        prop.Parameters("ka_bf").ReadOnly = True
        prop.Parameters("d_bf").ReadOnly = True
        prop.Parameters("uroughness_bf").ReadOnly = True
        prop.Parameters("vroughness_bf").ReadOnly = True
        prop.Parameters("index_bf").ReadOnly = True

def is_anisotropic_OnChanged():
    prop = PPG.Inspected(0)
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    is_double = prop.Parameters("is_double").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
        if is_double:
            prop.Parameters("vroughness_bf").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True
        prop.Parameters("vroughness_bf").ReadOnly = True

def is_double_OnChanged():
    prop = PPG.Inspected(0)
    is_double = prop.Parameters("is_double").Value
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    if is_double:
        prop.Parameters("ks_bf").ReadOnly = False
        prop.Parameters("ka_bf").ReadOnly = False
        prop.Parameters("d_bf").ReadOnly = False
        prop.Parameters("uroughness_bf").ReadOnly = False
        if is_anisotropic:
            prop.Parameters("vroughness_bf").ReadOnly = False
        prop.Parameters("index_bf").ReadOnly = False
    else:
        prop.Parameters("ks_bf").ReadOnly = True
        prop.Parameters("ka_bf").ReadOnly = True
        prop.Parameters("d_bf").ReadOnly = True
        prop.Parameters("uroughness_bf").ReadOnly = True
        prop.Parameters("vroughness_bf").ReadOnly = True
        prop.Parameters("index_bf").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderGlossyTranslucent"

    return True

#---------------------glossycoating----------------------------------
def LUXShadersPlugin_ShaderGlossyCoating_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxGlossyCoating")
    return True


def LUXShadersPlugin_ShaderGlossyCoating_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, False, "multibounce")
    add_input_boolean(nonport_param_options(), params, False, "is_anisotropic")
    add_input_material(standart_param_options(), params, [0.0, 0.0, 0.0], "base")
    add_input_color(standart_param_options(), params, [0.1, 0.1, 0.1], "ks")
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "ka")
    add_input_float(standart_param_options(), params, 0.0, "d", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "uroughness", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.1, "vroughness", 0.0, 1.0)
    add_input_color(standart_param_options(), params, [0.0, 0.0, 0.0], "index")
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("multibounce", "Multibounce")
    ppg_layout.AddItem("is_anisotropic", "Anisotropic Roughness")
    ppg_layout.AddItem("ks", "Specular Color")
    ppg_layout.AddItem("ka", "Absorption Color")
    ppg_layout.AddItem("d", "Absorption Depth")
    ppg_layout.AddItem("uroughness", "U Roughness")
    ppg_layout.AddItem("vroughness", "V Roughness")
    ppg_layout.AddItem("index", "Index")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True

def is_anisotropic_OnChanged():
    prop = PPG.Inspected(0)
    is_anisotropic = prop.Parameters("is_anisotropic").Value
    if is_anisotropic:
        prop.Parameters("vroughness").ReadOnly = False
    else:
        prop.Parameters("vroughness").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderGlossyCoating"

    return True

#---------------------disney----------------------------------
def LUXShadersPlugin_ShaderDisney_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxDisney")
    return True


def LUXShadersPlugin_ShaderDisney_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_param_options(), params, False, "is_film")
    add_input_color(standart_param_options(), params, [0.8, 0.8, 0.8], "basecolor")
    add_input_float(standart_param_options(), params, 0.0, "subsurface", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.0, "metallic", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.5, "specular", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.0, "speculartint", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.2, "roughness", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.0, "anisotropic", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.0, "sheen", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.0, "sheentint", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 0.0, "clearcoat", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 1.0, "clearcoatgloss", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 1.0, "filmamount", 0.0, 1.0)
    add_input_float(standart_param_options(), params, 300.0, "filmthickness", 0.0, 600.0)
    add_input_float(standart_param_options(), params, 1.5, "filmior", 0.0, 1.0)
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddTab("Shader")
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("is_film", "Thin Film Coating")
    ppg_layout.AddItem("basecolor", "Base Color")
    ppg_layout.AddItem("subsurface", "Subsurface")
    ppg_layout.AddItem("metallic", "Metallic")
    ppg_layout.AddItem("specular", "Specular")
    ppg_layout.AddItem("speculartint", "Specular Tint")
    ppg_layout.AddItem("roughness", "Roughness")
    ppg_layout.AddItem("anisotropic", "Anisotropic")
    ppg_layout.AddItem("sheen", "Sheen")
    ppg_layout.AddItem("sheentint", "Sheen Tint")
    ppg_layout.AddItem("clearcoat", "Clearcoat")
    ppg_layout.AddItem("clearcoatgloss", "Clearcoat Gloss")

    ppg_layout.AddItem("filmamount", "Film Amount")
    ppg_layout.AddItem("filmthickness", "Film Thickness")
    ppg_layout.AddItem("filmior", "Film IOR")
    ppg_layout.EndGroup()
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
    is_film = prop.Parameters("is_film").Value
    if is_film:
        prop.Parameters("filmamount").ReadOnly = False
        prop.Parameters("filmthickness").ReadOnly = False
        prop.Parameters("filmior").ReadOnly = False
    else:
        prop.Parameters("filmamount").ReadOnly = True
        prop.Parameters("filmthickness").ReadOnly = True
        prop.Parameters("filmior").ReadOnly = True

def is_film_OnChanged():
    prop = PPG.Inspected(0)
    is_film = prop.Parameters("is_film").Value
    if is_film:
        prop.Parameters("filmamount").ReadOnly = False
        prop.Parameters("filmthickness").ReadOnly = False
        prop.Parameters("filmior").ReadOnly = False
    else:
        prop.Parameters("filmamount").ReadOnly = True
        prop.Parameters("filmthickness").ReadOnly = True
        prop.Parameters("filmior").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderDisney"

    return True

#---------------------twosided----------------------------------
def LUXShadersPlugin_ShaderTwoSided_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Material")
    in_ctxt.SetAttribute("DisplayName", "luxTwoSided")
    return True


def LUXShadersPlugin_ShaderTwoSided_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_material(standart_param_options(), params, [0.0, 0.0, 0.0], "frontmaterial")
    add_input_material(standart_param_options(), params, [0.0, 0.0, 0.0], "backmaterial")
    # default parameters
    setup_defaul_material_parameters(params)

    # Output Parameter
    add_output_closure(shaderDef, "material")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    setup_default_material_ppg(ppg_layout)

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def OnInit():
    prop = PPG.Inspected(0)
    prop.Parameters("bump").ReadOnly = True
    prop.Parameters("normal").ReadOnly = True
    prop.Parameters("emission").ReadOnly = True
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderTwoSided"

    return True

#---------------------------------------------------------------
#---------------------Textures----------------------------------

#---------------------imagemap----------------------------------
def LUXShadersPlugin_TextureImage_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Texture/Texture")
    in_ctxt.SetAttribute("DisplayName", "luxImage")
    return True


def LUXShadersPlugin_TextureImage_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyTexture)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_image(nonport_param_options(), params, "", "file")
    add_input_boolean(nonport_param_options(), params, False, "is_normal_map")
    add_input_float(nonport_param_options(), params, 2.2, "gamma", 1.0, 3.0)
    add_input_float(nonport_param_options(), params, 1.0, "brightness", 0.0, 4.0)
    add_input_float(nonport_param_options(), params, 1.0, "height", 0.0, 4.0)
    add_input_string(nonport_param_options(), params, "default", "channels")
    add_input_string(nonport_param_options(), params, "opengl", "normalmap_orientation")
    add_input_string(nonport_param_options(), params, "linear", "filter")
    add_input_string(nonport_param_options(), params, "flat", "projection")
    add_input_string(nonport_param_options(), params, "repeat", "wrap")
    add_input_boolean(nonport_param_options(), params, False, "randomizedtiling_enable")
    add_input_2dmapping(standart_param_options(), params, None, "mapping_2d")

    # Output Parameter
    add_output_texture(shaderDef, "texture")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("file", "File")
    ppg_layout.AddItem("is_normal_map", "Normal Map")
    ppg_layout.AddItem("gamma", "Gamma")
    ppg_layout.AddItem("brightness", "Brightness")
    ppg_layout.AddItem("height", "Height")
    ppg_layout.AddEnumControl("channels", image_channels_enum, "Channels")
    ppg_layout.AddEnumControl("normalmap_orientation", image_normalmap_orientation_enum, "Orientation")
    ppg_layout.AddEnumControl("filter", image_filter_enum, "Filter")
    ppg_layout.AddEnumControl("projection", image_projection_enum, "Projection")
    ppg_layout.AddEnumControl("wrap", image_wrap_enum, "Wrap")
    ppg_layout.AddItem("randomizedtiling_enable", "Randomized Tiling")
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def update(prop):
    is_normal_map = prop.Parameters("is_normal_map").Value
    if is_normal_map:
        prop.Parameters("channels").ReadOnly = True
        prop.Parameters("gamma").ReadOnly = True
        prop.Parameters("brightness").ReadOnly = True
        prop.Parameters("normalmap_orientation").ReadOnly = False
        prop.Parameters("height").ReadOnly = False
    else:
        prop.Parameters("channels").ReadOnly = False
        prop.Parameters("gamma").ReadOnly = False
        prop.Parameters("brightness").ReadOnly = False
        prop.Parameters("normalmap_orientation").ReadOnly = True
        prop.Parameters("height").ReadOnly = True

def OnInit():
    prop = PPG.Inspected(0)
    update(prop)

def is_normal_map_OnChanged():
    prop = PPG.Inspected(0)
    update(prop)
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "TextureImage"

    return True

#---------------------constfloat1----------------------------------
def LUXShadersPlugin_TextureFloat_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Texture/Utils")
    in_ctxt.SetAttribute("DisplayName", "luxFloat")
    return True


def LUXShadersPlugin_TextureFloat_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyTexture)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_float(nonport_param_options(), params, 1.0, "value")

    # Output Parameter
    add_output_float(shaderDef, "value")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("value", "Value")
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "TextureFloat"

    return True

#---------------------constfloat3----------------------------------
def LUXShadersPlugin_TextureColor_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Texture/Utils")
    in_ctxt.SetAttribute("DisplayName", "luxColor")
    return True


def LUXShadersPlugin_TextureColor_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyTexture)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_color(nonport_param_options(), params, [1.0, 1.0, 1.0], "value")

    # Output Parameter
    add_output_texture(shaderDef, "color")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("value", "Value")
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "TextureColor"

    return True

#---------------------emission----------------------------------
def LUXShadersPlugin_Emission_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Light")
    in_ctxt.SetAttribute("DisplayName", "luxEmission")
    return True


def LUXShadersPlugin_Emission_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyTexture)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_string(nonport_param_options(), params, "AUTO", "dls_type")
    add_input_float(nonport_param_options(), params, 1.0, "gain", 0.0, 4.0)
    add_input_float(nonport_param_options(), params, 0.0, "exposure", -5.0, 5.0)
    add_input_float(nonport_param_options(), params, 1.0, "importance", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 90.0, "theta", 0.0, 180.0)
    add_input_integer(nonport_param_options(), params, 0, "lightgroup_id", 0, 7)
    add_input_color(standart_param_options(), params, [1.0, 1.0, 1.0], "color")

    # Output Parameter
    add_output_emission(shaderDef, "light")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("gain", "Gain")
    ppg_layout.AddItem("importance", "Importance")
    ppg_layout.AddItem("exposure", "Exposure")
    ppg_layout.AddItem("theta", "Spread Angle")
    ppg_layout.AddItem("color", "Color")
    ppg_layout.AddItem("lightgroup_id", "ID")
    ppg_layout.AddEnumControl("dls_type", emission_dlstypes_enum, "DLS")
    ppg_layout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "Emission"

    return True

#---------------------uvmapping2d, uvrandommapping2d----------------------------------
def LUXShadersPlugin_Mapping2D_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Shader/Mapping")
    in_ctxt.SetAttribute("DisplayName", "luxMapping2d")
    return True


def LUXShadersPlugin_Mapping2D_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyTexture)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_string(nonport_param_options(), params, "uvmapping2d", "mapping_type")
    add_input_string(nonport_param_options(), params, "object_id", "seed_type")
    add_input_integer(nonport_param_options(), params, 0, "uvindex", 0, 2)
    add_input_integer(nonport_param_options(), params, 0, "id_offset", 0, 12)
    add_input_boolean(nonport_param_options(), params, False, "is_center_map")
    add_input_boolean(nonport_param_options(), params, True, "is_uniform")
    add_input_float(nonport_param_options(), params, 1.0, "u_scale", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 1.0, "u_scale_min", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 1.0, "u_scale_max", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 1.0, "v_scale", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 1.0, "v_scale_min", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 1.0, "v_scale_max", 0.0, 2.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_min", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "rotation_max", 0.0, 360.0)
    add_input_float(nonport_param_options(), params, 0.0, "u_offset", 0.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.0, "u_offset_min", 0.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.0, "u_offset_max", 0.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.0, "v_offset", 0.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.0, "v_offset_min", 0.0, 1.0)
    add_input_float(nonport_param_options(), params, 0.0, "v_offset_max", 0.0, 1.0)

    # Output Parameter
    add_output_2dmapping(shaderDef, "mapping")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("uvindex", "UV Index")
    ppg_layout.AddEnumControl("mapping_type", mapping2d_type_enum, "Type")
    ppg_layout.AddEnumControl("seed_type", mapping2d_seed_enum, "Seed")
    ppg_layout.AddItem("id_offset", "Object ID Offset")
    ppg_layout.AddItem("is_center_map", "Center Map")
    ppg_layout.AddItem("is_uniform", "Uniform Scale")
    ppg_layout.AddGroup("Scale")
    ppg_layout.AddRow()
    ppg_layout.AddItem("u_scale", "U")
    ppg_layout.AddItem("v_scale", "V")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("U Scale")
    ppg_layout.AddRow()
    ppg_layout.AddItem("u_scale_min", "Min")
    ppg_layout.AddItem("u_scale_max", "Max")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("V Scale")
    ppg_layout.AddRow()
    ppg_layout.AddItem("v_scale_min", "Min")
    ppg_layout.AddItem("v_scale_max", "Max")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddItem("rotation", "Rotation")
    ppg_layout.AddGroup("Rotation")
    ppg_layout.AddRow()
    ppg_layout.AddItem("rotation_min", "Min")
    ppg_layout.AddItem("rotation_max", "Max")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Offset")
    ppg_layout.AddRow()
    ppg_layout.AddItem("u_offset", "U")
    ppg_layout.AddItem("v_offset", "V")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Offset U")
    ppg_layout.AddRow()
    ppg_layout.AddItem("u_offset_min", "Min")
    ppg_layout.AddItem("u_offset_max", "Max")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.AddGroup("Offset V")
    ppg_layout.AddRow()
    ppg_layout.AddItem("v_offset_min", "Min")
    ppg_layout.AddItem("v_offset_max", "Max")
    ppg_layout.EndRow()
    ppg_layout.EndGroup()
    ppg_layout.EndGroup()

    ppg_layout.Language = "Python"
    ppg_layout.Logic = '''
def update(prop):
    is_uniform = prop.Parameters("is_uniform").Value
    mapping_type = prop.Parameters("mapping_type").Value
    seed_type = prop.Parameters("seed_type").Value
    if mapping_type == "uvmapping2d":
        prop.Parameters("seed_type").ReadOnly = True
        prop.Parameters("id_offset").ReadOnly = True
        prop.Parameters("rotation_min").ReadOnly = True
        prop.Parameters("rotation_max").ReadOnly = True
        prop.Parameters("u_offset_min").ReadOnly = True
        prop.Parameters("u_offset_max").ReadOnly = True
        prop.Parameters("v_offset_min").ReadOnly = True
        prop.Parameters("v_offset_max").ReadOnly = True
        prop.Parameters("u_scale_min").ReadOnly = True
        prop.Parameters("u_scale_max").ReadOnly = True
        prop.Parameters("v_scale_min").ReadOnly = True
        prop.Parameters("v_scale_max").ReadOnly = True
        prop.Parameters("is_center_map").ReadOnly = False
        prop.Parameters("u_scale").ReadOnly = False
        prop.Parameters("rotation").ReadOnly = False
        prop.Parameters("u_offset").ReadOnly = False
        prop.Parameters("v_offset").ReadOnly = False
        if is_uniform:
            prop.Parameters("v_scale").ReadOnly = True
        else:
            prop.Parameters("v_scale").ReadOnly = False
    else:
        prop.Parameters("seed_type").ReadOnly = False
        if seed_type == "object_id":
            prop.Parameters("id_offset").ReadOnly = False
        else:
            prop.Parameters("id_offset").ReadOnly = True
        prop.Parameters("rotation_min").ReadOnly = False
        prop.Parameters("rotation_max").ReadOnly = False
        prop.Parameters("u_offset_min").ReadOnly = False
        prop.Parameters("u_offset_max").ReadOnly = False
        prop.Parameters("v_offset_min").ReadOnly = False
        prop.Parameters("v_offset_max").ReadOnly = False
        prop.Parameters("u_scale_min").ReadOnly = False
        prop.Parameters("u_scale_max").ReadOnly = False
        if is_uniform:
            prop.Parameters("v_scale_min").ReadOnly = True
            prop.Parameters("v_scale_max").ReadOnly = True
        else:
            prop.Parameters("v_scale_min").ReadOnly = False
            prop.Parameters("v_scale_max").ReadOnly = False
        prop.Parameters("is_center_map").ReadOnly = True
        prop.Parameters("u_scale").ReadOnly = True
        prop.Parameters("v_scale").ReadOnly = True
        prop.Parameters("rotation").ReadOnly = True
        prop.Parameters("u_offset").ReadOnly = True
        prop.Parameters("v_offset").ReadOnly = True

def OnInit():
    prop = PPG.Inspected(0)
    update(prop)

def is_uniform_OnChanged():
    prop = PPG.Inspected(0)
    update(prop)

def mapping_type_OnChanged():
    prop = PPG.Inspected(0)
    update(prop)

def seed_type_OnChanged():
    prop = PPG.Inspected(0)
    update(prop)
'''

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "Mapping2D"

    return True