#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

// stb_image is a third-party single-header library. Silence its warnings here so every
// file that includes this header builds cleanly with -Wall -Wextra.
#ifdef _MSC_VER
    #pragma warning (push, 0)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#ifdef _MSC_VER
    #pragma warning (pop)
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#include <cstdlib>
#include <iostream>
#include <string>

// Loads an image file (JPEG, PNG, ...) into memory as 8-bit RGB for image_texture.
//
//  - STB_IMAGE_IMPLEMENTATION above compiles stb_image's code into whichever .cpp file
//    includes this header, so only one .cpp file per program may include it.
class image_loader {
public:
    /*
     * Creates a loader with no image (width() and height() return 0).
     */
    image_loader() {}

    /*
     * Loads image_filename, searching several likely locations.
     *
     *  - If the RTW_IMAGES environment variable is set, that directory is tried first.
     *  - Then the file name as given (relative to the current directory), then images/,
     *    then ../images/, and so on up to six levels up. This finds the project's
     *    images/ folder whether the program runs from the project root or from build/.
     *  - If every attempt fails, an error is printed and width() and height() return 0.
     */
    image_loader(const char* image_filename) {
        auto filename = std::string(image_filename);
        auto imagedir = getenv("RTW_IMAGES");

        if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
        if (load(filename)) return;
        if (load("images/" + filename)) return;
        if (load("../images/" + filename)) return;
        if (load("../../images/" + filename)) return;
        if (load("../../../images/" + filename)) return;
        if (load("../../../../images/" + filename)) return;
        if (load("../../../../../images/" + filename)) return;
        if (load("../../../../../../images/" + filename)) return;

        std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
    }

    // The loader owns raw buffers that the destructor frees. A copy would free them a
    // second time, so copying is disabled.
    image_loader(const image_loader&) = delete;
    image_loader& operator=(const image_loader&) = delete;

    /*
     * Frees the float and byte pixel buffers.
     */
    ~image_loader() {
        delete[] bdata;
        STBI_FREE(fdata);
    }

    /*
     * Loads the file into memory. Returns true on success.
     *
     *  - stbi_loadf returns linear (gamma 1) floats in [0, 1], 3 per pixel (red, green,
     *    blue), left to right along each row, rows top to bottom.
     *  - The floats are then converted to bytes in bdata, which pixel_data() reads.
     */
    bool load(const std::string& filename) {
        auto n = bytes_per_pixel;  // Out parameter: components per pixel in the file (unused)
        fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
        if (fdata == nullptr) return false;

        bytes_per_scanline = image_width * bytes_per_pixel;
        convert_to_bytes();
        return true;
    }

    /*
     * Image size in pixels, or 0 if no image is loaded.
     */
    int width()  const { return (fdata == nullptr) ? 0 : image_width; }
    int height() const { return (fdata == nullptr) ? 0 : image_height; }

    /*
     * Returns a pointer to the 3 RGB bytes of pixel (x, y).
     *
     *  - x and y are clamped to the image, so out-of-range coordinates read the
     *    nearest edge pixel.
     *  - If no image is loaded, returns magenta so the problem is easy to spot.
     */
    const unsigned char* pixel_data(int x, int y) const {
        static const unsigned char magenta[] = { 255, 0, 255 };
        if (bdata == nullptr) return magenta;

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);

        return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
    }

private:
    const int      bytes_per_pixel = 3;
    float         *fdata = nullptr;         // Linear floating-point pixel data
    unsigned char *bdata = nullptr;         // Linear 8-bit pixel data
    int            image_width = 0;         // Loaded image width
    int            image_height = 0;        // Loaded image height
    int            bytes_per_scanline = 0;  // Bytes in one row of bdata

    /*
     * Returns x clamped to [low, high) (high itself is excluded).
     */
    static int clamp(int x, int low, int high) {
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }

    /*
     * Converts a float in [0, 1] to a byte in [0, 255]. Out-of-range values are clamped.
     */
    static unsigned char float_to_byte(float value) {
        if (value <= 0.0)
            return 0;
        if (1.0 <= value)
            return 255;
        return static_cast<unsigned char>(256.0 * value);
    }

    /*
     * Converts every float in fdata to a byte and stores the result in bdata.
     */
    void convert_to_bytes() {
        int total_bytes = image_width * image_height * bytes_per_pixel;
        bdata = new unsigned char[total_bytes];

        auto *bptr = bdata;
        auto *fptr = fdata;
        for (auto i = 0; i < total_bytes; i++, fptr++, bptr++)
            *bptr = float_to_byte(*fptr);
    }
};

#endif // IMAGE_LOADER_H
