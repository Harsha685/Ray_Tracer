#include "common.h"

// Hide the one warning from the third-party stb_image library
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include "image_loader.h"
#pragma GCC diagnostic pop

#include "camera.h"
#include "sphere.h"
#include "quad.h"
#include "hittable_list.h"

int main() {
    // Materials
    auto ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));  // Perlin marble
    auto glass  = make_shared<dielectric>(1.5);
    auto earth  = make_shared<lambertian>(make_shared<image_texture>("earth.jpg"));
    auto gold   = make_shared<metal>(color(0.9, 0.7, 0.3), 0.05);
    auto red    = make_shared<lambertian>(color(0.7, 0.12, 0.1));
    auto blue   = make_shared<lambertian>(color(0.1, 0.25, 0.6));
    auto teal   = make_shared<lambertian>(color(0.2, 0.6, 0.55));
    auto orange = make_shared<lambertian>(color(0.85, 0.45, 0.15));

    // Spheres: position, radius, material
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground));
    world.add(make_shared<sphere>(point3(-2.4, 0.9, 0), 0.9, glass));
    world.add(make_shared<sphere>(point3(0, 1.1, -0.4), 1.1, earth));
    world.add(make_shared<sphere>(point3(2.4, 0.9, 0), 0.9, gold));
    world.add(make_shared<sphere>(point3(-0.9, 0.4, 1.8), 0.4, red));
    world.add(make_shared<sphere>(point3(1.0, 0.35, 2.1), 0.35, blue));

    // Box: two opposite corners, material (a small cube in the front-right, offset from the
    // camera so its front, top and left faces are all visible)
    world.add(box(point3(2.3, 0, 1.4), point3(3.1, 0.8, 2.2), teal));


    // Camera
    camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1400;
    cam.samples_per_pixel = 400;
    cam.max_depth         = 100;
    cam.vfov              = 30;
    cam.lookfrom          = point3(0, 2, 10);
    cam.lookat            = point3(0, 0.8, 0);

    cam.render(world);
}
