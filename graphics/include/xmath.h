#ifndef _X_MATH_H_
#define _X_MATH_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef _MSC_VER
#define MSVC_ALIGNED(x) __declspec(align(x))
#define GCC_ALIGNED(x)
#else 
#ifdef __GNUC__ 
#define MSVC_ALIGNED(x)
#define GCC_ALIGNED(x) __attribute__((aligned(x)))
#endif
#endif

typedef struct __vector4
{
    union
    {
        float               vector4_f32[4];
        uint32_t            vector4_u32[4];
    };
} XVECTOR;

typedef MSVC_ALIGNED(16) struct _XVECTORU32
{
    union
    {
        uint32_t    u[4];
        XVECTOR     v;
    };
} XVECTORU32 GCC_ALIGNED(16);

typedef MSVC_ALIGNED(16) struct _XMATRIX
{
    union
    {
        XVECTOR r[4];
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} XMATRIX GCC_ALIGNED(16);

typedef MSVC_ALIGNED(16) struct _XVECTORF32
{
    union
    {
        float f[4];
        XVECTOR v;
    };
} XVECTORF32 GCC_ALIGNED(16);

typedef struct _XFLOAT4X4
{
    union
    {
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} XFLOAT4X4;

typedef struct _XFLOAT2
{
    float x;
    float y;
} XFLOAT2;

typedef struct _XFLOAT3
{
    float x;
    float y;
    float z;
} XFLOAT3;

typedef struct _XFLOAT4
{
    float x;
    float y;
    float z;
    float w;
} XFLOAT4;

#define X_PI      3.141592654f
#define X_2PI     6.283185307f
#define X_1DIVPI  0.318309886f
#define X_1DIV2PI 0.159154943f
#define X_PIDIV2  1.570796327f
#define X_PIDIV4  0.785398163f

#define XMISINF(x)  ((*(const uint32_t*)&(x) & 0x7FFFFFFF) == 0x7F800000)

#define XM_PERMUTE_0X (uint32_t)0
#define XM_PERMUTE_0Y (uint32_t)1
#define XM_PERMUTE_0Z (uint32_t)2
#define XM_PERMUTE_0W (uint32_t)3
#define XM_PERMUTE_1X (uint32_t)4
#define XM_PERMUTE_1Y (uint32_t)5
#define XM_PERMUTE_1Z (uint32_t)6
#define XM_PERMUTE_1W (uint32_t)7

#define XM_SWIZZLE_X (uint32_t)0
#define XM_SWIZZLE_Y (uint32_t)1
#define XM_SWIZZLE_Z (uint32_t)2
#define XM_SWIZZLE_W (uint32_t)3

#define g_UnitVectorEpsilon (XVECTOR){ { { 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f } } }
#define g_UnitQuaternionEpsilon (XVECTOR){ { { 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f } } }
#define g_UnitPlaneEpsilon (XVECTOR){ { { 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f } } }

#define g_RayEpsilon (XVECTOR){ { { 1e-20f, 1e-20f, 1e-20f, 1e-20f } } }
#define g_RayNegEpsilon (XVECTOR){ { { -1e-20f, -1e-20f, -1e-20f, -1e-20f } } }
#define g_FltMin (XVECTOR){ { { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX } } }
#define g_FltMax (XVECTOR){ { { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX } } }

#define X_SWIZZLE_X (uint32_t)0
#define X_SWIZZLE_Y (uint32_t)1
#define X_SWIZZLE_Z (uint32_t)2
#define X_SWIZZLE_W (uint32_t)3

#define X_CRMASK_CR6 (uint32_t)0x000000F0
#define X_CRMASK_CR6TRUE (uint32_t)0x00000080
#define X_CRMASK_CR6FALSE (uint32_t)0x00000020
#define X_CRMASK_CR6BOUNDS (uint32_t)0x00000020

#define g_XNegativeOne (XVECTORF32){ { { -1.0f, -1.0f, -1.0f, -1.0f } } }
#define g_XOne (XVECTORF32){ { { 1.0f, 1.0f, 1.0f, 1.0f } } }

#define g_XMIdentityR0 (XVECTORF32){ { { 1.0f, 0.0f, 0.0f, 0.0f } } }
#define g_XMIdentityR1 (XVECTORF32){ { { 0.0f, 1.0f, 0.0f, 0.0f } } }
#define g_XMIdentityR2 (XVECTORF32){ { { 0.0f, 0.0f, 1.0f, 0.0f } } }
#define g_XMIdentityR3 (XVECTORF32){ { { 0.0f, 0.0f, 0.0f, 1.0f } } }


inline float XConvertToRadians(float fDegrees) { return fDegrees * (X_PI / 180.0f); }
inline float XConvertToDegrees(float fRadians) { return fRadians * (180.0f / X_PI); }

XVECTOR XLoadFloat3(const XFLOAT3* pSource);
void XStoreFloat3(XFLOAT3* pDestination,XVECTOR V);
XVECTOR XVectorSet(float x, float y, float z, float w);
XVECTOR XVectorZero();
XVECTOR XVectorSubtract(XVECTOR V1, XVECTOR V2);
XVECTOR XVector3Dot(XVECTOR V1, XVECTOR V2);
XVECTOR XVector3Cross(XVECTOR V1, XVECTOR V2);
XVECTOR XVector3LengthSq(XVECTOR V);
XVECTOR XVectorSqrt(XVECTOR V);
XVECTOR XVector3Length(XVECTOR V);
XVECTOR XVector3Normalize(XVECTOR V);
XVECTOR XVectorNegate(XVECTOR V);
bool XVector3Equal(XVECTOR V1, XVECTOR V2);
bool XVector3IsInfinite(XVECTOR V);
XVECTOR XVectorSelect(XVECTOR V1, XVECTOR V2, XVECTOR Control);
XVECTOR XMVectorMergeXY(XVECTOR V1, XVECTOR V2);
XVECTOR XMVectorMergeZW(XVECTOR V1, XVECTOR V2);
XMATRIX XMatrixTranspose(XMATRIX M);
XMATRIX XMatrixLookToLH(XVECTOR EyePosition, XVECTOR EyeDirection, XVECTOR UpDirection);
XMATRIX XMMatrixLookAtLH(XVECTOR EyePosition, XVECTOR FocusPosition, XVECTOR UpDirection);
void XStoreFloat4x4(XFLOAT4X4* pDestination, XMATRIX M);
bool XMScalarNearEqual(float S1, float S2, float Epsilon);
void XMScalarSinCos(float* pSin, float* pCos, float Value);
XMATRIX XMatrixPerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ);
XMATRIX XLoadFloat4x4(const XFLOAT4X4* pSource);
XMATRIX XMatrixMultiply(XMATRIX M1,XMATRIX M2);
XVECTOR XVector4Dot(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorMultiply(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorNegativeMultiplySubtract(XVECTOR V1,XVECTOR V2,XVECTOR V3);
XVECTOR XVectorMultiplyAdd(XVECTOR V1,XVECTOR V2,XVECTOR V3);
XVECTOR XVectorSwizzle(XVECTOR V,uint32_t E0,uint32_t E1,uint32_t E2,uint32_t E3);
XVECTOR XMatrixDeterminant(XMATRIX M);
XVECTOR XVectorPermute(XVECTOR V1,XVECTOR V2,uint32_t PermuteX,uint32_t PermuteY,
                       uint32_t PermuteZ,uint32_t PermuteW);
XVECTOR XVectorReciprocal(XVECTOR V);
XMATRIX XMatrixInverse(XVECTOR* pDeterminant,XMATRIX  M);
XMATRIX XMatrixRotationX(float Angle);
XMATRIX XMatrixRotationY(float Angle);
XMATRIX XMatrixRotationZ(float Angle);
XMATRIX XMatrixScaling(float ScaleX,float ScaleY,float ScaleZ);
XMATRIX XMatrixTranslation(float OffsetX,float OffsetY,float OffsetZ);
XVECTOR XVectorMin(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorMax(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorScale(XVECTOR V,float    ScaleFactor);
XVECTOR XVectorAdd(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorSplatX(XVECTOR V);
XVECTOR XVectorSplatY(XVECTOR V);
XVECTOR XVectorSplatZ(XVECTOR V);
XVECTOR XVectorSplatW(XVECTOR V);
XVECTOR XVectorDivide(XVECTOR V1,XVECTOR V2);
XVECTOR XVector3TransformCoord(XVECTOR V,XMATRIX M);
XVECTOR XVector3TransformNormal(XVECTOR V,XMATRIX M);
XVECTOR XVectorSplatOne();
XVECTOR XVectorLessOrEqual(XVECTOR V1,XVECTOR V2);
bool XVector4Less(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorAbs(XVECTOR V);
bool XVector3IsUnit(XVECTOR V);
XVECTOR XVectorGreater(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorOrInt(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorLess(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorInBounds(XVECTOR V,XVECTOR Bounds);
XVECTOR XVectorAndCInt(XVECTOR V1,XVECTOR V2);
uint32_t XVector4EqualIntR(XVECTOR V1,XVECTOR V2);
XVECTOR XVectorTrueInt();
bool XVector3AnyTrue(XVECTOR V);
float XVectorGetX(XVECTOR V);
float XVectorGetY(XVECTOR V);
float XVectorGetZ(XVECTOR V);
void XStoreFloat(float* pDestination,XVECTOR V);
XVECTOR XVectorClamp(XVECTOR V,XVECTOR Min,XVECTOR Max);
XVECTOR XVectorACos(XVECTOR V);
XVECTOR XVector3AngleBetweenNormals(XVECTOR N1,XVECTOR N2);
bool XVector4NearEqual(XVECTOR V1,XVECTOR V2,XVECTOR Epsilon);
XVECTOR XVector3Transform(XVECTOR V,XMATRIX M);
XFLOAT4X4 Identity4x4();
XMATRIX XMatrixIdentity();

#endif /* _X_MATH_H_ */
