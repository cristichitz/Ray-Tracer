uint pcg_hash(uint *seed)
{
    uint state = *seed;
    *seed = state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

// Returns a float between min and max
float random_float(float min, float max, uint *seed)
{
    float random_unorm = (float)pcg_hash(seed) / (float)0xFFFFFFFFu; // 0.0 to 1.0
    return min + (max - min) * random_unorm;
}

float3 random_vec_between(float min, float max, uint *seed)
{
    return (float3)(
        random_float(min, max, seed), 
        random_float(min, max, seed), 
        random_float(min, max, seed)
    );
}

float3 random_unit_vector(uint *seed)
{
    while (1)
    {
        float3 p = random_vec_between(-1.0f, 1.0f, seed);
        float lensq = dot(p, p);
        if (1e-32f < lensq && lensq <= 1.0f)
            return (p / sqrt(lensq));
    }
}

bool  near_zero(float3 a)
{
  float s;
  
  s = 1e-8;
  return (fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s);
}
