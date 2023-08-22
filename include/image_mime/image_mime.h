// MIT License
//
// Copyright(c) 2023 Lev Faynshteyn
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef IMAGE_MIME_H
#define IMAGE_MIME_H

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <numeric>
#include <cassert>

namespace image_mime {

	static constexpr auto min_image_header_size = std::size_t{ 2u }; // the header is min 2 bytes in size
	static constexpr auto max_image_header_size = std::size_t{ 18u }; // the header is max 18 bytes in size

	// File magic numbers sources:
	// https://en.wikipedia.org/wiki/List_of_file_signatures
	// https://gist.github.com/leommoore/f9e57ba2aa4bf197ebc5
	static const auto gif_bytes_87a = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
	static const auto gif_bytes_89a = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38, 0x39, 0x61 };
	static const auto png_bytes = std::vector<std::uint8_t>{ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	static const auto bmp_bytes = std::vector<std::uint8_t>{ 0x42, 0x4D };
	static const auto pm_bytes = std::vector<std::uint8_t>{ 0x56, 0x49, 0x45, 0x57 };
	static const auto jpg_bytes_1 = std::vector<std::uint8_t>{ 0xFF, 0xD8, 0xFF, 0xE0 };
	static const auto jpg_bytes_2 = std::vector<std::uint8_t>{ 0xFF, 0xD8, 0xFF, 0xE1 };
	static const auto jpg_bytes_3 = std::vector<std::uint8_t>{ 0xFF, 0xD8, 0xFF, 0xDB };
	static const auto jpg_2000_bytes = std::vector<std::uint8_t>{ 0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A }; // JPEG-2000
	static const auto tiff_bytes_mono = std::vector<std::uint8_t>{ 0x0C, 0xED };
	static const auto tiff_bytes_intel = std::vector<std::uint8_t>{ 0x49, 0x49, 0x2A, 0x00 };
	static const auto tiff_bytes_motorola = std::vector<std::uint8_t>{ 0x4D, 0x4D, 0x00, 0x2A };
	static const auto tga_bytes_compressed = std::vector<std::uint8_t>{ 0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	static const auto tga_bytes_uncompressed = std::vector<std::uint8_t>{ 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	static const auto exr_bytes = std::vector<std::uint8_t>{ 0x76, 0x2F, 0x31, 0x01 }; // https://openexr.readthedocs.io/en/latest/OpenEXRFileLayout.html
	static const auto hdr_bytes = std::vector<std::uint8_t>{ 0x23, 0x3F, 0x52, 0x41, 0x44, 0x49, 0x41, 0x4E, 0x43, 0x45, 0x0A }; // https://en.wikipedia.org/wiki/RGBE_image_format
	static const auto ktx1_bytes = std::vector<std::uint8_t>{ 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A }; // https://registry.khronos.org/KTX/specs/1.0/ktxspec.v1.html
	static const auto ktx2_bytes = std::vector<std::uint8_t>{ 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A }; // https://registry.khronos.org/KTX/specs/2.0/ktxspec.v2.html
	static const auto webp_bytes = std::vector<std::uint8_t>{ 0x52, 0x49, 0x46, 0x46 };
	static const auto bpg_bytes = std::vector<std::uint8_t>{ 0x42, 0x50, 0x47, 0xFB };


	// Determine the extension of an image from its mime type.
	[[nodiscard]] inline auto get_extension_from_type(const std::string& mime_type) -> std::string {
		// An unordered map of mime types to extensions
		static auto mime_to_ext = std::unordered_map<std::string, std::string>
		{
			{ "image/gif",	".gif" },
			{ "image/png",	".png" },
			{ "image/jpeg",	".jpg" },
			{ "image/jp2",	".jp2" },
			{ "image/bmp",	".bmp" },
			{ "image/tiff",	".tiff" },
			{ "image/tga",	".tga" },
			{ "image/exr",	".exr" },
			{ "image/hdr",	".hdr" },
			{ "image/ktx",	".ktx" },
			{ "image/ktx2",	".ktx2" },
			{ "image/webp",	".webp" },
			{ "image/bpg",	".bpg" },
		};

		// Transform the mime type to lowercase
		auto mime_type_lc = mime_type;
		std::transform(mime_type.begin(), mime_type.end(), mime_type_lc.begin(), [](auto c) { return std::tolower(c); });

		// Find the extension
		const auto it = mime_to_ext.find(mime_type_lc);
		if (it != mime_to_ext.end()) {
			return it->second;
		}

		return "";
	}


	// Determine the mime type of an image from its extension.
	[[nodiscard]] inline auto get_type_from_extension(const std::string& extension) -> std::string {
		// An unordered map of extensions to mime types
		static auto ext_to_mime = std::unordered_map<std::string, std::string>
		{
			{ ".gif",	"image/gif" },
			{ ".png",	"image/png" },
			{ ".jpg",	"image/jpeg" },
			{ ".jpeg",	"image/jpeg" },
			{ ".jp2",	"image/jp2" },
			{ ".jpf",	"image/jp2" },
			{ ".j2c",	"image/jp2" },
			{ ".j2k",	"image/jp2" },
			{ ".bmp",	"image/bmp" },
			{ ".pm",	"image/pm" },
			{ ".tiff",	"image/tiff" },
			{ ".tif",	"image/tiff" },
			{ ".tga",	"image/tga" },
			{ ".exr",	"image/exr" },
			{ ".hdr",	"image/hdr" },
			{ ".ktx",	"image/ktx" },
			{ ".ktx2",	"image/ktx2" },
			{ ".webp",	"image/webp" },
			{ ".bpg",	"image/bpg" },
		};

		// Transform the extension to lowercase
		auto extension_lc = extension;
		std::transform(extension.begin(), extension.end(), extension_lc.begin(), [](auto c) { return std::tolower(c); });

		// Find the mime type
		const auto it = ext_to_mime.find(extension_lc);
		if (it != ext_to_mime.end()) {
			return it->second;
		}

		return "";
	}


	// Determine the mime type of an image from its file extension.
	[[nodiscard]] inline auto get_type_shallow(const std::string& path_to_file) -> std::string {

		namespace fs = std::filesystem;
		// Initialize path from path string
		const auto path = fs::path(path_to_file);

		// Get the extension of the file
		const auto extension = path.extension().string();

		// If the extension is empty, return an empty string
		if (extension.empty()) {
			return "";
		}

		return get_type_from_extension(extension);
	}

	namespace detail {

		// Approach 0: linearly searching through all magic numbers and trying to match them with the image bytes
		[[nodiscard]] auto get_type_deep_v0(const uint8_t* image_bytes, const size_t image_size, const std::string& mime_type_hint = "") -> std::string {

			// An unordered map of the mime types and their corresponding magic numbers.
			static auto mime_to_magic = std::vector<std::pair<std::string, std::vector<uint8_t>>>{

				{ "image/gif", gif_bytes_87a },
				{ "image/gif", gif_bytes_89a },

				{ "image/png", png_bytes },

				{ "image/bmp", bmp_bytes },

				{ "image/pm", pm_bytes },

				{ "image/jpeg", jpg_bytes_1 },
				{ "image/jpeg", jpg_bytes_2 },
				{ "image/jpeg", jpg_bytes_3 },
				{ "image/jp2", jpg_2000_bytes },

				{ "image/tiff", tiff_bytes_mono},
				{ "image/tiff", tiff_bytes_intel },
				{ "image/tiff", tiff_bytes_motorola },

				{ "image/tga", tga_bytes_compressed },
				{ "image/tga", tga_bytes_uncompressed },

				{ "image/exr", exr_bytes },

				{ "image/hdr", hdr_bytes },

				{ "image/ktx", ktx1_bytes },
				{ "image/ktx2", ktx2_bytes },

				{ "image/webp", webp_bytes },

				{ "image/bpg", bpg_bytes },
			};

			for (auto it = mime_to_magic.begin(); it != mime_to_magic.end(); ++it) {
				const auto& magic_bytes = it->second;
				if (std::equal(magic_bytes.begin(), magic_bytes.end(), image_bytes, image_bytes + std::min(image_size, magic_bytes.size()))) {
					return it->first;
				}
			}

			return "";
		}


		// Approach 1: use a hash map to quickly check the entry associated with the provided hint,
		// and then binary search through the actual keys that are vectors of raw magic data.
		[[nodiscard]] auto get_type_deep_v1(const uint8_t* image_bytes, const size_t image_size, const std::string& mime_type_hint = "") -> std::string {

			// An unordered map of the mime types and their corresponding magic numbers.
			static auto mime_to_magic = std::unordered_map<std::string, std::vector<std::vector<uint8_t>>>{

				{ "image/gif", {gif_bytes_87a , gif_bytes_89a} },

				{ "image/png", {png_bytes} },

				{ "image/bmp", {bmp_bytes} },

				{ "image/pm", {pm_bytes} },

				{ "image/jpeg", {jpg_bytes_1, jpg_bytes_2, jpg_bytes_3} },

				{ "image/jp2", { jpg_2000_bytes} },

				{ "image/tiff", {tiff_bytes_mono, tiff_bytes_intel, tiff_bytes_motorola} },

				{ "image/tga", {tga_bytes_compressed, tga_bytes_uncompressed} },

				{ "image/exr", {exr_bytes} },

				{ "image/hdr", {hdr_bytes} },

				{ "image/ktx", {ktx1_bytes} },
				{ "image/ktx2", {ktx2_bytes} },

				{ "image/webp", {webp_bytes} },

				{ "image/bpg", {bpg_bytes} },
			};

			// If we have the hint mime type (usually from the file extension), then we can use it to narrow down the search.
			auto it_hint = mime_to_magic.end();
			if (!mime_type_hint.empty()) {
				it_hint = mime_to_magic.find(mime_type_hint);
				if (it_hint != mime_to_magic.end()) {
					for (const auto& magic : it_hint->second) {
						if (std::equal(magic.begin(), magic.end(), image_bytes, image_bytes + std::min(image_size, magic.size()))) {
							return it_hint->first;
						}
					}
				}
			}

			// No hint or it didn't work (due to the magic data mismatch), we try to find the mime type of the image based on the rest of the magic numbers.
			for (auto it = mime_to_magic.begin(); it != mime_to_magic.end(); ++it) {
				if (it_hint != it) {
					for (const auto& magic : it->second) {
						if (std::equal(magic.begin(), magic.end(), image_bytes, image_bytes + std::min(image_size, magic.size()))) {
							return it->first;
						}
					}
				}
			}

			return "";
		}

		// Approach 2: use a vector of pairs of mime types and their corresponding magic numbers, sort it lexicographically once, and then perform binary search using std::lower_bound.
		[[nodiscard]] auto get_type_deep_v2(const uint8_t* image_bytes, const size_t image_size, const std::string& mime_type_hint = "") -> std::string {

			// An unordered map of the mime types and their corresponding magic numbers.
			static auto mime_to_magic = std::vector<std::pair<std::string, std::vector<uint8_t>>>{

				{ "image/gif", gif_bytes_87a },
				{ "image/gif", gif_bytes_89a },

				{ "image/png", png_bytes },

				{ "image/bmp", bmp_bytes },

				{ "image/pm", pm_bytes },

				{ "image/jpeg", jpg_bytes_1 },
				{ "image/jpeg", jpg_bytes_2 },
				{ "image/jpeg", jpg_bytes_3 },
				
				{ "image/jp2", jpg_2000_bytes },

				{ "image/tiff", tiff_bytes_mono},
				{ "image/tiff", tiff_bytes_intel },
				{ "image/tiff", tiff_bytes_motorola },

				{ "image/tga", tga_bytes_compressed },
				{ "image/tga", tga_bytes_uncompressed },

				{ "image/exr", exr_bytes },

				{ "image/hdr", hdr_bytes },

				{ "image/ktx", ktx1_bytes },
				{ "image/ktx2", ktx2_bytes },

				{ "image/webp", webp_bytes },

				{ "image/bpg", bpg_bytes },
			};

			// Sort the mime_to_magic vector lexicographically once.
			{
				static std::once_flag flag;
				std::call_once(
					flag,
					[]() {
						auto cmp = [](const auto& a, const auto& b) -> bool {
							// Compare the magic numbers lexographically (which is the defualt for std::vector).
							return a.second < b.second;
							// In the future, a custom comparator can be used with the lexographical search to compare magic numbers that need certain bytes to be skipped over.
							//auto lex_cmp = [](const uint8_t& magic_byte1, const uint8_t& magic_byte2) -> bool {
							//	return magic_byte1 < magic_byte2;
							//};
							//return std::lexicographical_compare(a.second.begin(), a.second.end(), b.second.begin(), b.second.end(), lex_cmp);
						};

						std::sort(mime_to_magic.begin(), mime_to_magic.end(), cmp);
					}
				);
			}

			// Perform binary search using std::lower_bound
			{
				auto cmp = [&image_size](const auto& magic_bytes, const uint8_t* p_data) -> bool {
					return std::lexicographical_compare(magic_bytes.begin(), magic_bytes.end(), p_data, p_data + std::min(image_size, magic_bytes.size()));
					};

				auto it = std::lower_bound(mime_to_magic.begin(), mime_to_magic.end(), image_bytes,
					[&cmp](const auto& mtm, const uint8_t* p_data) {
						return cmp(mtm.second, p_data);
					});

				if (
					it != mime_to_magic.end()
					&& std::equal(it->second.begin(), it->second.end(), image_bytes, image_bytes + std::min(image_size, it->second.size()))
					) {
					// mime type found
					return it->first;
				}
			}

			return "";
		}

		template <typename T>
		inline void hash_combine(std::size_t& s, const T& v) {
			std::hash<T> h;
			s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
		}

		// Approach 3: use an unordered map with a custom magic_number struct & hash function to look up the magic numbers in constant time.
		// Since you usually don't know the length of the header/magic number in the passed in image bytes, you have to calculate the hash value incrementally and test it against the map.
		// The hint can't really be used here efficiently, as the same mime type can have multiple magic numbers meaning that several hash values have to be calculated (again, in the incremental fashion as described above).
		[[nodiscard]] auto get_type_deep_v3(const uint8_t* image_bytes, const size_t image_size, const std::string& mime_type_hint = "") -> std::string {

			struct magic_number {
#if defined(_DEBUG)
				mutable bool flag = false;
#endif
				const std::size_t cached_hash_value = 0;

			public:
				magic_number() = delete;
				magic_number(const magic_number&) = default;
				magic_number(magic_number&&) = default;
				magic_number& operator=(const magic_number&) = default;
				magic_number& operator=(magic_number&&) = default;
				~magic_number() = default;

				magic_number(const uint8_t* data, const std::size_t size)
					: cached_hash_value(hash(data, size))
				{}

				magic_number(const std::size_t cached_hash)
					: cached_hash_value(cached_hash)
				{}

				auto operator==(const magic_number& other) const noexcept -> bool {
					return (cached_hash_value == other.cached_hash_value);
				}

			private:
				[[nodiscard]] auto hash(const uint8_t* data, const std::size_t size) const noexcept -> std::size_t {

#if defined(_DEBUG)
					assert(!flag && "cached_hash_value has to be initialized only once");
					flag = true;
#endif

					auto result = std::size_t{ 0 };

					try {
						result = std::accumulate(data, data + size, std::size_t{ 0u },
							[&](std::size_t current_hash, const uint8_t value) {
								hash_combine(current_hash, value);
								return current_hash;
							});
					}
					catch (...) {
						// Reading memory beyond the size of a buffer is an undefined behavior.
						// There's no specific exception type that is guaranteed to be thrown in such a case.
						// So we catch and suppress any and all here.
						assert(false && "magic_number::hash() failed");
					}

					return result;
				}

			};

			struct magic_number_hasher {
				size_t operator()(const magic_number& mn) const {
					return mn.cached_hash_value;
				}
			};

			static const auto magic_to_mime = std::unordered_map<magic_number, std::string, magic_number_hasher>{
				{{ gif_bytes_87a.data(), gif_bytes_87a.size() }, "image/gif"},
				{{ gif_bytes_89a.data(), gif_bytes_89a.size() }, "image/gif"},

				{{ png_bytes.data(), png_bytes.size() }, "image/png"},

				{{ bmp_bytes.data(), bmp_bytes.size() }, "image/bmp"},

				{{ pm_bytes.data(), pm_bytes.size() }, "image/pm"},

				{{ jpg_bytes_1.data(), jpg_bytes_1.size() }, "image/jpeg"},
				{{ jpg_bytes_2.data(), jpg_bytes_2.size() }, "image/jpeg"},
				{{ jpg_bytes_3.data(), jpg_bytes_3.size() }, "image/jpeg"},
				
				{{ jpg_2000_bytes.data(), jpg_2000_bytes.size() }, "image/jp2"},

				{{ tiff_bytes_mono.data(), tiff_bytes_mono.size() }, "image/tiff"},
				{{ tiff_bytes_intel.data(), tiff_bytes_intel.size() }, "image/tiff"},
				{{ tiff_bytes_motorola.data(), tiff_bytes_motorola.size() }, "image/tiff"},

				{{ tga_bytes_compressed.data(), tga_bytes_compressed.size() }, "image/tga"},
				{{ tga_bytes_uncompressed.data(), tga_bytes_uncompressed.size() }, "image/tga"},

				{{ exr_bytes.data(), exr_bytes.size() }, "image/exr"},

				{{ hdr_bytes.data(), hdr_bytes.size() }, "image/hdr"},

				{{ ktx1_bytes.data(), ktx1_bytes.size() }, "image/ktx"},
				{{ ktx2_bytes.data(), ktx2_bytes.size() }, "image/ktx2"},

				{{ webp_bytes.data(), webp_bytes.size() }, "image/webp"},

				{{ bpg_bytes.data(), bpg_bytes.size() }, "image/bpg"},
			};

			// Compute the hash value of the magic number by hand and use it to initialize a magic number instance,
			// so that we don't waste time recomputing it for every increment in length of the compared #image_bytes.
			auto magic_number_hash = std::size_t{ 0 };

			// Increment a value from min_image_header_size to max_image_header_size
			// and combine the hash value with the current magic_number_hash.
			// Then look for the hash value in the magic_to_mime map.
			for (auto i = size_t{ 0 }; i < std::min(image_size, max_image_header_size); ++i) {
				hash_combine(magic_number_hash, image_bytes[i]);

				if (i < min_image_header_size - 1) {
					continue;
				}

				const auto it = magic_to_mime.find(std::move(magic_number{ magic_number_hash }));
				if (it != magic_to_mime.end()) {
					return it->second;
				}
			}

			return "";
		}
	} // namespace detail


	// Determine the mime type of an image from its raw in-memory bytes.
	[[nodiscard]] auto get_type_deep(const uint8_t* image_bytes, const size_t image_size, const std::string& mime_type_hint = "") -> std::string {

		if (image_size < min_image_header_size) {
			assert(false && "The image header size in bytes is too small to determine its type.");
			return "";
		}

#if defined(GET_MIME_TYPE_DEEP_V0)
		return detail::get_type_deep_v0(image_bytes, image_size, mime_type_hint);
#elif defined(GET_MIME_TYPE_DEEP_V1)
		return detail::get_type_deep_v1(image_bytes, image_size, mime_type_hint);
#elif defined(GET_MIME_TYPE_DEEP_V2)
		return detail::get_type_deep_v2(image_bytes, image_size, mime_type_hint);
#else 
		return detail::get_type_deep_v3(image_bytes, image_size, mime_type_hint);
#endif
	}

	[[nodiscard]] auto get_type_deep(const std::vector<uint8_t>& image_bytes, const std::string& mime_type_hint = "") -> std::string {
		return get_type_deep(image_bytes.data(), image_bytes.size(), mime_type_hint);
	}

	[[nodiscard]] auto get_type(const std::string& path_to_file, const bool deep_check = false) -> std::string {

		const auto mime_type = get_type_shallow(path_to_file);

		if (deep_check) {
			// Read image file into memory
			auto file = std::ifstream(path_to_file, std::ios::binary | std::ios::ate);
			if (!file) {
				assert(false && "std::ifstream failed");
				return mime_type;
			}

			// Determine the file size
			file.seekg(0, std::ios::end);
			const auto file_size = file.tellg();
			if (!file_size) {
				assert(false && "file_size is 0");
				return mime_type;
			}
			file.seekg(0, std::ios::beg);

			const auto header_size = std::min(std::streamsize(file_size), std::streamsize(max_image_header_size));

			// Create a buffer to store the read data
			auto buffer = std::vector<char>(header_size);

			// Read the specified number of bytes from the file
			file.read(buffer.data(), header_size);

			if (file.fail()) {
				assert(false && "file.read failed");
				return mime_type;
			}

			return get_type_deep(reinterpret_cast<uint8_t*>(buffer.data()), buffer.size(), mime_type);
		}

		return mime_type;
	}

} // namespace image_mime

#endif // IMAGE_MIME_H