#pragma once

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;

struct transform
{
public:
    
    vec4 position;
    quat rotation;
    vec4 scale;
        
    const transform operator* (transform parent) const;
    const vec3 operator* (const vec3& vertex_pos) const;
    const mat4 matrix() const;    
    const vec3 invert(const vec3& vertex_pos) const;

    transform();
    transform(vec4 pos, quat rotation, vec4 scale);    
};

const ::transform lerp(::transform a, ::transform b, float t);