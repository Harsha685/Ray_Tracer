#include "common.h"
#include "sphere.h"
#include "hittable_list.h"

color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, 0.001, infinity, rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }
    vector3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main(){

    auto ar = 16.0/9.0;
    int img_w = 256;

    int img_h = int(img_w/ar);
    img_h = (img_h<1)?1:img_h;

    hittable_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    auto focal_length = 1.0;
    auto vp_h = 2.0;
    auto vp_w = vp_h*(double(img_w)/img_h);
    auto camera_center = point3(0,0,0);
    auto vp_u = vector3(vp_w,0,0);
    auto vp_v = vector3(0,-vp_h,0);

    auto pixel_spacing_u = vp_u/img_w;
    auto pixel_spacing_v = vp_v/img_h;

    auto vp_upper_left = camera_center - vector3(0,0,focal_length) - vp_u/2 - vp_v/2;
    auto pixel_00_location = vp_upper_left + 0.5*(pixel_spacing_u+pixel_spacing_v);

    std::cout << "P3\n" << img_w << ' ' << img_h << "\n255\n";

    for (int j=0;j<img_h;j++){
        std::clog << "\rScanlines remaining: " << (img_h - j) << ' ' << std::flush;

        for(int i=0;i<img_w;i++){
            auto pixel_center = pixel_00_location + (i*pixel_spacing_u) + (j*pixel_spacing_v);
            auto ray_dir = pixel_center - camera_center;
            ray r(camera_center, ray_dir);

            color pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                 \n";
}