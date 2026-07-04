// ╒══════════════════ ScriptLoading.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-17                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../defines.h"
#include "Syngine/Core/LuaManager.h"

#include <Syngine/Syngine.h>
using namespace Syngine;
using namespace Catch::Matchers;

// Collection of tests for script loading in the Syngine engine, ensuring that
// scripts are properly loaded, executed, and that their effects on game objects
// work as expected.

TEST_CASE("Load generic Lua script", "[Lua]") {
    SYN_STARTENGINE
    int result;

    std::string script = R"(
        function testFunction()
            return 42
        end
    )";

    REQUIRE_NOTHROW(LuaManager::SafeScript(script));
    REQUIRE_NOTHROW(LuaManager::DoFunction("testFunction", nullptr, 0));
    REQUIRE_NOTHROW(result = LuaManager::GetLastResult<int>());
    REQUIRE(result == 42);
}

TEST_CASE("Load Lua script that modifies game object", "[Lua]") {
    SYN_STARTENGINE

    // it's very important to actually follow the lua api
    std::string script = R"(
        local obj = scene.createGameObject("LuaObject")
        obj:AddComponent("Transform")

        function EditGameObject()
            local transform = obj:GetComponent("Transform")
            if transform then
                transform:SetPosition(10, 20, 30)
            end
        end

        function getX()
            local transform = obj:GetComponent("Transform")
            if transform then
                local x, y, z = transform:GetPosition()
                return x
            end
            return nil
        end
    )";

    LuaManager::SafeScript(script);
    LuaManager::DoFunction("EditGameObject", nullptr, 0);
    LuaManager::DoFunction("getX", nullptr, 0);

    float x = LuaManager::GetLastResult<float>();
    REQUIRE_THAT(x, WithinAbs(10.0f, FLOAT_MARGIN));
}
