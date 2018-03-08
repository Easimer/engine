# engine core entities

## prop_dynamic
Renderable 3D model that may move (translate, rotate or scale) in the world or it may have animations.
* 3D Renderable
* Translate/Rotate/Scale
* Animated
* Transformation matrix recalculated every frame
* Affects physical entities as a collider but does not react to physics forces itself

## prop_static
Renderable 3D model that will not move and is not animated in any way.
* 3D Renderable
* Won't translate/rotate/scale after ::spawn()
* Not animated
* Transformation matrix cached at spawn
* Affects physical entities as a collider but does not react to physics forces itself

## prop_physics (NYI)
Renderable 3D model that is affected by physical forces but may not be animated.
* 3D Renderable
* Translate/Rotate/Scale
* Not animated
* Transformation matrix recalculated every frame
* Affected by physical forces and collides with other entities

## light_point (NYI)
Point light that has a position, ambient, diffuse and specular colors (Red Green Blue and brightness)
and constant, linear and quadratic attenuation values.
* Shades 3D Renderables (that are NOT unlit)
* A 3D Renderable may only be affected by 2 non-sky light sources at a time (they are automatically selected based on distance).
Attenuation values are from 0.0 to 1.0 and they MUST add up to 1.

On Constant-Linear-Quadratic Falloff, see https://developer.valvesoftware.com/wiki/Constant-Linear-Quadratic_Falloff

## light_sky (NYI)
Directional light coming from a sky. It's characterized by a color, a brightness and a rotation.