//--------------------------------------------------------------------------------------
// Copyright 2013 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------

#ifndef __CPUTMath_h__
#define __CPUTMath_h__
#include <math.h>

/*
 * Constants
 */
static const float kEpsilon  = 0.00001f;
static const float kPi       = 3.14159265358979323846f;
static const float k2Pi      = kPi*2.0f;
static const float kPiDiv2   = kPi/2.0f;
static const float kInvPi    = 1.0f/kPi;
static const float kDegToRad = (kPi/180.0f);
static const float kRadToDeg = (180.0f/kPi);
static inline float DegToRad( float fDeg ) { return fDeg * kDegToRad; }
static inline float RadToDeg( float fRad ) { return fRad * kRadToDeg; }

template<typename T>
static inline void Swap(T &a, T &b)
{
    T t = b;
    b = a;
    a = t;
}

static inline double InvSqrt(const double x)
{
	return 1.0 / sqrt(x);
}

struct float2;
struct float3;
struct float4;

/**************************************\
float2
\**************************************/
struct float2
{
    union
    {
        struct
        {
            float x;
            float y;
        };
        float f[2];
    };

    /***************************************\
    |   Constructors                        |
    \***************************************/
    float2() {}
    explicit float2(float f) : x(f), y(f) { }
    explicit float2(float _x, float _y) : x(_x), y(_y) { }
    explicit float2(float* f) : x(f[0]), y(f[1]) { }
    float2(const float2 &v) : x(v.x), y(v.y) { }
    const float2 &operator=(const float2 &v)
    {
        x = v.x;
        y = v.y;        
        return *this;
    }

    /***************************************\
    |   Basic math operations               |
    \***************************************/
    float2 operator+(const float2 &r) const
    {
        return float2(x+r.x, y+r.y);
    }
    const float2 &operator+=(const float2 &r)
    {
        x += r.x;
        y += r.y;
        return *this;
    }
    float2 operator-(const float2 &r) const
    {
        return float2(x-r.x, y-r.y);
    }
    const float2 &operator-=(const float2 &r)
    {
        x -= r.x;
        y -= r.y;
        return *this;
    }
    /***************************************\
    |   Basic math operations with scalars  |
    \***************************************/
    float2 operator+(float f) const
    {
        return float2(x+f, y+f);
    }
    const float2 &operator+=(float f)
    {
        x += f;
        y += f;
        return *this;
    }
    float2 operator-(float f) const
    {
        return float2(x-f, y-f);
    }
    const float2 &operator-=(float f)
    {
        x -= f;
        y -= f;
        return *this;
    }
    float2 operator*(float f) const
    {
        return float2(x*f, y*f);
    }
    const float2 &operator*=(float f)
    {
        x *= f;
        y *= f;
        return *this;
    }
    float2 operator/(float f) const
    {
        return float2(x/f, y/f);
    }
    const float2 &operator/=(float f)
    {
        x /= f;
        y /= f;
        return *this;
    }

    /***************************************\
    |   Other math                          |
    \***************************************/
    // Equality
    bool operator==(const float2 &r) const
    {
        return (x==r.x && y == r.y);
    }
    bool operator!=(const float2 &r) const
    {
        return !(*this == r);
    }

    // Hadd
    float hadd(void) const
    {
        return x + y;
    }

    // Length
    float lengthSq(void) const
    {
        return x*x + y*y;
    }
    float length(void) const
    {
        return sqrtf(lengthSq());
    }
    void normalize(void)
    {
        *this /= length();
    }
};

inline float dot2(const float2 &l, const float2 &r)
{
    return l.x*r.x + l.y*r.y;
}

inline float2 normalize(const float2 &v)
{
    float length = v.length();
    return v / length;
}








/**************************************\
float3
\**************************************/
struct float3
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        float f[3];
    };

	static const float3 Left;
	static const float3 Right;
	static const float3 Up;
	static const float3 Down;
	static const float3 Forward;
	static const float3 Backward;
	static const float3 ZeroF;

    /***************************************\
    |   Constructors                        |
    \***************************************/
    float3() {}
    explicit float3(float f) : x(f), y(f), z(f) { }
    explicit float3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
    explicit float3(float* f) : x(f[0]), y(f[1]), z(f[2]) { }
    float3(const float3 &v) : x(v.x), y(v.y), z(v.z) { }
    float3(const float4 &v);
    const float3 &operator=(const float3 &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    /***************************************\
    |   Basic math operations               |
    \***************************************/
    float3 operator+(const float3 &r) const
    {
        return float3(x+r.x, y+r.y, z+r.z);
    }
    const float3 &operator+=(const float3 &r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
        return *this;
    }
    float3 operator-(const float3 &r) const
    {
        return float3(x-r.x, y-r.y, z-r.z);
    }
	float operator[](const int index) const {
		return (&x)[index];
	}


	float operator*(const float3 &a) const 
	{
		return x * a.x + y * a.y + z * a.z;
	}

	float Dot(const float3 &a) const
	{
		return x * a.x + y * a.y + z * a.z;
	}

	static float Dot(const float3 &a, const float3 &b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}


	float &operator[](const int index) {
		return (&x)[index];
	}
    const float3 &operator-=(const float3 &r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        return *this;
    }

	float NormalizeFast() 
	{
		float sqrLength, invLength;

		sqrLength = x * x + y * y + z * z;
		invLength = InvSqrt(sqrLength);
		x *= invLength;
		y *= invLength;
		z *= invLength;
		return invLength * sqrLength;
	}
    
	float3 operator*(float3 &r)
	{
		return float3(x*r.x, y*r.y, z*r.z);
	}
	

	float3 operator*(const float a) const {
		return float3(x * a, y * a, z * a);
	}

	friend float3 operator*(const float a, const float3 b) {
		return float3(b.x * a, b.y * a, b.z * a);
	}

    const float3 &operator*=(const float3 &r)
    {
        x *= r.x;
        y *= r.y;
        z *= r.z;
        return *this;
    }
	float3 &operator*=(float3 &r)
	{
		x *= r.x;
		y *= r.y;
		z *= r.z;
		return *this;
	}
	bool Compare(const float3 &a) const {
		return ((x == a.x) && (y == a.y) && (z == a.z));
	}

	bool Compare(const float3 &a, const float epsilon) const {
		if (fabs(x - a.x) > epsilon) {
			return false;
		}

		if (fabs(y - a.y) > epsilon) {
			return false;
		}

		if (fabs(z - a.z) > epsilon) {
			return false;
		}

		return true;
	}
    float3 operator/(const float3 &r) const
    {
        return float3(x/r.x, y/r.y, z/r.z);
    }
    const float3 &operator/=(const float3 &r)
    {
        x /= r.x;
        y /= r.y;
        z /= r.z;
        return *this;
    }

    /***************************************\
    |   Basic math operations with scalars  |
    \***************************************/
    float3 operator+(float f) const
    {
        return float3(x+f, y+f, z+f);
    }
    const float3 &operator+=(float f)
    {
        x += f;
        y += f;
        z += f;
        return *this;
    }
    float3 operator-(float f) const
    {
        return float3(x-f, y-f, z-f);
    }
    const float3 &operator-=(float f)
    {
        x -= f;
        y -= f;
        z -= f;
        return *this;
    }


    const float3 &operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    float3 operator/(float f) const
    {
        return float3(x/f, y/f, z/f);
    }
    const float3 &operator/=(float f)
    {
        x /= f;
        y /= f;
        z /= f;
        return *this;
    }

	void Zero()
	{
		x = 0;
		y = 0;
		z = 0;
	}

    /***************************************\
    |   Other math                          |
    \***************************************/
    // Equality
    bool operator==(const float3 &r) const
    {
        return x==r.x &&y == r.y &&z == r.z;
    }
    bool operator!=(const float3 &r) const
    {
        return !(*this == r);
    }

    // Hadd
    float hadd(void) const
    {
        return x + y + z;
    }

    // Length
    float lengthSq(void) const
    {
        return x*x + y*y + z*z;
    }
    float length(void) const
    {
        return sqrtf(lengthSq());
    }

    float3 normalize(void)
    {
        return (*this /= length());
    }

	float NormalizeSelf() {
		float sqrLength, invLength;

		sqrLength = x * x + y * y + z * z;
		invLength = InvSqrt(sqrLength);
		x *= invLength;
		y *= invLength;
		z *= invLength;
		return invLength * sqrLength;
	}
	float3 Cross(const float3 &a) const 
	{
		return float3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
	}

	float3 &Cross(const float3 &a, const float3 &b) 
	{
		x = a.y * b.z - a.z * b.y;
		y = a.z * b.x - a.x * b.z;
		z = a.x * b.y - a.y * b.x;

		return *this;
	}
};

inline float dot3(const float3 &l, const float3 &r)
{
    return l.x*r.x + l.y*r.y + l.z*r.z;
}
inline float3 cross3(const float3 &l, const float3 &r)
{
    return float3(  l.y*r.z-l.z*r.y,
                    l.z*r.x-l.x*r.z,
                    l.x*r.y-l.y*r.x);
}
inline float3 normalize(const float3 &v)
{
    float length = v.length();
    return v / length;
}

inline float3 abs3( const float3 &l ) 
{ 
    float3 tmp; 
    tmp.x = fabs(l.x); 
    tmp.y = fabs(l.y); 
    tmp.z = fabs(l.z); 
    return tmp; 
}


/**************************************\
float4
\**************************************/
__declspec(align(16)) struct float4
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        float f[4];
    };

    /***************************************\
    |   Constructors                        |
    \***************************************/
    float4() {}
    explicit float4(float f) : x(f), y(f), z(f), w(f) { }
    explicit float4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) { }
    explicit float4(float* f) : x(f[0]), y(f[1]), z(f[2]), w(f[3]) { }
    float4(const float4 &v) : x(v.x), y(v.y), z(v.z), w(v.w) { }
    float4(const float3 &v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) { }
    const float4 &operator=(const float4 &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    /***************************************\
    |   Basic math operations               |
    \***************************************/
    float4 operator+(const float4 &r) const
    {
        return float4(x+r.x, y+r.y, z+r.z, w+r.w);
    }
    const float4 &operator+=(const float4 &r)
    {
        x += r.x;
        y += r.y;
        z += r.z;
        w += r.w;
        return *this;
    }
    float4 operator-(const float4 &r) const
    {
        return float4(x-r.x, y-r.y, z-r.z, w-r.w);
    }
    const float4 &operator-=(const float4 &r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        w -= r.w;
        return *this;
    }
    float4 operator*(const float4 &r) const
    {
        return float4(x*r.x, y*r.y, z*r.z, w*r.w);
    }
    const float4 &operator*=(const float4 &r)
    {
        x *= r.x;
        y *= r.y;
        z *= r.z;
        w *= r.w;
        return *this;
    }
    float4 operator/(const float4 &r) const
    {
        return float4(x/r.x, y/r.y, z/r.z, w/r.w);
    }
    const float4 &operator/=(const float4 &r)
    {
        x /= r.x;
        y /= r.y;
        z /= r.z;
        w /= r.w;
        return *this;
    }

    /***************************************\
    |   Basic math operations with scalars  |
    \***************************************/
    float4 operator+(float f) const
    {
        return float4(x+f, y+f, z+f, w+f);
    }
    const float4 &operator+=(float f)
    {
        x += f;
        y += f;
        z += f;
        return *this;
    }
    float4 operator-(float f) const
    {
        return float4(x-f, y-f, z-f, w-f);
    }
    const float4 &operator-=(float f)
    {
        x -= f;
        y -= f;
        z -= f;
        w -= f;
        return *this;
    }
    float4 operator*(float f) const
    {
        return float4(x*f, y*f, z*f, w*f);
    }
    const float4 &operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        w *= f;
        return *this;
    }
    float4 operator/(float f) const
    {
        return float4(x/f, y/f, z/f, w/f);
    }
    const float4 &operator/=(float f)
    {
        x /= f;
        y /= f;
        z /= f;
        w /= f;
        return *this;
    }

    /***************************************\
    |   Other math                          |
    \***************************************/
    // Equality
    bool operator==(const float4 &r) const
    {
        return x==r.x && y == r.y && z == r.z && w == r.w;
    }
    bool operator!=(const float4 &r) const
    {
        return !(*this == r);
    }

    // Hadd
    float hadd(void) const
    {
        return x + y + z + w;
    }

    // Length
    float lengthSq(void) const
    {
        return x*x + y*y + z*z + w*w;
    }
    float length(void) const
    {
        return sqrtf(lengthSq());
    }
    void normalize(void)
    {
        *this /= length();
    }
};

inline float dot4(const float4 &l, const float4 &r)
{
    return l.x*r.x + l.y*r.y + l.z*r.z + l.w*r.w;
}
inline float4 normalize(const float4 &v)
{
    float length = v.length();
    return v / length;
}

inline float3::float3(const float4 &v) : x(v.x), y(v.y), z(v.z) { }


/**************************************\
float3x3
\**************************************/
struct float4x4;
struct float3x3
{
    struct
    {
        float3 r0;
        float3 r1;
        float3 r2;
    };

    /***************************************\
    |   Constructors                        |
    \***************************************/
    float3x3() {}
    explicit float3x3(float f) : r0(f), r1(f), r2(f) { }
    explicit float3x3(float* f) : r0(f+0), r1(f+3), r2(f+6) { }
    float3x3(const float3 &_r0, const float3 &_r1, const float3 &_r2) : r0(_r0), r1(_r1), r2(_r2) { }
    float3x3(float _m00, float _m01, float _m02,
             float _m10, float _m11, float _m12,
             float _m20, float _m21, float _m22)
             : r0(_m00,_m01,_m02)
             , r1(_m10,_m11,_m12)
             , r2(_m20,_m21,_m22)
    {
    }
    float3x3(const float4x4 &m);
    const float3x3 &operator=(const float3x3 &m)
    {
        r0 = m.r0;
        r1 = m.r1;
        r2 = m.r2;
        return *this;
    }

    /***************************************\
    |   Basic math operations               |
    \***************************************/

    #define MTX3_INDEX(f,r,c) ((f)[(r*3)+c])
    inline float3x3 operator*(const float3x3 &r) const
    {
        float3x3 m(1,0,0,0,1,0,0,0,1);

        const float* left     = (const float*)&this->r0;
        const float* right    = (const float*)&r.r0;
        float* result   = (float*)&m;

        int ii, jj, kk;
        for(ii=0; ii<3; ++ii) /* row */
        {
            for(jj=0; jj<3; ++jj) /* column */
            {
                float sum = MTX3_INDEX(left,ii,0)*MTX3_INDEX(right,0,jj);
                for(kk=1; kk<3; ++kk)
                {
                    sum += (MTX3_INDEX(left,ii,kk)*MTX3_INDEX(right,kk,jj));
                }
                MTX3_INDEX(result,ii,jj) = sum;
            }
        }
        return m;
    }
    #undef MTX3_INDEX

    inline float3 operator*(const float3 &v) const
    {
        float3 a;

        a.x = (r0*v);
        a.y = (r1*v);
        a.z = (r2*v);

        return a;
    }

    /***************************************\
    |   Basic math operations with scalars  |
    \***************************************/
    float3x3 operator+(float f) const
    {
        return float3x3(r0+f, r1+f, r2+f);
    }

	float3 const &operator[](int index) const {
		//assert( ( index >= 0 ) && ( index < 3 ) );
		return (&r0)[index];
	}

	float3 &operator[](int index) {
		//assert( ( index >= 0 ) && ( index < 3 ) );
		return (&r0)[index];
	}

    const float3x3 &operator+=(float f)
    {
        r0 += f;
        r1 += f;
        r2 += f;
        return *this;
    }


	friend float3 operator*(const float3 &vec, const float3x3 &mat) {
		return mat * vec;
	}

	friend float3x3 operator*(const float a, const float3x3 &mat) {
		return mat * a;
	}

	friend float3 &operator*=(float3 &vec, const float3x3 &mat) {
		float x = mat[0].x * vec.x + mat[1].x * vec.y + mat[2].x * vec.z;
		float y = mat[0].y * vec.x + mat[1].y * vec.y + mat[2].y * vec.z;
		vec.z = mat[0].z * vec.x + mat[1].z * vec.y + mat[2].z * vec.z;
		vec.x = x;
		vec.y = y;
		return vec;
	}

	float3x3 &operator*=(const float3x3 &a) {
		int i, j;
		const float *m2Ptr;
		float *m1Ptr, dst[3];

		m1Ptr = reinterpret_cast<float *>(this);
		m2Ptr = reinterpret_cast<const float *>(&a);

		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				dst[j] = m1Ptr[0] * m2Ptr[0 * 3 + j]
					+ m1Ptr[1] * m2Ptr[1 * 3 + j]
					+ m1Ptr[2] * m2Ptr[2 * 3 + j];
			}
			m1Ptr[0] = dst[0]; m1Ptr[1] = dst[1]; m1Ptr[2] = dst[2];
			m1Ptr += 3;
		}
		return *this;
	}

    float3x3 operator-(float f) const
    {
        return float3x3(r0-f, r1-f, r2-f);
    }
	bool Compare(const float3x3 &a) const {
		if (r0.Compare(a[0]) &&
			r1.Compare(a[1]) &&
			r2.Compare(a[2])) {
			return true;
		}
		return false;
	}

	bool Compare(const float3x3 &a, const float epsilon) const {
		if (r0.Compare(a[0], epsilon) &&
			r1.Compare(a[1], epsilon) &&
			r2.Compare(a[2], epsilon)) {
			return true;
		}
		return false;
	}
	
    const float3x3 &operator-=(float f)
    {
        r0 -= f;
        r1 -= f;
        r2 -= f;
        return *this;
    }
    float3x3 operator*(float f) const
    {
        return float3x3(r0*f, r1*f, r2*f);
    }
    const float3x3 &operator*=(float f)
    {
        r0 *= f;
        r1 *= f;
        r2 *= f;
        return *this;
    }
    float3x3 operator/(float f) const
    {
        return float3x3(r0/f, r1/f, r2/f);
    }
    const float3x3 &operator/=(float f)
    {
        r0 /= f;
        r1 /= f;
        r2 /= f;
        return *this;
    }

    /***************************************\
    |   Other math                          |
    \***************************************/
    // Equality
    bool operator==(const float3x3 &r) const
    {
        return r0 == r.r0 && r1 == r.r1 && r2 == r.r2;
    }
    bool operator!=(const float3x3 &r) const
    {
        return !(*this == r);
    }

    float determinant() const
    {
        float f0 = r0.x *  (r1.y*r2.z-r2.y*r1.z);
        float f1 = r0.y * -(r1.x*r2.z-r2.x*r1.z);
        float f2 = r0.z *  (r1.x*r2.y-r2.x*r1.y);

        return f0 + f1 + f2;
    }

    void transpose(void)
    {
        Swap(r0.y, r1.x);
        Swap(r0.z, r2.x);
        Swap(r1.z, r2.y);
    }

    void invert(void)
    {
        float det = determinant();
        float3x3 inv;

        inv.r0.x =   (r1.y*r2.z) - (r1.z*r2.y);
        inv.r0.y = -((r1.x*r2.z) - (r1.z*r2.x));
        inv.r0.z =   (r1.x*r2.y) - (r1.y*r2.x);

        inv.r1.x = -((r0.y*r2.z) - (r0.z*r2.y));
        inv.r1.y =   (r0.x*r2.z) - (r0.z*r2.x);
        inv.r1.z = -((r0.x*r2.y) - (r0.y*r2.x));

        inv.r2.x =   (r0.y*r1.z) - (r0.z*r1.y);
        inv.r2.y = -((r0.x*r1.z) - (r0.z*r1.x));
        inv.r2.z =   (r0.x*r1.y) - (r0.y*r1.x);

        inv.transpose();
        inv /= det;

        *this = inv;
    }
};
inline float3x3 float3x3Identity(void)
{
    return float3x3(1,0,0,
                    0,1,0,
                    0,0,1);
}
inline float determinant(const float3x3&m)
{
    return m.determinant();
}

inline float3x3 transpose(const float3x3 &m)
{
    float3x3 ret = m;
    ret.transpose();
    return ret;
}

inline float3x3 inverse(const float3x3 &m)
{
    float3x3 ret = m;
    ret.invert();
    return ret;
}

inline float3x3 float3x3RotationX(float rad)
{
    float       c = cosf( rad );
    float       s = sinf( rad );
    float3x3     m( 1.0f, 0.0f, 0.0f,
                    0.0f,    c,    s,
                    0.0f,   -s,    c );
    return m;
}
inline float3x3 float3x3RotationY(float rad)
{
    float       c = cosf( rad );
    float       s = sinf( rad );
    float3x3     m(    c, 0.0f,   -s,
                    0.0f, 1.0f, 0.0f,
                       s, 0.0f,    c );
    return m;
}
inline float3x3 float3x3RotationZ(float rad)
{

    float       c = cosf( rad );
    float       s = sinf( rad );
    float3x3     m(    c,    s, 0.0f,
                      -s,    c, 0.0f,
                    0.0f, 0.0f, 1.0f );
    return m;
}

inline float3x3 float3x3RotationAxis(const float3 &axis, float rad )
{
    float3 normAxis = normalize(axis);
    float c = cosf(rad);
    float s = sinf(rad);
    float t = 1 - c;

    float x = normAxis.x;
    float y = normAxis.y;
    float z = normAxis.z;

    float3x3 m;

    m.r0.x = (t * x * x) + c;
    m.r0.y = (t * x * y) + s * z;
    m.r0.z = (t * x * z) - s * y;

    m.r1.x = (t * x * y) - (s * z);
    m.r1.y = (t * y * y) + c;
    m.r1.z = (t * y * z) + (s * x);

    m.r2.x = (t * x * z) + (s * y);
    m.r2.y = (t * y * z) - (s * x);
    m.r2.z = (t * z * z) + c;

    return m;
}

inline float3x3 float3x3Scale(float x, float y, float z)
{
    return float3x3(x,0,0,
                    0,y,0,
                    0,0,z);
}

/**************************************\
float4x4
\**************************************/
struct float4x4
{
    struct
    {
        float4 r0;
        float4 r1;
        float4 r2;
        float4 r3;
    };

    /***************************************\
    |   Constructors                        |
    \***************************************/
    float4x4() {}
    explicit float4x4(float f) : r0(f), r1(f), r2(f), r3(f) { }
    explicit float4x4(float* f) : r0(f+0), r1(f+4), r2(f+8), r3(f+12) { }
    float4x4(const float4 &_r0, const float4 &_r1, const float4 &_r2, const float4 &_r3) : r0(_r0), r1(_r1), r2(_r2), r3(_r3) { }
    float4x4(float _m00, float _m01, float _m02, float _m03,
             float _m10, float _m11, float _m12, float _m13,
             float _m20, float _m21, float _m22, float _m23,
             float _m30, float _m31, float _m32, float _m33)
             : r0(_m00,_m01,_m02,_m03)
             , r1(_m10,_m11,_m12,_m13)
             , r2(_m20,_m21,_m22,_m23)
             , r3(_m30,_m31,_m32,_m33)
    {
    }
    float4x4(const float3x3 &m)
        : r0(m.r0, 0.0f)
        , r1(m.r1, 0.0f)
        , r2(m.r2, 0.0f)
        , r3(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }
    const float4x4 &operator=(const float4x4 &m)
    {
        r0 = m.r0;
        r1 = m.r1;
        r2 = m.r2;
        r3 = m.r3;
        return *this;
    }

    /***************************************\
    |   Basic math operations               |
    \***************************************/

    #define MTX4_INDEX(f,r,c) ((f)[(r*4)+c])
    inline float4x4 operator*(const float4x4 &r) const
    {
        float4x4 m(1,0,0,0,
                   0,1,0,0,
                   0,0,1,0,
                   0,0,0,1);

        const float* right     = (const float*)&this->r0;
        const float* left    = (const float*)&r.r0;
        float* result   = (float*)&m;

        int ii, jj, kk;
        for(ii=0; ii<4; ++ii) /* row */
        {
            for(jj=0; jj<4; ++jj) /* column */
            {
                float sum = MTX4_INDEX(left,ii,0)*MTX4_INDEX(right,0,jj);
                for(kk=1; kk<4; ++kk)
                {
                    sum += (MTX4_INDEX(left,ii,kk)*MTX4_INDEX(right,kk,jj));
                }
                MTX4_INDEX(result,ii,jj) = sum;
            }
        }
        return m;
    }
    #undef MTX4_INDEX

    inline float4 operator*(const float4 &v) const
    {
        float4 a;

        a.x = (r0*v).hadd();
        a.y = (r1*v).hadd();
        a.z = (r2*v).hadd();
        a.w = (r3*v).hadd();

        return a;
    }

    /***************************************\
    |   Basic math operations with scalars  |
    \***************************************/
    float4x4 operator+(float f) const
    {
        return float4x4(r0+f, r1+f, r2+f, r3+f);
    }
    const float4x4 &operator+=(float f)
    {
        r0 += f;
        r1 += f;
        r2 += f;
        r3 += f;
        return *this;
    }
    float4x4 operator-(float f) const
    {
        return float4x4(r0-f, r1-f, r2-f, r3-f);
    }
    const float4x4 &operator-=(float f)
    {
        r0 -= f;
        r1 -= f;
        r2 -= f;
        r3 -= f;
        return *this;
    }
    float4x4 operator*(float f) const
    {
        return float4x4(r0*f, r1*f, r2*f, r3*f);
    }
    const float4x4 &operator*=(float f)
    {
        r0 *= f;
        r1 *= f;
        r2 *= f;
        r3 *= f;
        return *this;
    }
    float4x4 operator/(float f) const
    {
        return float4x4(r0/f, r1/f, r2/f, r3/f);
    }
    const float4x4 &operator/=(float f)
    {
        r0 /= f;
        r1 /= f;
        r2 /= f;
        r3 /= f;
        return *this;
    }

    /***************************************\
    |   Other math                          |
    \***************************************/
    // Equality
    bool operator==(const float4x4 &r) const
    {
        return r0 == r.r0 && r1 == r.r1 && r2 == r.r2 && r3 == r.r3;
    }
    bool operator!=(const float4x4 &r) const
    {
        return !(*this == r);
    }

    float determinant() const
    {
        float det = 0.0f;

        float3x3 a( r1.y,r1.z,r1.w,
                    r2.y,r2.z,r2.w,
                    r3.y,r3.z,r3.w);

        float3x3 b( r1.x,r1.z,r1.w,
                    r2.x,r2.z,r2.w,
                    r3.x,r3.z,r3.w);

        float3x3 c( r1.x,r1.y,r1.w,
                    r2.x,r2.y,r2.w,
                    r3.x,r3.y,r3.w);

        float3x3 d( r1.x,r1.y,r1.z,
                    r2.x,r2.y,r2.z,
                    r3.x,r3.y,r3.z);


        det += r0.x * a.determinant();

        det -= r0.y * b.determinant();

        det += r0.z * c.determinant();

        det -= r0.w * d.determinant();

        return det;
    }

    void transpose(void)
    {
        Swap(r0.y, r1.x);
        Swap(r0.z, r2.x);
        Swap(r0.w, r3.x);
        Swap(r1.z, r2.y);
        Swap(r1.w, r3.y);
        Swap(r2.w, r3.z);
    }

    void invert(void)
    {
        float4x4 ret;
        float recip;

        /* temp matrices */

        /* row 1 */
        float3x3 a( r1.y,r1.z,r1.w,
                    r2.y,r2.z,r2.w,
                    r3.y,r3.z,r3.w);

        float3x3 b( r1.x,r1.z,r1.w,
                    r2.x,r2.z,r2.w,
                    r3.x,r3.z,r3.w);

        float3x3 c( r1.x,r1.y,r1.w,
                    r2.x,r2.y,r2.w,
                    r3.x,r3.y,r3.w);

        float3x3 d( r1.x,r1.y,r1.z,
                    r2.x,r2.y,r2.z,
                    r3.x,r3.y,r3.z);

        /* row 2 */
        float3x3 e( r0.y,r0.z,r0.w,
                    r2.y,r2.z,r2.w,
                    r3.y,r3.z,r3.w);

        float3x3 f( r0.x,r0.z,r0.w,
                    r2.x,r2.z,r2.w,
                    r3.x,r3.z,r3.w);

        float3x3 g( r0.x,r0.y,r0.w,
                    r2.x,r2.y,r2.w,
                    r3.x,r3.y,r3.w);

        float3x3 h( r0.x,r0.y,r0.z,
                    r2.x,r2.y,r2.z,
                    r3.x,r3.y,r3.z);


        /* row 3 */
        float3x3 i( r0.y,r0.z,r0.w,
                    r1.y,r1.z,r1.w,
                    r3.y,r3.z,r3.w);

        float3x3 j( r0.x,r0.z,r0.w,
                    r1.x,r1.z,r1.w,
                    r3.x,r3.z,r3.w);

        float3x3 k( r0.x,r0.y,r0.w,
                    r1.x,r1.y,r1.w,
                    r3.x,r3.y,r3.w);

        float3x3 l( r0.x,r0.y,r0.z,
                    r1.x,r1.y,r1.z,
                    r3.x,r3.y,r3.z);


        /* row 4 */
        float3x3 m( r0.y, r0.z, r0.w,
                    r1.y, r1.z, r1.w,
                    r2.y, r2.z, r2.w);

        float3x3 n( r0.x, r0.z, r0.w,
                    r1.x, r1.z, r1.w,
                    r2.x, r2.z, r2.w);

        float3x3 o( r0.x,r0.y,r0.w,
                    r1.x,r1.y,r1.w,
                    r2.x,r2.y,r2.w);

        float3x3 p( r0.x,r0.y,r0.z,
                    r1.x,r1.y,r1.z,
                    r2.x,r2.y,r2.z);

        /* row 1 */
        ret.r0.x = a.determinant();

        ret.r0.y = -b.determinant();

        ret.r0.z = c.determinant();

        ret.r0.w = -d.determinant();

        /* row 2 */
        ret.r1.x = -e.determinant();

        ret.r1.y = f.determinant();

        ret.r1.z = -g.determinant();

        ret.r1.w = h.determinant();

        /* row 3 */
        ret.r2.x = i.determinant();

        ret.r2.y = -j.determinant();

        ret.r2.z = k.determinant();

        ret.r2.w = -l.determinant();

        /* row 4 */
        ret.r3.x = -m.determinant();

        ret.r3.y = n.determinant();

        ret.r3.z = -o.determinant();

        ret.r3.w = p.determinant();

        ret.transpose();
        recip = 1.0f/determinant();
        ret *= recip;

        *this = ret;
    }

    // Axis access
    float3 getXAxis(void) const
    {
        return r0;
    }
    float3 getYAxis(void) const
    {
        return r1;
    }
    float3 getZAxis(void) const
    {
        return r2;
    }
    float3 getPosition(void) const
    {
        return float3(r3);
    }

    void orthonormalize(void)
    {
        float3 x = getXAxis();
        float3 y = getYAxis();
        float3 z;

        x.normalize();
        z = normalize(cross3(x, y));
        y = normalize(cross3(z, x));

        r0 = float4(x, 0.0f);
        r1 = float4(y, 0.0f);
        r2 = float4(z, 0.0f);
    }
};

inline float3x3::float3x3(const float4x4 &m)
    : r0(m.r0)
    , r1(m.r1)
    , r2(m.r2)
{
}
inline float determinant(const float4x4&m)
{
    return m.determinant();
}
inline float4x4 float4x4Identity(void)
{
    return float4x4(1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    0,0,0,1);
}
inline float4x4 transpose(const float4x4 &m)
{
    float4x4 ret = m;
    ret.transpose();
    return ret;
}

inline float4x4 inverse(const float4x4 &m)
{
    float4x4 ret = m;
    ret.invert();
    return ret;
}

inline float4x4 float4x4RotationX(float rad)
{
    float       c = cosf( rad );
    float       s = sinf( rad );
    float4x4     m( 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f,    c,    s, 0.0f,
                    0.0f,   -s,    c, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
    return m;
}
inline float4x4 float4x4RotationY(float rad)
{
    float       c = cosf( rad );
    float       s = sinf( rad );
    float4x4     m(    c, 0.0f,   -s, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                       s, 0.0f,    c, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
    return m;
}
inline float4x4 float4x4RotationZ(float rad)
{

    float       c = cosf( rad );
    float       s = sinf( rad );
    float4x4     m(    c,    s, 0.0f, 0.0f,
                      -s,    c, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
    return m;
}

inline float4x4 float4x4RotationAxis(const float3 &axis, float rad )
{
    float3 normAxis = normalize(axis);
    float c = cosf(rad);
    float s = sinf(rad);
    float t = 1 - c;

    float x = normAxis.x;
    float y = normAxis.y;
    float z = normAxis.z;

    float4x4 m = float4x4Identity();

    m.r0.x = (t * x * x) + c;
    m.r0.y = (t * x * y) + s * z;
    m.r0.z = (t * x * z) - s * y;

    m.r1.x = (t * x * y) - (s * z);
    m.r1.y = (t * y * y) + c;
    m.r1.z = (t * y * z) + (s * x);

    m.r2.x = (t * x * z) + (s * y);
    m.r2.y = (t * y * z) - (s * x);
    m.r2.z = (t * z * z) + c;

    return m;
}

inline float4x4 float4x4Scale(float x, float y, float z)
{
    return float4x4(x,0,0,0,
                    0,y,0,0,
                    0,0,z,0,
                    0,0,0,1);
}

inline float4x4 float4x4Translation(float x, float y, float z)
{
    float4x4 m = float4x4Identity();
    m.r3.x = x;
    m.r3.y = y;
    m.r3.z = z;
    return m;
}
inline float4x4 float4x4Translation(const float3 &v)
{
    return float4x4Translation(v.x, v.y, v.z);
}

inline float4x4 float4x4PerspectiveFovLH(float fov, float aspect, float nearPlane, float farPlane)
{
    float width  = tanf(fov * 0.5f);
    float height = width / aspect;
    float diff   = farPlane-nearPlane;
    float div    = farPlane / diff;

    float4x4 m(
        1.0f/width,          0.0f,            0.0f,    0.0f,
              0.0f,   1.0f/height,            0.0f,    0.0f,
              0.0f,          0.0f,            div,     1.0f,
              0.0f,          0.0f, -nearPlane*div,     0.0f
    );
    return m;
}
inline float4x4 float4x4PerspectiveFovRH(float fov, float aspect, float nearPlane, float farPlane)
{
    float4x4 m = float4x4Identity();

    float f = tanf(kPiDiv2 - (fov/2.0f));
    float diff = nearPlane-farPlane;
    float div = farPlane / diff;

    m.r0.x = f/aspect;
    m.r1.y = f;
    m.r2.z = div;
    m.r2.w = -1;
    m.r3.z = nearPlane * div;
    m.r3.w = 0;
    return m;
}
inline float4x4 float4x4PerspectiveLH(float width, float height, float nearPlane, float farPlane)
{
    float4x4 m = float4x4Identity();

    m.r0.x = 2*nearPlane/width;
    m.r1.y = 2*nearPlane/height;
    m.r2.z = farPlane/(farPlane-nearPlane);
    m.r2.w = 1;
    m.r3.z = nearPlane*farPlane/(nearPlane-farPlane);
    m.r3.w = 0;
    return m;
}
inline float4x4 float4x4PerspectiveRH(float width, float height, float nearPlane, float farPlane)
{
    float4x4 m = float4x4Identity();

    m.r0.x = 2*nearPlane/width;
    m.r1.y = 2*nearPlane/height;
    m.r2.z = farPlane/(nearPlane-farPlane);
    m.r2.w = -1;
    m.r3.z = nearPlane*farPlane/(nearPlane-farPlane);
    m.r3.w = 0;
    return m;
}
inline float4x4 float4x4OrthographicOffCenterLH(float left, float right, float top, float bottom, float nearPlane, float farPlane)
{
    float4x4 m = float4x4Identity();

    float diff = farPlane-nearPlane;

    m.r0.x = 2.0f/(right-left);
    m.r1.y = 2.0f/(top-bottom);
    m.r2.z = 1.0f/diff;
    m.r3.x = -((left+right)/(right-left));
    m.r3.y = -((top+bottom)/(top-bottom));
    m.r3.z = -nearPlane/diff;

    return m;
}
inline float4x4 float4x4OrthographicOffCenterRH(float left, float right, float top, float bottom, float nearPlane, float farPlane)
{
    float4x4 m = float4x4Identity();
    float diff = nearPlane-farPlane;

    m.r0.x = 2.0f/(right-left);
    m.r1.y = 2.0f/(top-bottom);
    m.r2.z = 1.0f/diff;
    m.r3.x = -((left+right)/(right-left));
    m.r3.y = -((top+bottom)/(top-bottom));
    m.r3.z = nearPlane/diff;

    return m;
}
inline float4x4 float4x4OrthographicLH(float width, float height, float nearPlane, float farPlane)
{
    float halfWidth = width/2.0f;
    float halfHeight = height/2.0f;

    return float4x4OrthographicOffCenterLH(-halfWidth, halfWidth, halfHeight, -halfHeight, nearPlane, farPlane);
}
inline float4x4 float4x4OrthographicRH(float width, float height, float nearPlane, float farPlane)
{
    float halfWidth = width/2.0f;
    float halfHeight = height/2.0f;

    return float4x4OrthographicOffCenterRH(-halfWidth, halfWidth, halfHeight, -halfHeight, nearPlane, farPlane);
}

/**************************************\
Quaternion
\**************************************/
struct quaternion : public float4
{

    /***************************************\
    |   Constructors                        |
    \***************************************/
    quaternion() {}
    explicit quaternion(float f) : float4(f) { }
    explicit quaternion(float _x, float _y, float _z, float _w) : float4(_x,_y,_z,_w) { }
    explicit quaternion(float* f) : float4(f) { }
    quaternion(const quaternion &v) : float4(v) { }
    quaternion(const float3 &v, float _w)
    {
        float3  norm = ::normalize(v);
        float   a = _w*0.5f;
        float   s = sinf(a);
        x = norm.x*s;
        y = norm.y*s;
        z = norm.z*s;
        w = cosf(a);
    }
    const quaternion &operator=(const quaternion &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    /* Methods */
    float3 getXAxis(void) const
    {
        float3 ret( 1-2*(y*y+z*z),
                      2*(x*y+w*z),
                      2*(x*z-y*w));
        return ret;
    }
    float3 getYAxis(void) const
    {
        float3 ret(   2*(x*y-z*w),
                    1-2*(x*x+z*z),
                      2*(y*z+x*w));
        return ret;
    }
    float3 getZAxis(void) const
    {
        float3 ret(   2*(x*z+y*w),
                      2*(y*z-x*w),
                    1-2*(x*x+y*y));
        return ret;
    }

    float3x3 getMatrix(void) const
    {
        quaternion q = *this;
        q.normalize();
        float xx = q.x * q.x;
        float yy = q.y * q.y;
        float zz = q.z * q.z;

        float xy = q.x * q.y;
        float zw = q.z * q.w;
        float xz = q.x * q.z;
        float yw = q.y * q.w;
        float yz = q.y * q.z;
        float xw = q.x * q.w;

        float3x3 ret(1 - 2*(yy+zz),     2*(xy+zw),     2*(xz-yw),
                         2*(xy-zw), 1 - 2*(xx+zz),     2*(yz+xw),
                         2*(xz+yw),     2*(yz-xw), 1 - 2*(xx+yy) );
        return ret;
    }

    quaternion conjugate(void) const
    {
        return quaternion(-x, -y, -z, w);
    }
    quaternion inverse(void) const
    {
        // Note: Only normalized quaternions are supportted at the moment
        quaternion q = *this;
        q.normalize();
        return q.conjugate();
    }
};

inline quaternion quaternionMultiply(const quaternion &l, const quaternion &r)
{
    quaternion q(   r.w*l.x + r.x*l.w + r.y*l.z - r.z*l.y,
                    r.w*l.y + r.y*l.w + r.z*l.x - r.x*l.z,
                    r.w*l.z + r.z*l.w + r.x*l.y - r.y*l.x,
                    r.w*l.w - r.x*l.x - r.y*l.y - r.z*l.z );
    return q;
}

inline quaternion quaternionIdentity(void)
{
    return quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

inline float3 Min( float3 &v0, float3 &v1 )
{
    float3 result;
    result.x = v0.x < v1.x ? v0.x : v1.x;
    result.y = v0.y < v1.y ? v0.y : v1.y;
    result.z = v0.z < v1.z ? v0.z : v1.z;
    return result;
}

inline float3 Max( float3 &v0, float3 &v1 )
{
    float3 result;
    result.x = v0.x > v1.x ? v0.x : v1.x;
    result.y = v0.y > v1.y ? v0.y : v1.y;
    result.z = v0.z > v1.z ? v0.z : v1.z;
    return result;
}

inline float4 Min( float4 &v0, float4 &v1 )
{
    float4 result;
    result.x = v0.x < v1.x ? v0.x : v1.x;
    result.y = v0.y < v1.y ? v0.y : v1.y;
    result.z = v0.z < v1.z ? v0.z : v1.z;
    result.w = v0.w < v1.w ? v0.w : v1.w;
    return result;
}

inline float4 Max( float4 &v0, float4 &v1 )
{
    float4 result;
    result.x = v0.x > v1.x ? v0.x : v1.x;
    result.y = v0.y > v1.y ? v0.y : v1.y;
    result.z = v0.z > v1.z ? v0.z : v1.z;
    result.w = v0.w > v1.w ? v0.w : v1.w;
    return result;
}

inline float4 operator*(const float4 &v, const float4x4 &m)
{
    float4 result;

    result  = m.r0 * v.x;
    result += m.r1 * v.y;
    result += m.r2 * v.z;
    result += m.r3 * v.w;

    return result;
}

/*
============================
CreateLookAtMatrix
============================
*/
inline float4x4 CreateLookAtMatrix(float3 cameraPosition, float3 cameraTarget, float3 cameraUpVector)
{
	float3 vector3_1 = (cameraPosition - cameraTarget).normalize();
	float3 vector3_2;
	
	vector3_2.Cross(cameraUpVector, vector3_1);
	vector3_2.NormalizeSelf();

	float3 vector1;
	vector1.Cross(vector3_1, vector3_2);

	float4x4 matrix;
	matrix.r0.x = vector3_2.x;
	matrix.r0.y = vector1.x;
	matrix.r0.z = vector3_1.x;
	matrix.r0.w = 0.0f;
	matrix.r1.x = vector3_2.y;
	matrix.r1.y = vector1.y;
	matrix.r1.z = vector3_1.y;
	matrix.r1.w = 0.0f;
	matrix.r2.x = vector3_2.z;
	matrix.r2.y = vector1.z;
	matrix.r2.z = vector3_1.z;
	matrix.r2.w = 0.0f;
	matrix.r3.x = -float3::Dot(vector3_2, cameraPosition);
	matrix.r3.y = -float3::Dot(vector1, cameraPosition);
	matrix.r3.z = -float3::Dot(vector3_1, cameraPosition);
	matrix.r3.w = 1.0f;

	return matrix;
}

__inline float3 abs(const float3 xyz)
{
	float3 result(fabs(xyz.x), fabs(xyz.y), fabs(xyz.z));
	return result;
}


__inline float4 abs(const float4 xyz)
{
	float4 result(fabs(xyz.x), fabs(xyz.y), fabs(xyz.z), fabs(xyz.w));
	return result;
}

__inline float3 fabs(const float3 xyz)
{
	float3 result(fabs(xyz.x), fabs(xyz.y), fabs(xyz.z));
	return result;
}


__inline float4 fabs(const float4 xyz)
{
	float4 result(fabs(xyz.x), fabs(xyz.y), fabs(xyz.z), fabs(xyz.w));
	return result;
}

static const float Identity[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};

__inline void _math_matrix_rotate(float4x4 &matrix, float angle, float x, float y, float z)
{
	float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;
	float m[16];
	bool optimized;
	#define M_PI 3.14159265358979323846
	s = sinf(angle * M_PI / 180.0);
	c = cosf(angle * M_PI / 180.0);

	memcpy(m, Identity, sizeof(Identity));
	optimized = false;

#define M(row,col)  m[col*4+row]

	if (x == 0.0F) {
		if (y == 0.0F) {
			if (z != 0.0F) {
				optimized = true;
				/* rotate only around z-axis */
				M(0, 0) = c;
				M(1, 1) = c;
				if (z < 0.0F) {
					M(0, 1) = s;
					M(1, 0) = -s;
				}
				else {
					M(0, 1) = -s;
					M(1, 0) = s;
				}
			}
		}
		else if (z == 0.0F) {
			optimized = true;
			/* rotate only around y-axis */
			M(0, 0) = c;
			M(2, 2) = c;
			if (y < 0.0F) {
				M(0, 2) = -s;
				M(2, 0) = s;
			}
			else {
				M(0, 2) = s;
				M(2, 0) = -s;
			}
		}
	}
	else if (y == 0.0F) {
		if (z == 0.0F) {
			optimized = true;
			/* rotate only around x-axis */
			M(1, 1) = c;
			M(2, 2) = c;
			if (x < 0.0F) {
				M(1, 2) = s;
				M(2, 1) = -s;
			}
			else {
				M(1, 2) = -s;
				M(2, 1) = s;
			}
		}
	}

	if (!optimized) {
		const float mag = sqrtf(x * x + y * y + z * z);

		if (mag <= 1.0e-4F) {
			/* no rotation, leave mat as-is */
			return;
		}

		x /= mag;
		y /= mag;
		z /= mag;


		/*
		*     Arbitrary axis rotation matrix.
		*
		*  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
		*  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
		*  (which is about the X-axis), and the two composite transforms
		*  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
		*  from the arbitrary axis to the X-axis then back.  They are
		*  all elementary rotations.
		*
		*  Rz' is a rotation about the Z-axis, to bring the axis vector
		*  into the x-z plane.  Then Ry' is applied, rotating about the
		*  Y-axis to bring the axis vector parallel with the X-axis.  The
		*  rotation about the X-axis is then performed.  Ry and Rz are
		*  simply the respective inverse transforms to bring the arbitrary
		*  axis back to its original orientation.  The first transforms
		*  Rz' and Ry' are considered inverses, since the data from the
		*  arbitrary axis gives you info on how to get to it, not how
		*  to get away from it, and an inverse must be applied.
		*
		*  The basic calculation used is to recognize that the arbitrary
		*  axis vector (x, y, z), since it is of unit length, actually
		*  represents the sines and cosines of the angles to rotate the
		*  X-axis to the same orientation, with theta being the angle about
		*  Z and phi the angle about Y (in the order described above)
		*  as follows:
		*
		*  cos ( theta ) = x / sqrt ( 1 - z^2 )
		*  sin ( theta ) = y / sqrt ( 1 - z^2 )
		*
		*  cos ( phi ) = sqrt ( 1 - z^2 )
		*  sin ( phi ) = z
		*
		*  Note that cos ( phi ) can further be inserted to the above
		*  formulas:
		*
		*  cos ( theta ) = x / cos ( phi )
		*  sin ( theta ) = y / sin ( phi )
		*
		*  ...etc.  Because of those relations and the standard trigonometric
		*  relations, it is pssible to reduce the transforms down to what
		*  is used below.  It may be that any primary axis chosen will give the
		*  same results (modulo a sign convention) using thie method.
		*
		*  Particularly nice is to notice that all divisions that might
		*  have caused trouble when parallel to certain planes or
		*  axis go away with care paid to reducing the expressions.
		*  After checking, it does perform correctly under all cases, since
		*  in all the cases of division where the denominator would have
		*  been zero, the numerator would have been zero as well, giving
		*  the expected result.
		*/

		xx = x * x;
		yy = y * y;
		zz = z * z;
		xy = x * y;
		yz = y * z;
		zx = z * x;
		xs = x * s;
		ys = y * s;
		zs = z * s;
		one_c = 1.0F - c;

		/* We already hold the identity-matrix so we can skip some statements */
		M(0, 0) = (one_c * xx) + c;
		M(0, 1) = (one_c * xy) - zs;
		M(0, 2) = (one_c * zx) + ys;
		/*    M(0,3) = 0.0F; */

		M(1, 0) = (one_c * xy) + zs;
		M(1, 1) = (one_c * yy) + c;
		M(1, 2) = (one_c * yz) - xs;
		/*    M(1,3) = 0.0F; */

		M(2, 0) = (one_c * zx) - ys;
		M(2, 1) = (one_c * yz) + xs;
		M(2, 2) = (one_c * zz) + c;
		/*    M(2,3) = 0.0F; */

		/*
		M(3,0) = 0.0F;
		M(3,1) = 0.0F;
		M(3,2) = 0.0F;
		M(3,3) = 1.0F;
		*/
	}
#undef M
	float4x4 _internalMatrix(&m[0]);
	//_internalMatrix.r[0] = XMVectorSet(m[0], m[1], m[2], m[3]);
	//_internalMatrix.r[1] = XMVectorSet(m[4], m[5], m[6], m[7]);
	//_internalMatrix.r[2] = XMVectorSet(m[8], m[9], m[10], m[11]);
	//_internalMatrix.r[3] = XMVectorSet(m[12], m[13], m[14], m[15]);
	float4x4 temp = matrix * _internalMatrix;
	matrix = temp;

	//matmul4(mat, mat, m);
	//matrix_multf(mat, m, MAT_FLAG_ROTATION);
}

static void glhFrustumf2(float *matrix, float left, float right, float bottom, float top, float znear, float zfar)
{
	float temp, temp2, temp3, temp4;
	temp = 2.0 * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = temp / temp2;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = temp / temp3;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = (right + left) / temp2;
	matrix[9] = (top + bottom) / temp3;
	matrix[10] = (-zfar - znear) / temp4;
	matrix[11] = -1.0;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = (-temp * zfar) / temp4;
	matrix[15] = 0.0;
}


static void glhPerspectivef2(float *matrix, float fovyInDegrees, float aspectRatio, float znear, float zfar)
{
	float ymax, xmax;
	float temp, temp2, temp3, temp4;
	ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
	//ymin = -ymax;
	//xmin = -ymax * aspectRatio;
	xmax = ymax * aspectRatio;
	glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

#endif // #ifndef __CPUTMath_h__
