add_header_only_library(tools
  INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tools INTERFACE magic_enum::magic_enum)
