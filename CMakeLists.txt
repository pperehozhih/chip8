cmake_minimum_required(VERSION 2.8)

project(chip-8)

set(CMAKE_CXX_EXTENSIONS OFF)
set(SFML_VERSION "2.5.1")
set(IMGUI_VERSION "1.69")

include(FindPackageHandleStandardArgs)

# Game section
aux_source_directory(src SRC_CHIP_ROOT_FILE)
file(GLOB HDR_CHIP_ROOT_FILE "src/*.h*")

set(SRC_FILE_SERVER ${SRC_FILE_SERVER} ${SRC_CHIP_ROOT_FILE})
set(HDR_FILE_SERVER ${HDR_FILE_SERVER} ${HDR_CHIP_ROOT_FILE})
source_group("chip" FILES ${SRC_CHIP_ROOT_FILE} ${HDR_CHIP_ROOT_FILE})

add_executable(${PROJECT_NAME} ${SRC_FILE_SERVER} ${HDR_FILE_SERVER})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
# target_link_libraries(${PROJECT_NAME} "spdlog" "sr" ${LIBEV_LIBRARY} ${HIREDIS_LIBRARY} ${CURL_LIBRARIES} -lpthread)
# add_dependencies(${PROJECT_NAME} "spdlog" "sr" "ev" "hiredis")

if (NOT EXISTS ${CMAKE_BINARY_DIR}/3rd)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/3rd"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	)
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/sfml)
	file(DOWNLOAD "https://github.com/SFML/SFML/archive/${SFML_VERSION}.zip" "${CMAKE_BINARY_DIR}/sfml-${SFML_VERSION}.zip")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/sfml-${SFML_VERSION}.zip"
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_BINARY_DIR}/SFML-${SFML_VERSION}" "${CMAKE_BINARY_DIR}/sfml"
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/sfml/build"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/sfml"
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} ..  -G "${CMAKE_GENERATOR}" -DBUILD_SHARED_LIBS=NO -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/3rd -DSFML_MISC_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/3rd/share/SFML -DSFML_DEPENDENCIES_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/3rd/Library/Frameworks
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/sfml/build"
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} --build . --target install --config Release
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/sfml/build"
	)
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/imgui)
	file(DOWNLOAD "https://github.com/ocornut/imgui/archive/v${IMGUI_VERSION}.zip" "${CMAKE_BINARY_DIR}/imgui-${IMGUI_VERSION}.zip")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/imgui-${IMGUI_VERSION}.zip"
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
	execute_process(
	 	COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_BINARY_DIR}/imgui-${IMGUI_VERSION}" "${CMAKE_BINARY_DIR}/imgui"
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/imgui/build"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/imgui"
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/cmake/imgui.cmake.in" "${CMAKE_BINARY_DIR}/imgui/CMakeLists.txt"
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} .. -G "${CMAKE_GENERATOR}" -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/3rd
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/imgui/build"
	)
	execute_process(
		COMMAND ${CMAKE_COMMAND} --build . --target install --config Release
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/imgui/build"
	)
endif()
