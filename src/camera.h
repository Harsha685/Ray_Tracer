#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "hittable.h"
#include "material.h"

// Positionable camera with antialiasing and defocus blur. Set the public fields, then
// call render().
class camera {
public:
    double aspect_ratio = 1.0;          // Ratio of image width over height
    int image_width = 100;              // Rendered image width in pixels
    int samples_per_pixel = 10;         // Random rays averaged per pixel (antialiasing)
    int max_depth = 10;                 // Maximum number of bounces a ray may make before we give up on it

    double vfov = 90;                   // Vertical view angle (field of view), in degrees
    point3 lookfrom = point3(0, 0, 0);  // Point camera is looking from
    point3 lookat   = point3(0, 0, -1); // Point camera is looking at
    vector3 vup     = vector3(0, 1, 0); // Camera-relative "up" direction

    double defocus_angle = 0;           // Variation angle of rays through each pixel, in degrees (0 = pinhole, no blur)
    double focus_dist = 10;             // Distance from lookfrom to the plane of perfect focus

    /*
     * Renders the whole scene and writes it to stdout as a plain-text PPM (P3) image.
     *
     *  - initialize() works out the image height and the viewport geometry.
     *  - The PPM header is printed: "P3", width, height, and the max color value 255.
     *  - Pixels are visited row by row (top to bottom), left to right inside a row.
     *  - For each pixel, samples_per_pixel rays are shot through random points inside
     *    the pixel and their colors are summed; multiplying by pixel_samples_scale
     *    averages them, which smooths jagged edges (antialiasing).
     *  - The averaged color is written out by write_color().
     *  - Progress is reported on stderr (std::clog) so it does not end up in the image.
     */
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
    point3 center;              // Camera position (where every ray starts when there is no blur)
    point3 pixel00_loc;         // World position of the center of pixel (0, 0), the top-left pixel
    vector3 pixel_delta_u;      // Offset from one pixel to the next one to the right
    vector3 pixel_delta_v;      // Offset from one pixel to the next one below
    vector3 u, v, w;            // Camera frame basis vectors: right, up, and backward (opposite the view direction)
    vector3 defocus_disk_u;     // Defocus disk horizontal radius
    vector3 defocus_disk_v;     // Defocus disk vertical radius

    /*
     * Computes every derived value the renderer needs from the public settings.
     *
     *  - Builds an orthonormal camera frame (u, v, w): w points backward (from lookat
     *    to lookfrom), u points to the camera's right, and v is the camera's up,
     *    tilted from vup so it is perpendicular to w.
     *  - Places the viewport, an imaginary rectangle the image is seen through,
     *    focus_dist in front of the camera (along -w). Its height comes from vfov.
     *  - image_height is derived from image_width and aspect_ratio (at least 1 pixel).
     *  - pixel_delta_u / pixel_delta_v step one pixel right / down across the viewport.
     *  - pixel00_loc is the world-space center of the top-left pixel, so any pixel's
     *    center is pixel00_loc + j * pixel_delta_u + i * pixel_delta_v.
     *  - Sizes the defocus disk (the lens) from defocus_angle and focus_dist.
     */
    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        pixel_samples_scale = 1.0 / samples_per_pixel;
        center = lookfrom;

        // Viewport dimensions
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);               // Half the viewport height at distance 1
        auto viewport_height = 2 * h * focus_dist;  // Viewport sits on the focus plane
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Unit basis vectors for the camera's coordinate frame
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Vectors across the horizontal and down the vertical viewport edges
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;  // Negative: image rows go down while camera up goes up

        // Horizontal and vertical delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Location of the upper-left pixel
        auto viewport_upper_left =
            center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;

        // Step half a pixel in from the corner to reach the first pixel's center
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Lens radius: the cone with apex at the focus plane and half-angle defocus_angle / 2
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    /*
     * Returns the color seen along ray r.
     *
     *  - If the ray hits an object, the object's material decides what happens:
     *    scatter() returns the bounced ray and its attenuation, and the result is
     *    attenuation times whatever the bounced ray sees.
     *  - If the material absorbs the ray, the result is black.
     *  - depth is the number of bounces still allowed. Each bounce passes depth - 1;
     *    at 0 the function returns black. Without this limit a ray trapped between
     *    surfaces would recurse forever and overflow the stack.
     *  - Hits closer than t = 0.001 are ignored so floating-point error cannot make a
     *    ray hit the surface it starts on (shadow acne).
     *  - If the ray hits nothing it escapes to the sky: a vertical gradient from white
     *    (looking down) to light blue (looking up).
     */
    color ray_color(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }

        vector3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);  // Blend factor: ray's y mapped from [-1, 1] to [0, 1]

        // Sky gradient: white at the bottom (a = 0) to light blue (0.5, 0.7, 1.0) at the top (a = 1)
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    /*
     * Returns a random offset inside one pixel, in pixel units.
     *
     *  - x and y are each uniform in [-0.5, 0.5], so the point lands anywhere in the
     *    unit square centered on the pixel's center.
     *  - z is unused (always 0).
     *  - get_ray() scales x by pixel_delta_u and y by pixel_delta_v to turn this into
     *    a world-space offset.
     */
    vector3 sample_square() const {
        return vector3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    /*
     * Returns a random point on the camera's defocus disk (the lens).
     *
     *  - The disk is centered on center and spanned by defocus_disk_u / defocus_disk_v.
     */
    point3 defocus_disk_sample() const {
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    /*
     * Builds a ray from the camera through a random point inside pixel (row i, column j).
     *
     *  - The pixel's center is pixel00_loc + j * pixel_delta_u + i * pixel_delta_v.
     *  - A random offset from sample_square() is added first, so each call aims at a
     *    slightly different spot inside the pixel; averaging these in render() is what
     *    produces antialiasing.
     *  - The ray starts at a random point on the defocus disk, or exactly at center
     *    when defocus_angle <= 0.
     *  - Its direction is the (non-normalized) vector from that origin to the sample point.
     *  - Points on the focus plane are hit by every ray of the pixel and stay sharp;
     *    anything nearer or farther is smeared across the lens, which is the blur.
     */
    ray get_ray(int i, int j) const {
        auto offset = sample_square();  // Random jitter within the pixel

        auto pixel_sample = pixel00_loc
                          + ((j + offset.x()) * pixel_delta_u)
                          + ((i + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }
};

#endif // CAMERA_H
