#include <iostream>
#include <vector>
#include <random>

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "file_mime/file_mime.h"
using namespace file_mime;

namespace {
	// Helper functions tests
	TEST(FileMime, TestsHelpers) {
		{
			auto mime_type = std::string{};

			mime_type = get_type_from_extension(".GIF");
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = get_type_from_extension(".pNg");
			EXPECT_EQ(mime_type, "image/png");

			mime_type = get_type_from_extension(".jpg");
			EXPECT_EQ(mime_type, "image/jpeg");

			mime_type = get_type_from_extension(".jpeg");
			EXPECT_EQ(mime_type, "image/jpeg");

			mime_type = get_type_from_extension(".jp2");
			EXPECT_EQ(mime_type, "image/jp2");

			mime_type = get_type_from_extension(".jpf");
			EXPECT_EQ(mime_type, "image/jp2");

			mime_type = get_type_from_extension(".j2c");
			EXPECT_EQ(mime_type, "image/jp2");

			mime_type = get_type_from_extension(".j2k");
			EXPECT_EQ(mime_type, "image/jp2");

			mime_type = get_type_from_extension(".bmp");
			EXPECT_EQ(mime_type, "image/bmp");

			mime_type = get_type_from_extension(".tiff");
			EXPECT_EQ(mime_type, "image/tiff");

			mime_type = get_type_from_extension(".tif");
			EXPECT_EQ(mime_type, "image/tiff");

			mime_type = get_type_from_extension(".tga");
			EXPECT_EQ(mime_type, "image/tga");

			mime_type = get_type_from_extension(".exr");
			EXPECT_EQ(mime_type, "image/exr");

			mime_type = get_type_from_extension(".hdr");
			EXPECT_EQ(mime_type, "image/hdr");

			mime_type = get_type_from_extension(".ktx");
			EXPECT_EQ(mime_type, "image/ktx");

			mime_type = get_type_from_extension(".ktx2");
			EXPECT_EQ(mime_type, "image/ktx2");

			mime_type = get_type_from_extension(".webp");
			EXPECT_EQ(mime_type, "image/webp");

			mime_type = get_type_from_extension(".bpg");
			EXPECT_EQ(mime_type, "image/bpg");

			mime_type = get_type_from_extension(".glb");
			EXPECT_EQ(mime_type, "model/gltf-binary");

			mime_type = get_type_from_extension("");
			EXPECT_EQ(mime_type, "");

			mime_type = get_type_from_extension(".unknown_extension");
			EXPECT_EQ(mime_type, "");
		}

		{
			auto ext = std::string{};

			ext = get_extension_from_type("image/GIF");
			EXPECT_EQ(ext, ".gif");

			ext = get_extension_from_type("image/png");
			EXPECT_EQ(ext, ".png");

			ext = get_extension_from_type("image/jpeg");
			EXPECT_EQ(ext, ".jpg");

			ext = get_extension_from_type("image/jp2");
			EXPECT_EQ(ext, ".jp2");

			ext = get_extension_from_type("image/bmp");
			EXPECT_EQ(ext, ".bmp");

			ext = get_extension_from_type("image/tiff");
			EXPECT_EQ(ext, ".tiff");

			ext = get_extension_from_type("image/tga");
			EXPECT_EQ(ext, ".tga");

			ext = get_extension_from_type("image/exr");
			EXPECT_EQ(ext, ".exr");

			ext = get_extension_from_type("image/hdr");
			EXPECT_EQ(ext, ".hdr");

			ext = get_extension_from_type("image/ktx");
			EXPECT_EQ(ext, ".ktx");

			ext = get_extension_from_type("image/ktx2");
			EXPECT_EQ(ext, ".ktx2");

			ext = get_extension_from_type("image/webp");
			EXPECT_EQ(ext, ".webp");

			ext = get_extension_from_type("image/bpg");
			EXPECT_EQ(ext, ".bpg");

			ext = get_extension_from_type("model/gltf-binary");
			EXPECT_EQ(ext, ".glb");

			ext = get_extension_from_type("image/unknown_mime_type");
			EXPECT_EQ(ext, "");

			ext = get_extension_from_type("");
			EXPECT_EQ(ext, "");
		}
	}

	// Tests with real files
	TEST(FileMime, TestsOnFiles) {
		auto mime_type = std::string{};

		mime_type = get_type("../non_existing_path/Image_0.unknown_extension");
		EXPECT_EQ(mime_type, "");

		mime_type = get_type("../test/test_files/Image_1.JPG"); // Tests the case insensitivity of the extension too
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = get_type("../test/test_files/Image_1.JpG", true); // Tests the case insensitivity of the extension too
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = get_type_shallow("../non_existing_path/Image_1.jpg");
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = get_type_shallow("c:/non_existing_path/Image_0.unknown_extension");
		EXPECT_EQ(mime_type, "");

		mime_type = get_type("../test/test_files/Image_2.jpeg", true);
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = get_type("../test/test_files/Image_2 - jpeg with wrong extension.png");
		EXPECT_EQ(mime_type, "image/png"); // Wrong mime type based purely on the extension returned

		mime_type = get_type("../test/test_files/Image_2 - jpeg with wrong extension.png", false);
		EXPECT_EQ(mime_type, "image/png"); // Wrong mime type based purely on the extension returned

		mime_type = get_type("../test/test_files/Image_2 - jpeg with wrong extension.png", true);
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = get_type("../test/test_files/Image_3.tga", true);
		EXPECT_EQ(mime_type, "image/tga");

		mime_type = get_type("../test/test_files/Image_4.png", true);
		EXPECT_EQ(mime_type, "image/png");

		mime_type = get_type("../test/test_files/Nonimage_1.pdf", true);
		EXPECT_EQ(mime_type, "");

		mime_type = get_type("../test/test_files/Image_5.bmp", true);
		EXPECT_EQ(mime_type, "image/bmp");

		mime_type = get_type("../test/test_files/Image_6.gif", true);
		EXPECT_EQ(mime_type, "image/gif");

		mime_type = get_type("../test/test_files/Image_7.ktx2", true);
		EXPECT_EQ(mime_type, "image/ktx2");

		mime_type = get_type("../test/test_files/Image_8.webp", true);
		EXPECT_EQ(mime_type, "image/webp");

		mime_type = get_type("../test/test_files/Image_9.tif", true);
		EXPECT_EQ(mime_type, "image/tiff");

		mime_type = get_type("../test/test_files/Image_10.jp2", true);
		EXPECT_EQ(mime_type, "image/jp2");

		mime_type = get_type("../test/test_files/Model_1.glb", true);
		EXPECT_EQ(mime_type, "model/gltf-binary");

		mime_type = get_type("../test/test_files/Model_1 - glb with wrong extension.gltf", false);
		EXPECT_EQ(mime_type, "");

		mime_type = get_type("../test/test_files/Model_1 - glb with wrong extension.gltf", true);
		EXPECT_EQ(mime_type, "model/gltf-binary");
	}

	// Tests with synthetic data
	TEST(FileMime, TestsOnData) {
		auto mime_type = std::string{};

		{
			const auto gif_bytes_87a = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
			const auto gif_bytes_87a_appended = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x01, 0x02, 0x03, 0x04 };
			const auto gif_bytes_87a_truncated = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38 };
			const auto gif_bytes_87a_too_short = std::vector<std::uint8_t>{ 0x47 };

			mime_type = get_type_deep(gif_bytes_87a);
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = get_type_deep(gif_bytes_87a_appended);
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = get_type_deep(gif_bytes_87a_truncated);
			EXPECT_EQ(mime_type, "");

			mime_type = get_type_deep(gif_bytes_87a_too_short);
			EXPECT_EQ(mime_type, "");

			mime_type = get_type_deep(gif_bytes_87a, "image/gif");
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = get_type_deep(gif_bytes_87a_appended, "image/gif");
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = get_type_deep(gif_bytes_87a_truncated, "image/gif");
			EXPECT_EQ(mime_type, "");

			mime_type = get_type_deep(gif_bytes_87a_too_short, "image/gif");
			EXPECT_EQ(mime_type, "");
		}
	}
} // namespace

namespace {

	static const auto known_mime_types_bytes = std::vector< std::pair<std::vector<std::uint8_t>, std::string> >{
		{ gif_bytes_87a, "image/gif"},
		{ gif_bytes_89a, "image/gif" },

		{ png_bytes, "image/png" },

		{ bmp_bytes, "image/bmp" },

		{ pm_bytes, "image/pm" },

		{ jpg_bytes_1, "image/jpeg" },
		{ jpg_bytes_2, "image/jpeg" },
		{ jpg_bytes_3, "image/jpeg" },
		
		{ jpg_2000_bytes, "image/jp2" },

		{ tiff_bytes_mono, "image/tiff" },
		{ tiff_bytes_intel, "image/tiff" },
		{ tiff_bytes_motorola, "image/tiff" },

		{ tga_bytes_compressed, "image/tga" },
		{ tga_bytes_uncompressed, "image/tga" },

		{ exr_bytes, "image/exr" },

		{ hdr_bytes, "image/hdr" },

		{ ktx1_bytes, "image/ktx" },
		{ ktx2_bytes, "image/ktx2" },

		{ webp_bytes, "image/webp" },

		{ bpg_bytes, "image/bpg" },

		{ glb_bytes, "model/gltf-binary" },
	};

	// Generates random bytes of random length (within the [#file_mime::min_image_header_size, #file_mime::max_image_header_size] range) the specified number of times
	// and stores them in a vector of vectors.
	[[nodiscard]] auto setup_fixture(const size_t n, const int dice_number) {
		auto random_file_header_bytes = std::vector< std::pair<std::vector<std::uint8_t>, std::string> >{};
		random_file_header_bytes.resize(n, {});

		auto rd = std::random_device{};
		auto gen = std::mt19937{ rd() };
		auto length_dis = std::uniform_int_distribution<>{min_file_header_size, max_file_header_size };
		auto bytes_dis = std::uniform_int_distribution<>{ std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max() };
		auto known_mime_chance_dis = std::uniform_int_distribution<>{ 0, dice_number };
		auto known_mime_types_dis = std::uniform_int_distribution<>{ 0, int(known_mime_types_bytes.size()) - 1 };

		for (auto& bytes : random_file_header_bytes) {

			// Random 1 in #dice_number chance of using a known mime type
			if (known_mime_chance_dis(gen) == 0) {
				bytes = known_mime_types_bytes[known_mime_types_dis(gen)];
			}
			else {
				bytes.first.resize(length_dis(gen));
				std::generate(bytes.first.begin(), bytes.first.end(), [&]() { return bytes_dis(gen); });
			}
		}

		return random_file_header_bytes;
	}

	void image_mime_benchmark(benchmark::State& state) {

		std::cout << "# of headers: " << state.range(0) << "\n";
		std::cout << "dice number: " << state.range(1) << "\n";
		auto random_file_header_bytes = setup_fixture(state.range(0), int(state.range(1)));

		for (auto _ : state) {
			for (const auto& bytes : random_file_header_bytes) {
				const auto mime_type = get_type_deep(bytes.first, bytes.second);
				benchmark::DoNotOptimize(mime_type);
			}
		}
	}

	void CustomArguments(benchmark::internal::Benchmark* b) {
		static constexpr auto number_of_headers = size_t{ 10000000U };
		static constexpr auto dice_number = int{ 10 };
		b->MeasureProcessCPUTime()->UseRealTime()->Unit(benchmark::kMillisecond)->Args({number_of_headers, dice_number});
	}

	BENCHMARK(image_mime_benchmark)->Apply(CustomArguments)->Iterations(100);

} // namespace

int main(int argc, char** argv) {

	std::cout << "image_mime v" << FILE_MIME_VERSION << " unit tests and benchmarks\n\n";

	// Unit tests
	{
		::testing::InitGoogleTest(&argc, argv);
		const auto tests_ret = RUN_ALL_TESTS();

		if (tests_ret) {
			return tests_ret;
		}
	}

#if defined(NDEBUG)
	// Benchmark tests
	{
		::benchmark::Initialize(&argc, argv);

		if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
			return 1;
		}

		::benchmark::RunSpecifiedBenchmarks();
		::benchmark::Shutdown();
	}
#endif

	return 0;
}
