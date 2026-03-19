add_llvm_library(AnnotateUnusedVars MODULE 
  AnnotateUnusedVars.cpp 
  PLUGIN_TOOL clang
)

set(ANNOTATE_UNUSED_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(ANNOTATE_UNUSED_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests)

file(MAKE_DIRECTORY ${ANNOTATE_UNUSED_BINARY_DIR})

configure_file(
  ${ANNOTATE_UNUSED_SOURCE_DIR}/lit.site.cfg.py.in
  ${ANNOTATE_UNUSED_BINARY_DIR}/lit.site.cfg.py
  @ONLY
)

add_custom_target(check-annotate-unused
  COMMAND ${PYTHON_EXECUTABLE} ${LLVM_MAIN_SRC_DIR}/utils/lit/lit.py -v ${ANNOTATE_UNUSED_BINARY_DIR}
  DEPENDS AnnotateUnusedVars clang FileCheck
  COMMENT "Running AnnotateUnusedVars tests from subfolder..."
)