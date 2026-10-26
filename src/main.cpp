#include "common.h"
#include "camera.h"
#include "sphere.h"
#include "hittable_list.h"

// Entry point: builds the scene, configures the camera, and renders the image to stdout.
// Run via `make run`, which redirects stdout to a .ppm file and converts it to PNG.
int main(){

    //world
    hittable_list world;
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    camera cam;
    cam.aspect_ratio = 16.0/9.0;     // Widescreen width:height
    cam.image_width  = 256;          // Width in pixels (height becomes 144)
    cam.samples_per_pixel = 100;     // Rays per pixel for antialiasing
    cam.max_depth = 50;              // Maximum bounces per ray
    cam.vfov = 90;  // Vertical field of view in degrees
    cam.lookfrom = point3(-2,2,1);
    cam.lookat   = point3(0,0,-1);
    cam.vup      = vector3(0,1,0);
    cam.render(world);


}
