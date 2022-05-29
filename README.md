# SoftLux addon for Softimage

This addon is integrates [LuxCoreRender](https://luxcorerender.org/) into Softimage. The repository contains the source of the addon. The core of the integration is written on ```c++```, shader definitions and some utility commands are writen on ```Python```. 

[Release page](https://github.com/Tugcga/SoftLux/releases) contains compiled and packed version of the addon.

[Wiki page](https://github.com/Tugcga/SoftLux/wiki) contains small manual. It covers basic functionality of the addon. The wiki pages are open for editing. So, if anyone would like to fix errors or add some information - feel free to improve the documentation of the addon.

If you have some question or find a bug - use [Issues page](https://github.com/Tugcga/SoftLux/issues).

# How to build

It's possible to build the addon from sources. You need some praparations.

1. Clone the repository into the directory ```\Addons\SoftLux\Application\Plugins\``` of some workgroup.

2. Download archives ```luxcorerender.zip``` and ```luxcore_binaries.zip``` from the [Release page](https://github.com/Tugcga/SoftLux/releases/tag/1.0).

3. Unzip archves to the repository folder. ```luxcore.lib``` (and ```include``` folder) should have the path ```\Addons\SoftLux\Application\Plugins\dst\luxcorerender\luxcore.lib```. ```luxcore.dll``` (and other files from the same directory) should have the path ```\Addons\SoftLux\Application\Plugins\bin\nt-x86-64\luxcore.dll```

After that simply run VisualStudio solution ```\Addons\SoftLux\Application\Plugins\src\SoftLuxPlugin.sln```. All should works. Compiled addon ```SoftLux.dll``` should be placed to the directory, which contains ```luxcore.dll```. For compile the addon requires Visual Studio 2019. 

# Features

Current version of the addon does not supports all features of the Luxcore render or Softimage, but at least it allows to render some images.

## What is supported from Luxcore

Current version of the Luxcore render, used by addon, is 2.6 (released at december 2021). 

* All materials, textures (except Blender procedurals), volumes

* ```Path CPU, Bidir CPU, Path OCL``` render engines

* Export scene into standalone format (text or binary)

* All supported samplers, filters and light stratagies

* All output film channels

* All image pipline plugins, except ```OpenColorIO``` tonemapper. These plugins implemented as custom pass shaders

* Object motion blur

* Hair shapes

* Mesh shapes (except ```Simplify```). This feature implemented as special material nodes, connected to the ```Contour``` input port of the root material node

* All light sources



## What does not supported from Luxcore

* ```Tile``` and ```RT``` render engines

* ```OpenColorIO``` tonemapper

* Blender procedure textures

* Densitygrid texture for volume objects. Now there is very restricted support of the OpenVDB, and there are no ways to render custom volumes (obtained from ICE, for example)

* Caching features of the Luxcore

* Subdivide shape does not work with mesh shape, which contains vertex color. It seems that this is a Luxcore bug

* Simplify shape. This shape is disabled, because it works but in wrong way


## What is supported from Softimage

One of the principle of the addon is use native Softimage functionality as mush as possible. So, this addon does not contains any custom properties and try to get data from the scene objects from native properties. Although in some situations it is not convenient.

* Incremental scene update for faster scene prepare stage before actual rendering

* Polygon meshes, ICE strands and hair objects

* Model instances and particle instances

* Polygon mesh vertex color and uvs

* Shaderball material previews

* Several buil-in texture nodes: ```ScalarToColor, ColorToScalar, ColorToVector, VectorToColor, Image```

* Texture baking

* Camera and custom camera shaders

* Default light sources

* Ambient global light source

* Custom pass shaders

* Export scene into standalone format for rendering in external standalone application


## What does not supported from Softimage

* Surfaces and curves

* Particles with shape different from instance shape

* Material preview for texture nodes

* ICE attributes, but there is possibility to save these attributes into vertex color and then use colors in shaders

* Built-in ```Image``` texture node can be used only for the first uv of the object. Also only texture repeat parameters are supported


# Known issues

* When render on GPU, by using ```PATH OCL``` render engine, the system recompile the kernels after each Softimage restart. It takes a lot of time. 
