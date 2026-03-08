// ╒═══════════════════════ Serializer.h ═╕
// │ Syngine                              │
// │ Created 2026-01-26                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

#include "Syngine/Core/Logger.h"

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <cstdint>
#include <cstddef>

#include "../../lib/miniscl.hpp"

#define SYNINT_PREFAB_VERSION "1.0"
#define SYNINT_SCENE_VERSION "1.0"
#define SYNINT_CORESETTINGS_VERSION "1.0"

namespace Syngine {
    class GameObject; // Forward declaration to avoid circular dependency with GameObject.h
namespace Internal {
std::string
ResolvePath(const char* path); // Had issues with FsUtils so added this as a
                               // workaround for now. Will refactor later.
}

/// @brief Utility class for serializing and deserializing engine and game data
/// to disk.
/// @since v0.0.1
class Serializer {
  public:
    // Helper types for serialization
    struct Float3 { float x, y, z; };
    struct Float4 { float x, y, z, w; };
    struct Mat3   { float data[9]; };
    struct Mat4   { float data[16]; };

    /// @brief A flexible data node that can represent various data types
    /// and hierarchical structures (like JSON or YAML).
    /// @since v0.0.1
    class DataNode {
        using NodeMap = std::unordered_map<std::string, DataNode>;
        using NodeArray = std::vector<DataNode>;

        // Internal data representation
        // std::monostate represents a null/empty state
        std::variant<std::monostate, int, float, bool, std::string, NodeMap, NodeArray> m_data;
        
        friend class Serializer; // Allow Serializer to access private members for serialization
      public:
        /// @brief The type of data stored in the DataNode
        /// @since v0.0.1
        enum class Type {
            Null,    //* No data
            Integer, //* Integer number
            Float,   //* Floating-point number
            Boolean, //* Boolean value
            String,  //* String value
            Object,  //* Key-value map
            Array    //* Ordered list (float4, mat3, component, etc.)
        };

        /// @brief Checks if the DataNode contains a key (for Object type nodes)
        /// @param key The key to check for
        /// @return True if the key exists, false otherwise
        /// @since v0.0.1
        bool Has(const std::string& key) const;

        /// @brief Accesses a child DataNode by key (for Object type nodes)
        /// @param key The key of the child node
        /// @return Reference to the child DataNode
        /// @example playerNode["health"] = 100;
        /// @since v0.0.1
        DataNode& operator[](const std::string& key) {
            if (!std::holds_alternative<NodeMap>(m_data)) {
                m_data = NodeMap{};
            }
            auto& map = std::get<NodeMap>(m_data);
            return map[key]; // This will default-construct a new DataNode if key
                            // doesn't exist
        }

        const DataNode& operator[](const std::string& key) const {
            if (!std::holds_alternative<NodeMap>(m_data)) {
                throw std::runtime_error("DataNode is not an object");
            }
            const auto& map = std::get<NodeMap>(m_data);
            auto        it  = map.find(key);
            if (it == map.end()) {
                throw std::runtime_error("Key not found in DataNode: " + key);
            }
            return it->second;
        }

        /// @brief Assignment operator to copy data from another DataNode
        /// @param other The other DataNode to copy from
        /// @return Reference to this DataNode
        /// @since v0.0.1
        DataNode& operator=(const DataNode& other) {
            if (this != &other) {
                this->m_data = other.m_data;
            }
            return *this;
        }

        /// @brief Assignment operator to set a value directly (int, float,
        /// bool, string, etc.)
        /// @tparam T The type of the value to set (int, float, bool,
        /// std::string, etc.)
        /// @param value The value to set
        /// @return Reference to this DataNode
        /// @since v0.0.1
        template <typename T> DataNode& operator=(const T& value);

        /// @brief Assignment operator for C-style string literals to set a string value
        /// @param value The C-style string literal to set
        /// @return Reference to this DataNode
        /// @since v0.0.1
        DataNode& operator=(const char* value) {
            m_data = std::string(value);
            return *this;
        }

        /// @brief Accesses a child DataNode by index (for Array type nodes)
        /// @param index The index of the child node
        /// @return Reference to the child DataNode
        /// @example playerNode / "name" / "last" = "Marston";
        /// @since v0.0.1
        DataNode& operator/(const std::string& value) { return (*this)[value]; }

        bool operator!() const { return std::holds_alternative<std::monostate>(m_data); }

        /// @brief Converts the DataNode to a specific type
        /// @tparam T The type to convert to (int, float, bool, std::string, etc.)
        /// @param defaultValue The default value to return if conversion fails
        /// @return The converted value or the default value
        /// @since v0.0.1
        template <typename T> T As(const T& defaultValue = T()) const;

        /// @brief Sets the value of the DataNode
        /// @tparam T The type of the value to set (int, float, bool, std::string, etc.)
        /// @param value The value to set
        /// @since v0.0.1
        template <typename T> void Set(const T& value);

        /// @brief Gets the type of data stored in the DataNode
        /// @return The Type enum value representing the data type
        /// @see Serializer::DataNode::Type
        /// @since v0.0.1
        Type GetType() const;

        /// @brief Checks if the DataNode is of a specific type
        /// @return True if the DataNode is an Object type
        /// @since v0.0.1
        bool IsObject() const;

        /// @brief Checks if the DataNode is of a specific type
        /// @return True if the DataNode is an Array type
        /// @since v0.0.1
        bool IsArray() const;

        /// @brief Checks if the DataNode is null/empty
        /// @return True if the DataNode is null/empty
        /// @since v0.0.1
        bool IsNull() const;

        /// @brief Gets the size of the Array type DataNode
        /// @return The number of elements in the array
        /// @pre IsArray() must be true (DataNode is of Array type)
        /// @since v0.0.1
        size_t Size() const;

        /// @brief Accesses an element of the Array type DataNode by index
        /// @param index The index of the element to access
        /// @return Reference to the child DataNode at the specified index
        /// @pre IsArray() must be true (DataNode is of Array type)
        /// @since v0.0.1
        DataNode& At(size_t index);

        /// @brief Appends a new empty DataNode to the Array type DataNode
        /// @return Reference to the newly appended DataNode
        /// @pre IsArray() must be true (DataNode is of Array type)
        /// @since v0.0.1
        DataNode& Append();

        auto begin() const; // Iterator for Object type nodes
        auto end() const;  // Iterator for Object type nodes

        /// @brief Gets all keys in the Object type DataNode. For iteration.
        /// @return Vector of keys as strings
        /// @pre IsObject() must be true (DataNode is of Object type)
        /// @since v0.0.1
        std::vector<std::string> GetKeys() const;
    };

    /// @brief Represents core engine settings like video, audio, controls, etc.
    /// @since v0.0.1
    struct CoreSettings {
        struct Video {
            uint32_t width = 1920; //* Default 1080p
            uint32_t height = 1080; //* Default 1080p
            bool     fullscreen = false; //* Windowed by default
            float    brightness = 1.0f; //* Normal brightness
            bool     vSync = true; //* V-Sync enabled by default
            
            DataNode Serialize() const; //* Helper to serialize video settings
            void Deserialize(const DataNode& node); //* Helper to deserialize video settings
        } video;

        /// @brief Audio settings like master volume and channel volumes
        /// @since v0.0.1
        struct Audio {
            float masterVolume = 1.0f; //* Master volume (0.0 to 1.0)
            float channels[8] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; //* Volume for 8 audio channels
            
            DataNode Serialize() const; //* Helper to serialize audio settings
            void Deserialize(const DataNode& node); //* Helper to deserialize audio settings
        } audio;

        /// @brief Control settings like mouse sensitivity and key bindings
        /// @since v0.0.1
        struct Controls {
            float mouseSensitivity = 1.0f; //* Normal sensitivity
            bool  invertY          = false; //* Y-axis not inverted by default
            DataNode binds; //* Key bindings as a map of action -> key
            
            DataNode Serialize() const; //* Helper to serialize control settings
            void Deserialize(const DataNode& node); //* Helper to deserialize control settings
        } controls;
        
        DataNode customSettings; //* For game-specific settings as DataNode tree
        
        DataNode Serialize() const; //* Helper to serialize core settings
        void Deserialize(const DataNode& node); //* Helper to deserialize core settings
        
        static bool SaveToFile(const std::string& path, const CoreSettings& settings); //* Save settings to file
        static CoreSettings LoadFromFile(const std::string& path); //* Load settings from file
    };

    /// @brief Represents a prefab/asset - a reusable GameObject template with all its components
    /// @since v0.0.1
    struct Prefab {
        std::string name; //* Human-readable name of the prefab
        std::string guid; //* Unique identifier for the prefab
        DataNode    rootGameObject; //* Serialized GameObject tree (with all
                                    //children and components)
        bool isValid =
            false; //* Indicates if the prefab was successfully loaded/created

        Prefab() =
            delete; // Force use of parameterized constructor or factory methods
        Prefab(GameObject* root); // Construct prefab from a GameObject (serializes it)
        
        DataNode Serialize() const; //* Helper to serialize prefab
        GameObject* Deserialize(const DataNode& node); //* Helper to deserialize prefab
        
        bool SaveToFile(const std::string& path); //* Save prefab to file
        Prefab LoadFromFile(const std::string& path); //* Load prefab from file
    };

    /// @brief Represents a complete game scene with all GameObjects and scene settings
    /// @since v0.0.1
    struct Scene {
        std::string name; //* Human-readable name of the scene
        std::string guid; //* Unique identifier for the scene
        DataNode gameObjects; //* Array of root GameObjects in the scene
        DataNode sceneSettings; //* Scene-specific settings (lighting, fog, skybox, etc.)
        
        DataNode Serialize() const; //* Helper to serialize scene
        void Deserialize(const DataNode& node); //* Helper to deserialize scene
        
        static bool SaveToFile(const std::string& path, const Scene& scene); //* Save scene to file
        static Scene LoadFromFile(const std::string& path); //* Load scene from file
    };

    /// @brief Represents a complete game save file
    /// @since v0.0.1
    struct SaveData {
        DataNode state; //* Serialized game state (player data, world state, etc. Game sets this up)
        
        DataNode Serialize() const; //* Helper to serialize save data
        void Deserialize(const DataNode& node); //* Helper to deserialize save data
        
        static bool SaveToFile(const std::string& path, const SaveData& saveData); //* Save save data to file
        static SaveData LoadFromFile(const std::string& path); //* Load save data from file
    };

    // Internal helper to read an asset from a bundle file.
    // The theory behind this is that we open the bundle, extract the asset as a
    // stream, and then the caller can handle deserializing that stream into
    // whatever type T they want (like an image, model, prefab, etc.)
    static inline scl::stream _ReadFromBundle(const std::string& bundlePath,
                                       const std::string& assetPath) {
        scl::pack::Packager pack;
        scl::path           resolvedBundlePath =
            Internal::ResolvePath(bundlePath.c_str()).c_str();
        if (!resolvedBundlePath.exists()) {
            Logger::LogF(LogLevel::ERR, "Bundle file not found: %s", bundlePath.c_str());
            return scl::stream();
        }
        if (!pack.open(resolvedBundlePath)) {
            Logger::LogF(LogLevel::ERR, "Failed to open bundle: %s", bundlePath.c_str());
            return scl::stream();
        }

        scl::pack::PackIndex* wts = pack.openFile(assetPath.c_str());
        wts->waitable().wait();
        scl::stream ms;
        size_t dataSize = wts->stream()->size();
        ms.write(wts->stream()->data(), dataSize);
        ms.seek(scl::StreamPos::start, 0);

        pack.close();
        return ms;
    }
};

// Template implementations for DataNode
template <typename T>
inline T Serializer::DataNode::As(const T& defaultValue) const {
    if (std::holds_alternative<T>(m_data)) {
        return std::get<T>(m_data);
    }
    return defaultValue;
}

template <typename T>
inline void Serializer::DataNode::Set(const T& value) {
    m_data = value;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<int>(const int& value) {
    m_data = value;
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<float>(const float& value) {
    m_data = value;
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<bool>(const bool& value) {
    m_data = value;
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<std::string>(const std::string& value) {
    m_data = value;
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<const char*>(const char* const& value) {
    m_data = std::string(value);
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<std::vector<float>>(const std::vector<float>& value) {
    m_data = NodeArray{};
    auto& arr = std::get<NodeArray>(m_data);
    for (const auto& elem : value) {
        arr.emplace_back().Set(elem);
    }
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<std::vector<int>>(const std::vector<int>& value) {
    m_data = NodeArray{};
    auto& arr = std::get<NodeArray>(m_data);
    for (const auto& elem : value) {
        arr.emplace_back().Set(elem);
    }
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<std::vector<std::string>>(const std::vector<std::string>& value) {
    m_data = NodeArray{};
    auto& arr = std::get<NodeArray>(m_data);
    for (const auto& elem : value) {
        arr.emplace_back().Set(elem);
    }
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<Serializer::Float3>(const Serializer::Float3& value) {
    m_data = NodeArray{};
    auto& arr = std::get<NodeArray>(m_data);
    arr.emplace_back().Set(value.x);
    arr.emplace_back().Set(value.y);
    arr.emplace_back().Set(value.z);
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<Serializer::Float4>(const Serializer::Float4& value) {
    m_data = NodeArray{};
    auto& arr = std::get<NodeArray>(m_data);
    arr.emplace_back().Set(value.x);
    arr.emplace_back().Set(value.y);
    arr.emplace_back().Set(value.z);
    arr.emplace_back().Set(value.w);
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<Serializer::Mat3>(const Serializer::Mat3& value) {
    m_data = NodeArray{};
    auto& arr = std::get<NodeArray>(m_data);
    for (int i = 0; i < 9; i++) {
        arr.emplace_back().Set(value.data[i]);
    }
    return *this;
}

template <>
inline Serializer::DataNode& Serializer::DataNode::operator=<Serializer::Mat4>(const Serializer::Mat4& value) {
    m_data = NodeArray{};
    auto& arr = std::get<NodeArray>(m_data);
    for (int i = 0; i < 16; i++) {
        arr.emplace_back().Set(value.data[i]);
    }
    return *this;
}

} // namespace Syngine
