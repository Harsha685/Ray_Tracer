#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "image_loader.h"

class Texture {
    public:
        virtual ~Texture() = default;
        virtual color value(double u, double v, const point3& p) const = 0;
};

class Solid_color : public Texture{
    public:
        Solid_color(const color& albedo) : albedo(albedo) {}

        Solid_color(double red, double green, double blue) : Solid_color(color(red,green,blue)) {}

        color value(double /*u*/, double /*v*/, const point3& /*p*/) const override {
            return albedo;
        }

    private:
        color albedo;
};

class checker_texture : public Texture {
  public:
    checker_texture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
      : inv_scale(1.0 / scale), even(even), odd(odd) {}

    checker_texture(double scale, const color& c1, const color& c2)
      : checker_texture(scale, make_shared<Solid_color>(c1), make_shared<Solid_color>(c2)) {}

    color value(double u, double v, const point3& p) const override {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

  private:
    double inv_scale;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};

class image_texture : public Texture {
  public:
    image_texture(const char* filename) : image(filename) {}

    color value(double u, double v, const point3& /*p*/) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (image.height() <= 0) return color(0,1,1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = interval(0,1).clamp(u);
        v = 1.0 - interval(0,1).clamp(v);  // Flip V to image coordinates

        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i,j);

        auto color_scale = 1.0 / 255.0;
        return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }

  private:
    image_loader image;
};
#endif //TEXTURE_H