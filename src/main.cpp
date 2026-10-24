#include "common.h"
#include "camera.h"
#include "sphere.h"
#include "hittable_list.h"

int main(){

    //world
    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    camera cam;
    cam.aspect_ratio = 16.0/9.0;
    cam.image_width  = 256;

    cam.render(world);
}
