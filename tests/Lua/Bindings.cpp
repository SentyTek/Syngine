// ╒═══════════════════════ Bindings.cpp ═╕
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
#include <sol/sol.hpp>

using namespace Syngine;
using namespace Catch::Matchers;

// Collection of tests for Lua bindings, ensuring we can properly bind functions
// and properties to Lua and that they work as expected.

static bool called = false;
void TestFunction() {
    called = true;
}

TEST_CASE("Test basic Lua function binding", "[Lua]") {
    SYN_STARTENGINE

    LuaManager::AddFunction("testFunction", std::function<void()>(TestFunction));

    REQUIRE_NOTHROW(LuaManager::SafeScript("testFunction()"));
    REQUIRE(called);
}

TEST_CASE("Test Lua binding with arguments", "[Lua]") {
    SYN_STARTENGINE

    int value = 0;
    LuaManager::AddFunction(
        "setValue",
        std::function<void(int)>([&value](int newValue) { value = newValue; }));

    REQUIRE_NOTHROW(LuaManager::SafeScript("setValue(42)"));
    REQUIRE(value == 42);
}

TEST_CASE("Using Sol2 to create new usertype and binding it to Lua", "[Lua]") {
    SYN_STARTENGINE

    // Create a simple C++ class to bind
    class MyClass {
    public:
        MyClass(int v) : value(v) {}
        int GetValue() const { return value; }
        void SetValue(int v) { value = v; }
    private:
        int value;
    };

#if defined(SYNGINE_TEST_ACCESS)
    sol::state& lua = *LuaManager::GetStateForTests();
    auto        myClassType =
        lua.new_usertype<MyClass>("MyClass",
                                  sol::constructors<MyClass(int)>(),
                                  "GetValue",
                                  &MyClass::GetValue,
                                  "SetValue",
                                  &MyClass::SetValue);

    // Test creating an instance and calling methods from Lua
    LuaManager::SafeScript(R"(
        function testMyClass()
            local obj = MyClass.new(10)
            obj:SetValue(20)
            return obj:GetValue()
        end
    )");

    LuaManager::DoFunction("testMyClass", nullptr, 0);
    int result = LuaManager::GetLastResult<int>();
    REQUIRE(result == 20);
#else
    FAIL("SYNGINE_TEST_ACCESS is not enabled for test build");
#endif
}
