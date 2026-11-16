add_my_library(repository
  INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Repository.cpp
  DEPS todo::tools todo::task SQLiteCpp
)
