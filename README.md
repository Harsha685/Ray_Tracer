# Ray Tracer

A ray tracer written from scratch in C++17, following Peter Shirley's
[*Ray Tracing in One Weekend*](https://raytracing.github.io/) series.

Every class in this project (vectors, rays, colors, hittable objects) was
rewritten by hand rather than copied, as a way to properly learn C++ and the
math behind rendering. The goal is to go beyond the book with multithreading
and a CUDA GPU port.

![Latest render](renders/latest.png)

## Features so far

- [x] 3D vector math library with operator overloading (`vector3`)
- [x] PPM image output
- [x] Camera, viewport and per-pixel ray generation
- [x] Sky background using a linear blend (lerp) on ray direction
- [x] Ray–sphere intersection (quadratic formula, simplified "half-b" form)
- [x] Surface normals, visualized as colors
- [x] Multiple objects via an abstract `hittable` class and `hittable_list`
- [x] Anti-aliasing
- [x] Diffuse (Lambertian), metal (with fuzz) and glass materials
- [x] Refraction with total internal reflection and Schlick reflectance
- [ ] Positionable camera and depth of field

## Building and running

Requirements: `g++` (C++17), `make`, and ImageMagick (for PNG conversion).

```bash
sudo apt install build-essential imagemagick
```

From the project root:

```bash
make view    # build, render, convert to PNG and open it
make run     # build and render only
make test    # build and run src/test.cpp
make clean   # remove build output
```

The renderer writes a PPM image to `build/image.ppm`, which is converted to a
timestamped PNG in `renders/` (e.g. `renders/render_YYYYMMDD_HHMMSS.png`).

## Project structure

```
raytracer/
├── src/
│   ├── common.h         # shared constants, random numbers, common includes
│   ├── vec3.h           # vector3 class, point3 alias, vector math
│   ├── color.h          # color alias, gamma correction and pixel output
│   ├── ray.h            # ray: origin + t * direction
│   ├── interval.h       # [min, max] ranges for hit tests and clamping
│   ├── hittable.h       # hit_record and the abstract hittable class
│   ├── hittable_list.h  # a list of hittables treated as one object
│   ├── sphere.h         # ray-sphere intersection
│   ├── material.h       # lambertian, metal and dielectric materials
│   ├── camera.h         # viewport, ray generation and the render loop
│   ├── main.cpp         # scene setup
│   └── test.cpp         # debug output for the intersection math
├── renders/        # saved output images
├── build/          # compiled binaries and temporary images (git-ignored)
├── Makefile
└── NOTES.md        # learning notes
```

## Roadmap

1. Finish *Ray Tracing in One Weekend*
2. Multithreading with `std::thread`, with benchmarks against the single-threaded version
3. *Ray Tracing: The Next Week* (BVH acceleration, textures, lights)
4. CUDA port, with CPU vs GPU benchmarks
5. *Ray Tracing: The Rest of Your Life* (Monte Carlo and importance sampling)

## What I learned

See [NOTES.md](NOTES.md) for notes on the concepts covered so far, including
operator overloading, `const` references, the viewport math, and the
derivation of the ray–sphere intersection.

## Acknowledgments

Based on the [*Ray Tracing in One Weekend*](https://raytracing.github.io/)
book series by Peter Shirley, Trevor David Black and Steve Hollasch.
