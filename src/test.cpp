// Debug test: prints hit info for 29 pixels down the middle column of the image.
// Output is about 30 lines. Build and run from the project root with: make test

#include "common.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iomanip>

int main() {
    // Same image and camera setup as main.cpp
    auto ar = 16.0 / 9.0;
    int img_w = 256;
    int img_h = int(img_w / ar);
    img_h = (img_h < 1) ? 1 : img_h;

    auto focal_length = 1.0;
    auto vp_h = 2.0;
    auto vp_w = vp_h * (double(img_w) / img_h);
    auto camera_center = point3(0, 0, 0);
    auto vp_u = vector3(vp_w, 0, 0);
    auto vp_v = vector3(0, -vp_h, 0);

    auto pixel_spacing_u = vp_u / img_w;
    auto pixel_spacing_v = vp_v / img_h;

    auto vp_upper_left = camera_center - vector3(0, 0, focal_length) - vp_u / 2 - vp_v / 2;
    auto pixel_00_location = vp_upper_left + 0.5 * (pixel_spacing_u + pixel_spacing_v);

    // Same world as main.cpp
    point3 ball_center(0, 0, -1);
    double ball_radius = 0.5;

    hittable_list world;
    world.add(make_shared<sphere>(ball_center, ball_radius));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "row  | object | t     | hit point             | normal                | rgb\n";

    int i = img_w / 2;  // middle column
    for (int j = 0; j < img_h; j += 5) {
        auto pixel_center = pixel_00_location + (i * pixel_spacing_u) + (j * pixel_spacing_v);
        ray r(camera_center, pixel_center - camera_center);

        hit_record rec;
        color c;
        std::string object;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            // A point on the ball is exactly ball_radius away from the ball's center
            double dist = (rec.point - ball_center).length();
            object = (std::fabs(dist - ball_radius) < 1e-6) ? "ball  " : "ground";
            c = 0.5 * (rec.normal + color(1, 1, 1));

            std::cout << std::setw(4) << j << " | " << object << " | " << rec.t
                      << " | " << rec.point << " | " << rec.normal << " | ";
        } else {
            vector3 unit_direction = unit_vector(r.direction());
            auto a = 0.5 * (unit_direction.y() + 1.0);
            c = (1.0 - a) * color(1, 1, 1) + a * color(0.5, 0.7, 1.0);

            std::cout << std::setw(4) << j << " | sky    | -     | -                     | -                     | ";
        }

        std::cout << int(255.999 * c.x()) << ' '
                  << int(255.999 * c.y()) << ' '
                  << int(255.999 * c.z()) << '\n';
    }
}