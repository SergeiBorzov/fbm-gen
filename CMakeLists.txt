cmake_minimum_required(VERSION 3.5)
project(fbm-gen)

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Use the highest warning level for Visual Studio.
  set(CMAKE_CXX_WARNING_LEVEL 4)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

file(GLOB Sources 
      "src/*.cpp"
      "src/Window/*.cpp"
      "src/Renderer/*.cpp"
      "src/Gui/*.cpp"
      "src/Third_Party/ImGui/*.cpp"
    )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
   add_executable(fbm-gen ${Sources})
   target_include_directories(fbm-gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Third_Party/ImGui )
   target_link_libraries(fbm-gen PRIVATE ${OPENGL_LIBRARIES} GLEW::GLEW glfw)
   set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT fbm-gen)
else()
   message(FATAL_ERROR "This platform is not supported.")
endif()
