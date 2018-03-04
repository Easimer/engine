# Engine Model Format Specification
The Engine Model Format is essentially a binary version of the old Studiomdl Data format separated into several files.

Every multibyte value are in big endian order.
## Header
### nID
8 bytes having the value 0xa655104063d85922.
### iType
8 bytes signifying the file data.
```
EMF_T_VERTEXDATA = 0x01,
EMF_T_COLLDATA = 0x02,
EMF_T_SKELETON = 0x03,
EMF_T_KEYFRAMES = 0x04
```
### szMaterial
256 character long path to the model material.

## Vertex Data
Describes the model itself. An array of triangles.
```
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
```
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
```
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