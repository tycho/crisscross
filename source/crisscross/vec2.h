/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2009 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_vec2_h
#define __included_cc_vec2_h

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
		static vec2 AngleModulus(float angle, float mod)
		{ return vec2(cosf(angle) * mod, sinf(angle) * mod); }

		/* ! Fetches the X component */
		float X() const { return x; }
		/* ! Fetches the Y component */
		float Y() const { return y; }
		/* ! Fetches the X component */
		float & X(){ return x; }
		/* ! Fetches the X component */
		float & Y(){ return y; }

		/* ! Fetches the modulus of the vector (distance from the origin) */
#ifdef TARGET_OS_WINDOWS
		/* Thank you, Microsoft. >:( */
		float Modulus() const { return _hypotf(y, x); }
#else
		float Modulus() const { return hypotf(y, x); }
#endif
		/* ! Fetches the square modulus of the vector (distance from the origin squared) */
		float ModulusSquared() const { return x * x + y * y; }
		/* ! Fetches the angle of the vector, measured from the half-line y=0, x>0 anticlockwise, in radians */
		float Angle() const { return atan2f(y, x); }

		/* ! Calculates a unit vector in the direction of this vector, or a zero vector if this is a zero vector */
		vec2 UnitVector() const { if (ModulusSquared() < 0.0001f) return vec2(0.0f, 0.0f);return (*this / Modulus()); }

		vec2 operator+(const vec2 & ov2) const
		{ return vec2(x + ov2.x, y + ov2.y); }
		vec2 & operator+=(const vec2 & ov2)
		{ x += ov2.x; y += ov2.y; return *this; }
		vec2 operator-(const vec2 & ov2) const
		{ return vec2(x - ov2.x, y - ov2.y); }
		vec2 & operator-=(const vec2 & ov2)
		{ x -= ov2.x; y -= ov2.y; return *this; }
		float operator*(const vec2 & ov2) const /* dot product */
		{ return x * ov2.x + y * ov2.y; }
		vec2 operator*(float factor) const
		{ return vec2(x * factor, y * factor); }
		vec2 & operator*=(float factor)
		{ x *= factor; y *= factor; return *this; }
		vec2 operator/(float factor) const
		{ return vec2(x / factor, y / factor); }
		vec2 & operator/=(float factor)
		{ x /= factor; y /= factor; return *this; }
		vec2 operator-() const
		{ return vec2(-x, -y); }

		/* ! Calculates the distance between two vectors */
		static float Distance(const vec2 & v1, const vec2 & v2)
		{ return (v2 - v1).Modulus(); }
		/* ! Calculates the squared distance between two vectors */
		static float DistanceSquared(const vec2 & v1, const vec2 & v2)
		{ return (v2 - v1).ModulusSquared(); }
};

#endif
