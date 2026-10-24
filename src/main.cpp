#include<iostream>
#include "vec3.h"
#include "ray.h"
#include "color.h"

double hit_sphere(const point3& center, double radius, const ray& r) {
    vector3 oc = center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = h*h - a*c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (h - std::sqrt(discriminant)) / a;
    }
}

color ray_color(const ray& r) {
    auto t = hit_sphere(point3(0,0,-1), 0.5, r);
    if(t > 0){
        vector3 Normal = unit_vector(r.at(t) - vector3(0,0,-1));
        return 0.5*color(Normal.x()+1, Normal.y()+1, Normal.z()+1);
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

    auto focal_lenght = 1.0;
    auto vp_h = 2.0;
    auto vp_w = vp_h*(double(img_w)/img_h);
    auto camera_center = point3(0,0,0);
    auto vp_u = vector3(vp_w,0,0);
    auto vp_v = vector3(0,-vp_h,0);

    auto pixel_spacing_u = vp_u/img_w;
    auto pixel_spacing_v = vp_v/img_h;

    auto vp_upper_left = camera_center - vector3(0,0,focal_lenght) - vp_u/2 - vp_v/2;
    auto pixel_00_location = vp_upper_left + 0.5*(pixel_spacing_u+pixel_spacing_v);

    std::cout << "P3\n" << img_w << ' ' << img_h << "\n255\n";

    for (int j=0;j<img_h;j++){
        std::clog << "\rScanlines remaining: " << (img_h - j) << ' ' << std::flush;

        for(int i=0;i<img_w;i++){
            auto pixel_center = pixel_00_location + (i*pixel_spacing_u) + (j*pixel_spacing_v);
            auto ray_dir = pixel_center - camera_center;
            ray r(camera_center, ray_dir);
            
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color); 
        }
    }
    std::clog << "\rDone.                 \n";
}