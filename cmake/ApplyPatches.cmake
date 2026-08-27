# ╒═══════════════════ ApplyPatches.txt ═╕
# │ Syngine                              │
# │ Created 2026-07-14                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# │ Licensed under the MIT License       │
# ╰──────────────────────────────────────╯

# Applies patches to third-party libraries if they haven't been applied yet.

function(apply_patch PATCH_FILE SOURCE_DIR NAME)
    message(STATUS "Patch file: ${PATCH_FILE}")
    message(STATUS "Source directory: ${SOURCE_DIR}")

    if(EXISTS "${PATCH_FILE}" AND EXISTS "${SOURCE_DIR}/.git")
        message(STATUS "Checking/Applying patch: ${NAME}...")

        # 1. Check if the patch is already applied to avoid git errors
        execute_process(
            COMMAND git apply --check "${PATCH_FILE}"
            WORKING_DIRECTORY "${SOURCE_DIR}"
            RESULT_VARIABLE PATCH_CHECK_RES
            OUTPUT_QUIET
            ERROR_QUIET
        )

        # If RESULT_VARIABLE is 0, the patch can be applied cleanly (meaning it isn't applied yet)
        if(PATCH_CHECK_RES EQUAL 0)
            execute_process(
                COMMAND git apply "${PATCH_FILE}"
                WORKING_DIRECTORY "${SOURCE_DIR}"
                RESULT_VARIABLE PATCH_APPLY_RES
            )
            if(PATCH_APPLY_RES EQUAL 0)
                message(STATUS "Successfully applied patch: ${NAME}.")
            else()
                message(WARNING "Failed to apply patch: ${NAME} automatically.")
            endif()
        else()
            message(STATUS "Patch: ${NAME} already applied or source modified. Skipping.")
        endif()
    endif()
endfunction()

# Usage:
# include(ApplyPatches)
# set(BGFX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/patches/fix_bgfx_uniform_assert.patch")
# set(BGFX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/bgfx.cmake/bgfx")
# apply_patch("${BGFX_PATCH_FILE}" "${BGFX_SOURCE_DIR}" "bgfx_uniform_assert_patch")
# 𝘚𝘩𝘳𝘪𝘮𝘱𝘭𝘦!
