#include "UMath.h"
#include "transform.h"

const transform lerp(transform a, transform b, float t)
{
    return transform(lerp(a.position, b.position, t), lerp(a.rotation, b.rotation, t), lerp(a.scale, b.scale, t));
}

const transform transform:: operator* (transform parent) const 
{
    transform t = *this;

    t.position = parent * t.position;
    t.rotation = t.rotation * parent.rotation;
                
    t.scale.x *= parent.scale.x;
    t.scale.y *= parent.scale.y;
    t.scale.z *= parent.scale.z;

    return t;   
}

const vec3 transform:: operator*(const vec3& vertex_pos) const
{
    return GLScale(scale) * (rotate(rotation, vertex_pos) + position);
}

const mat4 transform:: matrix() const
{
    mat4 res = mat4_identity;
    res = mat4(rotation);

    res.r1[3] = position.x;
    res.r2[3] = position.y;
    res.r3[3] = position.z;
        
    res *= GLScale(scale);
    return res;
}
    
const vec3 transform:: invert(const vec3& vertex_pos) const
{
    quat c = conjugate(rotation);
    return rotate(c, vertex_pos - position);
}

transform::transform()
{
    position = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    rotation = quat(0.0f, 0.0f, 0.0f, 1.0f);
    scale = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

transform:: transform(vec4 pos, quat rotation, vec4 scale) : position(pos), rotation(rotation), scale(scale) {}    


