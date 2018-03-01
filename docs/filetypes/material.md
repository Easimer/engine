# material file format
Defines a material, connecting textures to a shader. The file uses a minimal subset of the GoldSrc/Source QC format. These files are stored at /data/materials/.

## Fields
* $shader
  * Name of the shader

Further fields depend on the shader used. Usually they are like $diffuse, $normal, etc.