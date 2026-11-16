add_my_library(cli
  INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Cli.cpp
  DEPS todo::tools
)
