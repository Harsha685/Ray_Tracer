#include "common.h"
#include "camera.h"
#include "sphere.h"
#include "hittable_list.h"

// Entry point: builds the scene, configures the camera, and renders the image to stdout.
// Run via `make run`, which redirects stdout to a .ppm file and converts it to PNG.
int main(){

    //world
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));        // Small ball 1 unit in front of the camera, radius 0.5
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));   // Huge sphere acting as the ground; its top sits at y = -0.5, under the ball

    camera cam;
    cam.aspect_ratio = 16.0/9.0;     // Widescreen width:height
    cam.image_width  = 256;          // Width in pixels (height becomes 144)
    cam.samples_per_pixel = 100;     // Rays per pixel for antialiasing
    cam.max_depth = 50;              // Maximum bounces per ray
    cam.render(world);
}
