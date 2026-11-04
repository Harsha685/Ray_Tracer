#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "image_loader.h"
#include "perlin.h"

// Abstract base class for anything that gives a color at a point on a surface.
class Texture {
public:
    virtual ~Texture() = default;

    /*
     * Returns the color at surface coordinates (u, v) and world position p.
     * Each texture uses whichever of the two it needs.
     */
    virtual color value(double u, double v, const point3& p) const = 0;
};

// The same color everywhere.
class Solid_color : public Texture {
public:
    /*
     * Creates a texture that is albedo everywhere.
     */
    Solid_color(const color& albedo) : albedo(albedo) {}

    /*
     * Same, from separate red, green and blue values.
     */
    Solid_color(double red, double green, double blue) : Solid_color(color(red, green, blue)) {}

    /*
     * Returns the color, ignoring every argument.
     */
    color value(double /*u*/, double /*v*/, const point3& /*p*/) const override {
        return albedo;
    }

private:
    color albedo;   // The texture's only color
};

// 3D checkerboard: space is divided into cubes of side scale that alternate between
// two textures. Because it uses the world position, it works on any shape.
class checker_texture : public Texture {
public:
    /*
     * Creates a checker that alternates between the even and odd textures.
     */
    checker_texture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
        : inv_scale(1.0 / scale), even(even), odd(odd) {}

    /*
     * Same, from two solid colors.
     */
    checker_texture(double scale, const color& c1, const color& c2)
        : checker_texture(scale, make_shared<Solid_color>(c1), make_shared<Solid_color>(c2)) {}

    /*
     * Picks a texture by which cube p is in.
     *
     *  - floor(p / scale) gives the integer index of the cube on each axis.
     *  - Stepping one cube along any axis changes the sum of the indices by 1, so
     *    neighbouring cubes alternate between an even and an odd sum.
     */
    color value(double u, double v, const point3& p) const override {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;            // 1 / cube side length
    shared_ptr<Texture> even;    // Texture for cubes whose index sum is even
    shared_ptr<Texture> odd;     // Texture for cubes whose index sum is odd
};

// An image wrapped onto a surface using the hit's (u, v) coordinates.
class image_texture : public Texture {
public:
    /*
     * Loads the image (see image_loader for where the file is looked for).
     */
    image_texture(const char* filename) : image(filename) {}

    /*
     * Returns the image pixel at (u, v).
     *
     *  - u runs left to right across the image and v bottom to top, both in [0, 1].
     *    Image rows run top to bottom, so v is flipped.
     *  - If the image failed to load, returns cyan so the problem is easy to spot.
     */
    color value(double u, double v, const point3& /*p*/) const override {
        if (image.height() <= 0) return color(0, 1, 1);

        u = interval(0, 1).clamp(u);
        v = 1.0 - interval(0, 1).clamp(v);  // Flip v to image row order

        auto i = int(u * image.width());
        auto j = int(v * image.height());
        auto pixel = image.pixel_data(i, j);

        auto color_scale = 1.0 / 255.0;
        return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    image_loader image;   // The loaded image
};

/*
 * Marble-like texture driven by Perlin noise.
 *
 *  - scale sets the frequency: larger values give tighter stripes.
 *  - The stripes run along z as a sine wave; turbulence shifts the phase so the
 *    stripes bend and wander like veins in marble.
 *  - 0.5 * (1 + sin(...)) maps the sine from [-1, 1] into [0, 1] so it can be used
 *    directly as a grey level.
 */
class noise_texture : public Texture {
public:
    noise_texture(double scale) : scale(scale) {}

    color value(double /*u*/, double /*v*/, const point3& p) const override {
        return color(0.5, 0.5, 0.5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
    }

private:
    perlin noise;   // Noise generator with its own random gradient tables
    double scale;   // Stripe frequency
};

#endif // TEXTURE_H
