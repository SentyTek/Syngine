// ╒═══════════════════════ Serializer.h ═╕
// │ Syngine                              │
// │ Created 2026-01-26                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

#include "Syngine/Core/Logger.h"
#include "Syngine/Utils/FsUtils.h"

#include <miniscl.hpp>

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace Syngine {

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
        DataNode& operator[](const std::string& key);

        const DataNode& operator[](const std::string& key) const;

        /// @brief Assignment operator to copy data from another DataNode
        /// @param other The other DataNode to copy from
        /// @return Reference to this DataNode
        /// @since v0.0.1
        DataNode& operator=(const DataNode& other);

        /// @brief Accesses a child DataNode by index (for Array type nodes)
        /// @param index The index of the child node
        /// @return Reference to the child DataNode
        /// @example playerNode / "name" / "last" = "Marston";
        /// @since v0.0.1
        DataNode&       operator/(const std::string& value);

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
        DataNode rootGameObject; //* Serialized GameObject tree (with all children and components)
        
        DataNode Serialize() const; //* Helper to serialize prefab
        void Deserialize(const DataNode& node); //* Helper to deserialize prefab
        
        static bool SaveToFile(const std::string& path, const Prefab& prefab); //* Save prefab to file
        static Prefab LoadFromFile(const std::string& path); //* Load prefab from file
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
            _ResolveOSPath(bundlePath.c_str()).c_str();
        if (!resolvedBundlePath.exists()) {
            Logger::LogF(LogLevel::ERR, "Bundle file not found: %s", bundlePath.c_str());
            return scl::stream();
        }
        if (!pack.open(resolvedBundlePath)) {
            Logger::LogF(LogLevel::ERR, "Failed to open bundle: %s", bundlePath.c_str());
            return scl::stream();
        }
        auto&& wts = pack.openFile(assetPath.c_str());

        scl::stream ms;
        size_t dataSize = wts->stream()->size();
        ms.write(wts->stream()->data(), dataSize);
        ms.seek(scl::StreamPos::start, 0);

        pack.close();
        return ms;
    }
};

} // namespace Syngine
