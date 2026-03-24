# Syngine Documentation
# Guide: Using Prefabs
[<-- Back](../index.md)

This guide explains how Prefabs work in the current engine implementation.
It is based on the runtime flow in:

- engine/src/Syngine/Utils/Prefab.cpp
- engine/src/Syngine/Utils/Serializer.h
- engine/src/Syngine/ECS/GameObject.cpp
- game/src/Game.cpp

## What A Prefab Is (Right Now)

A prefab is a serialized GameObject tree (root object + children + components)
stored as XML. At runtime, loading a prefab creates a new GameObject tree from
that XML data.

In the current code, the prefab root XML tag is Prefab with:

- Version (must match SYNINT_PREFAB_VERSION, currently 1.0)
- Name (derived from root GameObject name)

## Save Flow

Current save API:

~~~cpp
GameObject* cube = new GameObject("cube", "default");
cube->AddComponent<Syngine::TransformComponent>();
cube->AddComponent<Syngine::MeshComponent>("meshes/cube.glb", false);

Syngine::Serializer::Prefab cubePrefab(cube);
cubePrefab.SaveToFile("cube_prefab.xml");
~~~

What happens internally:

1. Serializer::Prefab(GameObject* root) snapshots root->Serialize() into
	rootGameObjectData.
2. SaveToFile(path) writes XML under appdata/window-title path + your relative
	prefab path.
3. Internal function WriteGameObject recursively writes:
	- GameObject attributes
	- Component elements
    - Child GameObjects

## Load Flow

Current load API:

~~~cpp
Syngine::Serializer::Prefab prefab("cube_prefab.xml");
GameObject* cube = prefab.rootGameObject;
~~~

What happens internally:

1. Prefab constructor calls LoadFromFile(path).
2. XML is loaded and Version is validated.
3. Root GameObject element is deserialized to DataNode.
4. A runtime GameObject tree is created with:
	- new GameObject(prefab.rootGameObjectData)
5. prefab.rootGameObject points to the instantiated root object.

## XML Shape

A simplified prefab file looks like this:

~~~xml
<Prefab Version="1.0" Name="cube">
  <GameObject name="cube" type="default" gizmo="none" isActive="true" tags="default">
	 <Component type="1" position="0,10,0" rotation="0,0,0,1" scale="2,2,2" />
	 <Component type="2" path="meshes/cube.glb" hasTextures="false" />
	 <Children>
		<GameObject name="child" type="default" gizmo="none" isActive="true" tags="default">
		  <Component type="..." />
		</GameObject>
	 </Children>
  </GameObject>
</Prefab>
~~~

Notes:

- type on Component is ComponentTypeID.
- Component-specific fields are parsed through ComponentRegistry parse callbacks.

## How Components Participate In Prefabs

Prefab loading depends on ComponentRegistry.

For each Component XML node:

1. Prefab deserializer reads Component type.
2. ComponentRegistry::ParseXml(type, elem) converts XML -> DataNode.
3. GameObject(DataNode) calls ComponentRegistry::Instantiate(type, owner, data)
	to build the component instance.

If a component type is not registered, that component cannot be restored.

## GameObject Fields Currently Restored By Prefab Deserialize

GameObject-level deserialize currently handles:

- name
- type
- gizmo
- isActive
- tags (CSV in XML, restored to string array)
- components
- children (recursive)

## Current Limitations And Gotchas

- Prefab paths are resolved relative to appdata/window-title, not project root.
- Version mismatch rejects load immediately.
- Unknown/unregistered component types log an error and are skipped.
- Prefab(root) captures a serialized snapshot at construction time.

## Troubleshooting

If loading fails, check these first:

1. File exists at the appdata-resolved prefab path.
2. Root tag is Prefab with Version="1.0".
3. Root contains a GameObject element.
4. Component type IDs in XML match registered component types in your build.
5. Component .cpp files are linked so their static registrars execute.

## Recommended Workflow

1. Build your GameObject tree in code.
2. Construct Serializer::Prefab from the root object.
3. Save with a stable prefab filename.
4. Load later with Serializer::Prefab(path).
5. Use prefab.rootGameObject as the instantiated runtime object.