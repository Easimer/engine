# model file format
Defines a mesh. The engine uses the Valve Studiomdl Data format.
Contrary to GoldSrc/Source engine, SMD files are not compiled here (yet).
These files are stored at /data/models/.

Although a material is defined for every triangle in the data, only the first material will be used on every triangle of the model.

More info: https://developer.valvesoftware.com/wiki/.smd

Bug: in the current parser implementation, whitespaces longer than one space are detected as errors.