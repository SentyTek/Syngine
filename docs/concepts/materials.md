# Syngine Documentation
# Concept: Rendering - Materials
[<-- Back](../index.md)

Materials are how shaders know what to actually draw onto a model. It contains the actual instance data applied to a specific mesh. While shaders are aware of what uniforms need per-material data (called `MaterialParameterDesc`s or `TextureParameterDesc`s), the material(s) on a mesh contain the actual data sent to the GPU.

Syngine currently only has one basic Material: the `Material PBR` material, used by the Lit PBR shader. Syngine will attempt to load this material onto most imported glTF objects.

Materials may contain an array of `MaterialParameter`s and `TextureParameters`, uploaded by the renderer during draw time.
