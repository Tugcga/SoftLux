import win32com.client
from win32com.client import constants as c

null = None
false = 0
true = 1

app = Application


def log_message(message):
    app.LogMessage(message)


def XSILoadPlugin(in_reg):
    in_reg.Author = "Shekn Itrch"
    in_reg.Name = "LUXMenusPlugin"
    in_reg.Major = 1
    in_reg.Minor = 0

    in_reg.RegisterMenu(c.siMenuTbGetMaterialID, "Luxcore Materials", True, False)

    # volume materials
    in_reg.RegisterCommand("LuxApplyClear", "LuxApplyClear")
    in_reg.RegisterCommand("LuxApplyHeterogeneous", "LuxApplyHeterogeneous")
    in_reg.RegisterCommand("LuxApplyHomogeneous", "LuxApplyHomogeneous")

    # materials
    in_reg.RegisterCommand("LuxApplyCarpaint", "LuxApplyCarpaint")
    in_reg.RegisterCommand("LuxApplyCloth", "LuxApplyCloth")
    in_reg.RegisterCommand("LuxApplyDisney", "LuxApplyDisney")
    in_reg.RegisterCommand("LuxApplyGlass", "LuxApplyGlass")
    in_reg.RegisterCommand("LuxApplyGlossy", "LuxApplyGlossy")
    in_reg.RegisterCommand("LuxApplyGlossyCoating", "LuxApplyGlossyCoating")
    in_reg.RegisterCommand("LuxApplyGlossyTranslucent", "LuxApplyGlossyTranslucent")
    in_reg.RegisterCommand("LuxApplyMatte", "LuxApplyMatte")
    in_reg.RegisterCommand("LuxApplyMatteTranslucent", "LuxApplyMatteTranslucent")
    in_reg.RegisterCommand("LuxApplyMetal", "LuxApplyMetal")
    in_reg.RegisterCommand("LuxApplyMirror", "LuxApplyMirror")
    in_reg.RegisterCommand("LuxApplyMix", "LuxApplyMix")
    in_reg.RegisterCommand("LuxApplyNull", "LuxApplyNull")
    in_reg.RegisterCommand("LuxApplyTwoSided", "LuxApplyTwoSided")
    in_reg.RegisterCommand("LuxApplyVelvet", "LuxApplyVelvet")


def XSIUnloadPlugin(in_reg):
    strPluginName = in_reg.Name
    Application.LogMessage(str(strPluginName) + str(" has been unloaded."), c.siVerbose)
    return true


def LuxcoreMaterials_Init(ctxt):
    menu = ctxt.source
    menu.AddCommandItem("Carpaint", "LuxApplyCarpaint")
    menu.AddCommandItem("Cloth", "LuxApplyCloth")
    menu.AddCommandItem("Disney", "LuxApplyDisney")
    menu.AddCommandItem("Glass", "LuxApplyGlass")
    menu.AddCommandItem("Glossy", "LuxApplyGlossy")
    menu.AddCommandItem("Glossy Coating", "LuxApplyGlossyCoating")
    menu.AddCommandItem("Glossy Translucent", "LuxApplyGlossyTranslucent")
    menu.AddCommandItem("Matte", "LuxApplyMatte")
    menu.AddCommandItem("Matte Translucent", "LuxApplyMatteTranslucent")
    menu.AddCommandItem("Metal", "LuxApplyMetal")
    menu.AddCommandItem("Mirror", "LuxApplyMirror")
    menu.AddCommandItem("Mix", "LuxApplyMix")
    menu.AddCommandItem("Null", "LuxApplyNull")
    menu.AddCommandItem("Two Sided", "LuxApplyTwoSided")
    menu.AddCommandItem("Velvet", "LuxApplyVelvet")
    menu.AddSeparatorItem()
    menu.AddCommandItem("Volume Clear", "LuxApplyClear")
    menu.AddCommandItem("Volume Heterogeneous", "LuxApplyHeterogeneous")
    menu.AddCommandItem("Volume Homogeneous", "LuxApplyHomogeneous")


def apply_luxcore_material(selected_objects, full_name, inner_name, out_name, to_name="surface"):
    sel = None
    if selected_objects.Count > 0:
        sel = selected_objects
    else:
        o_picked = app.PickElement(c.siPolyMeshFilter, "Pick polymesh object", "Pick polymesh object")
        sel = [o_picked[2]]
    if sel is not None and len(sel) > 0:
        flag = False
        i = 0
        while not flag:
            sel_object = sel[i]
            if sel_object is not None and (sel_object.Type == "polySubComponent" or sel_object.Type == "polymsh" or sel_object.Type == "pointcloud"):
                flag = True
            else:
                i = i + 1
                if i > len(sel) - 1:
                    flag = True
                    i = -1
        if i != -1:
            # We find the polymesh
            first_obj = sel[i]
            if first_obj.Type == "polySubComponent":
                first_obj = app.CreateCluster(first_obj)[0]
            new_material = first_obj.AddMaterial("Phong", c.siBranch, inner_name)
            mat_str = str(new_material.Library) + "." + str(new_material.Name)
            node = app.CreateShaderFromProgID("LUXShadersPlugin." + full_name + ".1.0", mat_str, inner_name)
            app.SIConnectShaderToCnxPoint(mat_str + "." + inner_name + "." + out_name, mat_str + "." + to_name, False)
            app.DisconnectAndDeleteOrUnnestShaders(mat_str + ".Phong", mat_str)
            for j in range(i + 1, len(sel)):
                o = sel[j]
                if o.Type == "polySubComponent":
                    obj = app.CreateCluster(o)[0]
                else:
                    obj = o
                if obj.Type == "poly" or obj.Type == "polymsh" or obj.Type == "pointcloud":
                    app.AssignMaterial(str(new_material.FullName) + "," + str(obj.Name), "siLetLocalMaterialsOverlap")
            # may we need to add additional submaterials
            if inner_name == "luxMix":
                mix_node_a = app.CreateShaderFromProgID("LUXShadersPlugin.ShaderMatte.1.0", mat_str, "luxMatteA")
                mix_node_b = app.CreateShaderFromProgID("LUXShadersPlugin.ShaderMatte.1.0", mat_str, "luxMatteB")
                # connect node to the mix material
                app.SIConnectShaderToCnxPoint(mat_str + ".luxMatteA.material", mat_str + ".luxMix.material1", False)
                app.SIConnectShaderToCnxPoint(mat_str + ".luxMatteB.material", mat_str + ".luxMix.material2", False)
            elif inner_name == "luxTwoSided":
                two_sided_node_a = app.CreateShaderFromProgID("LUXShadersPlugin.ShaderMatte.1.0", mat_str, "luxMatteA")
                two_sided_node_b = app.CreateShaderFromProgID("LUXShadersPlugin.ShaderMatte.1.0", mat_str, "luxMatteB")
                app.SIConnectShaderToCnxPoint(mat_str + ".luxMatteA.material", mat_str + ".luxTwoSided.frontmaterial", False)
                app.SIConnectShaderToCnxPoint(mat_str + ".luxMatteB.material", mat_str + ".luxTwoSided.backmaterial", False)
            elif inner_name == "luxGlossyCoating":
                glossy_base = app.CreateShaderFromProgID("LUXShadersPlugin.ShaderMatte.1.0", mat_str, "luxMatte")
                app.SIConnectShaderToCnxPoint(mat_str + ".luxMatte.material", mat_str + ".luxGlossyCoating.base", False)
            app.InspectObj(node)
    return True


def LuxApplyCarpaint_Execute():
    app.LogMessage("LuxApplyCarpaint_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderCarpaint", "luxCarpaint", "material")
    return True


def LuxApplyCloth_Execute():
    app.LogMessage("LuxApplyCloth_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderCloth", "luxCloth", "material")
    return True


def LuxApplyDisney_Execute():
    app.LogMessage("LuxApplyDisney_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderDisney", "luxDisney", "material")
    return True


def LuxApplyGlass_Execute():
    app.LogMessage("LuxApplyGlass_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderGlass", "luxGlass", "material")
    return True


def LuxApplyGlossy_Execute():
    app.LogMessage("LuxApplyGlossys_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderGlossy", "luxGlossy", "material")
    return True


def LuxApplyGlossyCoating_Execute():
    app.LogMessage("LuxApplyGlossyCoating_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderGlossyCoating", "luxGlossyCoating", "material")
    return True


def LuxApplyGlossyTranslucent_Execute():
    app.LogMessage("LuxApplyGlossyTranslucent_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderGlossyTranslucent", "luxGlossyTranslucent", "material")
    return True


def LuxApplyMatte_Execute():
    app.LogMessage("LuxApplyMatte_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderMatte", "luxMatte", "material")
    return True


def LuxApplyMatteTranslucent_Execute():
    app.LogMessage("LuxApplyMatteTranslucent_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderMatteTranslucent", "luxMatteTranslucent", "material")
    return True


def LuxApplyMetal_Execute():
    app.LogMessage("LuxApplyMetal_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderMetal", "luxMetal", "material")
    return True


def LuxApplyMirror_Execute():
    app.LogMessage("LuxApplyMirror_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderMirror", "luxMirror", "material")
    return True


def LuxApplyMix_Execute():
    app.LogMessage("LuxApplyMix_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderMix", "luxMix", "material")
    return True


def LuxApplyNull_Execute():
    app.LogMessage("LuxApplyNull_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderNull", "luxNull", "material")
    return True


def LuxApplyTwoSided_Execute():
    app.LogMessage("LuxApplyTwoSided_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderTwoSided", "luxTwoSided", "material")
    return True


def LuxApplyVelvet_Execute():
    app.LogMessage("LuxApplyVelvet_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "ShaderVelvet", "luxVelvet", "material")
    return True


def LuxApplyClear_Execute():
    app.LogMessage("LuxApplyClear_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "VolumeClear", "luxClear", "volume", "volume")
    return True


def LuxApplyHeterogeneous_Execute():
    app.LogMessage("LuxApplyHeterogeneous_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "VolumeHeterogeneous", "luxHeterogeneous", "volume", "volume")
    return True


def LuxApplyHomogeneous_Execute():
    app.LogMessage("LuxApplyHomogeneous_Execute called", c.siVerbose)
    apply_luxcore_material(app.Selection, "VolumeHeterogeneous", "luxHomogeneous", "volume", "volume")
    return True