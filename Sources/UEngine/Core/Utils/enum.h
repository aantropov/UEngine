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

enum class ULogType : int
{
    Warning = 0,
    Error,
    Info
};

enum class ULogTarget : int
{
    None = 0,
    File = 1,
    MsgBox = 2    
};

enum class UBufferUsage : int
{
    Static = 0x88E4,
    Dynamic = 0x88E8
};

enum class UShaderType : int
{
    Vertex = 0x8B31,
    Fragment = 0x8B30
};

enum class UKeyState : int
{
    Up = 0,
    Down,
    Pressed
};

enum class UTextureFormat : int
{
    RGBA = 0,
    RG32F,
    RGBA_FLOAT,
    R32F,
    Depth32F,
    DepthShadow
};

enum class UTextureFiltration : int
{
    Linear = 0,
    Nearest
};

enum class UTextureWrapMode : int
{
    Repeat = 0,
    Clamp,
    ClampToEdge
};

enum class UResourceType : int
{
    Shader = 0,
    ShaderProgram,
    Texture,
    Cubemap,
    Model,
    PostEffect,
    Animation,
    Material,
    Script
};

enum class URenderPass : int
{
    Forward = 0,
    Deffered,
    Depth,
	DepthShadow,
    Normal
};

enum class UFramebufferAttachment : int
{
    Depth = 0x8D00,
    Color0 = 0x8CE0,
    Color1 = 0x8CE1,
    Color2 = 0x8CE2,
    Color3 = 0x8CE3,
    Color4 = 0x8CE4,
    Color5 = 0x8CE5,
    Color6 = 0x8CE6,
    Color7 = 0x8CE7,
    Color8 = 0x8CE8

};

enum class UPlayState : int
{
    PlayOnce = 0,
    PlayLoop,
    Stop
};

//1,3,5 are used for pack shadow casting info
enum class ULightType : int
{
    Directional = 0,
    Spot = 2,
    Point= 4
};

enum class UBlendMode : int
{
    Opaque = 0,
    Translucent,
    Additive
};