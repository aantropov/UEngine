#pragma once
#include "..\math\umath.h"

class UTransform
{
public:
	
	vec4 pos;
	quat rotation;
	vec4 scale;

	static const UTransform Lerp(UTransform a, UTransform b, float t)
	{
		return UTransform(lerp(a.pos, b.pos, t), lerp(a.rotation, b.rotation, t), lerp(a.scale, b.scale, t));
	}

	const UTransform operator* (UTransform parent) const 
	{
		UTransform t = *this;

		t.pos = parent.Transform(t.pos);
		t.rotation = t.rotation * parent.rotation;
				
		t.scale.x *= parent.scale.x;
		t.scale.y *= parent.scale.y;
		t.scale.z *= parent.scale.z;

		return t;   
	}

	const mat4 ToMatrix() const
	{
		mat4 res = mat4_identity;
		res = mat4(rotation);

		res.r1[3] = pos.x;
		res.r2[3] = pos.y;
		res.r3[3] = pos.z;
		
		res *= GLScale(scale);
		return res;
	}
	
	const vec3 Transform(const vec3& vertex_pos) const
    {
        return rotate(rotation, vertex_pos) + pos;
    }

    const vec3 InvertTransform (const vec3& vertex_pos) const
    {
        quat c = conjugate(rotation);
        return rotate(c, vertex_pos - pos);
    }

	UTransform()
	{
		pos = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		rotation = quat(0.0f, 0.0f, 0.0f, 1.0f);
		scale = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	UTransform(vec4 pos, quat rotation, vec4 scale) : pos(pos), rotation(rotation), scale(scale) {}	

};


