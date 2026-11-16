function(find_dependencies)
  find_lib(glaze)
  find_lib(magic_enum)
  find_lib(SQLiteCpp)

  if(BUILD_TESTING)
    find_lib(GTest)
  endif()
  if(BUILD_BENCHMARKS)
    find_lib(benchmark)
  endif()

endfunction()
