# Engine Model Format Specification
The Engine Model Format is essentially a binary version of the old Studiomdl Data format separated into several files.

Every multibyte value are in big endian order.
## Header
### nID
8 bytes having the value 0xa655104063d85922.
### iType
8 bytes signifying the file data.
```c++
EMF_T_MANIFEST = 0x00,
EMF_T_VERTEXDATA = 0x01,
EMF_T_COLLDATA = 0x02,
EMF_T_SKELETON = 0x03,
EMF_T_KEYFRAMES = 0x04
```

## Manifest
Contains the path to the model's material, framerate, animations, etc.

```c++
struct emf_manifest {
    char szMaterial[256];
    uint64_t nFramerate;
    uint64_t nAnimations;
    char szAnimations[64][nAnimations];
}
```

### szMaterial
Path to the model material

### nFramerate
Framerate of all animations

### nAnimations
Count of animations

### szAnimation
Animation name

## Vertex Data
Describes the model itself. An array of triangles.
```c++
struct emf_vertexdata {
	uint64_t nTriangles;
	emf_triangle triangles[nTriangles];
}

struct emf_triangle {
	emf_vertex vertices[3];
}
struct emf_vertex {
	uint64_t iBone;
	float px, py, pz;
	float nx, ny, nz;
	float u, v;
}
```

## Collision Mesh
Same format as the Vertex Data.

## Skeleton
```c++
struct emf_skeleton {
    uint64_t nBones;
    emf_bone bones[nBones];
}

struct emf_bone {
    uint64_t iBone;
    char szName[64];
}
```

## Keyframes
Animation framerate is specified in the linker file.
```c++
struct emf_keyframes {
    uint64_t nKeyframes;
    emf_keyframe keyframes[nKeyframe];
}

struct emf_keyframe {
    uint64_t iBone;
    uint64_t iTime;
    float px, py, pz;
    float rx, ry, rz; // euler engles in radians
}
```

## Model compilation
Models are compiled by the mdlc program. A QC file links the visual mesh, the collision mesh and animation source
files together. The mdlc processes this file and outputs binary files, like these:
```
- wolf.emf
- wolf.emf.vtx
- wolf.emf.coll
- wolf.emf.skel
- wolf.emf.anim.idle
- wolf.emf.anim.running
- wolf.emf.anim.attack
```

A linker file for this example may look like this:
```
$outfile            "data/models/wolf.emf"
$mesh               "wolf.smd"
$collision          "wolf_coll.smd"
$animation_idle     "wolf.smd"
$animation_running  "wolf_running.smd"
$animation_attack   "wolf_attack.smd"
```
