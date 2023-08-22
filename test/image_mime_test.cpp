#include <iostream>
#include <vector>
#include <random>

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "image_mime/image_mime.h"

namespace {
	// Helper functions tests
	TEST(ImageMime, TestsHelpers) {
		{
			auto mime_type = std::string{};

			mime_type = image_mime::get_type_from_extension(".GIF");
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = image_mime::get_type_from_extension(".pNg");
			EXPECT_EQ(mime_type, "image/png");

			mime_type = image_mime::get_type_from_extension(".jpg");
			EXPECT_EQ(mime_type, "image/jpeg");

			mime_type = image_mime::get_type_from_extension(".jpeg");
			EXPECT_EQ(mime_type, "image/jpeg");

			mime_type = image_mime::get_type_from_extension(".jp2");
			EXPECT_EQ(mime_type, "image/jp2");

			mime_type = image_mime::get_type_from_extension(".jpf");
			EXPECT_EQ(mime_type, "image/jp2");

			mime_type = image_mime::get_type_from_extension(".j2c");
			EXPECT_EQ(mime_type, "image/jp2");

			mime_type = image_mime::get_type_from_extension(".j2k");
			EXPECT_EQ(mime_type, "image/jp2");

			mime_type = image_mime::get_type_from_extension(".bmp");
			EXPECT_EQ(mime_type, "image/bmp");

			mime_type = image_mime::get_type_from_extension(".tiff");
			EXPECT_EQ(mime_type, "image/tiff");

			mime_type = image_mime::get_type_from_extension(".tif");
			EXPECT_EQ(mime_type, "image/tiff");

			mime_type = image_mime::get_type_from_extension(".tga");
			EXPECT_EQ(mime_type, "image/tga");

			mime_type = image_mime::get_type_from_extension(".exr");
			EXPECT_EQ(mime_type, "image/exr");

			mime_type = image_mime::get_type_from_extension(".hdr");
			EXPECT_EQ(mime_type, "image/hdr");

			mime_type = image_mime::get_type_from_extension(".ktx");
			EXPECT_EQ(mime_type, "image/ktx");

			mime_type = image_mime::get_type_from_extension(".ktx2");
			EXPECT_EQ(mime_type, "image/ktx2");

			mime_type = image_mime::get_type_from_extension(".webp");
			EXPECT_EQ(mime_type, "image/webp");

			mime_type = image_mime::get_type_from_extension(".bpg");
			EXPECT_EQ(mime_type, "image/bpg");

			mime_type = image_mime::get_type_from_extension("");
			EXPECT_EQ(mime_type, "");

			mime_type = image_mime::get_type_from_extension(".unknown_extension");
			EXPECT_EQ(mime_type, "");
		}

		{
			auto ext = std::string{};

			ext = image_mime::get_extension_from_type("image/GIF");
			EXPECT_EQ(ext, ".gif");

			ext = image_mime::get_extension_from_type("image/png");
			EXPECT_EQ(ext, ".png");

			ext = image_mime::get_extension_from_type("image/jpeg");
			EXPECT_EQ(ext, ".jpg");

			ext = image_mime::get_extension_from_type("image/jp2");
			EXPECT_EQ(ext, ".jp2");

			ext = image_mime::get_extension_from_type("image/bmp");
			EXPECT_EQ(ext, ".bmp");

			ext = image_mime::get_extension_from_type("image/tiff");
			EXPECT_EQ(ext, ".tiff");

			ext = image_mime::get_extension_from_type("image/tga");
			EXPECT_EQ(ext, ".tga");

			ext = image_mime::get_extension_from_type("image/exr");
			EXPECT_EQ(ext, ".exr");

			ext = image_mime::get_extension_from_type("image/hdr");
			EXPECT_EQ(ext, ".hdr");

			ext = image_mime::get_extension_from_type("image/ktx");
			EXPECT_EQ(ext, ".ktx");

			ext = image_mime::get_extension_from_type("image/ktx2");
			EXPECT_EQ(ext, ".ktx2");

			ext = image_mime::get_extension_from_type("image/webp");
			EXPECT_EQ(ext, ".webp");

			ext = image_mime::get_extension_from_type("image/bpg");
			EXPECT_EQ(ext, ".bpg");

			ext = image_mime::get_extension_from_type("image/unknown_mime_type");
			EXPECT_EQ(ext, "");

			ext = image_mime::get_extension_from_type("");
			EXPECT_EQ(ext, "");
		}
	}

	// Tests with real image and non-image files
	TEST(ImageMime, TestsOnFiles) {
		auto mime_type = std::string{};


		mime_type = image_mime::get_type("../non_existing_path/Image_0.unknown_extension");
		EXPECT_EQ(mime_type, "");

		mime_type = image_mime::get_type("../test/test_images/Image_1.JPG"); // Tests the case insensitivity of the extension too
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = image_mime::get_type("../test/test_images/Image_1.JpG", true); // Tests the case insensitivity of the extension too
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = image_mime::get_type_shallow("../non_existing_path/Image_1.jpg");
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = image_mime::get_type_shallow("c:/non_existing_path/Image_0.unknown_extension");
		EXPECT_EQ(mime_type, "");

		mime_type = image_mime::get_type("../test/test_images/Image_2.jpeg", true);
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = image_mime::get_type("../test/test_images/Image_2 - jpeg with wrong extension.png");
		EXPECT_EQ(mime_type, "image/png"); // Wrong mime type based purely on the extension returned

		mime_type = image_mime::get_type("../test/test_images/Image_2 - jpeg with wrong extension.png", false);
		EXPECT_EQ(mime_type, "image/png"); // Wrong mime type based purely on the extension returned

		mime_type = image_mime::get_type("../test/test_images/Image_2 - jpeg with wrong extension.png", true);
		EXPECT_EQ(mime_type, "image/jpeg");

		mime_type = image_mime::get_type("../test/test_images/Image_3.tga", true);
		EXPECT_EQ(mime_type, "image/tga");

		mime_type = image_mime::get_type("../test/test_images/Image_4.png", true);
		EXPECT_EQ(mime_type, "image/png");

		mime_type = image_mime::get_type("../test/test_images/Nonimage_1.pdf", true);
		EXPECT_EQ(mime_type, "");

		mime_type = image_mime::get_type("../test/test_images/Image_5.bmp", true);
		EXPECT_EQ(mime_type, "image/bmp");

		mime_type = image_mime::get_type("../test/test_images/Image_6.gif", true);
		EXPECT_EQ(mime_type, "image/gif");

		mime_type = image_mime::get_type("../test/test_images/Image_7.ktx2", true);
		EXPECT_EQ(mime_type, "image/ktx2");

		mime_type = image_mime::get_type("../test/test_images/Image_8.webp", true);
		EXPECT_EQ(mime_type, "image/webp");

		mime_type = image_mime::get_type("../test/test_images/Image_9.tif", true);
		EXPECT_EQ(mime_type, "image/tiff");

		mime_type = image_mime::get_type("../test/test_images/Image_10.jp2", true);
		EXPECT_EQ(mime_type, "image/jp2");
	}

	// Tests with synthetic data
	TEST(ImageMime, TestsOnData) {
		auto mime_type = std::string{};

		{
			const auto gif_bytes_87a = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
			const auto gif_bytes_87a_appended = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x01, 0x02, 0x03, 0x04 };
			const auto gif_bytes_87a_truncated = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38 };
			const auto gif_bytes_87a_too_short = std::vector<std::uint8_t>{ 0x47 };

			mime_type = image_mime::get_type_deep(gif_bytes_87a);
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = image_mime::get_type_deep(gif_bytes_87a_appended);
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = image_mime::get_type_deep(gif_bytes_87a_truncated);
			EXPECT_EQ(mime_type, "");

			mime_type = image_mime::get_type_deep(gif_bytes_87a_too_short);
			EXPECT_EQ(mime_type, "");

			mime_type = image_mime::get_type_deep(gif_bytes_87a, "image/gif");
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = image_mime::get_type_deep(gif_bytes_87a_appended, "image/gif");
			EXPECT_EQ(mime_type, "image/gif");

			mime_type = image_mime::get_type_deep(gif_bytes_87a_truncated, "image/gif");
			EXPECT_EQ(mime_type, "");

			mime_type = image_mime::get_type_deep(gif_bytes_87a_too_short, "image/gif");
			EXPECT_EQ(mime_type, "");
		}
	}
} // namespace

namespace {

	static const auto known_mime_types_bytes = std::vector< std::pair<std::vector<std::uint8_t>, std::string> >{
		{ image_mime::gif_bytes_87a, "image/gif"},
		{ image_mime::gif_bytes_89a, "image/gif" },

		{ image_mime::png_bytes, "image/png" },

		{ image_mime::bmp_bytes, "image/bmp" },

		{ image_mime::pm_bytes, "image/pm" },

		{ image_mime::jpg_bytes_1, "image/jpeg" },
		{ image_mime::jpg_bytes_2, "image/jpeg" },
		{ image_mime::jpg_bytes_3, "image/jpeg" },
		
		{ image_mime::jpg_2000_bytes, "image/jp2" },

		{ image_mime::tiff_bytes_mono, "image/tiff" },
		{ image_mime::tiff_bytes_intel, "image/tiff" },
		{ image_mime::tiff_bytes_motorola, "image/tiff" },

		{ image_mime::tga_bytes_compressed, "image/tga" },
		{ image_mime::tga_bytes_uncompressed, "image/tga" },

		{ image_mime::exr_bytes, "image/exr" },

		{ image_mime::hdr_bytes, "image/hdr" },

		{ image_mime::ktx1_bytes, "image/ktx" },
		{ image_mime::ktx2_bytes, "image/ktx2" },

		{ image_mime::webp_bytes, "image/webp" },

		{ image_mime::bpg_bytes, "image/bpg" },
	};

	// Generates random bytes of random length (within the [#image_mime::min_image_header_size, #image_mime::max_image_header_size] range) the specified number of times
	// and stores them in a vector of vectors.
	[[nodiscard]] auto setup_fixture(const size_t n, const int dice_number) {
		auto random_file_header_bytes = std::vector< std::pair<std::vector<std::uint8_t>, std::string> >{};
		random_file_header_bytes.resize(n, {});

		auto rd = std::random_device{};
		auto gen = std::mt19937{ rd() };
		auto length_dis = std::uniform_int_distribution<>{image_mime::min_image_header_size, image_mime::max_image_header_size };
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
		auto randeom_file_header_bytes = setup_fixture(state.range(0), int(state.range(1)));

		for (auto _ : state) {
			for (const auto& bytes : randeom_file_header_bytes) {
				const auto mime_type = image_mime::get_type_deep(bytes.first, bytes.second);
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

int main(int argc, char** argv)
{
	std::cout << "image_mime v" << IMAGE_MIME_VERSION << " unit tests and benchmarks\n\n";

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
