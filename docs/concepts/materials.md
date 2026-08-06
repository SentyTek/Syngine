# Syngine Documentation
# Concept: Rendering - Materials
[<-- Back](../index.md)

Materials in Syngine are split into two layers:

- `Material` (immutable asset): owns a shader plus default parameter/texture values.
- `MaterialInstance` (mutable runtime data): per-mesh overrides used at draw time.

Shaders declare what they need through metadata (`MaterialParameterDesc` and `TextureParameterDesc`). A `Material` is initialized from those declarations, then each mesh/submesh uses a `MaterialInstance` to provide actual values.

## Current Default Material Path

Syngine currently builds imported mesh materials from `GetDefaultMaterialPBR(...)`, which selects:

- `default_texture` when the source material has a base color/diffuse texture.
- `default` when it does not.

This means there is not just one runtime material object anymore. There are default material templates plus per-mesh `MaterialInstance`s.

## Model Loading (Assimp/glTF)

During 3D model loading, Syngine:

- Starts from a default PBR material instance.
- Sets `u_uvScale` (from `$uvScale`, or `1.0` fallback).
- If textures are disabled, reads diffuse/base color into `u_baseColor`.
- If textures are enabled, maps base color/diffuse texture to `s_albedo` (stage 0).
- If textures are enabled, maps normal texture to `s_normalMap` (stage 2).

Notes:

- Embedded textures are supported (common for `.glb`), and this is the primary path used by the current importer.
- If no scene materials exist, Syngine creates one fallback default PBR instance.
- Height maps are currently not used by shaders.

## Renderer Upload Behavior

At draw time, RenderCore uploads:

- Material parameters: instance override value if present, otherwise material default value.
- Material textures: instance override texture if present, otherwise material default texture.

Default textured materials include fallback textures:

- Albedo fallback: magenta (debug-visible missing texture).
- Normal fallback: flat normal map.

Because bindings are driven by shader metadata, only parameters/textures declared by the shader are valid for `Set(...)` and `SetTexture(...)`.
