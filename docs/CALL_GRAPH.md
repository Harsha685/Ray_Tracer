# Call Graph

What each function calls, starting from `main()`. Vector operators (`+`, `-`, `*`, `/`)
and trivial getters (`x()`, `origin()`, `time()`, ...) are left out to keep it readable.

- Solid arrows: direct calls.
- Dotted arrows: virtual dispatch (the actual function depends on the object's type).
- The `ray_color` self-loop is the bounce recursion, limited by `max_depth`.

```mermaid
flowchart TD
    main["main()"]

    subgraph scene["Scene setup (main.cpp)"]
        add["hittable_list::add()"]
        sphere_ctor["sphere() constructors"]
        mat_ctors["lambertian() / metal() / dielectric()"]
        vec_random["vector3::random()<br/>vector3::random(min, max)"]
    end

    subgraph cam["camera.h"]
        render["camera::render()"]
        initialize["camera::initialize()"]
        get_ray["camera::get_ray()"]
        sample_square["camera::sample_square()"]
        defocus["camera::defocus_disk_sample()"]
        ray_color["camera::ray_color()"]
    end

    subgraph hit["Intersection"]
        list_hit["hittable_list::hit()"]
        sphere_hit["sphere::hit()"]
        surrounds["interval::surrounds()"]
        ray_at["ray::at()"]
        face_normal["hit_record::set_face_normal()"]
    end

    subgraph mat["material.h"]
        lam["lambertian::scatter()"]
        met["metal::scatter()"]
        die["dielectric::scatter()"]
        reflectance["dielectric::reflectance()"]
    end

    subgraph out["color.h"]
        write_color["write_color()"]
        gamma["linear_to_gamma()"]
        clamp["interval::clamp()"]
    end

    subgraph util["vec3.h / common.h helpers"]
        unit_vector["unit_vector()"]
        length["vector3::length()"]
        length_sq["vector3::length_squared()"]
        cross["cross()"]
        dot["dot()"]
        rand_unit["random_unit_vector()"]
        rand_disk["random_in_unit_disk()"]
        near_zero["vector3::near_zero()"]
        reflect["reflect()"]
        refract["refract()"]
        deg2rad["degrees_to_radians()"]
        rand_d["random_double()"]
        rand_d_range["random_double(min, max)"]
    end

    %% main
    main --> add
    main --> sphere_ctor
    main --> mat_ctors
    main --> vec_random
    main --> rand_d
    main --> rand_d_range
    main --> length
    main --> render

    %% camera
    render --> initialize
    render --> get_ray
    render --> ray_color
    render --> write_color

    initialize --> deg2rad
    initialize --> unit_vector
    initialize --> cross

    get_ray --> sample_square
    get_ray --> defocus
    get_ray --> rand_d
    sample_square --> rand_d
    defocus --> rand_disk

    ray_color -. "world.hit()" .-> list_hit
    ray_color -. "rec.mat->scatter()" .-> lam
    ray_color -. "rec.mat->scatter()" .-> met
    ray_color -. "rec.mat->scatter()" .-> die
    ray_color -- "depth - 1" --> ray_color
    ray_color --> unit_vector

    %% intersection
    list_hit -. "object->hit()" .-> sphere_hit
    sphere_hit -- "center.at(time)" --> ray_at
    sphere_hit --> length_sq
    sphere_hit --> dot
    sphere_hit --> surrounds
    sphere_hit --> face_normal
    face_normal --> dot

    %% materials
    lam --> rand_unit
    lam --> near_zero
    met --> reflect
    met --> unit_vector
    met --> rand_unit
    met --> dot
    die --> unit_vector
    die --> dot
    die --> reflectance
    die --> rand_d
    die --> reflect
    die --> refract

    %% output
    write_color --> gamma
    write_color --> clamp

    %% helpers calling helpers
    unit_vector --> length
    length --> length_sq
    reflect --> dot
    refract --> dot
    refract --> length_sq
    rand_unit --> vec_random
    rand_unit --> length_sq
    rand_disk --> rand_d_range
    rand_disk --> length_sq
    vec_random --> rand_d
    vec_random --> rand_d_range
    rand_d_range --> rand_d
```

## Hot path

Per pixel sample, the work that actually dominates render time:

```mermaid
flowchart LR
    render["render()"] --> get_ray["get_ray()"] --> ray_color["ray_color()"]
    ray_color --> list_hit["hittable_list::hit()"] --> sphere_hit["sphere::hit()<br/>(once per object)"]
    ray_color --> scatter["material::scatter()"] --> ray_color
    render --> write_color["write_color()"]
```

Not shown: `random_on_hemisphere()`, `interval::contains()`, `interval::size()`,
`hittable_list::clear()` and the ray/interval constants exist but are never called by
the renderer.
