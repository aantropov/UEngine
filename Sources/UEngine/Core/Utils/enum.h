#pragma once

#include <string>
#include <vector>
#include <map>

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

//Thanks to https://habrahabr.ru/post/236403/

#define VA_SIZE(...) VA_SIZE_((VA_SIZE_PREFIX_ ## __VA_ARGS__ ## _VA_SIZE_POSTFIX,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))
#define VA_SIZE_INVOKE(...) INVOKE(VA_SIZE(__VA_ARGS__))
#define VA_SIZE_(__args) VA_GET_SIZE __args

#define VA_SIZE_PREFIX__VA_SIZE_POSTFIX ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,0

#define VA_GET_SIZE(__p0,__p1,__p2,__p3,__p4,__p5,__p6,__p7,__p8,__p9,__p10,__p11,__p12,__p13,__p14,__p15,__p16,__p17,__p18,__p19,__p20,__p21,__p22,__p23,__p24,__p25,__p26,__p27,__p28,__p29,__p30,__p31,__n,...) __n

#define INVOKE( ... ) INVOKE_A( __VA_ARGS__ )
#define INVOKE_A( ... ) __VA_ARGS__

#define VA_FOR(macro,...) INVOKE( CAT(VA_FOR, VA_SIZE_INVOKE(__VA_ARGS__)) ( macro, (__VA_ARGS__) ) )

#define VA_APPLY(x) x
#define VA_FIRST(a, ...) a
#define VA_WO_FIRST(a, ...) __VA_ARGS__

#define VA_FOR0(m,x)
#define VA_FOR1(m,x)  m( VA_APPLY(VA_FIRST x) )
#define VA_FOR2(m,x)  m( VA_APPLY(VA_FIRST x) )  VA_FOR1( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR3(m,x)  m( VA_APPLY(VA_FIRST x) )  VA_FOR2( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR4(m,x)  m( VA_APPLY(VA_FIRST x) )  VA_FOR3( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR5(m,x)  m( VA_APPLY(VA_FIRST x) )  VA_FOR4( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR6(m,x)  m( VA_APPLY(VA_FIRST x) )  VA_FOR5( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR7(m,x)  m( VA_APPLY(VA_FIRST x) )  VA_FOR6( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR8(m,x)  m( VA_APPLY(VA_FIRST x) )  VA_FOR7( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR9(m,x)  m( VA_APPLY(VA_FIRST x) )  VA_FOR8( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR10(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR9( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR11(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR10( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR12(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR11( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR13(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR12( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR14(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR13( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR15(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR14( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR16(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR15( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR17(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR16( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR18(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR17( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR19(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR18( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR20(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR19( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR21(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR20( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR22(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR21( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR23(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR22( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR24(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR23( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR25(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR24( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR26(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR25( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR27(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR26( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR28(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR27( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR29(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR28( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR30(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR29( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR31(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR30( m, (VA_APPLY(VA_WO_FIRST x)))
#define VA_FOR32(m,x) m( VA_APPLY(VA_FIRST x) )  VA_FOR31( m, (VA_APPLY(VA_WO_FIRST x)))

#define CAT(x,y) CAT_A(x, y)
#define CAT_A(x,y) x##y

#define M_STR(A) M_STR_(A)
#define M_STR_(A) #A

#define M_LOWER_STR(A) M_LOWER_STR_(A)
#define M_LOWER_STR_(A) tolower(#A)

#define M_IF(P, T, E) CAT(M_IF_, P)(T, E)
#define M_IF_1(T, E) E
#define M_IF_2(T, E) T
#define M_FIRST(A, ...) A
#define M_SECOND(A, B, ...) B
#define M_ID(...) __VA_ARGS__

#define ENUM_ENAME(A) M_IF(VA_SIZE(M_ID A), M_FIRST A = M_SECOND A, A),
#define ENUM_ELEM(A) M_IF(VA_SIZE(M_ID A), M_FIRST A, A)
#define ENUM_ELEM_TYPE(A) Type::ENUM_ELEM(A)
#define ENUM_ELEM_NAME(A) M_LOWER_STR(ENUM_ELEM(A))
#define ENUM_STRING_TO_TYPE(A) {ENUM_ELEM_NAME(A), ENUM_ELEM_TYPE(A)},
#define ENUM_TYPE_TO_STRING(A) {ENUM_ELEM_TYPE(A), ENUM_ELEM_NAME(A)},
#define ENUM_TYPE(A) ENUM_ELEM_TYPE(A),

#define STRING_ENUM(name, ...) \
enum class name {  VA_FOR(ENUM_ENAME, __VA_ARGS__) };  \
class name##Helper { \
public: \
    typedef name Type; \
    static const std::string &ToString( Type enumVal ) \
    {\
        static const std::map<Type,std::string> enumStringsMap = { VA_FOR(ENUM_TYPE_TO_STRING, __VA_ARGS__) }; \
        auto it = enumStringsMap.find(enumVal);\
        static std::string emptyString;\
        if(it==enumStringsMap.end())\
            return emptyString;\
        return it->second;\
    }\
    static Type FromString(const std::string &value)\
    {\
        static const std::map<std::string,Type> enumStringsMap = { VA_FOR(ENUM_STRING_TO_TYPE, __VA_ARGS__) }; \
        auto it = enumStringsMap.find(value);\
        if(it==enumStringsMap.end())\
            return (Type)0;\
        return it->second;\
    }\
    static const std::vector<Type>& values()\
    {\
        static const std::vector<Type> valueVector = { VA_FOR(ENUM_TYPE, __VA_ARGS__) }; \
        return valueVector;\
    }\
private: \
    inline static char easytolower(char in) \
    { \
        if(in<='Z' && in>='A') \
            return in-('Z'-'z'); \
        return in; \
    }\
    static std::string tolower(std::string &&tolower) \
    { \
        std::string temp = tolower; \
        for (std::string::size_type i=0; i<temp.length(); ++i) \
                temp[i] = easytolower(temp[i]); \
        return temp; \
    } \
};

//Enumerations

STRING_ENUM(ULogType,
    Warning,
    Error,
    Info);

STRING_ENUM(ULogTarget,
    None,
    File,
    MsgBox
    );

STRING_ENUM(UBufferUsage,
    (Static, 0x88E4),
    (Dynamic, 0x88E8)
    );

STRING_ENUM(UShaderType,
    (Vertex, 0x8B31),
    (Fragment, 0x8B30)
    );

STRING_ENUM(UKeyState,
    Up,
    Down,
    Pressed
    );

STRING_ENUM(UTextureFormat,
    RGBA,
    RG32F,
    RGBA_FLOAT,
    R32F,
    Depth32F,
    DepthShadow
    );

STRING_ENUM(UTextureFiltration,
    Linear,
    Nearest
    );

STRING_ENUM(UTextureWrapMode,
    Repeat,
    Clamp,
    ClampToEdge
    );

STRING_ENUM(UResourceType,
    Shader,
    ShaderProgram,
    Texture,
    Cubemap,
    Model,
    PostEffect,
    Animation,
    Material,
    Script
    );

STRING_ENUM(URenderPass,
    Forward,
    Deffered,
    Depth,
    DepthShadow,
    Normal
    );

STRING_ENUM(UFramebufferAttachment,
    (Depth, 0x8D00),
    (Color0, 0x8CE0),
    (Color1, 0x8CE1),
    (Color2, 0x8CE2),
    (Color3, 0x8CE3),
    (Color4, 0x8CE4),
    (Color5, 0x8CE5),
    (Color6, 0x8CE6),
    (Color7, 0x8CE7),
    (Color8, 0x8CE8)
    );

STRING_ENUM(UPlayState,
    PlayOnce,
    PlayLoop,
    Stop
    );

//1,3,5 are used for pack shadow casting info
STRING_ENUM(ULightType,
    (Directional, 0),
    (Spot, 2),
    (Point, 4)
    );

STRING_ENUM(UBlendMode,
    Opaque,
    Translucent,
    Additive
    );