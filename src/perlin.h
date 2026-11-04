#ifndef PERLIN_H
#define PERLIN_H

#include "common.h"

/*
 * Perlin noise: smooth, repeatable pseudo-random values over 3D space.
 *
 *  - A lattice of random unit vectors (gradients) sits at every integer point.
 *  - The value at p blends the 8 lattice corners around p: each corner contributes
 *    dot(gradient, p - corner), weighted by a smoothed distance (trilinear interpolation
 *    with Hermite smoothing).
 *  - The same p always returns the same value, so textures built on it are stable
 *    across samples and frames.
 *  - Output is roughly in [-1, 1]; it is exactly 0 at every integer lattice point.
 */
class perlin {
  public:
    /*
     * Builds the gradient table and three independent permutation tables.
     *
     *  - Each permutation maps one coordinate to a shuffled index; XOR-ing the three
     *    picks a gradient for any integer (i, j, k). The & 255 wrap makes the noise
     *    repeat every 256 units, which is far beyond any scene size used here.
     */
    perlin() {
        for (int i = 0; i < point_count; i++) {
            randvec[i] = unit_vector(vector3::random(-1, 1));
        }

        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    /*
     * Returns the noise value at p, roughly in [-1, 1].
     */
    double noise(const point3& p) const {
        auto u = p.x() - std::floor(p.x());
        auto v = p.y() - std::floor(p.y());
        auto w = p.z() - std::floor(p.z());

        auto i = int(std::floor(p.x()));
        auto j = int(std::floor(p.y()));
        auto k = int(std::floor(p.z()));
        vector3 c[2][2][2];

        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    c[di][dj][dk] = randvec[
                        perm_x[(i + di) & 255] ^
                        perm_y[(j + dj) & 255] ^
                        perm_z[(k + dk) & 255]
                    ];

        return perlin_interp(c, u, v, w);
    }

    /*
     * Sums several octaves of noise: each octave doubles the frequency and halves the
     * weight. The absolute value folds negative noise up, giving the sharp creases
     * that look like turbulence, smoke, or marble veins. Result is in [0, ~1].
     */
    double turb(const point3& p, int depth = 7) const {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++) {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return std::fabs(accum);
    }

  private:
    static const int point_count = 256;
    vector3 randvec[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];

    /*
     * Fills p with 0..point_count-1 and shuffles it in place.
     */
    static void perlin_generate_perm(int* p) {
        for (int i = 0; i < point_count; i++)
            p[i] = i;

        permute(p, point_count);
    }

    /*
     * Fisher-Yates shuffle using the project's random_int().
     */
    static void permute(int* p, int n) {
        for (int i = n - 1; i > 0; i--) {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    /*
     * Trilinear interpolation of the 8 corner contributions.
     *
     *  - u, v, w are first passed through the Hermite curve 3t^2 - 2t^3 so the blend
     *    has zero slope at cell edges; without it, the lattice grid shows as visible
     *    Mach bands.
     *  - Each corner contributes dot(gradient, offset from that corner to p).
     */
    static double perlin_interp(const vector3 c[2][2][2], double u, double v, double w) {
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accum = 0.0;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++) {
                    vector3 weight_v(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu))
                           * (j * vv + (1 - j) * (1 - vv))
                           * (k * ww + (1 - k) * (1 - ww))
                           * dot(c[i][j][k], weight_v);
                }

        return accum;
    }
};

#endif // PERLIN_H
