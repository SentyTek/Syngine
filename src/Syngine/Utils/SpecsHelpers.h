// ╒═════════════════════ SpecsHelpers.h ═╕
// │ Syngine                              │
// │ Created 2026-04-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <dxgi.h>
#include <intrin.h>
#elif __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#else
#include <sys/utsname.h>
#endif

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>

#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <string>

namespace Syngine {

static std::string _FormatGpuVendorIdHex(uint16_t vendorId) {
    char buffer[7];
    SDL_snprintf(buffer, sizeof(buffer), "0x%04X", vendorId);
    return std::string(buffer);
}

#ifdef _WIN32
static std::string _WideToUtf8(const wchar_t* value) {
    if (!value || value[0] == L'\0') {
        return "Unknown";
    }

    int requiredSize =
        WideCharToMultiByte(CP_UTF8, 0, value, -1, nullptr, 0, nullptr, nullptr);
    if (requiredSize <= 1) {
        return "Unknown";
    }

    std::string utf8(requiredSize, '\0');
    if (WideCharToMultiByte(
            CP_UTF8, 0, value, -1, utf8.data(), requiredSize, nullptr, nullptr) == 0) {
        return "Unknown";
    }

    utf8.pop_back();
    return utf8.empty() ? "Unknown" : utf8;
}

static std::string _GetWindowsGpuName(uint16_t vendorId, uint16_t deviceId) {
    IDXGIFactory1* factory = nullptr;
    HRESULT        result =
        CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&factory));
    if (FAILED(result) || !factory) {
        return "Unknown";
    }

    std::string gpuName = "Unknown";
    for (UINT adapterIndex = 0;; ++adapterIndex) {
        IDXGIAdapter1* adapter = nullptr;
        if (factory->EnumAdapters1(adapterIndex, &adapter) == DXGI_ERROR_NOT_FOUND) {
            break;
        }

        DXGI_ADAPTER_DESC1 description = {};
        if (SUCCEEDED(adapter->GetDesc1(&description)) &&
            (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
            description.VendorId == vendorId && description.DeviceId == deviceId) {
            gpuName = _WideToUtf8(description.Description);
            adapter->Release();
            break;
        }

        adapter->Release();
    }

    factory->Release();
    return gpuName;
}
#endif

static std::string _GetGpuName(const bgfx::Caps* caps) {
    if (!caps) {
        return "Unknown";
    }

#ifdef _WIN32
    const std::string gpuName = _GetWindowsGpuName(caps->vendorId, caps->deviceId);
    if (gpuName != "Unknown") {
        return gpuName;
    }
#endif

    return std::to_string(caps->deviceId);
}

static std::string _GetCPUName() {
#ifdef _WIN32
    // Get CPU info (why on EARTH is this so complicated?)
    std::string cpu;
    int         CPUInfo[4] = { -1 };
    char        CPUBrandString[0x40]; // Buffer for CPU brand string

    // Get the highest extended function supported by CPUID
    __cpuid(CPUInfo, 0x80000000);
    unsigned int nExIds = CPUInfo[0];

    // Get the CPU brand string if supported
    for (unsigned int i = 0x80000002; i <= nExIds && i <= 0x80000004; ++i) {
        __cpuid(CPUInfo, i);
        // Copy returned values into the CPU brand string buffer
        memcpy(
            CPUBrandString + (i - 0x80000002) * 16, CPUInfo, sizeof(CPUInfo));
    }
    CPUBrandString[sizeof(CPUBrandString) - 1] =
        '\0'; // Null-terminate the string
    cpu            = std::string(CPUBrandString);
    return cpu;
#elif __APPLE__
    char   cpuBrand[256];
    size_t size = sizeof(cpuBrand);
    sysctlbyname("machdep.cpu.brand_string", cpuBrand, &size, nullptr, 0);
    return std::string(cpuBrand);
#else
    std::ifstream cpuInfoFile("/proc/cpuinfo");
    std::string   line;
    std::string   cpuBrand = "Unknown";

    if (cpuInfoFile.is_open()) {
        while (std::getline(cpuInfoFile, line)) {
            if (line.find("model name") != std::string::npos) {
                cpuBrand = line.substr(line.find(":") + 2);
                break;
            }
        }
        cpuInfoFile.close();
    }
    return cpuBrand;
#endif
}

} // namespace Syngine
