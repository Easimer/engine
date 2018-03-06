# Engine Model Format
The Engine Model Format is essentially a binary version of the old Studiomdl Data format models.

Both EMF and SMD can be used, but EMF deserialization is faster, though prototyping is faster with SMD.
Serialization/deserialization is provided by Google's flatbuffers library and classes are generated
from schema files.

The data structures used are found in the file `/schemas/model.fbs`.

## Model compilation
Models are compiled by the mdlc program. A QC file links the visual mesh, the collision mesh and animation source
files together. 

A linker file for this example may look like this:
```
$outfile            "data/models/wolf.emf"
$mesh               "wolf.smd"
$collision          "wolf_coll.smd"
$animation_idle     "wolf.smd"
$animation_running  "wolf_running.smd"
$animation_attack   "wolf_attack.smd"
```

Invoke the mdlc compiler like `mdlc wolf.qc` and the data/models/wolf.emf file will be created.
