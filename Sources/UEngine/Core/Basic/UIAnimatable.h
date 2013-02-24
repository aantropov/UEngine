#pragma once 
#include "..\math\UMath.h"
#include "..\Utils\enum.h"
#include "..\Resources\UResource.h"
#include <string>
#include <vector>
#include <windows.h>
#include <hash_map>
#include <map>

class UBone
{
public:
	
	UBone(){}
	UBone(quat orient, vec4 pos, vec3 scale, std::string name, unsigned int parent): name(name), orient(orient), pos(pos), parent(parent), scale(scale) {}

	std::string name;

	unsigned int parent;
	quat orient;
	vec3 pos;
	vec3 scale;

	mat4 localMatrix() const
	{	
		mat4 res = mat4_identity;
		res = mat4(orient) /* GLTranslation(pos)*/;

		res.r1[3] = pos.x;
		res.r2[3] = pos.y;
		res.r3[3] = pos.z;
		
		res *= GLScale(scale);
		return res;
	}

	vec3 transform(const vec3& vertex_pos) const
    {
        return rotate(orient, vertex_pos) + pos;
    }

    vec3 invTransform (const vec3& vertex_pos) const
    {
        quat c = conjugate(orient);
        return rotate(c, vertex_pos-pos);
    }

	static UBone Lerp(UBone a, UBone b, float t)
	{
		return UBone(lerp(a.orient, b.orient, t), lerp(a.pos, b.pos, t), lerp(a.scale, b.scale, t), b.name, b.parent);
	}
	const UBone operator*(float f) const { return UBone(orient*f,  pos*f, scale*f, name, parent); }
	const UBone operator+(const UBone &bone) const { return UBone(orient + bone.orient,  pos + bone.pos, scale + bone.scale, name, parent); }
};

class UKeyFrame
{
private:

	void UpdateBone(unsigned int bone, vector<unsigned int> &updated, mat4 *matrixes)
	{	
		
		bool isUpdated = false;
		for(unsigned int i = 0; i < updated.size(); i++)
			if(updated[i] == bones[bone].parent)
			{
				isUpdated = true;
				break;
			}		

		if(bones[bone].parent == -1)
		{			
			updated.push_back(bone);
			matrixes[bone] = bones[bone].localMatrix();
			return;
		}

		if(!isUpdated)
			UpdateBone(bones[bone].parent, updated, matrixes);

		updated.push_back(bone);
		matrixes[bone] = bones[bone].localMatrix() * matrixes[bones[bone].parent]/**/;
	}

public:
	UKeyFrame() {}
	std::vector<UBone> bones;
		
	void UpdateMatrixes(mat4 *matrixes)
	{		
		vector<unsigned int> updated; 
		for(unsigned int i = 0; i < bones.size(); i++)
			UpdateBone(i, updated, matrixes);
	}
	static UKeyFrame Lerp(UKeyFrame a, UKeyFrame b, float t)
	{
		UKeyFrame temp;
		
		for(unsigned int i = 0; i < b.bones.size(); i++)
			temp.bones.push_back(UBone::Lerp(a.bones[i], b.bones[i], t));
		
		return temp;
	}
	const UKeyFrame operator*(float f) {
		for(unsigned int i = 0; i < bones.size(); i++)
			bones[i] = bones[i] * f;
		return *this;
	}
	const UKeyFrame operator+(const UKeyFrame &frame) {		
		for(unsigned int i = 0; i < frame.bones.size(); i++)
			bones[i] =bones[i] + frame.bones[i];
		return *this;
	}
};

class UAnimation : public UResource
{
public:

	UAnimation(){ frameRate = 0.0; startTime = 0.0; lastUpdateTime = 0.0; state = UANIMATION_STOP; }
	
	virtual ~UAnimation(){}
	virtual bool Load(std::string path);
	virtual void Free() {}

	UANIMATION_STATE state;

	double startTime;
	double lastUpdateTime;

	double frameRate;
	std::string name;
	std::vector<UKeyFrame> frames;	
	UKeyFrame currentFrame;

	void StopAnimation()
	{
		state = UANIMATION_STOP;
	}
	void StartAnimation()
	{
		StartAnimation(GetTickCount(),  UANIMATION_PLAY_ONCE);
	}
	void StartAnimation(double startTime, UANIMATION_STATE state)
	{
		this->state = state;
		this->startTime = startTime;
		lastUpdateTime = startTime;
		currentFrame = frames[0];
	}
	void Update(double delta)
	{
		if(state == UANIMATION_STOP || GetTickCount() < startTime)
			return;

		lastUpdateTime  += delta;
		
		unsigned int frame = (unsigned int) round((lastUpdateTime - startTime) / frameRate, 1.0);
		
		float t = (float)(((lastUpdateTime  - startTime) / frameRate) - (double)frame);
		//t = clamp(t, 0.0f, 1.0f);

		float fractpart, intpart;		
		fractpart = modf(t, &intpart);		

		unsigned int nextFrame = (unsigned int)(frame + 1 + intpart);
		if(nextFrame < frames.size())
		{			 
			currentFrame = UKeyFrame::Lerp(frames[nextFrame-1], frames[nextFrame], fractpart);
		}
		else
		{
			if(state == UANIMATION_PLAY_LOOP)
				StartAnimation(GetTickCount(), UANIMATION_PLAY_LOOP);
			else
				state = UANIMATION_STOP;
		}
		
		
	}
	
};

class UIAnimatable
{	
public:
		
	UIAnimatable(){}
	virtual void UpdateAnimation(double delta) = 0;
	virtual ~UIAnimatable(){}
};

class UISkinAnimatable : public UIAnimatable
{
public:
	
	mat4 boneMatrixes[MAX_BONES];	
	std::hash_map<std::string, UAnimation*> animations;

	unsigned int countPlayableAnimations;
	UKeyFrame currentFrame;

	UISkinAnimatable(){ for(int i = 0; i < MAX_BONES; i++) boneMatrixes[i] = mat4_identity; countPlayableAnimations = 0;}
	virtual ~UISkinAnimatable(){}
	
	virtual void UpdateAnimation(double delta)
	{	
		UKeyFrame tempFrame;
		int count = 0;
		for each(auto t in animations)
		{			
			if(t.second->state != UANIMATION_STOP)
			{
				t.second->Update(delta);

				if(tempFrame.bones.size() == 0)
					tempFrame = t.second->currentFrame;
				else
					tempFrame = tempFrame + t.second->currentFrame;

				count++;
			}

		}
		
		countPlayableAnimations = (unsigned int) count;
		if(tempFrame.bones.size() == 0)
			return;
		
		currentFrame = tempFrame * (1.0f / (float)count);
		currentFrame.UpdateMatrixes(boneMatrixes);
	}
};