cmake_minimum_required(VERSION 3.5)
project(fbm-gen)

set(CMAKE_CXX_STANDARD 17)

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
find_package(OpenCL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(cJSON CONFIG REQUIRED)
    

file(GLOB Sources 
      "src/*.cpp"
      "src/Core/*.cpp"
      "src/Input/*.cpp"
      "src/Window/*.cpp"
      "src/Graphics/*.cpp"
      "src/Gui/*.cpp"
      "src/Third_Party/ImGui/*.cpp"
    )


 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
   set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT fbm-gen)
else()
   message(FATAL_ERROR "This platform is not supported.")
endif()


add_executable(fbm-gen ${Sources})
target_include_directories(fbm-gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src 
                                          ${CMAKE_CURRENT_SOURCE_DIR}/src/Third_Party/ImGui
                                          ${CMAKE_CURRENT_SOURCE_DIR}/src/Third_Party/stb )

target_link_libraries(fbm-gen PRIVATE ${OPENGL_LIBRARIES} ${OPENCL_LIBRARIES} GLEW::GLEW glfw glm OpenCL::OpenCL cjson)

add_custom_command(TARGET fbm-gen PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:fbm-gen>)


