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

def XSILoadPlugin(in_reg):
    in_reg.Author = "Shekn Itrch"
    in_reg.Name = "LUXShadersPlugin"
    in_reg.Major = 1
    in_reg.Minor = 0

    #RegistrationInsertionPoint - do not remove this line
    # rendertree shader nodes
    in_reg.RegisterShader("ShaderMatte", 1, 0)
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


def standart_pram_options():
    param_options = XSIFactory.CreateShaderParamDefOptions()
    param_options.SetAnimatable(True)
    param_options.SetTexturable(True)
    param_options.SetReadOnly(False)
    param_options.SetInspectable(True)
    return param_options

def nonport_pram_options():
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
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 360.0, "degrees", 0.0, 360.0)

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
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 0.0, "lensradius", 0.0, 12.0)
    add_input_integer(nonport_pram_options(), params, 0, "blades", 0, 6)
    add_input_integer(nonport_pram_options(), params, 3, "power", 1, 12)
    add_input_float(nonport_pram_options(), params, 1.0, "scale_x", 0.1, 4.0)
    add_input_float(nonport_pram_options(), params, 1.0, "scale_y", 0.1, 4.0)
    add_input_string(nonport_pram_options(), params, "NONE", "distribution_type")

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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapLinear")
    return True


def LUXShadersPlugin_PassTonemapLinear_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 1.0, "scale", 0.0, 2.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapReinhard")
    return True


def LUXShadersPlugin_PassTonemapReinhard_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 1.0, "prescale", 0.0, 2.0)
    add_input_float(nonport_pram_options(), params, 1.2, "postscale", 0.0, 2.0)
    add_input_float(nonport_pram_options(), params, 3.75, "burn", 1.0, 8.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapAutoLinear")
    return True


def LUXShadersPlugin_PassTonemapAutoLinear_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapLuxLinear")
    return True


def LUXShadersPlugin_PassTonemapLuxLinear_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 100.0, "sensitivity", 1.0, 1024.0)
    add_input_float(nonport_pram_options(), params, 1.0 / 1000.0, "exposure", 0.0, 1.0)
    add_input_float(nonport_pram_options(), params, 2.8, "fstop", 1.0, 4.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxGammaCorrection")
    return True


def LUXShadersPlugin_PassGammaCorrection_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 2.2, "value", 1.0, 4.0)
    add_input_integer(nonport_pram_options(), params, 4096, "table_size", 2048, 8192)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxNOP")
    return True


def LUXShadersPlugin_PassNOP_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Data")
    in_ctxt.SetAttribute("DisplayName", "luxOutputSwitcher")
    return True


def LUXShadersPlugin_PassOutputSwitcher_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_string(nonport_pram_options(), params, "DEPTH", "channel")
    add_input_integer(nonport_pram_options(), params, 0, "index", 0, 4)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxGaussianFilter")
    return True


def LUXShadersPlugin_PassGaussianFilter_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 0.15, "weight", 0.0, 1.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxCameraResponse")
    return True


def LUXShadersPlugin_PassCameraResponse_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_string(nonport_pram_options(), params, "Advantix_100CD", "camera_name")  # parameter name is different from corresponding parameter in luxcore
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Generate")
    in_ctxt.SetAttribute("DisplayName", "luxContourLines")
    return True


def LUXShadersPlugin_PassContourLines_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 179.0, "scale", 1.0, 512.0)
    add_input_float(nonport_pram_options(), params, 100.0, "range", 1.0, 512.0)
    add_input_integer(nonport_pram_options(), params, 8, "steps", 1, 16)
    add_input_integer(nonport_pram_options(), params, 8, "zerogridsize", 1, 16)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxBackgroundImage")
    return True


def LUXShadersPlugin_PassBackgroundImage_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_image(standart_pram_options(), params, "", "file")
    add_input_float(nonport_pram_options(), params, 2.2, "gamma", 1.0, 4.0)
    
    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppg_layout = shaderDef.PPGLayout
    ppg_layout.AddGroup("Parameters")
    ppg_layout.AddItem("enable", "Enable")
    ppg_layout.AddItem("gamma", "Gamma")
    ppg_layout.AddItem("file", "File")
    # file_item.SetAttribute(c.siUIShowClip, True)
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxBloom")
    return True


def LUXShadersPlugin_PassBloom_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 0.07, "radius", 0.0, 1.0)
    add_input_float(nonport_pram_options(), params, 0.25, "weight", 0.0, 1.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Data")
    in_ctxt.SetAttribute("DisplayName", "luxObjectIDMask")
    return True


def LUXShadersPlugin_PassObjectIDMask_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_integer(nonport_pram_options(), params, 0, "id", 0, 12)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxVignetting")
    return True


def LUXShadersPlugin_PassVignetting_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 0.4, "scale", 0.0, 1.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxColorAberration")
    return True


def LUXShadersPlugin_PassColorAberration_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 0.005, "amount", 0.001, 0.01)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxPremultiplyAlpha")
    return True


def LUXShadersPlugin_PassPremultiplyAlpha_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Post-process")
    in_ctxt.SetAttribute("DisplayName", "luxMist")
    return True


def LUXShadersPlugin_PassMist_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_color(nonport_pram_options(), params, [1.0, 1.0, 1.0], "color")
    add_input_float(nonport_pram_options(), params, 0.005, "amount", 0.0, 1.0)
    add_input_float(nonport_pram_options(), params, 0.0, "startdistance", 0.0, 12.0)
    add_input_float(nonport_pram_options(), params, 10000.0, "enddistance", 10000.0, 60000.0)
    add_input_boolean(nonport_pram_options(), params, False, "excludebackground")
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Denoise")
    in_ctxt.SetAttribute("DisplayName", "luxBCDDenoiser")
    return True


def LUXShadersPlugin_PassBCDDenoiser_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 2.0, "warmupspp", 0.0, 4.0)
    add_input_float(nonport_pram_options(), params, 1.0, "histdistthresh", 0.0, 4.0)
    add_input_integer(nonport_pram_options(), params, 1, "patchradius", 1, 8)
    add_input_integer(nonport_pram_options(), params, 6, "searchwindowradius", 1, 12)
    add_input_float(nonport_pram_options(), params, 0.00000001, "mineigenvalue")
    add_input_boolean(nonport_pram_options(), params, True, "userandompixelorder")
    add_input_float(nonport_pram_options(), params, 1.0, "markedpixelsskippingprobability", 0.0, 1.0)
    add_input_integer(nonport_pram_options(), params, 0, "threadcount", 0, 12)
    add_input_integer(nonport_pram_options(), params, 3, "scales", 1, 6)
    add_input_boolean(nonport_pram_options(), params, False, "filterspikes")
    add_input_boolean(nonport_pram_options(), params, True, "applydenoise")
    add_input_float(nonport_pram_options(), params, 2.0, "spikestddev", 0.0, 4.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Generate")
    in_ctxt.SetAttribute("DisplayName", "luxPatterns")
    return True


def LUXShadersPlugin_PassPatterns_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_integer(nonport_pram_options(), params, 0, "index")
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Denoise")
    in_ctxt.SetAttribute("DisplayName", "luxIntelOIDN")
    return True


def LUXShadersPlugin_PassIntelOIDN_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_string(nonport_pram_options(), params, "RT", "filter_type")
    add_input_integer(nonport_pram_options(), params, 6000, "oidnmemory", 512, 8192)
    add_input_float(nonport_pram_options(), params, 1.0, "sharpness", 0.0, 1.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxWhiteBalance")
    return True


def LUXShadersPlugin_PassWhiteBalance_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 6500.0, "temperature", 1000.0, 40000.0)
    add_input_boolean(nonport_pram_options(), params, True, "reverse")
    add_input_boolean(nonport_pram_options(), params, False, "normalize")
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Bake")
    in_ctxt.SetAttribute("DisplayName", "luxBakeMapMargin")
    return True


def LUXShadersPlugin_PassBakeMapMargin_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_integer(nonport_pram_options(), params, 2, "margin", 1, 8)
    add_input_float(nonport_pram_options(), params, 0.0, "samplesthreshold", 0.0, 1.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxColorLUT")
    return True


def LUXShadersPlugin_PassColorLUT_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_string(nonport_pram_options(),params, "", "file")
    add_input_float(nonport_pram_options(), params, 1.0, "strength", 0.0, 1.0)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Denoise")
    in_ctxt.SetAttribute("DisplayName", "luxOptixDenoiser")
    return True


def LUXShadersPlugin_PassOptixDenoiser_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    add_input_float(nonport_pram_options(), params, 0.1, "sharpness", 0.0, 1.0)
    add_input_integer(nonport_pram_options(), params, 0, "minspp", 0, 12)
    
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
    in_ctxt.SetAttribute("Category", "LuxCore/Pass/Tonemap")
    in_ctxt.SetAttribute("DisplayName", "luxTonemapOpenColorIO")
    return True


def LUXShadersPlugin_PassTonemapOpenColorIO_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyOutput)

    # Input Parameter: input
    params = shaderDef.InputParamDefs
    
    # parameters
    add_input_boolean(nonport_pram_options(), params, True, "enable")
    
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
#--------------------RT Shader nodes-------------------------

#---------------------matte----------------------------------
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
    add_input_color(standart_pram_options(), params, 0.8, "kd")

    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppgLayout = shaderDef.PPGLayout
    ppgLayout.AddGroup("Parameters")
    ppgLayout.AddItem("kd", "Diffuse Color")
    ppgLayout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "ShaderMatte"

    return True