#include "common.h"
#include "camera.h"
#include "sphere.h"
#include "hittable_list.h"

// Entry point: builds the scene, configures the camera, and renders the image to stdout.
// Run via `make run`, which redirects stdout to a .ppm file and converts it to PNG.
int main(){

    //world
    hittable_list world;
    auto ground_mat = make_shared<lambertian>(color(1, 0.8, 0.2));
    auto ball_mat1   = make_shared<metal>(color(0.5, 0.5, 0.5),0.3);
    auto ball_mat2   = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto ball_mat3   = make_shared<dielectric>(1.00/1.33);

    world.add(make_shared<sphere>(point3(-1, 0.0, -1), 0.5, ball_mat1));
    world.add(make_shared<sphere>(point3(1, 0.0, -1), 0.5, ball_mat2));
    world.add(make_shared<sphere>(point3(0, 0.0, -1), 0.5, ball_mat3));    
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, ground_mat));

    camera cam;
    cam.aspect_ratio = 16.0/9.0;     // Widescreen width:height
    cam.image_width  = 256;          // Width in pixels (height becomes 144)
    cam.samples_per_pixel = 100;     // Rays per pixel for antialiasing
    cam.max_depth = 50;              // Maximum bounces per ray
    cam.render(world);
}
