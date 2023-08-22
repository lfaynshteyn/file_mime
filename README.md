# image_mime

This is a small header only library that can be used to determine the mime type of an image based on both the file extension and the file content using magic numbers.

The library in its current form is limited to detecting mime types of only a subset of some of the more popular raster image formats. This was done simply because it fulfilled my needs at the time, but the approach is generic and can be easily extended to include other image and non-image mime types.

There are 4 different algorithms implemented (mostly because it was an interesting intellectual exercise) that allow to determine the mime type based on the magic bytes in the image header:
- v0 - a simple linear search through a vector of magic number bytes.
- v1 - a hash map of mime types to magic bytes is used to quickly check for the entry associated with the provided mime type hint (usually derived from a file extension). A linear search through sub-arrays of mime types in the hash map is used to look up either the hint or, if none is provided or no match is found, to check the rest of the magic numbers.
- v2 - a binary search through a sorted vector of mime type/magic numbers pairs is used to look up the mime type.
- v3 - an incrementally calculated hash value is used to look up the magic numbers in the hash map.

v2 and v3 perform the fastest on my system, but YMMV, so profile before deciding on which one to use. v3 and v4 should also scale the best if you decided to broaden the set of the supported mime types/magic numbers.

One thing to note, is that some formats allow for 'gaps' in their magic number byte sequences, that is they can have certain bytes somewhere in the middle of the magic number byte sequence with non-defined/arbitrary values (e.g. WebP image files use bytes 4 through 7 out of 12 total magic bytes to store the file size). In such cases I truncated magic numbers to the first occurrence such bytes. This proved to be sufficient to uniquely represent all of the currently supported image formats, but should the set of supported mime types be broadened significantly, it might no longer be the case. In such a case one would need to modify the magic bytes hashing function and/or write a custom lexographical comparison function to skip over the non-contributing bytes (e.g., by storing a bit mask, along with the magic numbers, defining indices of the contributing bytes). I might introduce such a change myself in a future version, time permitting.

## Usage

```cpp

#include "image_mime/image_mime.h"

auto mime_type = std::string{};

// Get the mime type based on the file extension alone
mime_type = image_mime::get_type("../test/test_images/Image_1.jpg");
// Ditto
mime_type = image_mime::get_type_shallow("../test/test_images/Image_1.jpg");

// Perform an additional 'deep' data check to determine the mime type based on the magic numbers in the file header
auto deep_check = bool{ true };
mime_type = image_mime::get_type("../test/test_images/Image_1.jpg", deep_check);

// Get mime type based on the image raw data
auto gif_bytes_87a = std::vector<std::uint8_t>{ 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
mime_type = image_mime::get_type_deep(gif_bytes_87a);
// or
mime_type = image_mime::get_type_deep(gif_bytes_87a.data(), gif_bytes_87a.size());

// Ditto, but provide a mime type hint to speed up the look-up process
mime_type = image_mime::get_type_deep(gif_bytes_87a, "image/gif");

// Helper functions
mime_type = image_mime::get_type_from_extension(".jpg");
auto ext = image_mime::get_extension_from_type("image/jpeg");

```

Note: you will need **C++17** at a minimum to compile the code.
