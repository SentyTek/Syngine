# ╒═══════════════════ Versioning.cmake ═╕
# │ Syngine                              │
# │ Created YYYY-MM-DD                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2025     │
# │ Placeholder License                  │
# ╰──────────────────────────────────────╯

# Adds a function to handle versioning of the game engine. Generates a header file with version information, in src/Syngine/Utils/Version.h
function(define_version)
    # Get year and week number
    string(TIMESTAMP YEAR "%y" UTC)
    string(TIMESTAMP WEEK "%V" UTC)

    # Get short commit hash
    execute_process(
        COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_RESULT
        ERROR_QUIET
    )
    
    # Use default if git command failed
    if(NOT GIT_RESULT EQUAL 0)
        set(COMMIT "000")
    endif()

    # Combine
    set(VERSION_STRING "${YEAR}w${WEEK}.${COMMIT}")

    add_definitions(-DSYN_VERSION_STRING="${VERSION_STRING}")

    # Generate a header file with the version information
    file(WRITE "${SYNGINE_SOURCE_DIR}/src/Syngine/Utils/Version.h" "#pragma once\n\n#define SYN_VERSION_STRING \"${VERSION_STRING}\"\n")
endfunction()