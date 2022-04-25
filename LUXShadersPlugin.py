import win32com.client
from win32com.client import constants as c

null = None
false = 0
true = 1
app = Application

def XSILoadPlugin(in_reg):
    in_reg.Author = "Shekn Itrch"
    in_reg.Name = "LUXShadersPlugin"
    in_reg.Major = 1
    in_reg.Minor = 0

    #RegistrationInsertionPoint - do not remove this line
    # rendertree shader nodes
    in_reg.RegisterShader("Matte", 1, 0)
    # camera lense shader nodes
    in_reg.RegisterShader("LensePanorama", 1, 0)

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


def add_input_boolean(param_options, params, default_value=True, name="boolean"):
    param_options.SetDefaultValue(default_value)
    params.AddParamDef(name, c.siShaderDataTypeBoolean, param_options)


def add_input_vector(param_options, params, default_value=0, name="vector", min_visible=None, max_visible=None):
    param_options.SetDefaultValue(default_value)
    if min_visible is not None and max_visible is not None:
        param_options.SetSoftLimit(min_visible, max_visible)
    params.AddParamDef(name, c.siShaderDataTypeVector3, param_options)


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

def LUXShadersPlugin_LensePanorama_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore/Lense")
    in_ctxt.SetAttribute("DisplayName", "luxPanorama")
    return True


def LUXShadersPlugin_LensePanorama_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyLens)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_float(nonport_pram_options(), params, 360.0, "degrees", 0.0, 360.0)

    # Output Parameter: out
    add_output_closure(shaderDef, "out")

    # next init ppg
    ppgLayout = shaderDef.PPGLayout
    ppgLayout.AddGroup("Parameters")
    ppgLayout.AddItem("degrees", "Degrees")
    ppgLayout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("LuxCore")
    rendererDef.SymbolName = "LensePanorama"

    return True

#------------------------------------------------------------
#--------------------RT Shader nodes-------------------------

#---------------------matte----------------------------------
def LUXShadersPlugin_Matte_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "LuxCore")
    in_ctxt.SetAttribute("DisplayName", "luxMatte")
    return True


def LUXShadersPlugin_Matte_1_0_Define(in_ctxt):
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
    rendererDef.SymbolName = "Matte"

    return True