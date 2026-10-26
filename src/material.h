#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

// Abstract base class describing how a surface responds to an incoming ray.
class material {
public:
    /*
     * Virtual destructor so derived materials are destroyed correctly through a
     * shared_ptr<material>.
     */
    virtual ~material() = default;

    /*
     * Decides what happens to ray r_in when it hits the surface described by rec.
     *
     *  - Returns true if the ray bounces: scattered is set to the new ray and
     *    attenuation to how much of each color channel survives the bounce.
     *  - Returns false if the ray is absorbed (no further light is gathered).
     *  - The default implementation absorbs every ray.
     */
    virtual bool scatter(const ray& /*r_in*/, const hit_record& /*rec*/,
                         color& /*attenuation*/, ray& /*scattered*/) const {
        return false;
    }
};

// Matte (diffuse) surface that scatters light in random directions.
class lambertian : public material {
public:
    /*
     * Creates a diffuse material with the given albedo (fraction of light reflected
     * per color channel).
     */
    lambertian(const color& albedo) : albedo(albedo) {}

    /*
     * Scatters the ray in a random direction biased toward the surface normal.
     *
     *  - Direction is normal + random_unit_vector(), which gives a cosine-weighted
     *    (Lambertian) distribution.
     *  - If the random vector almost cancels the normal, the direction would be near
     *    zero and cause NaNs later, so the normal is used instead.
     *  - Always scatters; attenuation is the albedo.
     */
    bool scatter(const ray& /*r_in*/, const hit_record& rec,
                 color& attenuation, ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.point, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;   // Fraction of light reflected per color channel
};

// Reflective surface, optionally blurred by a fuzz factor.
class metal : public material {
public:
    /*
     * Creates a metal material.
     *
     *  - albedo tints the reflected light.
     *  - fuzz is the radius of the random offset added to the reflection;
     *    0 is a perfect mirror. Values above 1 are clamped to 1.
     */
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    /*
     * Reflects the ray about the surface normal, then perturbs it by fuzz.
     *
     *  - The reflected direction is normalized before the fuzz offset is added, so
     *    fuzz is relative to a unit-length reflection.
     *  - If the perturbed ray points into the surface it is absorbed (returns false).
     */
    bool scatter(const ray& r_in, const hit_record& rec,
                 color& attenuation, ray& scattered) const override {
        vector3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.point, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;   // Tint applied to reflected light
    double fuzz;    // Reflection blur radius in [0, 1]
};

// Clear material such as glass or water that both reflects and refracts.
class dielectric : public material {
public:
    /*
     * Creates a dielectric with the given refractive index.
     *
     *  - The index is relative to the surrounding medium (e.g. 1.5 for glass in air).
     *  - An index below 1 (e.g. 1.0 / 1.5) models an air bubble inside glass.
     */
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    /*
     * Refracts or reflects the ray depending on the angle and the refractive index.
     *
     *  - The index ratio is 1 / refraction_index when entering the surface and
     *    refraction_index when leaving it.
     *  - If ratio * sin(theta) > 1, Snell's law has no solution (total internal
     *    reflection), so the ray reflects.
     *  - Otherwise it reflects with probability given by Schlick's approximation,
     *    and refracts the rest of the time.
     *  - Glass absorbs nothing, so attenuation is always white.
     */
    bool scatter(const ray& r_in, const hit_record& rec,
                 color& attenuation, ray& scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vector3 unit_direction = unit_vector(r_in.direction());

        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vector3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.point, direction);
        return true;
    }

private:
    double refraction_index;   // Refractive index relative to the surrounding medium

    /*
     * Schlick's approximation of the fraction of light reflected at a surface.
     *
     *  - r0 is the reflectance when looking straight at the surface.
     *  - Reflectance rises toward 1 at grazing angles: r0 + (1 - r0) * (1 - cos)^5.
     *  - This is why glass looks mirror-like at its edges.
     */
    static double reflectance(double cosine, double refraction_index) {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

#endif // MATERIAL_H
