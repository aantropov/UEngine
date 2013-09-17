#pragma once

#include <cmath>
#include "stdlib.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "mat2.h"
#include "mat3.h"
#include "mat4.h"
#include "transform.h"
#include "frustum.h"
#include "bounds.h"

// math constants
const float math_epsilon = 1e-5f;
const float math_pi      = 3.1415926536f;
const float math_radians = math_pi / 180.0f;
const float math_degrees = 180.0f / math_pi;

// functions
inline float round(float value, float precious)
{
    return  precious * (value > 0.0 ? (floor(value/precious)) : ceil(value/precious));
}

inline double round(double value, double precious)
{
    return precious * (value > 0.0 ? (floor(value/precious)) : ceil(value/precious));
}

#ifndef min
__forceinline float min(float a, float b)
{
    return (a < b ? a : b);
}
#endif

#ifndef max
__forceinline float max(float a, float b)
{
    return (a < b ? b : a);
}
#endif

// clamp x to [a,b]
inline float clamp(float x, float a, float b)
{
    return (x < a ? a : ( x > b ? b : x ));
}

// linear interpolation
inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

inline quat lerp(quat a, quat b, float t)
{    
    if(dot(a,b) < 0)
        return a - (b + a) * t;
    else
        return a + (b - a) * t;
}

inline vec3 lerp(vec3 a, vec3 b, float t)
{
    return vec3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t));
}
inline vec4 lerp(vec4 a, vec4 b, float t)
{
    return vec4(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t), lerp(a.w, b.w, t));
}
// spherical interpolation
inline quat slerp(float latitude, float longitude, float angle)
{
    quat q;
    float sin_a = sin( angle / 2 );
    float cos_a = cos( angle / 2 );

    float sin_lat = sin( latitude );
    float cos_lat = cos( latitude );

    float sin_long = sin( longitude );
    float cos_long = cos( longitude );

    q.x = sin_a * cos_lat * sin_long;
    q.y = sin_a * sin_lat;
    q.z = sin_a * sin_lat * cos_long;
    q.w = cos_a;

    return q;
}
// random number in [0,1]
inline float unirand()
{
    return (float)rand() / (float)RAND_MAX;
}

// random number in [0,a]
inline float unirand(float a)
{
    return a * unirand();
}

// random number in [a,b]
inline float unirand(float a, float b)
{
    return lerp(a, b, unirand());
}

// random point on sphere
inline const vec3 sphrand()
{
    float u = unirand(2.0f * math_pi);
    float h = unirand(2.0f)  - 1.0f;
    float r = sqrtf(1.0f - h * h);
    return vec3(cosf(u) * r, sinf(u) * r, h);
}

// constants
const vec2 vec2_zero(0, 0);
const vec2 vec2_x(1, 0);
const vec2 vec2_y(0, 1);

const vec3 vec3_zero(0, 0, 0);
const vec3 vec3_x(1, 0, 0);
const vec3 vec3_y(0, 1, 0);
const vec3 vec3_z(0, 0, 1);


const vec4 vec4_zero(0, 0, 0, 0);
const vec4 vec4_x(1, 0, 0, 0);
const vec4 vec4_y(0, 1, 0, 0);
const vec4 vec4_z(0, 0, 1, 0);

const mat2 mat2_zero(0, 0,
                     0, 0);
const mat2 mat2_identity(1, 0,
                         0, 1);

const mat3 mat3_zero(0, 0, 0,
                     0, 0, 0,
                     0, 0, 0);
const mat3 mat3_identity(1, 0, 0,
                         0, 1, 0,
                         0, 0, 1);

const mat4 mat4_zero(0, 0, 0, 0,
                     0, 0, 0, 0,
                     0, 0, 0, 0,
                     0, 0, 0, 0);
const mat4 mat4_identity(1, 0, 0, 0,
                         0, 1, 0, 0,
                         0, 0, 1, 0,
                         0, 0, 0, 1);

// vec2
inline void vec2::set(const vec3 &v) { x = v.x; y = v.y; }
inline void vec2::set(const vec4 &v) { x = v.x; y = v.y; }

// vec3
inline void vec3::set(const vec2 &v) { x = v.x; y = v.y; z = 0;   }
inline void vec3::set(const vec4 &v) { x = v.x; y = v.y; z = v.z; }

// vec4
inline void vec4::set(const vec2 &v) { x = v.x; y = v.y; z = 0;   w = 0; }
inline void vec4::set(const vec3 &v) { x = v.x; y = v.y; z = v.z; w = 0; }

// quat
inline void quat::set(const vec2 &v)          { x = v.x; y = v.y; z = 0;   w = 1;       }
inline void quat::set(const vec3 &v)          { x = v.x; y = v.y; z = v.z; w = 1;       }
inline void quat::set(const vec4 &v)          { x = v.x; y = v.y; z = v.z; w = v.w;     }
inline void quat::set(const vec3 &v, float w) { x = v.x; y = v.y; z = v.z; this->w = w; }

inline void quat::set(const mat3 &M)
{
    const float t = M[0] + M[4] + M[8];
    float       s;

    if (t > 0.0f)
    {
        s = 0.5f / sqrtf(t + 1.0f);
        x = (M[7] - M[5]) * s;
        y = (M[2] - M[6]) * s;
        z = (M[3] - M[1]) * s;
        w = 0.25f / s;
    } else if (M[0] > M[4] && M[0] > M[8])
    {
        s = 2.0f * sqrtf(1.0f + M[0] - M[4] - M[8]);
        x = 0.25f * s;
        y = (M[3] + M[1]) / s;
        z = (M[2] + M[6]) / s;
        w = (M[7] - M[5]) / s;
    } else if (M[4] > M[8])
    {
        s = 2.0f * sqrtf(1.0f + M[4] - M[0] - M[8]);
        x = (M[3] + M[1]) / s;
        y = 0.25f * s;
        z = (M[7] + M[5]) / s;
        w = (M[2] - M[6]) / s;
    } else
    {
        s = 2.0f * sqrtf(1.0f + M[8] - M[0] - M[4]);
        x = (M[2] + M[6]) / s;
        y = (M[7] + M[5]) / s;
        z = 0.25f * s;
        w = (M[3] - M[1]) / s;
    }
}

inline void quat::set(const mat4 &M)
{
    set(mat3(M));
}

// mat2
inline void mat2::set(const mat3 &M)
{
    set(M[0], M[1],
        M[3], M[4]);
}

inline void mat2::set(const mat4 &M)
{
    set(M[0], M[1],
        M[4], M[5]);
}

inline const vec2 operator*(const mat2 &M, const vec2 &v)
{
    return vec2(M[0] * v.x + M[1] * v.y,
                M[2] * v.x + M[3] * v.y);
}

// mat3
inline void mat3::set(const mat2 &M)
{
    set(M[0], M[1], 0,
        M[2], M[3], 0,
        0,    0,    1);
}

inline void mat3::set(const mat4 &M)
{
    set(M[0], M[1], M[2],
        M[4], M[5], M[6],
        M[8], M[9], M[10]);
}

inline void mat3::set(const quat &q)
{
    const float xx = q.x * q.x,
                xy = q.x * q.y,
                xz = q.x * q.z,
                xw = q.x * q.w,
                yy = q.y * q.y,
                yz = q.y * q.z,
                yw = q.y * q.w,
                zz = q.z * q.z,
                zw = q.z * q.w;

    set(1 - 2 * (yy + zz), 2 * (xy - zw), 2 * (xz + yw),
        2 * (xy + zw), 1 - 2 * (xx + zz), 2 * (yz - xw),
        2 * (xz - yw), 2 * (yz + xw), 1 - 2 * (xx + yy));
}

inline const vec2 operator*(const mat3 &M, const vec2 &v)
{
    return vec2(M[0] * v.x + M[1] * v.y + M[2],
                M[3] * v.x + M[4] * v.y + M[5]);
}

inline const vec3 operator*(const mat3 &M, const vec3 &v)
{
    return vec3(M[0] * v.x + M[1] * v.y + M[2] * v.z,
                M[3] * v.x + M[4] * v.y + M[5] * v.z,
                M[6] * v.x + M[7] * v.y + M[8] * v.z);
}

// mat4
inline void mat4::set(const mat2 &M)
{
    set(M[0], M[1], 0, 0,
        M[2], M[3], 0, 0,
        0,    0,    1, 0,
        0,    0,    0, 1);
}

inline void mat4::set(const mat3 &M)
{
    set(M[0], M[1], M[2], 0,
        M[3], M[4], M[5], 0,
        M[6], M[7], M[8], 0,
        0,    0,    0,    1);
}

inline void mat4::set(const quat &q)
{
    set(mat3(q));
}

inline const vec2 operator*(const mat4 &M, const vec2 &v)
{
    return vec2(M[0] * v.x + M[1] * v.y + M[2] + M[3],
                M[4] * v.x + M[5] * v.y + M[6] + M[7]);
}

inline const vec3 operator*(const mat4 &M, const vec3 &v)
{
    return vec3(M[ 0] * v.x + M[ 1] * v.y + M[ 2] * v.z + M[ 3],
                M[ 4] * v.x + M[ 5] * v.y + M[ 6] * v.z + M[ 7],
                M[ 8] * v.x + M[ 9] * v.y + M[10] * v.z + M[11]);
}

inline const vec4 operator*(const mat4 &M, const vec4 &v)
{
    return vec4(M[ 0] * v.x + M[ 1] * v.y + M[ 2] * v.z + M[ 3] * v.w,
                M[ 4] * v.x + M[ 5] * v.y + M[ 6] * v.z + M[ 7] * v.w,
                M[ 8] * v.x + M[ 9] * v.y + M[10] * v.z + M[11] * v.w,
                M[12] * v.x + M[13] * v.y + M[14] * v.z + M[15] * v.w);
}

// project (x,y) on a unity sphere
inline const vec3 sproject(float x, float y, float xcenter, float ycenter)
{
    vec3 v(x / xcenter - 1.0f, 1.0f - y / ycenter, 0.0f);
    float length = v.x * v.x + v.y * v.y;

    if (length > 1.0f)
    {
        v = normalize(v);
        v.z = 0.0f;
    } else
    {
        v.z = sqrtf(1.0f - length);
    }

    return v;
}

// create arcball
inline const quat arcball(const vec3 &from, const vec3 &to, float xcenter, float ycenter)
{
    vec3 vf = sproject(from.x, from.y, xcenter, ycenter);
    vec3 vt = sproject(to.x,   to.y,   xcenter, ycenter);

    return quat(cross(vf, vt), dot(vf, vt));
}

// shortest arc
inline const quat shortarc(const vec3 &from, const vec3 &to)
{
    quat q(cross(from, to), dot(from, to));

    q = normalize(q);
    q.w += 1.0f;

    if (q.w <= math_epsilon)
    {
        if ((from.z * from.z) > (from.x * from.x))
            q.set(0, from.z, -from.y, q.w);
        else 
            q.set(from.y, -from.x, 0, q.w);
    }

    return normalize(q);
}

/*
inline vec3 VertexTangentSpace(vec3& v0pos, vec3& v1pos, vec3& v2pos, vec2& v0tex, vec2& v1tex, vec2& v2tex)
{
    vec3 tangent = vec3(0.0f,0.0f,0.0f);
    vec3 bitangent = vec3(0.0f,0.0f,0.0f);

    vec3 e0 = v1pos - v0pos;
    vec3 e1 = v2pos - v0pos;
    vec2 e0uv = v1tex - v0tex;
    vec2 e1uv = v2tex - v0tex;

   float cp = e0uv.y * e1uv.x - e0uv.x * e1uv.y;

   if ( cp != 0.0f ) {
      float k = 1.0f / cp;
      tangent   = (e0 * -e1uv.y + e1 * e0uv.y) * k;
      bitangent = (e0 * -e1uv.x + e1 * e0uv.x) * k;

      normalize(tangent);
      normalize(bitangent);
   }
   return bitangent;
}
*/

inline vec3 VertexTangentSpace(vec3 &p1, vec3 &p2, vec3 &p3, vec2 &t1, vec2 &t2, vec2 &t3)
{
  vec3 e1  = p2 - p1;
  vec3 e2  = p3 - p1;
  vec2 et1 = t2 - t1;
  vec2 et2 = t3 - t1;
  vec3 tangent = vec3_zero;
  vec3 binormal = vec3_zero;

  float tmp = 0.0;
  
  if (fabsf(et1.x * et2.y - et1.y * et2.x) < math_epsilon)
    tmp = 1.0f;
  else
    tmp = 1.0f / (et1.x * et2.y - et1.y * et2.x);
  
  tangent  = (e1 * et2.y - e2 * et1.y) * tmp;
  binormal = (e2 * et1.x - e1 * et2.x) * tmp;
  normalize(tangent);
  
  return normalize(binormal);
}

/*
inline vec3 VertexTangentSpace(vec3& v0pos, vec3& v1pos, vec3& v2pos, vec2& v0tex, vec2& v1tex, vec2& v2tex)
{ 
    vec3 v0t = vec3(0, 0, 0);
    vec3 e0 ( v1pos.x - v0pos.x, v1tex.x - v0tex.x, v1tex.y - v0tex.y );
    vec3 e1 ( v2pos.x - v0pos.x, v2tex.x - v0tex.x, v2tex.y - v0tex.y );

    vec3 cp= cross(e0, e1);
    
    if ( fabs ( cp[0] ) > math_epsilon )
        v0t.x = -cp.y / cp.x;

    e0.x = v1pos.y - v0pos.y;
    e1.x = v2pos.y - v0pos.y;
    cp   = cross(e0, e1);

    if ( fabs ( cp[0] ) > math_epsilon )
        v0t.y = -cp.y / cp.x;

    e0.z = v1pos.z - v0pos.z;
    e1.z = v2pos.z - v0pos.z;
    cp   = cross(e0, e1);

    if ( fabs ( cp[0] ) > math_epsilon )
        v0t.z = -cp.y / cp.x; 

    return normalize(v0t);
}/**/