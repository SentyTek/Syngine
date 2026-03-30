# Syngine Documentation
# Guide: Creating a Component
[<-- Back](../index.md)

This guide shows how to create a custom component using `ComponentTypeID`, register it with `ComponentRegistry`, add it to a `GameObject`, and support prefab/XML loading.

## Overview

In Syngine, a custom component needs 4 things:

1. A unique `ComponentTypeID`
2. A class that inherits from `Syngine::Component`
3. Serialization and deserialization support
4. Static registration in `ComponentRegistry`

---

## 1. Pick a Unique ComponentTypeID

Built-in components use IDs from `DefaultComponents`. For custom components, use your own stable value that does not collide with built-ins.

Example:

```cpp
static constexpr Syngine::ComponentTypeID SYN_COMPONENT_HEALTH = 1001;
```

Use a consistent range for your game (for example: `1000+` for game-specific components).

---

## 2. Create the Header

Create a new file such as `HealthComponent.h`:

```cpp
#pragma once

#include "Syngine/ECS/Component.h"

namespace Syngine {

class HealthComponent : public Component {
  public:
	static constexpr ComponentTypeID componentType = 1001;

	explicit HealthComponent(GameObject* owner, int maxHealth = 100)
		: m_maxHealth(maxHealth), m_health(maxHealth) {
		m_owner = owner;
	}

	ComponentTypeId GetComponentType() override { return componentType; }

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<HealthComponent>(*this);
	}

	Serializer::DataNode Serialize() const override {
		Serializer::DataNode node;
		node["type"] = static_cast<Syngine::ComponentTypeId>(componentType);
		node["health"] = m_health;
		node["maxHealth"] = m_maxHealth;
		return node;
	}

	void Update(float deltaTime) override {
		(void)deltaTime;
		// Optional per-frame behavior.
	}

	int GetHealth() const { return m_health; }
	int GetMaxHealth() const { return m_maxHealth; }
	void SetHealth(int health) { m_health = (health < 0) ? 0 : health; }

  private:
	int m_health = 100;
	int m_maxHealth = 100;
    GameObject* m_owner = nullptr;
};

} // namespace Syngine
```

This contains several useful parts:
- The constructor sets values obviously. The first parameter in a Component ctor must be `GameObject*`, as this is set by the engine.
- `GetComponentType()` and `Clone()` methods are required by the engine as well.
- `Serialize()` to convert the live Component into DataNode(s) for saving to disk.
- `Init()` and `Update(float deltaTime)` methods are provided as well, the engine should call these accordingly, but they are optional in a Component.

---

## 3. Register the Component (CPP)

Create `HealthComponent.cpp` and register parser + factory:

```cpp
#include "HealthComponent.h"
#include "Syngine/ECS/ComponentRegistry.h"

namespace Syngine {

static ComponentRegistrar s_healthRegistrar(
	HealthComponent::componentType,

	// Parse XML -> DataNode
	[](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
		Serializer::DataNode node;
		node["type"] = static_cast<uint64_t>(HealthComponent::componentType);

		for (const auto& attr : elem->attributes()) {
			const std::string key = attr->tag().cstr();
			const std::string value = attr->data().cstr();

			if (key == "health") {
				node["health"] = std::stoi(value);
			} else if (key == "maxHealth") {
				node["maxHealth"] = std::stoi(value);
			}
		}

		return node;
	},

	// DataNode -> Component instance
	[](GameObject* owner, const Serializer::DataNode& data)
		-> std::unique_ptr<Component> {
		const int maxHealth = data["maxHealth"].As<int>(100);
		auto comp = std::make_unique<HealthComponent>(owner, maxHealth);
		comp->SetHealth(data["health"].As<int>(maxHealth));
		return comp;
	}
);

} // namespace Syngine
```

The static `ComponentRegistrar` ensures the component is registered during startup.

---

## 4. Add the Component to a GameObject

You can add the custom component the same way as built-ins:

```cpp
GameObject* enemy = new GameObject("Enemy");
enemy->AddComponent<Syngine::TransformComponent>();
enemy->AddComponent<Syngine::HealthComponent>(250);

auto* hp = enemy->GetComponent<Syngine::HealthComponent>();
if (hp) {
	hp->SetHealth(200);
}
```

If you only have an ID at runtime (for tools/editor code):

```cpp
Syngine::Component* c = enemy->GetComponent(Syngine::HealthComponent::componentType);
```

---

## 5. Ensure It Is Compiled and Linked

`HealthComponent.cpp` must be part of your target build. If it is not linked, static registration will never run.

Add it to your game or engine `CMakeLists.txt` sources list.

---

## 6. Prefab/XML Example

A serialized component entry should look like:

```xml
<Component type="1001" health="80" maxHealth="100" />
```

On load:

1. Prefab reads `type`
2. `ComponentRegistry::ParseXml(type, elem)` builds a `DataNode`
3. `ComponentRegistry::Instantiate(type, owner, data)` creates the component

---

## Best Practices

- Keep component IDs stable once shipped.
- Keep `Serialize()` and parser keys in sync (`health`, `maxHealth`, etc).
- Use `Clone()` for deep copy of custom fields.
- Avoid heavy work in constructors; do setup in `Init()` where possible.
- Use `Update()` and `PostPhysicsUpdate()` only when needed.

---

## Common Mistakes

- Duplicate component IDs across two classes.
- Forgetting to compile/link the component `.cpp` file (no registrar execution).
- Using different field names between `Serialize()` and XML parser.
- Returning `nullptr` from factory on valid data paths.

---

With this pattern, your custom components behave the same as built-in components for add/get/remove, serialization, and prefab deserialization.
