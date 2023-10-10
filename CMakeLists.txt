cmake_minimum_required(VERSION 3.23)

# Make URL download use the new CMP0135 policy: https://cmake.org/cmake/help/latest/policy/CMP0135.html
cmake_policy(SET CMP0135 NEW)

#cmake_policy(SET CMP0048 NEW)

# Only use C++ in this project
project(file_mime VERSION 2.0.0 LANGUAGES CXX)

# Include the path to the header folder
include_directories(${PROJECT_SOURCE_DIR}/include)

# Include Google Test & Benchmark libraries
include(FetchContent)

FetchContent_Declare(
	googletest
	# Specify the commit we depend on and update it regularly.
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_Declare(
	googlebenchmark
	# Specify the commit we depend on and update it regularly.
	URL https://github.com/google/benchmark/archive/refs/tags/v1.8.2.zip
)

# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(
	googletest
	googlebenchmark
)

# The main unit test executable
add_executable(file_mime_test ${PROJECT_SOURCE_DIR}/test/file_mime_test.cpp)
target_sources(file_mime_test
	PUBLIC
	${PROJECT_SOURCE_DIR}/test/file_mime_test.cpp
	PUBLIC FILE_SET HEADERS
	BASE_DIRS ${PROJECT_SOURCE_DIR}/include
	FILES
		${PROJECT_SOURCE_DIR}/include/file_mime/file_mime.h
)
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT file_mime_test)

# Link against Google Test & Benchmark
target_link_libraries(file_mime_test gtest benchmark::benchmark)

# Preprocesor definitions to select the 'deep' mime type checking algorithm and set the version
target_compile_definitions(file_mime_test PRIVATE GET_MIME_TYPE_DEEP_V2 FILE_MIME_VERSION="${PROJECT_VERSION}")

# Restrict the C++ version to 17 and above
set_property(TARGET file_mime_test PROPERTY CXX_STANDARD 17)
set_property(TARGET file_mime_test PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET file_mime_test PROPERTY CXX_EXTENSIONS Off)

