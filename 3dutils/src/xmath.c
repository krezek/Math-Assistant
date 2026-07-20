#include "pch.h"
#include "platform.h"

#include <xmath.h>

XVECTOR XLoadFloat3(const XFLOAT3* pSource)
{
    assert(pSource);

    XVECTOR V;
    V.vector4_f32[0] = pSource->x;
    V.vector4_f32[1] = pSource->y;
    V.vector4_f32[2] = pSource->z;
    V.vector4_f32[3] = 0.f;
    return V;
}

void XStoreFloat3
        (
                XFLOAT3* pDestination,
                XVECTOR V
        )
{
    assert(pDestination);

    pDestination->x = V.vector4_f32[0];
    pDestination->y = V.vector4_f32[1];
    pDestination->z = V.vector4_f32[2];
}

XVECTOR XVectorSet(float x, float y, float z, float w)
{
    XVECTORF32 vResult = { { { x, y, z, w } } };
    return vResult.v;
}

XVECTOR XVectorZero()
{
    XVECTORF32 vResult = { { { 0.0f, 0.0f, 0.0f, 0.0f } } };
    return vResult.v;
}

XVECTOR XVectorSubtract(XVECTOR V1, XVECTOR V2)
{
    XVECTORF32 Result = { { {
                                    V1.vector4_f32[0] - V2.vector4_f32[0],
                                    V1.vector4_f32[1] - V2.vector4_f32[1],
                                    V1.vector4_f32[2] - V2.vector4_f32[2],
                                    V1.vector4_f32[3] - V2.vector4_f32[3]
                            } } };
    return Result.v;
}

XVECTOR XVector3Dot(XVECTOR V1, XVECTOR V2)
{
    float fValue = V1.vector4_f32[0] * V2.vector4_f32[0] + V1.vector4_f32[1] * V2.vector4_f32[1] + V1.vector4_f32[2] * V2.vector4_f32[2];
    XVECTORF32 vResult;
    vResult.f[0] =
    vResult.f[1] =
    vResult.f[2] =
    vResult.f[3] = fValue;
    return vResult.v;
}

XVECTOR XVector3Cross(XVECTOR V1, XVECTOR V2)
{
    // [ V1.y*V2.z - V1.z*V2.y, V1.z*V2.x - V1.x*V2.z, V1.x*V2.y - V1.y*V2.x ]

    XVECTORF32 vResult = { { {
                                     (V1.vector4_f32[1] * V2.vector4_f32[2]) - (V1.vector4_f32[2] * V2.vector4_f32[1]),
                                     (V1.vector4_f32[2] * V2.vector4_f32[0]) - (V1.vector4_f32[0] * V2.vector4_f32[2]),
                                     (V1.vector4_f32[0] * V2.vector4_f32[1]) - (V1.vector4_f32[1] * V2.vector4_f32[0]),
                                     0.0f
                             } } };
    return vResult.v;
}

XVECTOR XVector3LengthSq(XVECTOR V)
{
    return XVector3Dot(V, V);
}

XVECTOR XVectorSqrt(XVECTOR V)
{
    XVECTORF32 Result = { { {
                                    sqrtf(V.vector4_f32[0]),
                                    sqrtf(V.vector4_f32[1]),
                                    sqrtf(V.vector4_f32[2]),
                                    sqrtf(V.vector4_f32[3])
                            } } };
    return Result.v;
}

XVECTOR XVector3Length(XVECTOR V)
{
    XVECTOR Result;

    Result = XVector3LengthSq(V);
    Result = XVectorSqrt(Result);

    return Result;
}

XVECTOR XVector3Normalize(XVECTOR V)
{
    float fLength;
    XVECTOR vResult;

    vResult = XVector3Length(V);
    fLength = vResult.vector4_f32[0];

    // Prevent divide by zero
    if (fLength > 0)
    {
        fLength = 1.0f / fLength;
    }

    vResult.vector4_f32[0] = V.vector4_f32[0] * fLength;
    vResult.vector4_f32[1] = V.vector4_f32[1] * fLength;
    vResult.vector4_f32[2] = V.vector4_f32[2] * fLength;
    vResult.vector4_f32[3] = V.vector4_f32[3] * fLength;
    return vResult;
}

XVECTOR XVectorNegate(XVECTOR V)
{
    XVECTORF32 Result = { { {
                                    -V.vector4_f32[0],
                                    -V.vector4_f32[1],
                                    -V.vector4_f32[2],
                                    -V.vector4_f32[3]
                            } } };
    return Result.v;
}

bool XVector3Equal(XVECTOR V1, XVECTOR V2)
{
    return (((V1.vector4_f32[0] == V2.vector4_f32[0]) && (V1.vector4_f32[1] == V2.vector4_f32[1]) && (V1.vector4_f32[2] == V2.vector4_f32[2])) != 0);
}

bool XVector3IsInfinite(XVECTOR V)
{
    return (XMISINF(V.vector4_f32[0]) ||
            XMISINF(V.vector4_f32[1]) ||
            XMISINF(V.vector4_f32[2]));
}

XVECTOR XVectorSelect(XVECTOR V1, XVECTOR V2, XVECTOR Control)
{
    XVECTORU32 Result = { { {
                                    (V1.vector4_u32[0] & ~Control.vector4_u32[0]) | (V2.vector4_u32[0] & Control.vector4_u32[0]),
                                    (V1.vector4_u32[1] & ~Control.vector4_u32[1]) | (V2.vector4_u32[1] & Control.vector4_u32[1]),
                                    (V1.vector4_u32[2] & ~Control.vector4_u32[2]) | (V2.vector4_u32[2] & Control.vector4_u32[2]),
                                    (V1.vector4_u32[3] & ~Control.vector4_u32[3]) | (V2.vector4_u32[3] & Control.vector4_u32[3]),
                            } } };
    return Result.v;
}

XVECTOR XMVectorMergeXY(XVECTOR V1, XVECTOR V2)
{
    XVECTORU32 Result = { { {
                                    V1.vector4_u32[0],
                                    V2.vector4_u32[0],
                                    V1.vector4_u32[1],
                                    V2.vector4_u32[1],
                            } } };
    return Result.v;
}

XVECTOR XMVectorMergeZW(XVECTOR V1, XVECTOR V2)
{
    XVECTORU32 Result = { { {
                                    V1.vector4_u32[2],
                                    V2.vector4_u32[2],
                                    V1.vector4_u32[3],
                                    V2.vector4_u32[3]
                            } } };
    return Result.v;
}

XMATRIX XMatrixTranspose(XMATRIX M)
{
    // Original matrix:
    //
    //     m00m01m02m03
    //     m10m11m12m13
    //     m20m21m22m23
    //     m30m31m32m33

    XMATRIX P;
    P.r[0] = XMVectorMergeXY(M.r[0], M.r[2]); // m00m20m01m21
    P.r[1] = XMVectorMergeXY(M.r[1], M.r[3]); // m10m30m11m31
    P.r[2] = XMVectorMergeZW(M.r[0], M.r[2]); // m02m22m03m23
    P.r[3] = XMVectorMergeZW(M.r[1], M.r[3]); // m12m32m13m33

    XMATRIX MT;
    MT.r[0] = XMVectorMergeXY(P.r[0], P.r[1]); // m00m10m20m30
    MT.r[1] = XMVectorMergeZW(P.r[0], P.r[1]); // m01m11m21m31
    MT.r[2] = XMVectorMergeXY(P.r[2], P.r[3]); // m02m12m22m32
    MT.r[3] = XMVectorMergeZW(P.r[2], P.r[3]); // m03m13m23m33
    return MT;
}

static XVECTORU32 g_XSelect1110 = { { { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 } } };
static XVECTORF32 g_XIdentityR3 = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

XMATRIX XMatrixLookToLH(XVECTOR EyePosition, XVECTOR EyeDirection, XVECTOR UpDirection)
{
    assert(!XVector3Equal(EyeDirection, XVectorZero()));
    assert(!XVector3IsInfinite(EyeDirection));
    assert(!XVector3Equal(UpDirection, XVectorZero()));
    assert(!XVector3IsInfinite(UpDirection));

    XVECTOR R2 = XVector3Normalize(EyeDirection);

    XVECTOR R0 = XVector3Cross(UpDirection, R2);
    R0 = XVector3Normalize(R0);

    XVECTOR R1 = XVector3Cross(R2, R0);

    XVECTOR NegEyePosition = XVectorNegate(EyePosition);

    XVECTOR D0 = XVector3Dot(R0, NegEyePosition);
    XVECTOR D1 = XVector3Dot(R1, NegEyePosition);
    XVECTOR D2 = XVector3Dot(R2, NegEyePosition);

    XMATRIX M;
    M.r[0] = XVectorSelect(D0, R0, g_XSelect1110.v);
    M.r[1] = XVectorSelect(D1, R1, g_XSelect1110.v);
    M.r[2] = XVectorSelect(D2, R2, g_XSelect1110.v);
    M.r[3] = g_XIdentityR3.v;

    M = XMatrixTranspose(M);

    return M;
}

XMATRIX XMMatrixLookAtLH(XVECTOR EyePosition, XVECTOR FocusPosition, XVECTOR UpDirection)
{
    XVECTOR EyeDirection = XVectorSubtract(FocusPosition, EyePosition);
    return XMatrixLookToLH(EyePosition, EyeDirection, UpDirection);
}

void XStoreFloat4x4(XFLOAT4X4* pDestination, XMATRIX M)
{
    assert(pDestination);

    pDestination->m[0][0] = M.r[0].vector4_f32[0];
    pDestination->m[0][1] = M.r[0].vector4_f32[1];
    pDestination->m[0][2] = M.r[0].vector4_f32[2];
    pDestination->m[0][3] = M.r[0].vector4_f32[3];

    pDestination->m[1][0] = M.r[1].vector4_f32[0];
    pDestination->m[1][1] = M.r[1].vector4_f32[1];
    pDestination->m[1][2] = M.r[1].vector4_f32[2];
    pDestination->m[1][3] = M.r[1].vector4_f32[3];

    pDestination->m[2][0] = M.r[2].vector4_f32[0];
    pDestination->m[2][1] = M.r[2].vector4_f32[1];
    pDestination->m[2][2] = M.r[2].vector4_f32[2];
    pDestination->m[2][3] = M.r[2].vector4_f32[3];

    pDestination->m[3][0] = M.r[3].vector4_f32[0];
    pDestination->m[3][1] = M.r[3].vector4_f32[1];
    pDestination->m[3][2] = M.r[3].vector4_f32[2];
    pDestination->m[3][3] = M.r[3].vector4_f32[3];
};

bool XMScalarNearEqual(float S1, float S2, float Epsilon)
{
    float Delta = S1 - S2;
    return (fabsf(Delta) <= Epsilon);
}

void XMScalarSinCos(float* pSin, float* pCos, float Value)
{
    assert(pSin);
    assert(pCos);

    // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
    float quotient = X_1DIV2PI * Value;
    if (Value >= 0.0f)
    {
        quotient = (float)((int)(quotient + 0.5f));
    }
    else
    {
        quotient = (float)((int)(quotient - 0.5f));
    }
    float y = Value - X_2PI * quotient;

    // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
    float sign;
    if (y > X_PIDIV2)
    {
        y = X_PI - y;
        sign = -1.0f;
    }
    else if (y < -X_PIDIV2)
    {
        y = -X_PI - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    // 11-degree minimax approximation
    *pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

    // 10-degree minimax approximation
    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    *pCos = sign * p;
}

XMATRIX XMatrixPerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
    assert(NearZ > 0.f && FarZ > 0.f);
    assert(!XMScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
    assert(!XMScalarNearEqual(AspectRatio, 0.0f, 0.00001f));
    assert(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    float    SinFov;
    float    CosFov;
    XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

    float Height = CosFov / SinFov;
    float Width = Height / AspectRatio;
    float fRange = FarZ / (FarZ - NearZ);

    XMATRIX M;
    M.m[0][0] = Width;
    M.m[0][1] = 0.0f;
    M.m[0][2] = 0.0f;
    M.m[0][3] = 0.0f;

    M.m[1][0] = 0.0f;
    M.m[1][1] = Height;
    M.m[1][2] = 0.0f;
    M.m[1][3] = 0.0f;

    M.m[2][0] = 0.0f;
    M.m[2][1] = 0.0f;
    M.m[2][2] = fRange;
    M.m[2][3] = 1.0f;

    M.m[3][0] = 0.0f;
    M.m[3][1] = 0.0f;
    M.m[3][2] = -fRange * NearZ;
    M.m[3][3] = 0.0f;
    return M;
}

XMATRIX XLoadFloat4x4(const XFLOAT4X4* pSource)
{
    assert(pSource);

    XMATRIX M;
    M.r[0].vector4_f32[0] = pSource->m[0][0];
    M.r[0].vector4_f32[1] = pSource->m[0][1];
    M.r[0].vector4_f32[2] = pSource->m[0][2];
    M.r[0].vector4_f32[3] = pSource->m[0][3];

    M.r[1].vector4_f32[0] = pSource->m[1][0];
    M.r[1].vector4_f32[1] = pSource->m[1][1];
    M.r[1].vector4_f32[2] = pSource->m[1][2];
    M.r[1].vector4_f32[3] = pSource->m[1][3];

    M.r[2].vector4_f32[0] = pSource->m[2][0];
    M.r[2].vector4_f32[1] = pSource->m[2][1];
    M.r[2].vector4_f32[2] = pSource->m[2][2];
    M.r[2].vector4_f32[3] = pSource->m[2][3];

    M.r[3].vector4_f32[0] = pSource->m[3][0];
    M.r[3].vector4_f32[1] = pSource->m[3][1];
    M.r[3].vector4_f32[2] = pSource->m[3][2];
    M.r[3].vector4_f32[3] = pSource->m[3][3];
    return M;
}

XMATRIX XMatrixMultiply(XMATRIX M1,XMATRIX M2)
{
    XMATRIX mResult;
    // Cache the invariants in registers
    float x = M1.m[0][0];
    float y = M1.m[0][1];
    float z = M1.m[0][2];
    float w = M1.m[0][3];
    // Perform the operation on the first row
    mResult.m[0][0] = (M2.m[0][0] * x) + (M2.m[1][0] * y) + (M2.m[2][0] * z) + (M2.m[3][0] * w);
    mResult.m[0][1] = (M2.m[0][1] * x) + (M2.m[1][1] * y) + (M2.m[2][1] * z) + (M2.m[3][1] * w);
    mResult.m[0][2] = (M2.m[0][2] * x) + (M2.m[1][2] * y) + (M2.m[2][2] * z) + (M2.m[3][2] * w);
    mResult.m[0][3] = (M2.m[0][3] * x) + (M2.m[1][3] * y) + (M2.m[2][3] * z) + (M2.m[3][3] * w);
    // Repeat for all the other rows
    x = M1.m[1][0];
    y = M1.m[1][1];
    z = M1.m[1][2];
    w = M1.m[1][3];
    mResult.m[1][0] = (M2.m[0][0] * x) + (M2.m[1][0] * y) + (M2.m[2][0] * z) + (M2.m[3][0] * w);
    mResult.m[1][1] = (M2.m[0][1] * x) + (M2.m[1][1] * y) + (M2.m[2][1] * z) + (M2.m[3][1] * w);
    mResult.m[1][2] = (M2.m[0][2] * x) + (M2.m[1][2] * y) + (M2.m[2][2] * z) + (M2.m[3][2] * w);
    mResult.m[1][3] = (M2.m[0][3] * x) + (M2.m[1][3] * y) + (M2.m[2][3] * z) + (M2.m[3][3] * w);
    x = M1.m[2][0];
    y = M1.m[2][1];
    z = M1.m[2][2];
    w = M1.m[2][3];
    mResult.m[2][0] = (M2.m[0][0] * x) + (M2.m[1][0] * y) + (M2.m[2][0] * z) + (M2.m[3][0] * w);
    mResult.m[2][1] = (M2.m[0][1] * x) + (M2.m[1][1] * y) + (M2.m[2][1] * z) + (M2.m[3][1] * w);
    mResult.m[2][2] = (M2.m[0][2] * x) + (M2.m[1][2] * y) + (M2.m[2][2] * z) + (M2.m[3][2] * w);
    mResult.m[2][3] = (M2.m[0][3] * x) + (M2.m[1][3] * y) + (M2.m[2][3] * z) + (M2.m[3][3] * w);
    x = M1.m[3][0];
    y = M1.m[3][1];
    z = M1.m[3][2];
    w = M1.m[3][3];
    mResult.m[3][0] = (M2.m[0][0] * x) + (M2.m[1][0] * y) + (M2.m[2][0] * z) + (M2.m[3][0] * w);
    mResult.m[3][1] = (M2.m[0][1] * x) + (M2.m[1][1] * y) + (M2.m[2][1] * z) + (M2.m[3][1] * w);
    mResult.m[3][2] = (M2.m[0][2] * x) + (M2.m[1][2] * y) + (M2.m[2][2] * z) + (M2.m[3][2] * w);
    mResult.m[3][3] = (M2.m[0][3] * x) + (M2.m[1][3] * y) + (M2.m[2][3] * z) + (M2.m[3][3] * w);
    return mResult;
}

XVECTOR XVector4Dot
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORF32 Result;
    Result.f[0] =
    Result.f[1] =
    Result.f[2] =
    Result.f[3] = V1.vector4_f32[0] * V2.vector4_f32[0] + V1.vector4_f32[1] * V2.vector4_f32[1] + V1.vector4_f32[2] * V2.vector4_f32[2] + V1.vector4_f32[3] * V2.vector4_f32[3];
    return Result.v;
}

XVECTOR XVectorMultiply
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORF32 Result = { { {
                                    V1.vector4_f32[0] * V2.vector4_f32[0],
                                    V1.vector4_f32[1] * V2.vector4_f32[1],
                                    V1.vector4_f32[2] * V2.vector4_f32[2],
                                    V1.vector4_f32[3] * V2.vector4_f32[3]
                            } } };
    return Result.v;
}

XVECTOR XVectorNegativeMultiplySubtract
        (
                XVECTOR V1,
                XVECTOR V2,
                XVECTOR V3
        )
{
    XVECTORF32 Result = { { {
                                    V3.vector4_f32[0] - (V1.vector4_f32[0] * V2.vector4_f32[0]),
                                    V3.vector4_f32[1] - (V1.vector4_f32[1] * V2.vector4_f32[1]),
                                    V3.vector4_f32[2] - (V1.vector4_f32[2] * V2.vector4_f32[2]),
                                    V3.vector4_f32[3] - (V1.vector4_f32[3] * V2.vector4_f32[3])
                            } } };
    return Result.v;
}

XVECTOR XVectorMultiplyAdd
        (
                XVECTOR V1,
                XVECTOR V2,
                XVECTOR V3
        )
{
    XVECTORF32 Result = { { {
                                    V1.vector4_f32[0] * V2.vector4_f32[0] + V3.vector4_f32[0],
                                    V1.vector4_f32[1] * V2.vector4_f32[1] + V3.vector4_f32[1],
                                    V1.vector4_f32[2] * V2.vector4_f32[2] + V3.vector4_f32[2],
                                    V1.vector4_f32[3] * V2.vector4_f32[3] + V3.vector4_f32[3]
                            } } };
    return Result.v;
}

XVECTOR XVectorSwizzle
        (
                XVECTOR V,
                uint32_t E0,
                uint32_t E1,
                uint32_t E2,
                uint32_t E3
        )
{
    assert((E0 < 4) && (E1 < 4) && (E2 < 4) && (E3 < 4));

    XVECTORF32 Result = { { {
                                    V.vector4_f32[E0],
                                    V.vector4_f32[E1],
                                    V.vector4_f32[E2],
                                    V.vector4_f32[E3]
                            } } };
    return Result.v;
}

XVECTOR XMatrixDeterminant(XMATRIX M)
{
    static const XVECTORF32 Sign = { { { 1.0f, -1.0f, 1.0f, -1.0f } } };

    XVECTOR V0 = XVectorSwizzle(M.r[2], XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_X);
    XVECTOR V1 = XVectorSwizzle(M.r[3], XM_SWIZZLE_Z, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_Y);
    XVECTOR V2 = XVectorSwizzle(M.r[2], XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_X);
    XVECTOR V3 = XVectorSwizzle(M.r[3], XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_Z);
    XVECTOR V4 = XVectorSwizzle(M.r[2], XM_SWIZZLE_Z, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_Y);
    XVECTOR V5 = XVectorSwizzle(M.r[3], XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_Z);

    XVECTOR P0 = XVectorMultiply(V0, V1);
    XVECTOR P1 = XVectorMultiply(V2, V3);
    XVECTOR P2 = XVectorMultiply(V4, V5);

    V0 = XVectorSwizzle(M.r[2], XM_SWIZZLE_Z, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_Y);
    V1 = XVectorSwizzle(M.r[3], XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_X);
    V2 = XVectorSwizzle(M.r[2], XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_Z);
    V3 = XVectorSwizzle(M.r[3], XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_X);
    V4 = XVectorSwizzle(M.r[2], XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_Z);
    V5 = XVectorSwizzle(M.r[3], XM_SWIZZLE_Z, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_Y);

    P0 = XVectorNegativeMultiplySubtract(V0, V1, P0);
    P1 = XVectorNegativeMultiplySubtract(V2, V3, P1);
    P2 = XVectorNegativeMultiplySubtract(V4, V5, P2);

    V0 = XVectorSwizzle(M.r[1], XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_W, XM_SWIZZLE_Z);
    V1 = XVectorSwizzle(M.r[1], XM_SWIZZLE_Z, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_Y);
    V2 = XVectorSwizzle(M.r[1], XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_X);

    XVECTOR S = XVectorMultiply(M.r[0], Sign.v);
    XVECTOR R = XVectorMultiply(V0, P0);
    R = XVectorNegativeMultiplySubtract(V1, P1, R);
    R = XVectorMultiplyAdd(V2, P2, R);

    return XVector4Dot(S, R);
}

XVECTOR XVectorPermute
        (
                XVECTOR V1,
                XVECTOR V2,
                uint32_t PermuteX,
                uint32_t PermuteY,
                uint32_t PermuteZ,
                uint32_t PermuteW
        )
{
    assert(PermuteX <= 7 && PermuteY <= 7 && PermuteZ <= 7 && PermuteW <= 7);

    const uint32_t* aPtr[2];
    aPtr[0] = (const uint32_t*)(&V1);
    aPtr[1] = (const uint32_t*)(&V2);

    XVECTOR Result;
    uint32_t* pWork = (uint32_t*)(&Result);

    const uint32_t i0 = PermuteX & 3;
    const uint32_t vi0 = PermuteX >> 2;
    pWork[0] = aPtr[vi0][i0];

    const uint32_t i1 = PermuteY & 3;
    const uint32_t vi1 = PermuteY >> 2;
    pWork[1] = aPtr[vi1][i1];

    const uint32_t i2 = PermuteZ & 3;
    const uint32_t vi2 = PermuteZ >> 2;
    pWork[2] = aPtr[vi2][i2];

    const uint32_t i3 = PermuteW & 3;
    const uint32_t vi3 = PermuteW >> 2;
    pWork[3] = aPtr[vi3][i3];

    return Result;
}

XVECTOR XVectorReciprocal(XVECTOR V)
{
    XVECTORF32 Result = { { {
                                    1.f / V.vector4_f32[0],
                                    1.f / V.vector4_f32[1],
                                    1.f / V.vector4_f32[2],
                                    1.f / V.vector4_f32[3]
                            } } };
    return Result.v;
}

static XVECTORU32 g_XSelect0101 = { { { 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF } } };

XMATRIX XMatrixInverse
        (
                XVECTOR* pDeterminant,
                XMATRIX  M
        )
{
    XMATRIX MT = XMatrixTranspose(M);

    XVECTOR V0[4], V1[4];
    V0[0] = XVectorSwizzle(MT.r[2], XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Y);
    V1[0] = XVectorSwizzle(MT.r[3], XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W);
    V0[1] = XVectorSwizzle(MT.r[0], XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Y);
    V1[1] = XVectorSwizzle(MT.r[1], XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W);
    V0[2] = XVectorPermute(MT.r[2], MT.r[0], XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Z);
    V1[2] = XVectorPermute(MT.r[3], MT.r[1], XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_1W);

    XVECTOR D0 = XVectorMultiply(V0[0], V1[0]);
    XVECTOR D1 = XVectorMultiply(V0[1], V1[1]);
    XVECTOR D2 = XVectorMultiply(V0[2], V1[2]);

    V0[0] = XVectorSwizzle(MT.r[2], XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W);
    V1[0] = XVectorSwizzle(MT.r[3], XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Y);
    V0[1] = XVectorSwizzle(MT.r[0], XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W);
    V1[1] = XVectorSwizzle(MT.r[1], XM_SWIZZLE_X, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Y);
    V0[2] = XVectorPermute(MT.r[2], MT.r[0], XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_1W);
    V1[2] = XVectorPermute(MT.r[3], MT.r[1], XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Z);

    D0 = XVectorNegativeMultiplySubtract(V0[0], V1[0], D0);
    D1 = XVectorNegativeMultiplySubtract(V0[1], V1[1], D1);
    D2 = XVectorNegativeMultiplySubtract(V0[2], V1[2], D2);

    V0[0] = XVectorSwizzle(MT.r[1], XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y);
    V1[0] = XVectorPermute(D0, D2, XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X);
    V0[1] = XVectorSwizzle(MT.r[0], XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_X);
    V1[1] = XVectorPermute(D0, D2, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0Z);
    V0[2] = XVectorSwizzle(MT.r[3], XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y);
    V1[2] = XVectorPermute(D1, D2, XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X);
    V0[3] = XVectorSwizzle(MT.r[2], XM_SWIZZLE_Z, XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_X);
    V1[3] = XVectorPermute(D1, D2, XM_PERMUTE_0W, XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0Z);

    XVECTOR C0 = XVectorMultiply(V0[0], V1[0]);
    XVECTOR C2 = XVectorMultiply(V0[1], V1[1]);
    XVECTOR C4 = XVectorMultiply(V0[2], V1[2]);
    XVECTOR C6 = XVectorMultiply(V0[3], V1[3]);

    V0[0] = XVectorSwizzle(MT.r[1], XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Y, XM_SWIZZLE_Z);
    V1[0] = XVectorPermute(D0, D2, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X);
    V0[1] = XVectorSwizzle(MT.r[0], XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Y);
    V1[1] = XVectorPermute(D0, D2, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0X);
    V0[2] = XVectorSwizzle(MT.r[3], XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Y, XM_SWIZZLE_Z);
    V1[2] = XVectorPermute(D1, D2, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1Z);
    V0[3] = XVectorSwizzle(MT.r[2], XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_Y);
    V1[3] = XVectorPermute(D1, D2, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1Z, XM_PERMUTE_0X);

    C0 = XVectorNegativeMultiplySubtract(V0[0], V1[0], C0);
    C2 = XVectorNegativeMultiplySubtract(V0[1], V1[1], C2);
    C4 = XVectorNegativeMultiplySubtract(V0[2], V1[2], C4);
    C6 = XVectorNegativeMultiplySubtract(V0[3], V1[3], C6);

    V0[0] = XVectorSwizzle(MT.r[1], XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_X);
    V1[0] = XVectorPermute(D0, D2, XM_PERMUTE_0Z, XM_PERMUTE_1Y, XM_PERMUTE_1X, XM_PERMUTE_0Z);
    V0[1] = XVectorSwizzle(MT.r[0], XM_SWIZZLE_Y, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Z);
    V1[1] = XVectorPermute(D0, D2, XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1X);
    V0[2] = XVectorSwizzle(MT.r[3], XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_X);
    V1[2] = XVectorPermute(D1, D2, XM_PERMUTE_0Z, XM_PERMUTE_1W, XM_PERMUTE_1Z, XM_PERMUTE_0Z);
    V0[3] = XVectorSwizzle(MT.r[2], XM_SWIZZLE_Y, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Z);
    V1[3] = XVectorPermute(D1, D2, XM_PERMUTE_1W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z);

    XVECTOR C1 = XVectorNegativeMultiplySubtract(V0[0], V1[0], C0);
    C0 = XVectorMultiplyAdd(V0[0], V1[0], C0);
    XVECTOR C3 = XVectorMultiplyAdd(V0[1], V1[1], C2);
    C2 = XVectorNegativeMultiplySubtract(V0[1], V1[1], C2);
    XVECTOR C5 = XVectorNegativeMultiplySubtract(V0[2], V1[2], C4);
    C4 = XVectorMultiplyAdd(V0[2], V1[2], C4);
    XVECTOR C7 = XVectorMultiplyAdd(V0[3], V1[3], C6);
    C6 = XVectorNegativeMultiplySubtract(V0[3], V1[3], C6);

    XMATRIX R;
    R.r[0] = XVectorSelect(C0, C1, g_XSelect0101.v);
    R.r[1] = XVectorSelect(C2, C3, g_XSelect0101.v);
    R.r[2] = XVectorSelect(C4, C5, g_XSelect0101.v);
    R.r[3] = XVectorSelect(C6, C7, g_XSelect0101.v);

    XVECTOR Determinant = XVector4Dot(R.r[0], MT.r[0]);

    if (pDeterminant != NULL)
        *pDeterminant = Determinant;

    XVECTOR Reciprocal = XVectorReciprocal(Determinant);

    XMATRIX Result;
    Result.r[0] = XVectorMultiply(R.r[0], Reciprocal);
    Result.r[1] = XVectorMultiply(R.r[1], Reciprocal);
    Result.r[2] = XVectorMultiply(R.r[2], Reciprocal);
    Result.r[3] = XVectorMultiply(R.r[3], Reciprocal);
    return Result;
}

XMATRIX XMatrixRotationX(float Angle)
{
    float    fSinAngle;
    float    fCosAngle;
    XMScalarSinCos(&fSinAngle, &fCosAngle, Angle);

    XMATRIX M;
    M.m[0][0] = 1.0f;
    M.m[0][1] = 0.0f;
    M.m[0][2] = 0.0f;
    M.m[0][3] = 0.0f;

    M.m[1][0] = 0.0f;
    M.m[1][1] = fCosAngle;
    M.m[1][2] = fSinAngle;
    M.m[1][3] = 0.0f;

    M.m[2][0] = 0.0f;
    M.m[2][1] = -fSinAngle;
    M.m[2][2] = fCosAngle;
    M.m[2][3] = 0.0f;

    M.m[3][0] = 0.0f;
    M.m[3][1] = 0.0f;
    M.m[3][2] = 0.0f;
    M.m[3][3] = 1.0f;
    return M;
}

XMATRIX XMatrixRotationY(float Angle)
{
    float    fSinAngle;
    float    fCosAngle;
    XMScalarSinCos(&fSinAngle, &fCosAngle, Angle);

    XMATRIX M;
    M.m[0][0] = fCosAngle;
    M.m[0][1] = 0.0f;
    M.m[0][2] = -fSinAngle;
    M.m[0][3] = 0.0f;

    M.m[1][0] = 0.0f;
    M.m[1][1] = 1.0f;
    M.m[1][2] = 0.0f;
    M.m[1][3] = 0.0f;

    M.m[2][0] = fSinAngle;
    M.m[2][1] = 0.0f;
    M.m[2][2] = fCosAngle;
    M.m[2][3] = 0.0f;

    M.m[3][0] = 0.0f;
    M.m[3][1] = 0.0f;
    M.m[3][2] = 0.0f;
    M.m[3][3] = 1.0f;
    return M;
}

XMATRIX XMatrixRotationZ(float Angle)
{
    float    fSinAngle;
    float    fCosAngle;
    XMScalarSinCos(&fSinAngle, &fCosAngle, Angle);

    XMATRIX M;
    M.m[0][0] = fCosAngle;
    M.m[0][1] = fSinAngle;
    M.m[0][2] = 0.0f;
    M.m[0][3] = 0.0f;

    M.m[1][0] = -fSinAngle;
    M.m[1][1] = fCosAngle;
    M.m[1][2] = 0.0f;
    M.m[1][3] = 0.0f;

    M.m[2][0] = 0.0f;
    M.m[2][1] = 0.0f;
    M.m[2][2] = 1.0f;
    M.m[2][3] = 0.0f;

    M.m[3][0] = 0.0f;
    M.m[3][1] = 0.0f;
    M.m[3][2] = 0.0f;
    M.m[3][3] = 1.0f;
    return M;
}

XMATRIX XMatrixScaling
        (
                float ScaleX,
                float ScaleY,
                float ScaleZ
        )
{

    XMATRIX M;
    M.m[0][0] = ScaleX;
    M.m[0][1] = 0.0f;
    M.m[0][2] = 0.0f;
    M.m[0][3] = 0.0f;

    M.m[1][0] = 0.0f;
    M.m[1][1] = ScaleY;
    M.m[1][2] = 0.0f;
    M.m[1][3] = 0.0f;

    M.m[2][0] = 0.0f;
    M.m[2][1] = 0.0f;
    M.m[2][2] = ScaleZ;
    M.m[2][3] = 0.0f;

    M.m[3][0] = 0.0f;
    M.m[3][1] = 0.0f;
    M.m[3][2] = 0.0f;
    M.m[3][3] = 1.0f;
    return M;
}

XMATRIX XMatrixTranslation
        (
                float OffsetX,
                float OffsetY,
                float OffsetZ
        )
{
    XMATRIX M;
    M.m[0][0] = 1.0f;
    M.m[0][1] = 0.0f;
    M.m[0][2] = 0.0f;
    M.m[0][3] = 0.0f;

    M.m[1][0] = 0.0f;
    M.m[1][1] = 1.0f;
    M.m[1][2] = 0.0f;
    M.m[1][3] = 0.0f;

    M.m[2][0] = 0.0f;
    M.m[2][1] = 0.0f;
    M.m[2][2] = 1.0f;
    M.m[2][3] = 0.0f;

    M.m[3][0] = OffsetX;
    M.m[3][1] = OffsetY;
    M.m[3][2] = OffsetZ;
    M.m[3][3] = 1.0f;
    return M;
}

XVECTOR XVectorMin
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORF32 Result = { { {
                                    (V1.vector4_f32[0] < V2.vector4_f32[0]) ? V1.vector4_f32[0] : V2.vector4_f32[0],
                                    (V1.vector4_f32[1] < V2.vector4_f32[1]) ? V1.vector4_f32[1] : V2.vector4_f32[1],
                                    (V1.vector4_f32[2] < V2.vector4_f32[2]) ? V1.vector4_f32[2] : V2.vector4_f32[2],
                                    (V1.vector4_f32[3] < V2.vector4_f32[3]) ? V1.vector4_f32[3] : V2.vector4_f32[3]
                            } } };
    return Result.v;
}

XVECTOR XVectorMax
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORF32 Result = { { {
                                    (V1.vector4_f32[0] > V2.vector4_f32[0]) ? V1.vector4_f32[0] : V2.vector4_f32[0],
                                    (V1.vector4_f32[1] > V2.vector4_f32[1]) ? V1.vector4_f32[1] : V2.vector4_f32[1],
                                    (V1.vector4_f32[2] > V2.vector4_f32[2]) ? V1.vector4_f32[2] : V2.vector4_f32[2],
                                    (V1.vector4_f32[3] > V2.vector4_f32[3]) ? V1.vector4_f32[3] : V2.vector4_f32[3]
                            } } };
    return Result.v;
}

XVECTOR XVectorScale
        (
                XVECTOR V,
                float    ScaleFactor
        )
{
    XVECTORF32 Result = { { {
                                    V.vector4_f32[0] * ScaleFactor,
                                    V.vector4_f32[1] * ScaleFactor,
                                    V.vector4_f32[2] * ScaleFactor,
                                    V.vector4_f32[3] * ScaleFactor
                            } } };
    return Result.v;
}

XVECTOR XVectorAdd
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORF32 Result = { { {
                                    V1.vector4_f32[0] + V2.vector4_f32[0],
                                    V1.vector4_f32[1] + V2.vector4_f32[1],
                                    V1.vector4_f32[2] + V2.vector4_f32[2],
                                    V1.vector4_f32[3] + V2.vector4_f32[3]
                            } } };
    return Result.v;
}

XVECTOR XVectorSplatX(XVECTOR V)
{
    XVECTORF32 vResult;
    vResult.f[0] =
    vResult.f[1] =
    vResult.f[2] =
    vResult.f[3] = V.vector4_f32[0];
    return vResult.v;
}

XVECTOR XVectorSplatY(XVECTOR V)
{
    XVECTORF32 vResult;
    vResult.f[0] =
    vResult.f[1] =
    vResult.f[2] =
    vResult.f[3] = V.vector4_f32[1];
    return vResult.v;
}

XVECTOR XVectorSplatZ(XVECTOR V)
{
    XVECTORF32 vResult;
    vResult.f[0] =
    vResult.f[1] =
    vResult.f[2] =
    vResult.f[3] = V.vector4_f32[2];
    return vResult.v;
}

XVECTOR XVectorSplatW(XVECTOR V)
{
    XVECTORF32 vResult;
    vResult.f[0] =
    vResult.f[1] =
    vResult.f[2] =
    vResult.f[3] = V.vector4_f32[3];
    return vResult.v;
}

XVECTOR XVectorDivide
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORF32 Result = { { {
                                    V1.vector4_f32[0] / V2.vector4_f32[0],
                                    V1.vector4_f32[1] / V2.vector4_f32[1],
                                    V1.vector4_f32[2] / V2.vector4_f32[2],
                                    V1.vector4_f32[3] / V2.vector4_f32[3]
                            } } };
    return Result.v;
}

XVECTOR XVector3TransformCoord
        (
                XVECTOR V,
                XMATRIX M
        )
{
    XVECTOR Z = XVectorSplatZ(V);
    XVECTOR Y = XVectorSplatY(V);
    XVECTOR X = XVectorSplatX(V);

    XVECTOR Result = XVectorMultiplyAdd(Z, M.r[2], M.r[3]);
    Result = XVectorMultiplyAdd(Y, M.r[1], Result);
    Result = XVectorMultiplyAdd(X, M.r[0], Result);

    XVECTOR W = XVectorSplatW(Result);
    return XVectorDivide(Result, W);
}

XVECTOR XVector3TransformNormal
        (
                XVECTOR V,
                XMATRIX M
        )
{
    XVECTOR Z = XVectorSplatZ(V);
    XVECTOR Y = XVectorSplatY(V);
    XVECTOR X = XVectorSplatX(V);

    XVECTOR Result = XVectorMultiply(Z, M.r[2]);
    Result = XVectorMultiplyAdd(Y, M.r[1], Result);
    Result = XVectorMultiplyAdd(X, M.r[0], Result);

    return Result;
}

XVECTOR XVectorSplatOne()
{
    XVECTORF32 vResult;
    vResult.f[0] =
    vResult.f[1] =
    vResult.f[2] =
    vResult.f[3] = 1.0f;
    return vResult.v;
}

XVECTOR XVectorLessOrEqual
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORU32 Control = { { {
                                     (V1.vector4_f32[0] <= V2.vector4_f32[0]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[1] <= V2.vector4_f32[1]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[2] <= V2.vector4_f32[2]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[3] <= V2.vector4_f32[3]) ? 0xFFFFFFFF : 0
                             } } };
    return Control.v;
}

bool XVector4Less
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    return (((V1.vector4_f32[0] < V2.vector4_f32[0]) && (V1.vector4_f32[1] < V2.vector4_f32[1]) && (V1.vector4_f32[2] < V2.vector4_f32[2]) && (V1.vector4_f32[3] < V2.vector4_f32[3])) != 0);
}

XVECTOR XVectorAbs(XVECTOR V)
{
    XVECTORF32 vResult = { { {
                                     fabsf(V.vector4_f32[0]),
                                     fabsf(V.vector4_f32[1]),
                                     fabsf(V.vector4_f32[2]),
                                     fabsf(V.vector4_f32[3])
                             } } };
    return vResult.v;
}

bool XVector3IsUnit(XVECTOR V)
{
    XVECTOR Difference = XVectorSubtract(XVector3Length(V), XVectorSplatOne());
    return XVector4Less(XVectorAbs(Difference), g_UnitVectorEpsilon);
}

XVECTOR XVectorGreater
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORU32 Control = { { {
                                     (V1.vector4_f32[0] > V2.vector4_f32[0]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[1] > V2.vector4_f32[1]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[2] > V2.vector4_f32[2]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[3] > V2.vector4_f32[3]) ? 0xFFFFFFFF : 0
                             } } };
    return Control.v;
}

XVECTOR XVectorOrInt
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORU32 Result = { { {
                                    V1.vector4_u32[0] | V2.vector4_u32[0],
                                    V1.vector4_u32[1] | V2.vector4_u32[1],
                                    V1.vector4_u32[2] | V2.vector4_u32[2],
                                    V1.vector4_u32[3] | V2.vector4_u32[3]
                            } } };
    return Result.v;
}

XVECTOR XVectorLess
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORU32 Control = { { {
                                     (V1.vector4_f32[0] < V2.vector4_f32[0]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[1] < V2.vector4_f32[1]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[2] < V2.vector4_f32[2]) ? 0xFFFFFFFF : 0,
                                     (V1.vector4_f32[3] < V2.vector4_f32[3]) ? 0xFFFFFFFF : 0
                             } } };
    return Control.v;
}

XVECTOR XVectorInBounds
        (
                XVECTOR V,
                XVECTOR Bounds
        )
{
    XVECTORU32 Control = { { {
                                     (V.vector4_f32[0] <= Bounds.vector4_f32[0] && V.vector4_f32[0] >= -Bounds.vector4_f32[0]) ? 0xFFFFFFFF : 0,
                                     (V.vector4_f32[1] <= Bounds.vector4_f32[1] && V.vector4_f32[1] >= -Bounds.vector4_f32[1]) ? 0xFFFFFFFF : 0,
                                     (V.vector4_f32[2] <= Bounds.vector4_f32[2] && V.vector4_f32[2] >= -Bounds.vector4_f32[2]) ? 0xFFFFFFFF : 0,
                                     (V.vector4_f32[3] <= Bounds.vector4_f32[3] && V.vector4_f32[3] >= -Bounds.vector4_f32[3]) ? 0xFFFFFFFF : 0
                             } } };
    return Control.v;
}

XVECTOR XVectorAndCInt
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    XVECTORU32 Result = { { {
                                    V1.vector4_u32[0] & ~V2.vector4_u32[0],
                                    V1.vector4_u32[1] & ~V2.vector4_u32[1],
                                    V1.vector4_u32[2] & ~V2.vector4_u32[2],
                                    V1.vector4_u32[3] & ~V2.vector4_u32[3]
                            } } };
    return Result.v;
}

uint32_t XVector4EqualIntR
        (
                XVECTOR V1,
                XVECTOR V2
        )
{
    uint32_t CR = 0;
    if (V1.vector4_u32[0] == V2.vector4_u32[0] &&
        V1.vector4_u32[1] == V2.vector4_u32[1] &&
        V1.vector4_u32[2] == V2.vector4_u32[2] &&
        V1.vector4_u32[3] == V2.vector4_u32[3])
    {
        CR = X_CRMASK_CR6TRUE;
    }
    else if (V1.vector4_u32[0] != V2.vector4_u32[0] &&
             V1.vector4_u32[1] != V2.vector4_u32[1] &&
             V1.vector4_u32[2] != V2.vector4_u32[2] &&
             V1.vector4_u32[3] != V2.vector4_u32[3])
    {
        CR = X_CRMASK_CR6FALSE;
    }
    return CR;
}

XVECTOR XVectorTrueInt()
{
    XVECTORU32 vResult = { { { 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU } } };
    return vResult.v;
}

bool XComparisonAnyTrue(uint32_t CR) { return (CR & X_CRMASK_CR6FALSE) != X_CRMASK_CR6FALSE; }

bool XVector3AnyTrue(XVECTOR V)
{
    // Duplicate the fourth element from the first element.
    XVECTOR C = XVectorSwizzle(V, X_SWIZZLE_X, X_SWIZZLE_Y, X_SWIZZLE_Z, X_SWIZZLE_X);

    return XComparisonAnyTrue(XVector4EqualIntR(C, XVectorTrueInt()));
}

float XVectorGetX(XVECTOR V)
{
    return V.vector4_f32[0];
}

float XVectorGetY(XVECTOR V)
{
    return V.vector4_f32[1];
}

float XVectorGetZ(XVECTOR V)
{
    return V.vector4_f32[2];
}

void XStoreFloat
        (
                float* pDestination,
                XVECTOR V
        )
{
    assert(pDestination);
    *pDestination = XVectorGetX(V);
}

XVECTOR XVectorClamp
        (
                XVECTOR V,
                XVECTOR Min,
                XVECTOR Max
        )
{
    //assert(XVector3LessOrEqual(Min, Max));

    XVECTOR Result;
    Result = XVectorMax(Min, V);
    Result = XVectorMin(Max, Result);
    return Result;
}

XVECTOR XVectorACos(XVECTOR V)
{
    // 7-degree minimax approximation

    XVECTORF32 Result = { { {
                                    acosf(V.vector4_f32[0]),
                                    acosf(V.vector4_f32[1]),
                                    acosf(V.vector4_f32[2]),
                                    acosf(V.vector4_f32[3])
                            } } };
    return Result.v;
}

XVECTOR XVector3AngleBetweenNormals
        (
                XVECTOR N1,
                XVECTOR N2
        )
{
    XVECTOR Result = XVector3Dot(N1, N2);
    Result = XVectorClamp(Result, g_XNegativeOne.v, g_XOne.v);
    Result = XVectorACos(Result);
    return Result;
}

bool XVector4NearEqual
        (
                XVECTOR V1,
                XVECTOR V2,
                XVECTOR Epsilon
        )
{
    float dx, dy, dz, dw;

    dx = fabsf(V1.vector4_f32[0] - V2.vector4_f32[0]);
    dy = fabsf(V1.vector4_f32[1] - V2.vector4_f32[1]);
    dz = fabsf(V1.vector4_f32[2] - V2.vector4_f32[2]);
    dw = fabsf(V1.vector4_f32[3] - V2.vector4_f32[3]);
    return (((dx <= Epsilon.vector4_f32[0]) &&
             (dy <= Epsilon.vector4_f32[1]) &&
             (dz <= Epsilon.vector4_f32[2]) &&
             (dw <= Epsilon.vector4_f32[3])) != 0);
}

XVECTOR XVector3Transform
        (
                XVECTOR V,
                XMATRIX M
        )
{
    XVECTOR Z = XVectorSplatZ(V);
    XVECTOR Y = XVectorSplatY(V);
    XVECTOR X = XVectorSplatX(V);

    XVECTOR Result = XVectorMultiplyAdd(Z, M.r[2], M.r[3]);
    Result = XVectorMultiplyAdd(Y, M.r[1], Result);
    Result = XVectorMultiplyAdd(X, M.r[0], Result);

    return Result;
}

XFLOAT4X4 Identity4x4()
{
    XFLOAT4X4 I = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
    return I;
}

XMATRIX XMatrixIdentity()
{
    XMATRIX M;
    M.r[0] = g_XMIdentityR0.v;
    M.r[1] = g_XMIdentityR1.v;
    M.r[2] = g_XMIdentityR2.v;
    M.r[3] = g_XMIdentityR3.v;
    return M;
}