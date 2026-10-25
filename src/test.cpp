// Debug test: prints hit info for 29 pixels down the middle column of the image.
// Output is about 30 lines. Build and run from the project root with: make test

#include "common.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iomanip>

// Traces one ray through the center of every 5th pixel down the middle column of the
// image (no antialiasing, no bounces) and prints, per pixel: which object was hit (ball,
// ground, or sky), the ray parameter t, the hit point, the surface normal, and the color
// the normal-shading would give. Useful for checking the intersection math by eye.
int main() {
    // Same image and camera setup as main.cpp
    auto ar = 16.0 / 9.0;           // Aspect ratio (width / height)
    int img_w = 256;                // Image width in pixels
    int img_h = int(img_w / ar);    // Image height in pixels
    img_h = (img_h < 1) ? 1 : img_h;

    auto focal_length = 1.0;                       // Distance from the camera to the viewport
    auto vp_h = 2.0;                               // Viewport height in world units
    auto vp_w = vp_h * (double(img_w) / img_h);    // Viewport width in world units
    auto camera_center = point3(0, 0, 0);
    auto vp_u = vector3(vp_w, 0, 0);               // Vector along the viewport's top edge (left to right)
    auto vp_v = vector3(0, -vp_h, 0);              // Vector down the viewport's left edge (top to bottom)

    auto pixel_spacing_u = vp_u / img_w;
    auto pixel_spacing_v = vp_v / img_h;

    auto vp_upper_left = camera_center - vector3(0, 0, focal_length) - vp_u / 2 - vp_v / 2;
    auto pixel_00_location = vp_upper_left + 0.5 * (pixel_spacing_u + pixel_spacing_v);

    // Same world as main.cpp
    point3 ball_center(0, 0, -1);
    double ball_radius = 0.5;

    hittable_list world;
    auto ground_mat = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto ball_mat   = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(ball_center, ball_radius, ball_mat));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, ground_mat));  // Ground: huge sphere whose top is at y = -0.5

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "row  | object | t     | hit point             | normal                | rgb\n";

    int j = img_w / 2;  // j = column (x): the middle column, matching camera.h's naming
    for (int i = 0; i < img_h; i += 5) {  // i = row (y); sample every 5th row to keep output short
        auto pixel_center = pixel_00_location + (j * pixel_spacing_u) + (i * pixel_spacing_v);
        ray r(camera_center, pixel_center - camera_center);

        hit_record rec;
        color c;
        std::string object;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            // A point on the ball is exactly ball_radius away from the ball's center
            double dist = (rec.point - ball_center).length();
            // 1e-6 is a tolerance for floating-point error
            object = (std::fabs(dist - ball_radius) < 1e-6) ? "ball  " : "ground";
            c = 0.5 * (rec.normal + color(1, 1, 1));

            std::cout << std::setw(4) << i << " | " << object << " | " << rec.t
                      << " | " << rec.point << " | " << rec.normal << " | ";
        } else {
            vector3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction.y() + 1.0);
            c = (1.0 - a) * color(1, 1, 1) + a * color(0.5, 0.7, 1.0);

            std::cout << std::setw(4) << i << " | sky    | -     | -                     | -                     | ";
        }

        // 255.999 maps [0, 1] to [0, 255] without 1.0 ever becoming 256
        std::cout << int(255.999 * c.x()) << ' '
                  << int(255.999 * c.y()) << ' '
                  << int(255.999 * c.z()) << '\n';
    }
}
