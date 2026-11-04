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

// The original showcase scene: textured/glass/metal spheres, two boxes and a light sphere,
// under the sky gradient.
void spheres_and_boxes() {
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

    // Rotated + translated box: build it with one corner at the origin, spin it 30 degrees
    // around the y-axis (rotate_y pivots around the world origin, so rotate first), then
    // move it into place in the front-left of the scene.
    shared_ptr<hittable> tall_box = box(point3(0, 0, 0), point3(0.6, 1.0, 0.6), orange);
    tall_box = make_shared<rotate_y>(tall_box, 30);
    tall_box = make_shared<translate>(tall_box, vector3(-3.1, 0, 1.6));
    world.add(tall_box);

    // Light sphere: a small emitter floating above the earth sphere, near the top of the frame.
    // Emission above 1 makes it bright enough to visibly light the objects around it.
    auto light = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<sphere>(point3(0, 2.9, -0.4), 0.4, light));

    // Camera
    camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 100;
    cam.vfov              = 30;
    cam.lookfrom          = point3(0, 2, 10);
    cam.lookat            = point3(0, 0.8, 0);

    cam.render(world);
}

// Classic Cornell box: a 555-unit room with a red left wall, green right wall, white floor,
// ceiling and back wall, a square ceiling light, and two white boxes (one tall, one short)
// rotated around the y-axis. The only light comes from the ceiling, so the sky is turned off.
void cornell_box() {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    // Walls: quad(corner, edge u, edge v, material)
    world.add(make_shared<quad>(point3(555, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), green));     // right wall (x = 555)
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), red));         // left wall (x = 0)
    world.add(make_shared<quad>(point3(343, 554, 332), vector3(-130, 0, 0), vector3(0, 0, -105), light)); // ceiling light
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));       // floor
    world.add(make_shared<quad>(point3(555, 555, 555), vector3(-555, 0, 0), vector3(0, 0, -555), white)); // ceiling
    world.add(make_shared<quad>(point3(0, 0, 555), vector3(555, 0, 0), vector3(0, 555, 0), white));     // back wall

    // Tall box: built at the origin, rotated 15 degrees, then moved into the back-left
    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vector3(265, 0, 295));
    world.add(box1);

    // Short box: rotated -18 degrees and placed in the front-right
    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vector3(130, 0, 65));
    world.add(box2);

    camera cam;
    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.use_sky           = false;
    cam.background        = color(0, 0, 0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main() {
    // Pick which scene to render
    switch (2) {
        case 1: spheres_and_boxes(); break;
        case 2: cornell_box();       break;
    }
}
