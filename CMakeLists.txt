cmake_minimum_required(VERSION 2.8)
project(Project)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/source")
set(DEP_DIR "${PROJECT_SOURCE_DIR}/dependencies")

# Source / header files
include("sources.txt")

# GLFW
set(GLFW_DIR "${DEP_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(${GLFW_DIR})
list(APPEND INCLUDES
   ${GLFW_DIR}/include
)
list(APPEND LIBS
   glfw ${GLFW_LIBRARIES}
)

# glew
set(GLEW_DIR "${DEP_DIR}/glew")
list(APPEND LIB_SOURCES
   ${GLEW_DIR}/src/glew.c
)
list(APPEND INCLUDES
   ${GLEW_DIR}/include
)

#glm
list(APPEND INCLUDES
   ${DEP_DIR}
)

# dl libs
list(APPEND LIBS
   ${CMAKE_DL_LIBS}
)

foreach(HEADER ${HEADERS})
   list(APPEND ABS_HEADERS
      "${SRC_DIR}/${HEADER}"
   )
endforeach(HEADER)

foreach(SOURCE ${SOURCES})
   list(APPEND ABS_SOURCES
      "${SRC_DIR}/${SOURCE}"
   )
endforeach(SOURCE)

foreach(RESOURCE ${RESOURCES})
   list(APPEND ABS_RESOURCES
      "${SRC_DIR}/${RESOURCE}"
   )
endforeach(RESOURCE)

file(COPY ${ABS_RESOURCES} DESTINATION ${PROJECT_BINARY_DIR})

include_directories(${INCLUDES})
add_executable(${PROJECT_NAME} ${ABS_SOURCES} ${ABS_HEADERS} ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
