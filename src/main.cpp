#include "common.h"

#include "camera.h"
#include "sphere.h"
#include "quad.h"
#include "hittable_list.h"
#include "bvh.h"
#include "constant_medium.h"

// Final scene from "Ray Tracing: The Next Week": a field of random-height boxes, a moving
// sphere, glass/metal spheres, a fog-filled glass sphere, global mist, the earth, a
// Perlin sphere and a rotated cluster of 1000 small spheres.
// thread_count = render threads to run; 0 = one per logical CPU core.
void final_scene(int image_width, int samples_per_pixel, int max_depth, int thread_count) {
    // Ground: a 20 x 20 grid of boxes, each 100 wide with a random height
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));  // 400 boxes: a BVH skips most of them per ray

    // Ceiling light
    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(point3(123,554,147), vector3(300,0,0), vector3(0,0,265), light));

    // Moving sphere (motion blur)
    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vector3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    // Glass and fuzzy metal spheres
    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    // Glass sphere filled with blue fog: the sphere itself gives the glass surface,
    // and the constant_medium using it as a boundary gives the fog inside
    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    // Thin white mist filling the whole scene
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    // Earth (image texture) and marble (Perlin noise) spheres
    auto emat = make_shared<lambertian>(make_shared<image_texture>("earth.jpg"));
    world.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

    // Cluster of 1000 small white spheres, rotated 15 degrees and moved into place
    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
            vector3(-100,270,395)
        )
    );

    camera cam;
    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.thread_count      = thread_count;
    cam.use_sky           = false;              // Lit only by the ceiling light
    cam.background        = color(0, 0, 0);

    cam.vfov     = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main() {
    // width, samples per pixel, max bounces, threads (0 = all cores)
    final_scene(800, 10000, 40, 0);
}
