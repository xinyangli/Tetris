include_directories(../include)
aux_source_directory(. SOURCE_FILES)

add_executable(tetris ${SOURCE_FILES})
if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrtd.lib")
endif()
target_link_libraries(tetris ${CONAN_LIBS})