# API Reference

A file-by-file reference of every type, function and constant in `src/`.
Each entry gives the signature and a short description of its purpose. The
source files carry the full explanation (math, edge cases, rationale) in a
block comment above each function.

Header dependency order, from lowest to highest level:

```
common.h  ->  interval.h, vec3.h  ->  color.h, ray.h
hittable.h  ->  hittable_list.h, material.h  ->  sphere.h  ->  camera.h  ->  main.cpp
```

`common.h` is the base header and includes no project headers, so there are no
include cycles. Every other header includes exactly what it uses and can be
included on its own.

---

## common.h

Shared includes, aliases, constants and random-number utilities.

### Aliases

| Name | Definition | Purpose |
|------|------------|---------|
| `shared_ptr` | `std::shared_ptr` | Reference-counted ownership of scene objects and materials |
| `make_shared` | `std::make_shared` | Allocates an object owned by a `shared_ptr` |

### Constants

| Name | Type | Value | Purpose |
|------|------|-------|---------|
| `infinity` | `const double` | `std::numeric_limits<double>::infinity()` | Unbounded upper limit for ray intervals |
| `pi` | `const double` | `3.1415926535897932385` | Angle conversions |

### Functions

| Signature | Purpose |
|-----------|---------|
| `double degrees_to_radians(double degrees)` | Converts degrees to radians for use with `std::sin`, `std::cos`, `std::tan` |
| `double random_double()` | Uniform random number in [0, 1). Default-seeded Mersenne Twister, so renders are reproducible |
| `double random_double(double min, double max)` | Uniform random number in [min, max) |

---

## interval.h

### class `interval`

A closed range of real numbers [min, max]. Used for the valid `t` range of a
ray and for clamping color values.

| Member | Purpose |
|--------|---------|
| `double min, max` | Interval bounds |
| `interval()` | Creates an empty interval (min = +inf, max = -inf) |
| `interval(double min, double max)` | Creates [min, max] |
| `double size() const` | Length of the interval (negative if empty) |
| `bool contains(double x) const` | True if x is in [min, max] (endpoints included) |
| `bool surrounds(double x) const` | True if x is in (min, max) (endpoints excluded). Used for hit tests |
| `double clamp(double x) const` | Returns x limited to [min, max] |
| `static const interval empty` | Contains no numbers |
| `static const interval universe` | Contains every number |

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
| `color` | `vector3` | RGB color, each channel nominally in [0, 1] |

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

## hittable.h

### class `hit_record`

Data describing a single ray/surface intersection.

| Member | Purpose |
|--------|---------|
| `shared_ptr<material> mat` | Material of the surface hit |
| `vector3 normal` | Unit normal, always facing against the incoming ray |
| `point3 point` | World-space hit position |
| `double t` | Ray parameter at the hit |
| `bool front_face` | True if the ray hit the outside of the surface |
| `void set_face_normal(const ray& r, const vector3& outward_normal)` | Sets `normal` and `front_face` from a unit outward normal |

### class `hittable` (abstract)

Base class for anything a ray can intersect.

| Member | Purpose |
|--------|---------|
| `virtual ~hittable()` | Virtual destructor for safe polymorphic deletion |
| `virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0` | Tests for a hit with t strictly inside `ray_t`; fills `rec` on success |

---

## hittable_list.h

### class `hittable_list : hittable`

A collection of objects treated as a single hittable (the scene).

| Member | Purpose |
|--------|---------|
| `std::vector<shared_ptr<hittable>> objects` | Contained objects |
| `hittable_list()` | Empty list |
| `hittable_list(shared_ptr<hittable> obj)` | List with one object |
| `void clear()` | Removes all objects |
| `void add(shared_ptr<hittable> obj)` | Appends an object |
| `bool hit(const ray&, interval, hit_record&) const` | Returns the closest hit among all objects |

---

## sphere.h

### class `sphere : hittable`

| Member | Purpose |
|--------|---------|
| `sphere(const point3& static_center, double radius, shared_ptr<material> mat)` | Stationary sphere with the given center, radius (clamped to >= 0) and material |
| `sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat)` | Moving sphere whose center travels from `center1` at time 0 to `center2` at time 1 |
| `bool hit(const ray&, interval, hit_record&) const` | Ray-sphere intersection via the simplified quadratic formula |

Private data: `center` (a `ray`; `center.at(time)` is the center at that time), `radius`, `mat`.

---

## material.h

### class `material` (abstract)

| Member | Purpose |
|--------|---------|
| `virtual ~material()` | Virtual destructor |
| `virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const` | Produces a scattered ray and its attenuation; returns false if the ray is absorbed. Default absorbs |

### class `lambertian : material`

Diffuse (matte) surface.

| Member | Purpose |
|--------|---------|
| `lambertian(const color& albedo)` | Diffuse material with the given reflectance |
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

---

## camera.h

### class `camera`

Positionable camera with antialiasing, defocus blur and motion blur. Configure the public
fields, then call `render()`.

#### Public settings

| Field | Default | Purpose |
|-------|---------|---------|
| `double aspect_ratio` | `1.0` | Image width / height |
| `int image_width` | `100` | Image width in pixels |
| `int samples_per_pixel` | `10` | Rays averaged per pixel |
| `int max_depth` | `10` | Maximum ray bounces |
| `double vfov` | `90` | Vertical field of view in degrees |
| `point3 lookfrom` | `(0, 0, 0)` | Camera position |
| `point3 lookat` | `(0, 0, -1)` | Point the camera looks at |
| `vector3 vup` | `(0, 1, 0)` | Camera-relative up direction |
| `double defocus_angle` | `0` | Lens cone angle in degrees; 0 disables blur |
| `double focus_dist` | `10` | Distance from `lookfrom` to the plane of perfect focus |

#### Public methods

| Signature | Purpose |
|-----------|---------|
| `void render(const hittable& world)` | Renders the scene to stdout as a P3 PPM image; progress on stderr |

#### Private methods

| Signature | Purpose |
|-----------|---------|
| `void initialize()` | Derives image height, camera basis, viewport, pixel deltas and defocus disk |
| `color ray_color(const ray& r, int depth, const hittable& world) const` | Recursively traces a ray; returns sky gradient on a miss |
| `vector3 sample_square() const` | Random offset in [-0.5, 0.5]^2 within a pixel |
| `point3 defocus_disk_sample() const` | Random ray origin on the lens disk |
| `ray get_ray(int i, int j) const` | Jittered ray for pixel (row i, column j) from the lens, at a random time in [0, 1) |

---

## main.cpp

| Signature | Purpose |
|-----------|---------|
| `int main()` | Builds the scene (ground, diffuse, glass with air bubble, fuzzy metal), configures the camera and renders |

## test.cpp

| Signature | Purpose |
|-----------|---------|
| `int main()` | Traces one primary ray per 5th pixel down the middle column of a fixed two-sphere scene and prints hit object, t, point, normal and color. Used to check intersection math |
