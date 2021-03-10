/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2021 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_vec2_h
#define __included_cc_vec2_h

#include <crisscross/cc_attr.h>
#include <math.h>

/* ! A 2-dimensional vector */
class vec2
{
	private:
		float x, y;
	public:
		/* ! Generic constructor */
		vec2() : x(0.0f), y(0.0f)
		{
		}
		/* ! Specific constructor */
		/*!
		 * \param _x The x component
		 * \param _y The y component
		 */
		vec2(float _x, float _y) : x(_x), y(_y)
		{
		}

		/* ! Constructs a vector from an angle and a modulus */
		/*!
		 * \param angle The angle from the half-line y=0, x>0, anticlockwise, in radians
		 * \param mod The distance from the origin
		 * \return A new vec2
		 */
		__forceinline static vec2 AngleModulus(float angle, float mod)
		{ return vec2(cosf(angle) * mod, sinf(angle) * mod); }

		/* ! Fetches the X component */
		__forceinline float X() const { return x; }
		/* ! Fetches the Y component */
		__forceinline float Y() const { return y; }
		/* ! Fetches the X component */
		__forceinline float & X(){ return x; }
		/* ! Fetches the X component */
		__forceinline float & Y(){ return y; }

		/* ! Fetches the modulus of the vector (distance from the origin) */
#if defined(_MSC_VER) && _MSC_VER >= 1400
		/* Thank you, Microsoft. >:( */
		__forceinline float Modulus() const { return _hypotf(y, x); }
#elif defined(_MSC_VER)
		__forceinline float Modulus() const { return sqrtf(ModulusSquared()); }
#else
		__forceinline float Modulus() const { return hypotf(y, x); }
#endif
		/* ! Fetches the square modulus of the vector (distance from the origin squared) */
		__forceinline float ModulusSquared() const { return x * x + y * y; }
		/* ! Fetches the angle of the vector, measured from the half-line y=0, x>0 anticlockwise, in radians */
		__forceinline float Angle() const { return atan2f(y, x); }

		/* ! Calculates a unit vector in the direction of this vector, or a zero vector if this is a zero vector */
		__forceinline vec2 UnitVector() const { if (ModulusSquared() < 0.0001f) return vec2(0.0f, 0.0f);return (*this / Modulus()); }

		__forceinline vec2 operator+(const vec2 & ov2) const
		{ return vec2(x + ov2.x, y + ov2.y); }
		__forceinline vec2 & operator+=(const vec2 & ov2)
		{ x += ov2.x; y += ov2.y; return *this; }
		__forceinline vec2 operator-(const vec2 & ov2) const
		{ return vec2(x - ov2.x, y - ov2.y); }
		__forceinline vec2 & operator-=(const vec2 & ov2)
		{ x -= ov2.x; y -= ov2.y; return *this; }
		__forceinline float operator*(const vec2 & ov2) const /* dot product */
		{ return x * ov2.x + y * ov2.y; }
		__forceinline vec2 operator*(float factor) const
		{ return vec2(x * factor, y * factor); }
		__forceinline vec2 & operator*=(float factor)
		{ x *= factor; y *= factor; return *this; }
		__forceinline vec2 operator/(float factor) const
		{ return vec2(x / factor, y / factor); }
		__forceinline vec2 & operator/=(float factor)
		{ x /= factor; y /= factor; return *this; }
		__forceinline vec2 operator-() const
		{ return vec2(-x, -y); }

		/* ! Calculates the distance between two vectors */
		__forceinline static float Distance(const vec2 & v1, const vec2 & v2)
		{ return (v2 - v1).Modulus(); }
		/* ! Calculates the squared distance between two vectors */
		__forceinline static float DistanceSquared(const vec2 & v1, const vec2 & v2)
		{ return (v2 - v1).ModulusSquared(); }
};

static_assert(std::is_trivially_copyable<vec2>::value);

#endif
