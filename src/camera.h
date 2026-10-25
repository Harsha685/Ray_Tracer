#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "hittable.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio = 1.0;      // Ratio of image width over height
    int image_width = 100;          // Rendered image width in pixels
    int samples_per_pixel = 10;     // Random rays averaged per pixel (antialiasing)
    int max_depth = 10;             // Maximum number of bounces a ray may make before we give up on it

    // Renders the whole scene and writes it to stdout as a plain-text PPM (P3) image.
    //
    // Steps:
    //   1. initialize() works out the image height and the viewport geometry.
    //   2. The PPM header is printed: "P3", width, height, and the max color value 255.
    //   3. Every pixel is visited row by row (top to bottom), left to right inside a row.
    //   4. For each pixel, samples_per_pixel rays are shot through random points inside
    //      that pixel and their colors are summed. Multiplying by pixel_samples_scale turns
    //      that sum into an average, which smooths jagged edges (antialiasing).
    //   5. The averaged color is written out as three bytes by write_color().
    // Progress is reported on stderr (std::clog) so it does not end up inside the image.
    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int i = 0; i < image_height; i++) {      // i = row (y), top to bottom
            std::clog << "\rScanlines remaining: " << (image_height - i) << ' ' << std::flush;

            for (int j = 0; j < image_width; j++) {   // j = column (x), left to right
                color pixel_color(0, 0, 0);

                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::clog << "\rDone.                 \n";
    }

  private:
    int image_height;           // Rendered image height, derived from width and aspect ratio
    double pixel_samples_scale; // 1 / samples_per_pixel, turns the summed samples into an average
    point3 center;              // Camera position (where every ray starts)
    point3 pixel00_loc;         // World position of the center of pixel (0, 0), the top-left pixel
    vector3 pixel_delta_u;      // Offset from one pixel to the next one to the right
    vector3 pixel_delta_v;      // Offset from one pixel to the next one below

    // Computes every derived value the renderer needs from the public settings.
    //
    // The camera sits at the origin looking down the -z axis. One unit in front of it
    // (focal_length) is an imaginary rectangle, the viewport, which is the "window" the
    // image is seen through. The viewport is divided into an image_width x image_height
    // grid; each grid cell is one pixel. This function finds:
    //   - image_height (at least 1 pixel tall),
    //   - the vectors pixel_delta_u / pixel_delta_v that step one pixel right / down,
    //   - pixel00_loc, the world-space center of the top-left pixel.
    // With these, any pixel's center is pixel00_loc + j * pixel_delta_u + i * pixel_delta_v.
    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        pixel_samples_scale = 1.0 / samples_per_pixel;
        center = point3(0, 0, 0);

        // Viewport dimensions
        auto focal_length = 1.0;     // Distance from the camera to the viewport plane
        auto viewport_height = 2.0;  // Viewport spans y = -1 to +1 in world units
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Vectors across the horizontal and down the vertical viewport edges
        auto viewport_u = vector3(viewport_width, 0, 0);
        auto viewport_v = vector3(0, -viewport_height, 0);  // Negative: image rows go down while world y goes up

        // Horizontal and vertical delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Location of the upper-left pixel
        auto viewport_upper_left =
            center - vector3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

        // Step half a pixel in from the corner to reach the first pixel's center
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    // Returns the color seen along ray r.
    //
    // If the ray hits an object, the object's material decides what happens:
    // scatter() returns the bounced ray and its attenuation (how much of each
    // color survives). The result is attenuation times whatever the bounced ray
    // sees. If the material absorbs the ray, the result is black.
    //
    // depth counts how many more bounces are allowed. Every bounce passes depth - 1, and
    // once it reaches 0 we return black (no light gathered). Without this limit a ray
    // trapped between surfaces would recurse forever and overflow the stack.
    //
    // If the ray hits nothing it escapes to the sky, which is a vertical gradient from
    // white (looking down) to light blue (looking up).
    color ray_color(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;

        // Start at 0.001 instead of 0 so floating-point error can't make a ray hit the surface it starts on
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;

            // Ask the surface's material how the ray bounces and how much light it keeps
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);

            return color(0, 0, 0);  // The material absorbed the ray
        }

        vector3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);  // Blend factor: ray's y mapped from [-1, 1] to [0, 1]

        // Sky gradient: white at the bottom (a = 0) to light blue (0.5, 0.7, 1.0) at the top (a = 1)
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    // Returns a random offset inside one pixel, in pixel units.
    //
    // x and y are each uniform in [-0.5, 0.5], so the point lands anywhere in the
    // unit square centered on the pixel's center. z is unused (always 0). get_ray()
    // scales x by pixel_delta_u and y by pixel_delta_v to turn this into a world offset.
    vector3 sample_square() const {
        return vector3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    // Builds a ray from the camera through a random point inside pixel (row i, column j).
    //
    // The pixel's center is pixel00_loc + j * pixel_delta_u + i * pixel_delta_v. A random
    // offset from sample_square() is added to j and i first, so each call aims at a
    // slightly different spot inside the pixel. Averaging many such rays in render() is
    // what produces antialiasing. The ray starts at the camera center and its direction
    // is the (non-normalized) vector from the camera to that sample point.
    ray get_ray(int i, int j) const {
        auto offset = sample_square();  // Random jitter within the pixel

        auto pixel_sample = pixel00_loc
                          + ((j + offset.x()) * pixel_delta_u)
                          + ((i + offset.y()) * pixel_delta_v);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }
};

#endif // CAMERA_H
