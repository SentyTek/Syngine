
# Configure for active languages, prioritizing CXX
get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
if("CXX" IN_LIST languages)
  message(STATUS "Using CXX for .clangd")
  set(CLANGD_LANG "CXX" CACHE STRING "" FORCE)
  set(CLANG_LANG_POSTFIX "++" CACHE STRING "" FORCE)
  set(CLANGD_PATH_FILTER ".*\.h, .*\.hpp, .*\.hh, .*\.cpp, .*\.cc" CACHE STRING "" FORCE)
  elseif("C" IN_LIST languages)
  message(STATUS "Using C for .clangd")
  set(CLANGD_LANG "C" CACHE STRING "" FORCE)
  set(CLANG_LANG_POSTFIX "" CACHE STRING "" FORCE)
  set(CLANGD_PATH_FILTER ".*\.h, .*\.c" CACHE STRING "" FORCE)
else()
  message(WARNING "CXX and C are not enabled in this project, clangdConfigure not available")
endif()

set(CLANGD_LANG_STANDARD ${CMAKE_${CLANGD_LANG}_STANDARD})
if (NOT DEFINED CLANGD_LANG_STANDARD)
  message(SEND_ERROR "${CLANGD_LANG} standard is not defined!")
endif()
message(STATUS "${CLANGD_LANG} standard: ${CLANGD_LANG_STANDARD}")


# - Function for configuring a .clangd file for a target
# Automatically detects C/C++ (preferes C++)
#
# clangd(target
#   [OUTPUT_DIRECTORY <dir>]
#   [EXCLUDE_PATHS [arg1, ...]
#   [WARNINGS [arg1, ...]]
#   [INLAY_HINTS]
#   [INLAY_BLOCK_END]
#   [INLAY_DESIGNATORS]
#   [INLAY_PARAMETER_NAMES]
#   [INLAY_DEDUCED_TYPES]
#   [INLAY_TYPENAME_LIMIT <length>]
#  
# Requires any target to be passed as <target>
# OUTPUT_DIRECTORY: Path to a directory where .clangd will be written to. Defaults to CMAKE_CURRENT_LISTS_DIR
# EXCLUDE_PATHS: optional, adds paths that clangd excludes from processing
# WARNINGS: optional, clang warning names (Ex: all, no-unused-includes) to use
# INLAY_HINTS: enables inlay-hints for block end comments
# INLAY_BLOCK_END: enables block end inlay-hint comments
# INLAY_DESIGNATORS: enables inlay-hints for designators in aggregate initialization
# INLAY_PARAMETER_NAMES: enables inlay-hints for parameter names in function calls
# INLAY_DEDUCED_TYPES: enables inlay-hints for deduced types
# INLAY_TYPENAME_LIMIT: Character limit for type hints. 0 means no limit. Default is 24

function(clangd target)
  set(options INLAY_HINTS INLAY_BLOCK_END INLAY_DESIGNATORS INLAY_PARAMETER_NAMES INLAY_DEDUCED_TYPES)
  set(oneValueArgs OUTPUT_DIRECTORY INLAY_TYPENAME_LIMIT)
  set(multiValueArgs EXCLUDE_PATHS WARNINGS)
  cmake_parse_arguments(PARSE_ARGV 0 arg
    "${options}" "${oneValueArgs}" "${multiValueArgs}"
  )

  get_target_property(inc_dirs ${target} INCLUDE_DIRECTORIES)
  set(CLANGD_INCLUDE_DIRS "")

  foreach(dir ${inc_dirs})
    set(CLANGD_INCLUDE_DIRS "${CLANGD_INCLUDE_DIRS}, -I${dir}")
  endforeach()

  set(CLANGD_CONFIG_HEADER
"If:
  PathMatch: [${CLANGD_PATH_FILTER}]")
  set(CLANGD_CONFIG_EXCLUDE "")
  if (DEFINED arg_EXCLUDE_PATHS)
    set(CLANGD_CONFIG_EXCLUDE 
"  PathExclude: [")
    foreach(exclude_path ${arg_EXCLUDE_PATHS})
      string(APPEND CLANGD_CONFIG_EXCLUDE "${exclude_path},")
    endforeach()
    string(APPEND CLANGD_CONFIG_EXCLUDE "]")
  endif()

  set(CLANGD_CONFIG_WARNINGS "")
  if(DEFINED arg_WARNINGS)
    foreach(warning ${arg_WARNINGS})
      string(APPEND CLANGD_CONFIG_WARNINGS ", -W${warning}")
    endforeach()
  endif()

  set(CLANGD_CONFIG_COMPILE_STUFF
"CompileFlags:
  Add: [-xc${CLANG_LANG_POSTFIX}, -std=c${CLANG_LANG_POSTFIX}${CLANGD_LANG_STANDARD}${CLANGD_CONFIG_WARNINGS}${CLANGD_INCLUDE_DIRS}]
  Remove: [-std:*]
  Compiler: clang${CLANG_LANG_POSTFIX}")

  set(CLANGD_CONFIG_INLAY_HINTS 
"InlayHints:
  Enabled: false")
  if (arg_INLAY_HINTS)
    set(CLANGD_CONFIG_INLAY_HINTS 
"InlayHints:
  Enabled: true
")
    if (arg_INLAY_TYPENAME_LIMIT)
      string(APPEND CLANGD_CONFIG_INLAY_HINTS 
"  TypeNameLimit: ${arg_INLAY_TYPENAME_LIMIT}
")
    else()
    string(APPEND CLANGD_CONFIG_INLAY_HINTS 
"  TypeNameLimit: 24
")
    endif()

    if (arg_INLAY_BLOCK_END)
      string(APPEND CLANGD_CONFIG_INLAY_HINTS
"  BlockEnd: true
")
    else()
      string(APPEND CLANGD_CONFIG_INLAY_HINTS
"  BlockEnd: false
")
    endif()

    if (arg_INLAY_DESIGNATORS)
      string(APPEND CLANGD_CONFIG_INLAY_HINTS
"  Designators: true
")
    else()
      string(APPEND CLANGD_CONFIG_INLAY_HINTS
"  Designators: false
")
    endif()

    if (arg_INLAY_PARAMETER_NAMES)
      string(APPEND CLANGD_CONFIG_INLAY_HINTS
"  ParameterNames: true
")
    else()
      string(APPEND CLANGD_CONFIG_INLAY_HINTS
"  ParameterNames: false
")
    endif()

    if (arg_INLAY_DEDUCED_TYPES)
      string(APPEND CLANGD_CONFIG_INLAY_HINTS
"  DeducedTypes: true
")
    else()
      string(APPEND CLANGD_CONFIG_INLAY_HINTS
"  DeducedTypes: false
")
    endif()
    
  endif()

  if (NOT DEFINED arg_OUTPUT_DIRECTORY)
    set(arg_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  endif()

  file(WRITE "${arg_OUTPUT_DIRECTORY}/.clangd" 
"${CLANGD_CONFIG_HEADER}
${CLANGD_CONFIG_EXCLUDE}
${CLANGD_CONFIG_COMPILE_STUFF}
${CLANGD_CONFIG_INLAY_HINTS}
")

endfunction()
