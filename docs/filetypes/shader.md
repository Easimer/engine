# shader file format
Defines a shader program by connecting at least a vertex and fragment shader. The file uses a minimal subset of the GoldSrc/Source QC format. These files are stored at /data/shaders/.

## Fields
* $name
  * Name of the shader
* $description
  * Description of the shader
* $vertexshader
  * Path to vertex shader
* $fragmentshader
  * Path to fragment shader
* $uniformview
  * Name of the uniform where the view matrix is uploaded
* $uniformtrans
  * Name of the uniform where the transform matrix is uploaded
* $uniformproj
  * Name of the uniform where the projection matrix is uploaded
* $diffusekey
  * Name of the field where the diffuse texture is specified in the material file
* $normalkey
  * Name of the field where the normal texture is specified in the material file
* $specularkey
  * Name of the field where the specular texture is specified in the material file
* $opacitykey
  * Name of the field where the opacity texture is specified in the material file
* $diffusedefault
  * Path to default diffuse texture
* $normaldefault
  * Path to default normal texture
* $speculardefault
  * Path to default specular texture
* $opacitydefault
  * Path to default opacity texture