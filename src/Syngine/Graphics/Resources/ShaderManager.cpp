// ╒══════════════════ ShaderManager.cpp ═╕
// │ Syngine                              │
// │ Created 2026-07-19                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Graphics/Resources/ShaderManager.h>
#include <Syngine/Graphics/Rendering/Renderer.h>
#include <Syngine/Graphics/Resources/UniformRegistry.h>
#include <Syngine/Utils/Serializer.h>

#include <bgfx/bgfx.h>
#include <miniscl.hpp>

#include <string>
#include <vector>

namespace Syngine {

std::vector<Shader> ShaderManager::m_loadedShaders; // List of loaded shaders

std::vector<ShaderManager::_UniformMeta>
ShaderManager::_parseShaderMetadata(scl::pack::Packager& packager,
                                    const std::string&   shaderName) {
    scl::xml::XmlDocument doc;
    scl::stream           xmlStream =
        Serializer::_ReadFromBundle("", "meta.xml", packager);
    if (xmlStream.size() == 0) {
        Syngine::Logger::Error(
            "ParseShaderMetadata: failed to get meta.xml from bundle");
        return {};
    }

    // Convert stream to string for XML parsing
    scl::string xmlStr;
    xmlStream.seek(scl::StreamPos::start, 0);
    xmlStr.clear();
    xmlStream >> xmlStr;
    if (!doc.load_string(xmlStr)) {
        Syngine::Logger::Error(
            "ParseShaderMetadata: failed to parse meta.xml from bundle");
        return {};
    }

    /* clang-format off */
    /* General idea of XML file:
     * Note that the meta.xml file is once per bundle, so it includes data for
     * multiple shaders if present
     * <SyngineShaderMetadata>
     *     <Shader name="shaderName">
     *         <Uniforms>
     *             <Uniform name="uniformName" type="vec4" freq="draw" num="1" src="Camera.ViewProjection" />
     *         </Uniforms>
     *         <MaterialParams>
     *             <Param name="u_baseColor" type="vec4" num="1" />
     *         </MaterialParams>
     *         <Samplers>
     *             <Sampler name="s_albedo" stage="0" />
     *         </Samplers>
     *     </Shader>
     *     ...
     * </SyngineShaderMetadata>
     */
    /* clang-format on */

    auto versionAttr = doc.find_attr("Version");
    if (!versionAttr || versionAttr->data() != SYNINT_SHADER_METADATA_VERSION) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            true,
            "ParseShaderMetadata: meta.xml version mismatch in bundle. "
            "Expected %s, got %s. Is the bundle malformed or outdated?",
            SYNINT_SHADER_METADATA_VERSION,
            versionAttr ? versionAttr->data().cstr() : "none");
        return {};
    }

    // Find the shader element with the matching name
    scl::xml::XmlElem* shaderElem = nullptr;
    for (auto* child : doc.find_children("Shader")) {
        auto nameAttr = child->find_attr("name");
        if (nameAttr && nameAttr->data() == shaderName) {
            shaderElem = child;
            break;
        }
    }

    if (!shaderElem) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "ParseShaderMetadata: shader '%s' not found in meta.xml",
            shaderName.c_str());
        return {};
    }

    std::vector<_UniformMeta> requestedBindings;

    // Parse uniforms
    char sectionsWithUniforms = 0; // Count of uniform sections that are empty

    // Engine uniforms (uniforms with a source provided by the engine)
    auto* uniformsElem = shaderElem->find_children("Uniforms").front();
    for (auto* uniformElem : uniformsElem->find_children("Uniform")) {
        sectionsWithUniforms++;
        auto nameAttr = uniformElem->find_attr("name");
        auto typeAttr = uniformElem->find_attr("type");
        auto freqAttr = uniformElem->find_attr("freq");
        auto srcAttr  = uniformElem->find_attr("src");
        auto numAttr  = uniformElem->find_attr("num");

        // Validate
        if (!nameAttr || !typeAttr || !freqAttr || !srcAttr) {
            Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                  true,
                                  "ParseShaderMetadata: uniform missing "
                                  "required attributes in shader '%s'",
                                  shaderName.c_str());
            return {};
        }

        scl::string uName = nameAttr->data();
        scl::string uType = typeAttr->data();
        scl::string uFreq = freqAttr->data();
        scl::string uSrc  = srcAttr->data();
        scl::string uNum =
            numAttr ? numAttr->data() : "1"; // Default to 1 if not specified

        // Make sure type and freq is valid at all
        auto typeIt =
            UniformRegistry::GetInstance().m_uniformTypeMap.find(uType.cstr());
        auto freqIt =
            UniformRegistry::GetInstance().m_uniformFreqMap.find(uFreq.cstr());
        if (typeIt == UniformRegistry::GetInstance().m_uniformTypeMap.end() ||
            freqIt == UniformRegistry::GetInstance().m_uniformFreqMap.end()) {
            Syngine::Logger::LogF(
                Syngine::LogLevel::ERR,
                true,
                "ParseShaderMetadata: unknown uniform param(s) type '%s' "
                "or frequency '%s' in shader '%s'",
                uType.cstr(),
                uFreq.cstr(),
                shaderName.c_str());
            return {};
        }

        // All checks passed
        // .getter is omitted here and set in _resolveBindings
        requestedBindings.push_back(
            { uName.cstr(),
              typeIt->second,
              freqIt->second,
              static_cast<uint16_t>(std::stoi(uNum.cstr())),
              uSrc.cstr(),
              _UniformStage::ENGINE });
    }

    // Engine samplers are samplers provided by the engine rather than by
    // materials
    auto* engineSamplersElem =
        shaderElem->find_children("EngineSamplers").front();
    for (auto* samplerElem : engineSamplersElem->find_children("Sampler")) {
        sectionsWithUniforms++;
        auto nameAttr  = samplerElem->find_attr("name");
        auto stageAttr = samplerElem->find_attr("stage");
        auto srcAttr   = samplerElem->find_attr("src");

        // Validate
        if (!nameAttr || !stageAttr || !srcAttr) {
            Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                  true,
                                  "ParseShaderMetadata: engine sampler missing "
                                  "required attributes in shader '%s'",
                                  shaderName.c_str());
            return {};
        }

        // All checks passed
        requestedBindings.push_back(
            { nameAttr->data().cstr(),
              UniformType::SAMPLER,
              UniformFrequency::DRAW, // Engine samplers are always per-draw
              1,
              srcAttr->data().cstr(),
              _UniformStage::SAMPLER,
              static_cast<uint8_t>(std::stoi(stageAttr->data().cstr())) });
    }

    // Material parameter uniforms (uniforms with no source, provided by the
    // material)
    auto* materialParamsElem =
        shaderElem->find_children("MaterialParams").front();
    for (auto* paramElem : materialParamsElem->find_children("Param")) {
        sectionsWithUniforms++;
        auto nameAttr = paramElem->find_attr("name");
        auto typeAttr = paramElem->find_attr("type");
        auto numAttr  = paramElem->find_attr("num");

        // Validate
        if (!nameAttr || !typeAttr || !numAttr) {
            Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                  true,
                                  "ParseShaderMetadata: param missing "
                                  "required attributes in shader '%s'",
                                  shaderName.c_str());
            return {};
        }

        scl::string uName = nameAttr->data();
        scl::string uType = typeAttr->data();
        scl::string uNum  = numAttr->data();

        // Make sure type is valid at all
        auto typeIt =
            UniformRegistry::GetInstance().m_uniformTypeMap.find(uType.cstr());
        if (typeIt == UniformRegistry::GetInstance().m_uniformTypeMap.end()) {
            Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                  true,
                                  "ParseShaderMetadata: unknown param type "
                                  "'%s' in shader '%s'",
                                  uType.cstr(),
                                  shaderName.c_str());
            return {};
        }

        // All checks passed
        requestedBindings.push_back(
            { uName.cstr(),
              typeIt->second,
              UniformFrequency::DRAW, // Material params are always per-draw
              static_cast<uint16_t>(std::stoi(uNum.cstr())),
              "none",
              _UniformStage::MATERIAL });
    }

    // Sampler uniforms (textures)
    auto* samplersElem = shaderElem->find_children("Samplers").front();
    for (auto* samplerElem : samplersElem->find_children("Sampler")) {
        sectionsWithUniforms++;
        auto nameAttr  = samplerElem->find_attr("name");
        auto stageAttr = samplerElem->find_attr("stage");

        // Validate
        if (!nameAttr || !stageAttr) {
            Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                  true,
                                  "ParseShaderMetadata: sampler missing "
                                  "required attributes in shader '%s'",
                                  shaderName.c_str());
            return {};
        }

        // All checks passed
        requestedBindings.push_back(
            { nameAttr->data().cstr(),
              UniformType::SAMPLER,
              UniformFrequency::DRAW, // Samplers are always per-draw
              1,
              "none",
              _UniformStage::SAMPLER,
              static_cast<uint8_t>(std::stoi(stageAttr->data().cstr())) });
    }

    if (sectionsWithUniforms == 0) {
        // No uniforms. Still valid, so return something so it doesnt fail out
        // of the function
        requestedBindings.push_back({ "none",
                                      UniformType::UNKNOWN,
                                      UniformFrequency::UNKNOWN,
                                      0,
                                      "none",
                                      _UniformStage::ENGINE });
    }

    return requestedBindings;
}

bool ShaderManager::_resolveBindings(
    std::vector<ShaderManager::_UniformMeta>& requestedBindings) {
    // Check if all requested bindings are registered in the UniformRegistry
    // If they do, make sure the type matches as well
    for (auto& uMeta : requestedBindings) {
        // if name is none then this is a placeholder for shaders with no
        // uniforms, so skip it
        if (uMeta.name == "none") {
            break;
        }
        if (uMeta.stage == ShaderManager::_UniformStage::MATERIAL ||
            (uMeta.stage == ShaderManager::_UniformStage::SAMPLER &&
             uMeta.src == "none")) {
            // Material params and samplers don't have a source, so skip them
            // Engine samplers do have a source, so dont skip those
            // The renderer creates the actual textures for EngineSamplers, the
            // src just calls a provider to get the texture handle
            continue;
        }
        UniformDataProvider* srcIt = UniformRegistry::FindProvider(uMeta.src);
        if (!srcIt) {
            Syngine::Logger::LogF(
                Syngine::LogLevel::ERR,
                true,
                "ParseShaderMetadata: uniform source '%s' not registered.",
                uMeta.src.c_str());
            return false;
        }
        if (srcIt->type != uMeta.type) {
            std::string expectedTypeStr;
            std::string actualTypeStr;
            switch (srcIt->type) {
            case UniformType::SAMPLER: expectedTypeStr = "sampler"; break;
            case UniformType::VEC4: expectedTypeStr = "vec4"; break;
            case UniformType::MAT4: expectedTypeStr = "mat4"; break;
            case UniformType::MAT3: expectedTypeStr = "mat3"; break;
            default: expectedTypeStr = "unknown"; break;
            }
            switch (uMeta.type) {
            case UniformType::SAMPLER: actualTypeStr = "sampler"; break;
            case UniformType::VEC4: actualTypeStr = "vec4"; break;
            case UniformType::MAT4: actualTypeStr = "mat4"; break;
            case UniformType::MAT3: actualTypeStr = "mat3"; break;
            default: actualTypeStr = "unknown"; break;
            }
            Syngine::Logger::LogF(
                Syngine::LogLevel::ERR,
                true,
                "ParseShaderMetadata: uniform source '%s' type mismatch. "
                "Expected '%s', got '%s'.",
                uMeta.src.c_str(),
                expectedTypeStr.c_str(),
                actualTypeStr.c_str());
            return false;
        }
        // fill in the getter function pointer from the registry
        uMeta.getter = srcIt->getter;
    }

    return true;
}

bool ShaderManager::_createUniforms(
    const std::vector<ShaderManager::_UniformMeta>& requestedBindings,
    std::vector<Shader::EngineUniform>&             outFrameUniforms,
    std::vector<Shader::EngineUniform>&             outViewUniforms,
    std::vector<Shader::EngineUniform>&             outDrawUniforms,
    std::vector<Shader::EngineSampler>&             outEngineSamplers,
    std::vector<MaterialParameterDesc>&             outMaterialParams,
    std::vector<TextureParameterDesc>&              outTextureParams) {

    outFrameUniforms.reserve(requestedBindings.size());
    outViewUniforms.reserve(requestedBindings.size());
    outDrawUniforms.reserve(requestedBindings.size());
    outEngineSamplers.reserve(requestedBindings.size());
    outMaterialParams.reserve(requestedBindings.size());
    outTextureParams.reserve(requestedBindings.size());

    // Create uniforms in bgfx for each requested binding
    for (const auto& uMeta : requestedBindings) {
        if (uMeta.name == "none") {
            return true; // none uniform is a placeholder for shaders with no
                         // uniforms, so skip it
        }
        bgfx::UniformHandle handle = bgfx::createUniform(
            uMeta.name.c_str(),
            static_cast<bgfx::UniformType::Enum>(uMeta.type),
            uMeta.num);
        if (!bgfx::isValid(handle)) {
            Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                  true,
                                  "CreateUniforms: failed to create uniform "
                                  "'%s' of type '%d' and num '%d'.",
                                  uMeta.name.c_str(),
                                  static_cast<int>(uMeta.type),
                                  uMeta.num);
            return false;
        }

        UniformRegistry::GetInstance().m_UniformHandles.push_back(handle);
        UniformRegistry::GetInstance().m_UniformHandlesMap[uMeta.name] = handle;

        if (uMeta.stage == ShaderManager::_UniformStage::MATERIAL) {
            outMaterialParams.push_back(
                { uMeta.name, uMeta.type, uMeta.num, handle });
        } else if (uMeta.stage == ShaderManager::_UniformStage::SAMPLER) {
            if (uMeta.src == "none") {
                outTextureParams.push_back(
                    { uMeta.name, uMeta.texStage, handle });
            } else {
                // This is an engine sampler, so add it to the engine
                // samplers
                outEngineSamplers.push_back(
                    { uMeta.name, handle, uMeta.texStage, uMeta.getter });
            }
        } else if (uMeta.stage == ShaderManager::_UniformStage::ENGINE) {
            switch (uMeta.frequency) {
            case UniformFrequency::FRAME:
                outFrameUniforms.push_back({ handle, uMeta.getter, uMeta.num });
                break;
            case UniformFrequency::VIEW:
                outViewUniforms.push_back({ handle, uMeta.getter, uMeta.num });
                break;
            case UniformFrequency::DRAW:
                outDrawUniforms.push_back({ handle, uMeta.getter, uMeta.num });
                break;
            default:
                Syngine::Logger::LogF(
                    Syngine::LogLevel::ERR,
                    true,
                    "CreateUniforms: unknown uniform frequency '%d' for "
                    "uniform '%s'.",
                    static_cast<int>(uMeta.frequency),
                    uMeta.name.c_str());
                return false;
            }
        }
    }

    outFrameUniforms.shrink_to_fit();
    outViewUniforms.shrink_to_fit();
    outDrawUniforms.shrink_to_fit();
    outEngineSamplers.shrink_to_fit();
    outMaterialParams.shrink_to_fit();
    outTextureParams.shrink_to_fit();

    return true;
}

bgfx::ShaderHandle ShaderManager::_LoadShaderFromMemory(const void* data,
                                                        size_t      size) {
    if (!data || size == 0) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "LoadShaderFromMemory: invalid data or size");
        return BGFX_INVALID_HANDLE;
    }

    const bgfx::Memory* mem = bgfx::copy(data, (uint32_t)size);
    return bgfx::createShader(mem);
}

std::optional<Shader>
ShaderManager::_BuildShader(const std::string&    bundlePath,
                            const std::string&    shaderName,
                            const Syngine::ViewID viewId) {

    // Open the bundle
    scl::pack::Packager packager;
    scl::path           resolvedBundlePath =
        Internal::ResolvePath(bundlePath.c_str()).c_str();
    if (!resolvedBundlePath.exists()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "LoadShader: bundle file not found: %s",
                              bundlePath.c_str());
        return std::nullopt;
    }

    if (!packager.open(resolvedBundlePath.cstr())) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "LoadShader: failed to open bundle: %s",
                              bundlePath.c_str());
        return std::nullopt;
    }

    // Start actually loading the shader, starting with parsing the metadata
    std::vector<ShaderManager::_UniformMeta> requestedUniforms =
        ShaderManager::_parseShaderMetadata(packager, shaderName);
    if (requestedUniforms.empty()) {
        if (shaderName.starts_with("default")) {
#ifndef NDEBUG
            Syngine::Logger::LogF(
                Syngine::LogLevel::FATAL,
                false,
                "LoadShader: failed to parse shader metadata "
                "for default shader %s in bundle %s. Look at the log for "
                "more "
                "details.",
                shaderName.c_str(),
                bundlePath.c_str());
#else
            Syngine::Logger::LogF(
                Syngine::LogLevel::ERR,
                false,
                "Failed to load a default shader. Please check the log for "
                "more details. Shader: %s",
                shaderName.c_str());
#endif
            return std::nullopt;
        }
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "Failed to parse shader metadata for %s in bundle %s",
            shaderName.c_str(),
            bundlePath.c_str());
        return std::nullopt;
    }

    // Load binaries
    scl::stream vertStream = Serializer::_ReadFromBundle(
        bundlePath, shaderName + ".vert.bin", packager);
    scl::stream fragStream = Serializer::_ReadFromBundle(
        bundlePath, shaderName + ".frag.bin", packager);
    if (vertStream.size() == 0 || fragStream.size() == 0) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "LoadShader: failed to read shader binaries "
                              "for %s in bundle %s",
                              shaderName.c_str(),
                              bundlePath.c_str());
        return std::nullopt;
    }

    // Resolve bindings
    if (requestedUniforms.front().name != "none" &&
        !ShaderManager::_resolveBindings(requestedUniforms)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "LoadShader: failed to resolve uniform bindings "
                              "for %s in bundle %s",
                              shaderName.c_str(),
                              bundlePath.c_str());
        return std::nullopt;
    }

    // Create uniforms
    std::vector<Shader::EngineUniform> frameUniforms;
    std::vector<Shader::EngineUniform> viewUniforms;
    std::vector<Shader::EngineUniform> drawUniforms;
    std::vector<Shader::EngineSampler> engineSamplers;
    std::vector<MaterialParameterDesc> materialParams;
    std::vector<TextureParameterDesc>  textureParams;
    if (!ShaderManager::_createUniforms(requestedUniforms,
                                        frameUniforms,
                                        viewUniforms,
                                        drawUniforms,
                                        engineSamplers,
                                        materialParams,
                                        textureParams)) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "LoadShader: failed to create uniforms for %s in bundle %s",
            shaderName.c_str(),
            bundlePath.c_str());
        return std::nullopt;
    }

    // Create program
    bgfx::ShaderHandle vs =
        _LoadShaderFromMemory(vertStream.data(), vertStream.size());
    bgfx::ShaderHandle fs =
        _LoadShaderFromMemory(fragStream.data(), fragStream.size());
    bgfx::ProgramHandle program = bgfx::createProgram(vs, fs, true);
    if (!bgfx::isValid(program)) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "LoadShader: failed to create program for %s in bundle %s",
            shaderName.c_str(),
            bundlePath.c_str());
        bgfx::destroy(vs);
        bgfx::destroy(fs);
        return std::nullopt;
    }

    packager.close();
    return Shader(program,
                  bundlePath,
                  shaderName,
                  frameUniforms,
                  viewUniforms,
                  drawUniforms,
                  engineSamplers,
                  materialParams,
                  textureParams,
                  viewId);
}

size_t ShaderManager::LoadShader(const std::string&    bundlePath,
                                 const std::string&    shaderName,
                                 const Syngine::ViewID viewId) {
    auto shader = _BuildShader(bundlePath, shaderName, viewId);
    if (!shader) {
        return 0;
    }

    m_loadedShaders.push_back(std::move(*shader));
    return m_loadedShaders.size(); // Return the new shader ID (1-based)
}

bool ShaderManager::ReloadShader(size_t shaderId) {
    if (shaderId == 0 || shaderId > m_loadedShaders.size()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "ReloadShader: invalid shader ID %zu",
                              shaderId);
        return false;
    }

    const size_t shaderIndex = shaderId - 1;

    const Shader& oldShader = m_loadedShaders[shaderIndex];

    std::string bundlePath = oldShader.bundlePath;
    std::string shaderName = oldShader.shaderName;

    if (!bgfx::isValid(oldShader.m_program)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "ReloadShader: invalid shader ID %zu",
                              shaderId);
        return false;
    }

    // Build replacement shader first just in case it fails, so we don't lose
    // the old one
    auto replacement = _BuildShader(
        oldShader.bundlePath, oldShader.shaderName, oldShader.m_viewId);
    if (!replacement) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            true,
            "ReloadShader: failed to load new shader for %s in bundle %s",
            oldShader.shaderName.c_str(),
            oldShader.bundlePath.c_str());
        return false;
    }

    const bgfx::ProgramHandle oldProgram =
        m_loadedShaders[shaderIndex].m_program;

    m_loadedShaders[shaderIndex] = std::move(*replacement);
    if (bgfx::isValid(oldProgram)) {
        bgfx::destroy(oldProgram);
    }

    return true;
}

bool ShaderManager::UnloadShader(size_t shaderId) {
    if (shaderId == 0 || shaderId > m_loadedShaders.size()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "UnloadShader: invalid shader ID %zu",
                              shaderId);
        return false;
    }

    const size_t shaderIndex = shaderId - 1;

    const bgfx::ProgramHandle program = m_loadedShaders[shaderIndex].m_program;
    if (!bgfx::isValid(program)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "UnloadShader: invalid program for shader ID %zu",
                              shaderId);
        return false;
    }

    // Destroy the program
    bgfx::destroy(program);

    return true;
}

Shader* ShaderManager::Get(size_t shaderId) {
    if (shaderId == 0 || shaderId > m_loadedShaders.size()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "GetShader: invalid shader ID %zu",
                              shaderId);
        return nullptr;
    }

    const size_t shaderIndex = shaderId - 1;
    return &m_loadedShaders[shaderIndex];
}

Shader* ShaderManager::Get(const std::string& shaderName) {
    for (auto& shader : m_loadedShaders) {
        if (shader.shaderName == shaderName) {
            return &shader;
        }
    }
    Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                          true,
                          "GetShader: shader '%s' not found",
                          shaderName.c_str());
    return nullptr;
}

bool ShaderManager::UnloadAllShaders() {
    for (auto& shader : m_loadedShaders) {
        if (bgfx::isValid(shader.m_program)) {
            bgfx::destroy(shader.m_program);
        }
    }
    m_loadedShaders.clear();
    return true;
}

bool ShaderManager::ReloadAllShaders() {
    for (size_t i = 0; i < m_loadedShaders.size(); ++i) {
        if (!ReloadShader(i + 1)) { // Shader IDs are 1-based
            Syngine::Logger::LogF(
                Syngine::LogLevel::ERR,
                true,
                "ReloadAllShaders: failed to reload shader ID "
                "%zu",
                i + 1);
            return false;
        }
    }

    return true;
}

std::vector<Shader*>
ShaderManager::GetProgramsByViewID(Syngine::ViewID viewId) {
    std::vector<Shader*> shadersForView;
    for (auto& shader : m_loadedShaders) {
        if (shader.m_viewId == viewId) {
            shadersForView.push_back(&shader);
        }
    }
    return shadersForView;
}

} // namespace Syngine
