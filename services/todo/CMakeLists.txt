add_service(todo
  MAIN ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  DOMAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/commander/Commander.cpp
  DEPS todo::tools todo::task todo::config todo::cli
)
