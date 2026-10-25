#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material {
  public:
    virtual ~material() = default;

    // Default: the ray is absorbed (no bounce)
    virtual bool scatter(const ray& /*r_in*/, const hit_record& /*rec*/,color& /*attenuation*/, ray& /*scattered*/) const {
        return false;
    }
};

class lambertian : public material {
  public:
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& /*r_in*/, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // If the random vector almost cancels the normal, use the normal instead
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.point, scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

class metal : public material {
  public:
    metal(const color& albedo , double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vector3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.point, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    color albedo;
    double fuzz;
};

#endif // MATERIAL_H
