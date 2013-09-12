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

    std::string name;
    unsigned int parent;
    ::transform tf;
    
    UBone(){}
    UBone(::transform tf, std::string name, unsigned int parent): name(name), tf(tf), parent(parent) {}
    
    static UBone Lerp(UBone a, UBone b, float t)
    {
        return UBone(lerp(a.tf, b.tf, t), b.name, b.parent);
    }

    const UBone operator* (float f) const { return UBone(::transform(tf.position * f, tf.rotation * f, tf.scale * f), name, parent); }
    const UBone operator+ (const UBone &bone) const { return UBone(::transform(tf.position + bone.tf.position, tf.rotation + bone.tf.rotation, tf.scale + bone.tf.scale), name, parent); }
};

class UKeyFrame
{
private:
    
    void UpdateBone(unsigned int bone, vector<unsigned int> &updated, mat4 *matrixes)
    {
        bool isUpdated = false;
        for(unsigned int i = 0; i < updated.size(); i++)
        {
            if(updated[i] == bones[bone].parent)
            {
                isUpdated = true;
                break;
            }
        }

        if(bones[bone].parent == -1)
        {
            updated.push_back(bone);
            matrixes[bone] = bones[bone].tf.matrix();
            return;
        }

        if(!isUpdated)
        {
            UpdateBone(bones[bone].parent, updated, matrixes);
        }

        updated.push_back(bone);
        matrixes[bone] = bones[bone].tf.matrix() /* matrixes[bones[bone].parent]/**/;
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

    const UKeyFrame operator*(float f)
    {
        for(unsigned int i = 0; i < bones.size(); i++)
            bones[i] = bones[i] * f;
        return *this;
    }

    const UKeyFrame operator+(const UKeyFrame &frame)
    {
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