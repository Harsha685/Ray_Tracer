#include "common.h"
#include "camera.h"
#include "sphere.h"
#include "hittable_list.h"
#include "bvh.h"

/*
 * Entry point: builds the scene, configures the camera, and renders the image to stdout.
 *
 *  - The scene is the final render from Ray Tracing in One Weekend: a large gray ground
 *    sphere, a 22 x 22 grid of small random spheres, and three large feature spheres.
 *  - Run via `make run`, which redirects stdout to a .ppm file and converts it to PNG.
 *  - Progress is printed to stderr, so it never ends up inside the image.
 */
int main() {
    // World
    hittable_list world;

    // Ground: huge sphere whose top is at y = 0
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    // Small spheres: one per grid cell, jittered inside the cell, resting on the ground (radius 0.2 at y = 0.2)
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();  // Picks the material: 80% diffuse, 15% metal, 5% glass
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            // Skip spheres that would overlap the large metal sphere at (4, 1, 0)
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // Diffuse: product of two random colors, which skews toward darker shades
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vector3(0, random_double(0,.5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // Metal: light tint, fuzz between 0 (mirror) and 0.5
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // Glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // Three large feature spheres: glass (center), diffuse brown (left), mirror metal (right)
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // Wrap the scene in a BVH so each ray only tests objects whose boxes it passes through
    world = hittable_list(make_shared<bvh_node>(world));

    // Camera
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;  // Widescreen width:height
    cam.image_width       = 1400;        // Width in pixels (height becomes 225)
    cam.samples_per_pixel = 200;          // Rays per pixel for antialiasing
    cam.max_depth         = 100;         // Maximum bounces per ray

    cam.vfov     = 20;                   // Vertical field of view in degrees
    cam.lookfrom = point3(13, 2, 3);     // Low and off to the side, looking across the field
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vector3(0, 1, 0);

    cam.defocus_angle = 0.6;             // Lens aperture angle; 0 disables blur
    cam.focus_dist    = 10.0;            // Distance to the sharp plane (lookfrom -> lookat is about 13.5)

    cam.render(world);
}
