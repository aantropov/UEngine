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
#define UE_DEBUG
//#define UE_RELEASE

// Enumerations

// Log message type
enum ULOG_MSG_TYPE {
	ULOG_MSG_WARNING = 0,
	ULOG_MSG_ERROR,
	ULOG_MSG_INFO

};

// Log message out
enum ULOG_OUT {
	ULOG_OUT_FILE = 0,
	ULOG_OUT_MSG,
	ULOG_OUT_NONE

};

// Window type (IT IS NOT USED!)
enum UWND_STATE {
	UWND_STATE_WINDOW = 0,
	UWND_STATE_FULLSCREEN
};

// VBO
enum UVBO_DRAW {	
	UVBO_STATIC = 0x88E4,
	UVBO_DYNAMIC = 0x88E8
};


// Shader
enum USHADER_TYPE {
	USHADER_VERTEX = 0x8B31,
	USHADER_PIXEL = 0x8B30
};

//Input
enum UINPUT_KEY_STATE {
	UINPUT_UP = 0,
	UINPUT_DOWN,
	UINPUT_PRESSED
};

//Texture
enum UTEXTURE_TYPE {
	UTEXTURE_COLOR = 0,
	UTEXTURE_FLOAT,
	UTEXTURE_DEPTH,
	UTEXTURE_DEPTH_SHADOW
};

//Resource
enum URESOURCE_TYPE {
	URESOURCE_SHADER = 0,
	URESOURCE_SHADER_PROGRAM,
	URESOURCE_TEXTURE,
	URESOURCE_MODEL,
	URESOURCE_POST_EFFECT,
	URESOURCE_SKIN_ANIMATION,
	URESOURCE_MATERIAL,
	URESOURCE_SCRIPT
};

enum URENDER_TYPE {
	URENDER_FORWARD = 0,
	URENDER_DEFFERED,
	URENDER_DEPTH,
	URENDER_NORMAL
};

enum UFRAMEBUFFER_ATTACHMENT {
	UFB_ATTACHMENT_DEPTH = 0x8D00,
	UFB_ATTACHMENT_COLOR0 = 0x8CE0,
	UFB_ATTACHMENT_COLOR1 = 0x8CE1,
	UFB_ATTACHMENT_COLOR2 = 0x8CE2,
	UFB_ATTACHMENT_COLOR3 = 0x8CE3,
	UFB_ATTACHMENT_COLOR4 = 0x8CE4,
	UFB_ATTACHMENT_COLOR5 = 0x8CE5
};

//Animation
enum UANIMATION_STATE{
	UANIMATION_PLAY_ONCE,
	UANIMATION_PLAY_LOOP,
	UANIMATION_STOP
};