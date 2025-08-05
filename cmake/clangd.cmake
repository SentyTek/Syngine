# Configures the clangd-format file for the workspace (root/engine/cmake/clangd.cmake)

function(clangdConfigure target)
get_target_property(inc_dirs ${target} INCLUDE_DIRECTORIES)
get_target_property(cxx_standard ${target} CXX_STANDARD)
#message(STATUS "DIRS: ${inc_dirs}")
set(CLANGD_INCLUDE_DIRS "")

foreach(dir ${inc_dirs})
set(CLANGD_INCLUDE_DIRS "${CLANGD_INCLUDE_DIRS}, -I${dir}")
endforeach()

file(WRITE "${CMAKE_CURRENT_LIST_DIR}/.clangd" "If:
  PathMatch: [.*\.h, .*\.cpp]
  PathExclude: third_party/*
CompileFlags:
  Add: [-std=c++${cxx_standard}, -Wno-deprecated-declarations, -Wno-unused-includes${CLANGD_INCLUDE_DIRS}]
")

endfunction()
