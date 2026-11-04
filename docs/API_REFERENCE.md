# API Reference

A file-by-file reference of every type, function and constant in `src/`.
Each entry gives the signature and a short description of its purpose. The
source files carry the full explanation (math, edge cases, rationale) in a
block comment above each function.

Header dependency order, from lowest to highest level:

```
common.h helpers  ->  interval.h, vec3.h  ->  color.h, ray.h  ->  AABB.h
  ->  hittable.h  ->  hittable_list.h  ->  bvh.h
perlin.h, image_loader.h  ->  texture.h  ->  material.h
  ->  sphere.h, quad.h, constant_medium.h  ->  camera.h  ->  main.cpp
```

`common.h` defines its helpers first, then includes `color.h`, `interval.h`,
`ray.h` and `vec3.h` at the bottom, so including it gives a file all of those.
Every `.cpp` file must include `common.h` first. `vec3.h` and `interval.h` use its
helpers but do not include it, which avoids an include cycle, so they and the
headers built on them cannot be compiled on their own.

`image_loader.h` compiles stb_image's implementation, so only one `.cpp` file per
program may include it (directly or through `texture.h` / `material.h`).

---

## common.h

Shared includes, aliases, constants, random-number utilities and the common
project headers.

### Aliases

| Name | Definition | Purpose |
|------|------------|---------|
| `shared_ptr` | `std::shared_ptr` | Reference-counted ownership of scene objects, materials and textures |
| `make_shared` | `std::make_shared` | Allocates an object owned by a `shared_ptr` |

### Constants

| Name | Type | Value | Purpose |
|------|------|-------|---------|
| `infinity` | `const double` | `std::numeric_limits<double>::infinity()` | Unbounded upper limit for ray intervals |
| `pi` | `const double` | `3.1415926535897932385` | Angle conversions and sphere (u, v) mapping |

### Functions

| Signature | Purpose |
|-----------|---------|
| `double degrees_to_radians(double degrees)` | Converts degrees to radians for use with `std::sin`, `std::cos`, `std::tan` |
| `double random_double()` | Uniform random number in [0, 1). Each thread has its own `thread_local` Mersenne Twister, seeded from a shared atomic counter (0, 1, 2, ...). The main thread builds the scene with seed 0, so the scene is identical on every run |
| `double random_double(double min, double max)` | Uniform random number in [min, max) |
| `int random_int(int min, int max)` | Uniform random integer in [min, max], both ends included. Used to pick BVH split axes and shuffle Perlin tables |

---

## interval.h

### class `interval`

A closed range of real numbers [min, max]. Used for the valid `t` range of a
ray, for clamping color values and for each axis of a bounding box.

| Member | Purpose |
|--------|---------|
| `double min, max` | Interval bounds |
| `interval()` | Creates an empty interval (min = +inf, max = -inf) |
| `interval(double min, double max)` | Creates [min, max] |
| `interval(const interval& a, const interval& b)` | Tightest interval enclosing both a and b |
| `double size() const` | Length of the interval (negative if empty) |
| `bool contains(double x) const` | True if x is in [min, max] (endpoints included) |
| `bool surrounds(double x) const` | True if x is in (min, max) (endpoints excluded). Used for sphere hit tests |
| `double clamp(double x) const` | Returns x limited to [min, max] |
| `interval extend(double delta) const` | Copy widened by delta (delta / 2 on each side) |
| `static const interval empty` | Contains no numbers |
| `static const interval universe` | Contains every number |

### Free functions

| Signature | Purpose |
|-----------|---------|
| `interval operator+(const interval&, double)` | Interval shifted by a displacement |
| `interval operator+(double, const interval&)` | Same, operands swapped |

---

## vec3.h

### class `vector3`

A 3-component vector of doubles, used for points, directions and colors.

| Member | Purpose |
|--------|---------|
| `double coords[3]` | Component storage |
| `vector3()` | Zero vector |
| `vector3(double c0, double c1, double c2)` | Vector with the given components |
| `double x() const`, `y()`, `z()` | Component getters |
| `vector3 operator-() const` | Negation |
| `double operator[](int i) const` | Read-only component access (no bounds check) |
| `double& operator[](int i)` | Writable component access (no bounds check) |
| `vector3& operator+=(const vector3&)` | In-place addition |
| `vector3& operator-=(const vector3&)` | In-place subtraction |
| `vector3& operator*=(double)` | In-place scalar multiplication |
| `vector3& operator/=(double)` | In-place scalar division |
| `double length() const` | Euclidean length |
| `double length_squared() const` | Squared length; avoids the square root when only comparing |
| `static vector3 random()` | Each component random in [0, 1) |
| `static vector3 random(double min, double max)` | Each component random in [min, max) |
| `bool near_zero() const` | True if every component is below 1e-8 in magnitude. Guards against degenerate scatter directions |

### Aliases

| Name | Definition | Purpose |
|------|------------|---------|
| `point3` | `vector3` | Marks a value as a position rather than a direction |

### Free functions: arithmetic

| Signature | Purpose |
|-----------|---------|
| `std::ostream& operator<<(std::ostream&, const vector3&)` | Prints `x y z` |
| `vector3 operator+(const vector3&, const vector3&)` | Vector addition |
| `vector3 operator-(const vector3&, const vector3&)` | Vector subtraction |
| `vector3 operator*(const vector3&, const vector3&)` | Component-wise multiplication (color tinting) |
| `vector3 operator/(const vector3&, const vector3&)` | Component-wise division |
| `vector3 operator*(double, const vector3&)` | Scalar multiplication |
| `vector3 operator*(const vector3&, double)` | Scalar multiplication |
| `vector3 operator/(const vector3&, double)` | Scalar division |
| `double dot(const vector3&, const vector3&)` | Dot product |
| `vector3 cross(const vector3&, const vector3&)` | Cross product (right-hand rule) |

### Free functions: geometry and sampling

| Signature | Purpose |
|-----------|---------|
| `vector3 unit_vector(const vector3& v)` | Normalizes v to length 1 |
| `vector3 random_unit_vector()` | Uniform random direction on the unit sphere (rejection sampling) |
| `vector3 random_on_hemisphere(const vector3& normal)` | Random unit direction on the same side as `normal`. Not used by current materials; kept for reference |
| `vector3 reflect(const vector3& v, const vector3& n)` | Mirror reflection of v about unit normal n |
| `vector3 refract(const vector3& v, const vector3& n, double n1_over_n2)` | Refraction of unit vector v by Snell's law. Caller handles total internal reflection |
| `vector3 random_in_unit_disk()` | Random point in the unit disk on z = 0. Used for defocus blur |

---

## color.h

### Aliases

| Name | Definition | Purpose |
|------|------------|---------|
| `color` | `vector3` | RGB color, each channel nominally in [0, 1] (lights use values above 1) |

### Functions

| Signature | Purpose |
|-----------|---------|
| `double linear_to_gamma(double linear_component)` | Gamma-2 correction (square root); negative input returns 0 |
| `void write_color(std::ostream& op, const color& c)` | Gamma-corrects, clamps and writes one pixel as `R G B` bytes for a P3 PPM file |

---

## ray.h

### class `ray`

A half-line P(t) = origin + t * direction that exists at a moment in time
(used for motion blur).

| Member | Purpose |
|--------|---------|
| `ray()` | Zero origin and direction, time 0 |
| `ray(const point3& origin, const vector3& direction, double time)` | Ray from origin along direction at the given time |
| `ray(const point3& origin, const vector3& direction)` | Ray from origin along direction at time 0 |
| `const point3& origin() const` | Starting point |
| `const vector3& direction() const` | Direction (not normalized) |
| `double time() const` | Moment in time the ray exists at |
| `point3 at(double t) const` | Point at parameter t |

---

## AABB.h

### class `aabb`

Axis-aligned bounding box: one `interval` per axis. Used to reject rays
cheaply before testing the objects inside.

| Member | Purpose |
|--------|---------|
| `interval x, y, z` | Extent on each axis |
| `aabb()` | Empty box (every axis empty) |
| `aabb(const interval& x, const interval& y, const interval& z)` | Box from three intervals, padded so no side is thinner than 0.0001 |
| `aabb(const point3& a, const point3& b)` | Box with opposite corners a and b (any order), padded the same way |
| `aabb(const aabb& box0, const aabb& box1)` | Smallest box enclosing both boxes |
| `const interval& axis_interval(int n) const` | Interval on axis n (0 = x, 1 = y, 2 = z) |
| `bool hit_interval(const ray& r, interval i) const` | Slab test: true if the ray is inside the box for some t in i |
| `void pad_to_minimums()` (private) | Widens any side thinner than 0.0001, so flat objects such as quads still get a box the slab test can hit |

### Free functions

| Signature | Purpose |
|-----------|---------|
| `aabb operator+(const aabb&, const vector3& offset)` | Box translated by offset |
| `aabb operator+(const vector3& offset, const aabb&)` | Same, operands swapped |

---

## hittable.h

### class `hit_record`

Data describing a single ray/surface intersection.

| Member | Purpose |
|--------|---------|
| `material* mat` | Material of the surface hit. A plain non-owning pointer (the object owns the material), so copying a `hit_record` never touches a shared reference count |
| `vector3 normal` | Unit normal, always facing against the incoming ray |
| `point3 point` | World-space hit position |
| `double t` | Ray parameter at the hit |
| `double u, v` | Surface texture coordinates, set by every shape (spheres from latitude/longitude, quads from their plane coordinates, volumes to 0) |
| `bool front_face` | True if the ray hit the outside of the surface |
| `void set_face_normal(const ray& r, const vector3& outward_normal)` | Sets `normal` and `front_face` from a unit outward normal |

### class `hittable` (abstract)

Base class for anything a ray can intersect.

| Member | Purpose |
|--------|---------|
| `virtual ~hittable()` | Virtual destructor for safe polymorphic deletion |
| `virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0` | Tests for a hit with t inside `ray_t`; fills `rec` on success |
| `virtual aabb bounding_box() const = 0` | Box enclosing the object (over its whole motion, for moving objects) |

### class `translate : hittable`

Instance that moves another object without copying its geometry.

| Member | Purpose |
|--------|---------|
| `translate(shared_ptr<hittable> object, const vector3& offset)` | Wraps object so it appears shifted by offset |
| `bool hit(const ray&, interval, hit_record&) const` | Shifts the ray by -offset, tests the object, then shifts the hit point back by +offset |
| `aabb bounding_box() const` | Object's box shifted by offset |

### class `rotate_y : hittable`

Instance that rotates another object around the y-axis (pivot at the world
origin, so rotate first, then translate).

| Member | Purpose |
|--------|---------|
| `rotate_y(shared_ptr<hittable> object, double angle)` | Wraps object so it appears rotated by angle degrees. Stores sin and cos, and builds the box enclosing the 8 rotated corners of the object's box |
| `bool hit(const ray&, interval, hit_record&) const` | Rotates the ray by -angle into object space, tests the object, then rotates the hit point and normal by +angle back |
| `aabb bounding_box() const` | Box enclosing the rotated object |

---

## hittable_list.h

### class `hittable_list : hittable`

A collection of objects treated as a single hittable (the scene, or one box's
six faces).

| Member | Purpose |
|--------|---------|
| `std::vector<shared_ptr<hittable>> objects` | Contained objects |
| `hittable_list()` | Empty list |
| `hittable_list(shared_ptr<hittable> obj)` | List with one object |
| `void clear()` | Removes all objects and resets the bounding box |
| `void add(shared_ptr<hittable> obj)` | Appends an object and grows the bounding box to include it |
| `bool hit(const ray&, interval, hit_record&) const` | Returns the closest hit among all objects |
| `aabb bounding_box() const` | Box enclosing every object in the list |

---

## bvh.h

### class `bvh_node : hittable`

Bounding volume hierarchy: a binary tree of boxes. A ray that misses a node's
box skips everything below it.

| Member | Purpose |
|--------|---------|
| `bvh_node(hittable_list list)` | Builds a tree over every object in list (sorts a copy, so the caller's list is unchanged) |
| `bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end)` | Builds a tree over `objects[start, end)`: sorts along a random axis and splits the span in half |
| `bool hit(const ray&, interval, hit_record&) const` | Returns false if the ray misses this node's box; otherwise tests both children, cutting the right child's range off at the left child's hit |
| `aabb bounding_box() const` | Box enclosing both children |

Private helpers: `box_compare` (orders two objects by their box minimum on an axis) and `box_x_compare` / `box_y_compare` / `box_z_compare` (the same, fixed to one axis, for `std::sort`).

---

## sphere.h

### class `sphere : hittable`

| Member | Purpose |
|--------|---------|
| `sphere(const point3& static_center, double radius, shared_ptr<material> mat)` | Stationary sphere with the given center, radius (clamped to >= 0) and material |
| `sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat)` | Moving sphere whose center travels from `center1` at time 0 to `center2` at time 1 |
| `bool hit(const ray&, interval, hit_record&) const` | Ray-sphere intersection via the simplified quadratic formula; also sets (u, v) |
| `aabb bounding_box() const` | Box enclosing the sphere over its whole motion |
| `static void get_sphere_uv(const point3& p, double& u, double& v)` (private) | Maps a point on the unit sphere to (u, v): u from the angle around the y-axis, v from the angle up from the bottom pole |

Private data: `center` (a `ray`; `center.at(time)` is the center at that time), `radius`, `mat`, `bound_box`.

---

## quad.h

### class `quad : hittable`

A flat parallelogram with corner `Q` and edge vectors `u` and `v`.

| Member | Purpose |
|--------|---------|
| `quad(const point3& Q, const vector3& u, const vector3& v, shared_ptr<material> mat)` | Creates the quad and caches its unit normal, plane constant D and `w = n / dot(n, n)` |
| `void set_bounding_box()` | Sets the box enclosing all four corners |
| `bool hit(const ray&, interval, hit_record&) const` | Ray-plane intersection, then checks the hit point's plane coordinates (alpha, beta) are inside the quad |
| `bool is_interior(double a, double b, hit_record& rec) const` | True if a and b are both in [0, 1]; stores them as (u, v) |
| `aabb bounding_box() const` | Box enclosing the quad |

### Free functions

| Signature | Purpose |
|-----------|---------|
| `shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat)` | Axis-aligned box with opposite corners a and b, as six quads whose normals point outward |

---

## constant_medium.h

### class `constant_medium : hittable`

A volume of constant density (smoke, fog, mist) filling a convex boundary
shape. Rays may scatter anywhere inside it.

| Member | Purpose |
|--------|---------|
| `constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<Texture> tex)` | Medium inside boundary, colored by a texture. Higher density scatters sooner |
| `constant_medium(shared_ptr<hittable> boundary, double density, const color& albedo)` | Same, with a single color |
| `bool hit(const ray&, interval, hit_record&) const` | Finds where the ray enters and leaves the boundary, samples an exponentially distributed scatter distance, and reports a hit there if it is before the exit |
| `aabb bounding_box() const` | The boundary's box |

Private data: `boundary`, `neg_inv_density` (-1 / density), `phase_function` (an `isotropic` material).

---

## perlin.h

### class `perlin`

| Member | Purpose |
|--------|---------|
| `perlin()` | Builds 256 random unit gradient vectors and three shuffled permutation tables |
| `double noise(const point3& p) const` | Smooth gradient noise at `p`, roughly in [-1, 1]; 0 at integer lattice points |
| `double turb(const point3& p, int depth = 7) const` | `|sum of depth octaves|`, each octave at double frequency and half weight |

Private helpers: `perlin_generate_perm` (fill and shuffle a table), `permute` (Fisher-Yates), `perlin_interp` (Hermite-smoothed trilinear blend of the 8 corner gradients).

---

## image_loader.h

### class `image_loader`

Loads an image file into memory as 8-bit RGB through stb_image. Not copyable
(it owns raw buffers).

| Member | Purpose |
|--------|---------|
| `image_loader()` | No image; `width()` and `height()` return 0 |
| `image_loader(const char* image_filename)` | Loads the file, trying `$RTW_IMAGES` first, then the name as given, then `images/` in the current directory and up to six parent directories. Prints an error if none work |
| `bool load(const std::string& filename)` | Loads one path; returns true on success |
| `int width() const`, `int height() const` | Image size in pixels, or 0 if nothing is loaded |
| `const unsigned char* pixel_data(int x, int y) const` | Pointer to the 3 RGB bytes of pixel (x, y), clamped to the image. Magenta if nothing is loaded |

Private helpers: `clamp` (integer clamp to [low, high)), `float_to_byte` ([0, 1] float to [0, 255] byte), `convert_to_bytes` (fills the byte buffer from the float buffer).

---

## texture.h

### class `Texture` (abstract)

| Member | Purpose |
|--------|---------|
| `virtual ~Texture()` | Virtual destructor |
| `virtual color value(double u, double v, const point3& p) const = 0` | Returns the color at surface coordinates `(u, v)` and world position `p` |

### class `Solid_color : Texture`

| Member | Purpose |
|--------|---------|
| `Solid_color(const color& albedo)` | Texture with a single color |
| `Solid_color(double red, double green, double blue)` | Same, from separate channels |
| `color value(...) const` | Returns the color, ignoring all arguments |

### class `checker_texture : Texture`

| Member | Purpose |
|--------|---------|
| `checker_texture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)` | 3D checker of two textures with cubes of side `scale` |
| `checker_texture(double scale, const color& c1, const color& c2)` | Same, from two solid colors |
| `color value(...) const` | Picks a texture by whether the cube indices of `p` sum to an even number |

### class `image_texture : Texture`

| Member | Purpose |
|--------|---------|
| `image_texture(const char* filename)` | Loads the image through `image_loader` |
| `color value(...) const` | Image pixel at (u, v), with v flipped to image row order. Cyan if the image failed to load |

### class `noise_texture : Texture`

| Member | Purpose |
|--------|---------|
| `noise_texture(double scale)` | Marble texture; `scale` sets the stripe frequency |
| `color value(...) const` | Grey level `0.5 * (1 + sin(scale * p.z + 10 * turb(p)))`; ignores `u, v` |

---

## material.h

### class `material` (abstract)

| Member | Purpose |
|--------|---------|
| `virtual ~material()` | Virtual destructor |
| `virtual color emitted(double u, double v, const point3& p) const` | Light given off by the surface. Default is black |
| `virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const` | Produces a scattered ray and its attenuation; returns false if the ray is absorbed. Default absorbs |

### class `lambertian : material`

Diffuse (matte) surface.

| Member | Purpose |
|--------|---------|
| `lambertian(const color& albedo)` | Diffuse material with the given reflectance |
| `lambertian(shared_ptr<Texture> tex)` | Diffuse material whose reflectance comes from a texture |
| `bool scatter(...) const` | Scatters along `normal + random_unit_vector()` (cosine-weighted); falls back to the normal if degenerate |

### class `metal : material`

Reflective surface.

| Member | Purpose |
|--------|---------|
| `metal(const color& albedo, double fuzz)` | Metal with tint `albedo` and blur `fuzz` (clamped to <= 1) |
| `bool scatter(...) const` | Mirror reflection perturbed by fuzz; absorbed if it ends up below the surface |

### class `dielectric : material`

Transparent surface (glass, water).

| Member | Purpose |
|--------|---------|
| `dielectric(double refraction_index)` | Dielectric with the given index relative to the surrounding medium |
| `bool scatter(...) const` | Reflects on total internal reflection or with Schlick probability; otherwise refracts |
| `static double reflectance(double cosine, double refraction_index)` (private) | Schlick's approximation of reflectance |

### class `diffuse_light : material`

Light-emitting surface. Inherits the default `scatter()`, so it absorbs every
ray and only contributes its emission.

| Member | Purpose |
|--------|---------|
| `diffuse_light(shared_ptr<Texture> tex)` | Light whose color comes from a texture |
| `diffuse_light(const color& emit)` | Light of a single color (values above 1 light up the surroundings) |
| `color emitted(double u, double v, const point3& p) const` | The texture's color at the hit point |

### class `isotropic : material`

Phase function for volumes: scatters in a uniformly random direction.

| Member | Purpose |
|--------|---------|
| `isotropic(const color& albedo)` | Isotropic material of a single color |
| `isotropic(shared_ptr<Texture> tex)` | Isotropic material colored by a texture |
| `bool scatter(...) const` | New ray from the hit point in a random unit direction; attenuation is the texture's color |

---

## camera.h

### class `camera`

Positionable camera with antialiasing, defocus blur and motion blur that renders on
multiple threads. Configure the public fields, then call `render()`.

#### Public settings

| Field | Default | Purpose |
|-------|---------|---------|
| `double aspect_ratio` | `1.0` | Image width / height |
| `int image_width` | `100` | Image width in pixels |
| `int samples_per_pixel` | `10` | Rays averaged per pixel |
| `int max_depth` | `10` | Maximum ray bounces |
| `int thread_count` | `0` | Render threads; 0 uses one per logical CPU core |
| `bool use_sky` | `true` | Rays that miss everything see the sky gradient (true) or `background` (false) |
| `color background` | `(0, 0, 0)` | Color of rays that miss everything when `use_sky` is false |
| `double vfov` | `90` | Vertical field of view in degrees |
| `point3 lookfrom` | `(0, 0, 0)` | Camera position |
| `point3 lookat` | `(0, 0, -1)` | Point the camera looks at |
| `vector3 vup` | `(0, 1, 0)` | Camera-relative up direction |
| `double defocus_angle` | `0` | Lens cone angle in degrees; 0 disables blur |
| `double focus_dist` | `10` | Distance from `lookfrom` to the plane of perfect focus |

#### Public methods

| Signature | Purpose |
|-----------|---------|
| `void render(const hittable& world)` | Renders the scene in parallel into a frame buffer (threads claim rows from an atomic counter), then writes it to stdout as a P3 PPM image; progress on stderr |

#### Private methods

| Signature | Purpose |
|-----------|---------|
| `void initialize()` | Derives image height, camera basis, viewport, pixel deltas and defocus disk |
| `void write_framebuffer() const` | Writes the PPM header and every pixel of the frame buffer to stdout, in row order |
| `color ray_color(const ray& r, int depth, const hittable& world) const` | Recursively traces a ray: emitted light plus attenuation times the scattered ray's color. Returns the sky gradient or `background` on a miss |
| `vector3 sample_square() const` | Random offset in [-0.5, 0.5]^2 within a pixel |
| `point3 defocus_disk_sample() const` | Random ray origin on the lens disk |
| `ray get_ray(int i, int j) const` | Jittered ray for pixel (row i, column j) from the lens, at a random time in [0, 1) |

---

## main.cpp

| Signature | Purpose |
|-----------|---------|
| `void final_scene(int image_width, int samples_per_pixel, int max_depth, int thread_count)` | Builds the book 2 final scene (a BVH of 400 random-height ground boxes, a ceiling light, a moving sphere, glass and metal spheres, a fog-filled glass sphere, scene-wide mist, earth and marble spheres, and a rotated BVH of 1000 small spheres), configures the camera and renders |
| `int main()` | Calls `final_scene` with the render settings |

## test.cpp

| Signature | Purpose |
|-----------|---------|
| `int main()` | Traces one primary ray per 5th pixel down the middle column of a fixed two-sphere scene and prints hit object, t, point, normal and color. Used to check intersection math |
