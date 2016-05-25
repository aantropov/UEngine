// enum.h 
#pragma once

#define UE_MAXCHAR     8024
#define MAX_UNIFORM_LOCATIONS 1000
#define MAX_LIGHTS  8
#define SHADOW_TEXTURE_CHANNEL  16
#define MAX_BONES  64

#define USCRIPT_GLOBAL_SCOPE "USCRIPT"
#define USCRIPT_INIT_FUNCTION "Init"
#define USCRIPT_RELEASE_FUNCTION "Release"
#define USCRIPT_UPDATE_FUNCTION "Update"

//Debug or Release
//#define UE_DEBUG
//#define UE_RELEASE

// Enumerations

// Log message type
enum ULOG_MSG_TYPE : int
{
    ULOG_MSG_WARNING = 0,
    ULOG_MSG_ERROR,
    ULOG_MSG_INFO
};

// Log message out
enum ULOG_OUT : int
{
    ULOG_OUT_FILE = 1,
    ULOG_OUT_MSG = 2,
    ULOG_OUT_NONE = 4
};

// Window type (IT IS NOT USED!)
enum UWND_STATE : int
{
    UWND_STATE_WINDOW = 0,
    UWND_STATE_FULLSCREEN
};

// VBO
enum UVBO_DRAW : int
{
    UVBO_STATIC = 0x88E4,
    UVBO_DYNAMIC = 0x88E8
};

// Shader
enum USHADER_TYPE : int
{
    USHADER_VERTEX = 0x8B31,
    USHADER_FRAGMENT = 0x8B30
};

//Input
enum UINPUT_KEY_STATE : int
{
    UINPUT_UP = 0,
    UINPUT_DOWN,
    UINPUT_PRESSED
};

//Texture
enum UTEXTURE_TYPE : int
{
    UTEXTURE_COLOR = 0,
    UTEXTURE_ARRAY,
    UTEXTURE_RG32F,
    UTEXTURE_FLOAT,
    UTEXTURE_FLOAT16,
    UTEXTURE_FLOAT32,
    UTEXTURE_DEPTH,
    UTEXTURE_DEPTH_SHADOW
};

enum UTEXTURE_FILTER : int
{
    UTEXTURE_FILTER_LINEAR = 0,
    UTEXTURE_FILTER_NEAREST
};

enum UTEXTURE_WRAP : int
{
    UTEXTURE_WRAP_REPEAT = 0,
    UTEXTURE_WRAP_CLAMP,
    UTEXTURE_WRAP_CLAMP_TO_EDGE
};

//Resource
enum URESOURCE_TYPE : int
{
    URESOURCE_SHADER = 0,
    URESOURCE_SHADER_PROGRAM,
    URESOURCE_TEXTURE,
    URESOURCE_CUBEMAP,
    URESOURCE_MODEL,
    URESOURCE_POST_EFFECT,
    URESOURCE_SKIN_ANIMATION,
    URESOURCE_MATERIAL,
    URESOURCE_SCRIPT
};

enum URENDER_TYPE : int
{
    URENDER_FORWARD = 0,
    URENDER_DEFFERED,
    URENDER_DEPTH,
    URENDER_NORMAL
};

enum UFRAMEBUFFER_ATTACHMENT : int
{
    UFB_ATTACHMENT_DEPTH = 0x8D00,
    UFB_ATTACHMENT_COLOR0 = 0x8CE0,
    UFB_ATTACHMENT_COLOR1 = 0x8CE1,
    UFB_ATTACHMENT_COLOR2 = 0x8CE2,
    UFB_ATTACHMENT_COLOR3 = 0x8CE3,
    UFB_ATTACHMENT_COLOR4 = 0x8CE4,
    UFB_ATTACHMENT_COLOR5 = 0x8CE5,
    UFB_ATTACHMENT_COLOR6 = 0x8CE6
};

//Animation
enum UANIMATION_STATE : int
{
    UANIMATION_PLAY_ONCE,
    UANIMATION_PLAY_LOOP,
    UANIMATION_STOP
};

enum ULIGHT_TYPE : int
{
    ULIGHT_TYPE_DIRECTIONAL = 0,
    //ULIGHT_TYPE_DIRECTIONAL_SHADOW_CASTER,
    ULIGHT_TYPE_SPOT = 2,
    //ULIGHT_TYPE_SPOT_SHADOW_CASTER,
    ULIGHT_TYPE_POINT= 4
    //ULIGHT_TYPE_POINT_SHADOW_CASTER
};