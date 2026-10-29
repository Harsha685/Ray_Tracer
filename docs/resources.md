# Resources

Books, articles and references used while building this ray tracer.

## Ray Tracing in One Weekend series

The main guide for the project, by Peter Shirley, Trevor David Black and Steve Hollasch.

- [Ray Tracing in One Weekend — series home](https://raytracing.github.io/)
- [Book 1: *Ray Tracing in One Weekend*](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
  — vectors, rays, spheres, antialiasing, materials (Lambertian, metal, dielectric),
  positionable camera, defocus blur
- [Book 2: *Ray Tracing: The Next Week*](https://raytracing.github.io/books/RayTracingTheNextWeek.html)
  — motion blur, bounding volume hierarchies (AABB, BVH), textures, lights, instances
- [Book 3: *Ray Tracing: The Rest of Your Life*](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html)
  — Monte Carlo integration, importance sampling, PDFs
- [Source code for the series (GitHub)](https://github.com/RayTracing/raytracing.github.io)

## Books

- *Fundamentals of Computer Graphics* — Steve Marschner and Peter Shirley (5th ed.,
  CRC Press). Covers the math foundations (vectors, transforms), ray tracing basics,
  shading and spatial data structures.
- [*Physically Based Rendering: From Theory to Implementation*](https://pbr-book.org/)
  — Matt Pharr, Wenzel Jakob and Greg Humphreys. Free online; the in-depth reference
  for BVHs, materials, sampling and light transport.
- [*Ray Tracing Gems*](https://www.realtimerendering.com/raytracinggems/) — Eric Haines
  and Tomas Akenine-Möller (eds.). Free PDF; practical techniques and tips.

## Bounding Volume Hierarchies

- [Jacco Bikker — *How to build a BVH, Part 1: Basics*](https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/)
  — first article in a multi-part series covering BVH construction, traversal,
  SAH and faster rays

## Tools

- [PPM image format (Netpbm)](https://netpbm.sourceforge.net/doc/ppm.html) — the
  output format written by the renderer
- [ImageMagick](https://imagemagick.org/) — converts the PPM output to PNG
