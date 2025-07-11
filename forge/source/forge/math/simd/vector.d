module forge.math.simd.vector;

import inteli;

auto dot(float4 a, float4 b) pure
{
    auto x0 = _mm_mul_ps(a, b);
    auto x1 = _mm_hadd_ps(x0, x0);

    return _mm_hadd_ps(x1, x1)[0];
}
