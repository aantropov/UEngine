#include "frustum.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "mat2.h"
#include "mat3.h"
#include "mat4.h"
#include "bounds.h"

void frustum:: extract(mat4 modl, mat4 proj)
{
    mat4 clip = transpose(modl) * transpose(proj);
    float t;
    
    plane[0][0] = clip.m[ 3] - clip.m[ 0];
    plane[0][1] = clip.m[ 7] - clip.m[ 4];
    plane[0][2] = clip.m[11] - clip.m[ 8];
    plane[0][3] = clip.m[15] - clip.m[12];
 
    t = sqrt( plane[0][0] * plane[0][0] + plane[0][1] * plane[0][1] + plane[0][2] * plane[0][2] );
    plane[0][0] /= t;
    plane[0][1] /= t;
    plane[0][2] /= t;
    plane[0][3] /= t;
 
    plane[1][0] = clip.m[ 3] + clip.m[ 0];
    plane[1][1] = clip.m[ 7] + clip.m[ 4];
    plane[1][2] = clip.m[11] + clip.m[ 8];
    plane[1][3] = clip.m[15] + clip.m[12];
 
    t = sqrt( plane[1][0] * plane[1][0] + plane[1][1] * plane[1][1] + plane[1][2] * plane[1][2] );
    plane[1][0] /= t;
    plane[1][1] /= t;
    plane[1][2] /= t;
    plane[1][3] /= t;
 
    plane[2][0] = clip.m[ 3] + clip.m[ 1];
    plane[2][1] = clip.m[ 7] + clip.m[ 5];
    plane[2][2] = clip.m[11] + clip.m[ 9];
    plane[2][3] = clip.m[15] + clip.m[13];
 
    t = sqrt( plane[2][0] * plane[2][0] + plane[2][1] * plane[2][1] + plane[2][2] * plane[2][2] );
    plane[2][0] /= t;
    plane[2][1] /= t;
    plane[2][2] /= t;
    plane[2][3] /= t;
 
    plane[3][0] = clip.m[ 3] - clip.m[ 1];
    plane[3][1] = clip.m[ 7] - clip.m[ 5];
    plane[3][2] = clip.m[11] - clip.m[ 9];
    plane[3][3] = clip.m[15] - clip.m[13];
 
    t = sqrt( plane[3][0] * plane[3][0] + plane[3][1] * plane[3][1] + plane[3][2] * plane[3][2] );
    plane[3][0] /= t;
    plane[3][1] /= t;
    plane[3][2] /= t;
    plane[3][3] /= t;
 
    plane[4][0] = clip.m[ 3] - clip.m[ 2];
    plane[4][1] = clip.m[ 7] - clip.m[ 6];
    plane[4][2] = clip.m[11] - clip.m[10];
    plane[4][3] = clip.m[15] - clip.m[14];
 
    t = sqrt( plane[4][0] * plane[4][0] + plane[4][1] * plane[4][1] + plane[4][2] * plane[4][2] );
    plane[4][0] /= t;
    plane[4][1] /= t;
    plane[4][2] /= t;
    plane[4][3] /= t;
 
    plane[5][0] = clip.m[ 3] + clip.m[ 2];
    plane[5][1] = clip.m[ 7] + clip.m[ 6];
    plane[5][2] = clip.m[11] + clip.m[10];
    plane[5][3] = clip.m[15] + clip.m[14];
 
    t = sqrt(plane[5][0] * plane[5][0] + plane[5][1] * plane[5][1] + plane[5][2] * plane[5][2] );
    plane[5][0] /= t;
    plane[5][1] /= t;
    plane[5][2] /= t;
    plane[5][3] /= t;
}  

bool IsPointInFrustum(vec3 point, frustum fr)
{
    for(int p = 0; p < 6; p++ )
        if(fr.plane[p][0] * point.x + fr.plane[p][1] * point.y + fr.plane[p][2] * point.z + fr.plane[p][3] <= 0 )
             return false;
   return true;
}

bool IsSphereInFrustum(sphere sph, frustum fr)
{
   for(int p = 0; p < 6; p++ )
      if(fr.plane[p][0] * sph.center.x + fr.plane[p][1] * sph.center.y + fr.plane[p][2] * sph.center.z + fr.plane[p][3] <= -sph.radius )
         return false;
   return true;
}

bool IsCubeInFrustum(vec3 center, float size, frustum fr)
{
   for(int p = 0; p < 6; p++)
   {
      if(fr.plane[p][0] * (center.x - size) + fr.plane[p][1] * (center.y - size) + fr.plane[p][2] * (center.z - size) + fr.plane[p][3] > 0)
         continue;
      if(fr.plane[p][0] * (center.x + size) + fr.plane[p][1] * (center.y - size) + fr.plane[p][2] * (center.z - size) + fr.plane[p][3] > 0)
         continue;
      if(fr.plane[p][0] * (center.x - size) + fr.plane[p][1] * (center.y + size) + fr.plane[p][2] * (center.z - size) + fr.plane[p][3] > 0)
         continue;
      if(fr.plane[p][0] * (center.x + size) + fr.plane[p][1] * (center.y + size) + fr.plane[p][2] * (center.z - size) + fr.plane[p][3] > 0)
         continue;
      if(fr.plane[p][0] * (center.x - size) + fr.plane[p][1] * (center.y - size) + fr.plane[p][2] * (center.z + size) + fr.plane[p][3] > 0)
         continue;
      if(fr.plane[p][0] * (center.x + size) + fr.plane[p][1] * (center.y - size) + fr.plane[p][2] * (center.z + size) + fr.plane[p][3] > 0)
         continue;
      if(fr.plane[p][0] * (center.x - size) + fr.plane[p][1] * (center.y + size) + fr.plane[p][2] * (center.z + size) + fr.plane[p][3] > 0)
         continue;
      if(fr.plane[p][0] * (center.x + size) + fr.plane[p][1] * (center.y + size) + fr.plane[p][2] * (center.z + size) + fr.plane[p][3] > 0)
         continue;
      return false;
   }
   return true;
}