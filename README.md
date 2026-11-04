# Ray Tracer

A ray tracer written from scratch in C++17, following Peter Shirley's
[*Ray Tracing in One Weekend*](https://raytracing.github.io/) series.

This branch (`main`) is the single-threaded renderer from books 1 and 2
(*Ray Tracing in One Weekend* and *Ray Tracing: The Next Week*), following the
books directly. A version parallelized across all CPU cores with `std::thread`,
with benchmarks, lives on the
[`cpu-multithreading`](https://github.com/Harsha685/Ray_Tracer/tree/cpu-multithreading)
branch.

![Book 2 final scene](renders/render_20261104_132019.png)

| Book 1 final scene | Cornell box | Cornell box with smoke |
|:---:|:---:|:---:|
| ![Book 1 final scene](renders/render_20261026_195658.png) | ![Cornell box](renders/render_20261104_122207.png) | ![Cornell box with smoke](renders/render_20261104_122613.png) |

## Features

- **Math and output**
  - 3D vector math library with operator overloading (`vector3`)
  - PPM image output with gamma correction, converted to PNG
- **Shapes**
  - Spheres, including moving spheres for motion blur
  - Quadrilaterals (`quad`), and boxes built from six quads (`box()`)
  - Instances: `translate` and `rotate_y` move or rotate any object without
    copying its geometry
  - Constant-density volumes (`constant_medium`) for smoke, fog and mist
- **Acceleration**
  - Axis-aligned bounding boxes (`aabb`) with the slab test
  - Bounding volume hierarchy (`bvh_node`), so a ray tests about log n
    boxes instead of every object
- **Materials**
  - Lambertian (diffuse)
  - Metal with adjustable fuzz
  - Dielectric (glass) with refraction, total internal reflection and
    Schlick reflectance
  - Diffuse light (emissive surfaces)
  - Isotropic scattering inside volumes
- **Textures**
  - Solid color and 3D checkerboard
  - Image textures (loaded with [stb_image](https://github.com/nothings/stb)),
    mapped onto spheres and quads through (u, v) coordinates
  - Perlin noise with turbulence (marble)
- **Camera**
  - Positionable camera (`lookfrom`, `lookat`, `vup`, adjustable field of view)
  - Antialiasing by averaging jittered samples per pixel
  - Defocus blur (depth of field) via a thin-lens model
  - Motion blur: rays carry a random time in [0, 1)
  - Sky gradient or a flat background color, for scenes lit only by lights
  - Recursive ray bouncing with a configurable depth limit

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
timestamped PNG in `renders/` (`renders/render_YYYYMMDD_HHMMSS.png`).

`src/main.cpp` builds the book 2 final scene. Its arguments are set in
`main()`: `final_scene(image_width, samples_per_pixel, max_depth)`.
The default of 800×800 at 10,000 samples per pixel is a final-quality render
and takes a long time; lower `samples_per_pixel` (e.g. to 50) for quick
previews.

Image textures are loaded from `images/` (the renderer searches the current
directory, `images/`, and the `images/` folder of up to six parent
directories). Set the `RTW_IMAGES` environment variable to look in another
directory first.

## Project structure

```
raytracer/
├── src/
│   ├── common.h           # shared constants, random numbers, common includes
│   ├── vec3.h             # vector3 class, point3 alias, vector math and sampling
│   ├── color.h            # color alias, gamma correction and pixel output
│   ├── ray.h              # ray: origin + t * direction, at a moment in time
│   ├── interval.h         # [min, max] ranges for hit tests, clamping and boxes
│   ├── AABB.h             # axis-aligned bounding box and the slab hit test
│   ├── hittable.h         # hit_record, the abstract hittable class, translate and rotate_y
│   ├── hittable_list.h    # a list of hittables treated as one object
│   ├── bvh.h              # bounding volume hierarchy
│   ├── sphere.h           # ray-sphere intersection and sphere (u, v) mapping
│   ├── quad.h             # ray-quad intersection and box()
│   ├── constant_medium.h  # constant-density volumes (smoke, fog)
│   ├── perlin.h           # Perlin noise and turbulence
│   ├── image_loader.h     # image file loading through stb_image
│   ├── texture.h          # solid color, checker, image and noise textures
│   ├── material.h         # lambertian, metal, dielectric, diffuse_light, isotropic
│   ├── camera.h           # camera setup, ray generation and the render loop
│   ├── main.cpp           # scene setup (book 2 final scene)
│   ├── test.cpp           # debug output for the intersection math
│   └── external/
│       └── stb_image.h    # third-party image loader (v2.30, MIT or public domain)
├── docs/
│   ├── API_REFERENCE.md   # every type and function, file by file
│   ├── CALL_GRAPH.md      # what calls what, starting from main()
│   └── resources.md       # books and articles used
├── images/                # texture images (earth.jpg)
├── renders/               # saved output images
├── build/                 # compiled binaries and temporary images (git-ignored)
└── Makefile
```

## Documentation

- [docs/API_REFERENCE.md](docs/API_REFERENCE.md) lists every class, function
  and constant per source file along with its purpose.
- [docs/CALL_GRAPH.md](docs/CALL_GRAPH.md) shows which functions call which,
  and the hot path that dominates render time.
- [docs/resources.md](docs/resources.md) lists the books and articles used.
- Each function in `src/` has a block comment explaining what it does and
  the reasoning or math behind it.

## Roadmap

1. ~~*Ray Tracing in One Weekend*~~ (complete)
2. ~~Multithreading with `std::thread`, with benchmarks against the
   single-threaded version~~ (complete, on `cpu-multithreading`)
3. ~~*Ray Tracing: The Next Week* (BVH acceleration, textures, lights,
   instances, volumes)~~ (complete)
4. CUDA port, with CPU vs GPU benchmarks
5. *Ray Tracing: The Rest of Your Life* (Monte Carlo and importance sampling)

## Acknowledgments

Based on the [*Ray Tracing in One Weekend*](https://raytracing.github.io/)
book series by Peter Shirley, Trevor David Black and Steve Hollasch. Image
loading uses Sean Barrett's [stb_image](https://github.com/nothings/stb).
