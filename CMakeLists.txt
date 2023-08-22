# C++17 support was intorduced in CMake v3.12
cmake_minimum_required(VERSION 3.12)

# Make URL download use the new CMP0135 policy: https://cmake.org/cmake/help/latest/policy/CMP0135.html
cmake_policy(SET CMP0135 NEW)

#cmake_policy(SET CMP0048 NEW)

# Only use C++ in this project
project(image_mime VERSION 1.0.0 LANGUAGES CXX)

# Include the path to the header folder
include_directories(${PROJECT_SOURCE_DIR}/include)

# Include Google Test & Benchamrk libraries
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
add_executable(image_mime_test ${PROJECT_SOURCE_DIR}/test/image_mime_test.cpp)

# Link against Google Test & Benchmark
target_link_libraries(image_mime_test gtest benchmark::benchmark)

# Preprocesor definitions to select the 'deep' mime type checking algorithm and set the version
target_compile_definitions(image_mime_test PRIVATE GET_MIME_TYPE_DEEP_V2 IMAGE_MIME_VERSION="${PROJECT_VERSION}")

# Restrict the C++ version to 17 and above
set_property(TARGET image_mime_test PROPERTY CXX_STANDARD 17)
set_property(TARGET image_mime_test PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET image_mime_test PROPERTY CXX_EXTENSIONS Off)

