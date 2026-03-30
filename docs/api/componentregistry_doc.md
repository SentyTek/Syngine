# Syngine API Documentation


## ComponentRegistry.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/ComponentRegistry.h)

---

## Goto:


- [Serializer::DataNode](#componentregistryserializer::datanode)
- [Register](#componentregistryregister)
- [ParseXml](#componentregistryparsexml)
- [Instantiate](#componentregistryinstantiate)

---

#### **`ComponentRegistry::Serializer::DataNode`**


 ComponentRegistry is a global registry that maps component type IDs to their corresponding XML parsing and instantiation functions. This allows for dynamic creation of components from serialized data without hardcoding each type in the deserialization logic.

Signature:

```cpp
 public: using ParseXmlFn = std::function<Serializer::DataNode(const scl::xml::XmlElem*)>;
```

---

#### **`ComponentRegistry::Register`**


 Register a component type with its XML parsing and instantiation functions

**Example:** static Syngine::ComponentRegistrar s_compRegistrar(MY_COMPONENT_TYPE_ID,  // ParseXML: XML element -> DataNode [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {}, // Instantiate: DataNode -> Component instance [](GameObject* owner, const Serializer::DataNode& data) -> std::unique_ptr<Syngine::Component> {} );

Signature:

```cpp
 static void Register(Syngine::ComponentTypeID type, ParseXmlFn parseFn, InstantiateFn instantiateFn);
```

**Parameters:**

- `type`: The unique ID of the component type (should match the enum value in Components)
- `parseFn`: A function that takes an XML element and returns a DataNode representing the component's data
- `instantiateFn`: A function that takes a GameObject and a DataNode, and returns a unique pointer to a new instance of the component initialized with the data

**This function has been available since:** v0.0.1

---

#### **`ComponentRegistry::ParseXml`**


 Parse an XML element into a DataNode for a given component type

Signature:

```cpp
 static Serializer::DataNode ParseXml(Syngine::ComponentTypeID type, const scl::xml::XmlElem* node);
```

**Parameters:**

- `type`: The unique ID of the component type
- `node`: The XML element containing the component data

**Returns:** A DataNode representing the component's data, or an empty DataNode if the type is not registered

**This function has been available since:** v0.0.1

---

#### **`ComponentRegistry::Instantiate`**


 Instantiate a component of a given type from a DataNode

Signature:

```cpp
 static std::unique_ptr<Component> Instantiate(Syngine::ComponentTypeID type, GameObject* owner, const Serializer::DataNode& data);
```

**Parameters:**

- `type`: The unique ID of the component type
- `owner`: The GameObject that will own the new component instance
- `data`: The DataNode containing the component's data

**Returns:** A unique pointer to the new component instance, or nullptr if the type is not registered

---
