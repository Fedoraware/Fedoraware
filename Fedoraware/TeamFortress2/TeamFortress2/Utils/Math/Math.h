#pragma once

#include <math.h>
#include <cmath>		//std::isfinite std::remainder
#include <cfloat>		//FLT_EPSILON
#include <algorithm>	//std::min std::max

#define M_RADPI		57.295779513082
#define PI			3.14159265358979323846
#define DEG2RAD(x)	((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)	((float)(x) * (float)(180.0f / (float)(PI)))

#pragma warning (disable : 26495)
#pragma warning (disable : 26451)
#pragma warning (disable : 4244)

#undef max
#undef min



class Vec2
{
public:
	Vec2(void)
	{
		x = y = 0.0f;
	}

	Vec2(float X, float Y)
	{
		x = X; y = Y;
	}

	Vec2(float* v)
	{
		x = v[0]; y = v[1];
	}

	Vec2(const float* v)
	{
		x = v[0]; y = v[1];
	}

	Vec2(const Vec2& v)
	{
		x = v.x; y = v.y;
	}

	Vec2& operator=(const Vec2& v)
	{
		x = v.x; y = v.y; return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	Vec2& operator+=(const Vec2& v)
	{
		x += v.x; y += v.y; return *this;
	}

	Vec2& operator-=(const Vec2& v)
	{
		x -= v.x; y -= v.y; return *this;
	}

	Vec2& operator*=(const Vec2& v)
	{
		x *= v.x; y *= v.y; return *this;
	}

	Vec2& operator/=(const Vec2& v)
	{
		x /= v.x; y /= v.y; return *this;
	}

	Vec2& operator+=(float v)
	{
		x += v; y += v; return *this;
	}

	Vec2& operator-=(float v)
	{
		x -= v; y -= v; return *this;
	}

	Vec2& operator*=(float v)
	{
		x *= v; y *= v; return *this;
	}

	Vec2& operator/=(float v)
	{
		x /= v; y /= v; return *this;
	}

	Vec2 operator+(const Vec2& v) const
	{
		return Vec2(x + v.x, y + v.y);
	}

	Vec2 operator-(const Vec2& v) const
	{
		return Vec2(x - v.x, y - v.y);
	}

	Vec2 operator*(const Vec2& v) const
	{
		return Vec2(x * v.x, y * v.y);
	}

	Vec2 operator/(const Vec2& v) const
	{
		return Vec2(x / v.x, y / v.y);
	}

	Vec2 operator+(float v) const
	{
		return Vec2(x + v, y + v);
	}

	Vec2 operator-(float v) const
	{
		return Vec2(x - v, y - v);
	}

	Vec2 operator*(float v) const
	{
		return Vec2(x * v, y * v);
	}

	Vec2 operator/(float v) const
	{
		return Vec2(x / v, y / v);
	}

	void Set(float X = 0.0f, float Y = 0.0f)
	{
		x = X; y = Y;
	}

	float Length(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float LengthSqr(void) const
	{
		return (x * x + y * y);
	}

	float DistTo(const Vec2& v) const
	{
		return (*this - v).Length();
	}

	float DistToSqr(const Vec2& v) const
	{
		return (*this - v).LengthSqr();
	}

	float Dot(const Vec2& v) const
	{
		return (x * v.x + y * v.y);
	}

	bool IsZero(void) const
	{
		return (x > -0.01f && x < 0.01f &&
				y > -0.01f && y < 0.01f);
	}

public:
	float x, y;
};

class Vec3
{
public:
	Vec3(void)
	{
		x = y = z = 0.0f;
	}

	void Zero()
	{
		x = y = z = 0.f;
	}

	Vec3(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}

	__forceinline void Clear() {
		x = y = z = 0.f;
	}

	Vec3(float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	Vec3(const float* v)
	{
		x = v[0]; y = v[1]; z = v[2];
	}

	Vec3(const Vec3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	Vec3(const Vec2& v)
	{
		x = v.x; y = v.y; z = 0.0f;
	}

	Vec3& operator=(const Vec3& v)
	{
		x = v.x; y = v.y; z = v.z; return *this;
	}

	Vec3& operator=(const Vec2& v)
	{
		x = v.x; y = v.y; z = 0.0f; return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	Vec3& operator+=(const Vec3& v)
	{
		x += v.x; y += v.y; z += v.z; return *this;
	}

	Vec3& operator-=(const Vec3& v)
	{
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	Vec3& operator*=(const Vec3& v)
	{
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	Vec3& operator/=(const Vec3& v)
	{
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	Vec3& operator+=(float v)
	{
		x += v; y += v; z += v; return *this;
	}

	Vec3& operator-=(float v)
	{
		x -= v; y -= v; z -= v; return *this;
	}

	Vec3& operator*=(float v)
	{
		x *= v; y *= v; z *= v; return *this;
	}

	Vec3& operator/=(float v)
	{
		x /= v; y /= v; z /= v; return *this;
	}

	Vec3 operator+(const Vec3& v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}

	Vec3 operator-(const Vec3& v) const
	{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}

	Vec3 operator*(const Vec3& v) const
	{
		return Vec3(x * v.x, y * v.y, z * v.z);
	}

	Vec3 operator/(const Vec3& v) const
	{
		return Vec3(x / v.x, y / v.y, z / v.z);
	}

	Vec3 operator+(float v) const
	{
		return Vec3(x + v, y + v, z + v);
	}

	Vec3 operator-(float v) const
	{
		return Vec3(x - v, y - v, z - v);
	}

	Vec3 operator*(float v) const
	{
		return Vec3(x * v, y * v, z * v);
	}

	Vec3 operator/(float v) const
	{
		return Vec3(x / v, y / v, z / v);
	}

	void Set(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
	{
		x = X; y = Y; z = Z;
	}

	float Length(void) const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float LengthSqr(void) const
	{
		return (x * x + y * y + z * z);
	}

	float Normalize()
	{
		float fl_length = Length();
		float fl_length_normal = 1.f / (FLT_EPSILON + fl_length);

		x = x * fl_length_normal;
		y = y * fl_length_normal;
		z = z * fl_length_normal;

		return fl_length;
	}

	Vec3 toAngle() const noexcept
	{
		return Vec3{ RAD2DEG(atan2(-z, hypot(x, y))),
					 RAD2DEG(atan2(y, x)),
					 0.0f };
	}

	float const* Base() const
	{
		return (float const*)this;
	}

	//-----------------------------------------------------------------------------
	inline float* Base()
	{
		return (float*)this;
	}


	Vec3 fromAngle() const noexcept
	{
		return Vec3{ cos(DEG2RAD(x)) * cos(DEG2RAD(y)),
					 cos(DEG2RAD(x)) * sin(DEG2RAD(y)),
					 -sin(DEG2RAD(x)) };
	}

	void Rotate2D(const float& flYaw)
	{
		float s, c, r = DEG2RAD(flYaw);
		s = sin(r); c = cos(r);

		float _x = x, _y = y;

		x = (_x * c) - (_y * s);
		y = (_x * s) + (_y * c);
	}

	float NormalizeInPlace()
	{
		return Normalize();
	}

	float Length2D(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float Length2DSqr(void) const
	{
		return (x * x + y * y);
	}

	float DistTo(const Vec3& v) const
	{
		return (*this - v).Length();
	}

	float DistToSqr(const Vec3& v) const
	{
		return (*this - v).LengthSqr();
	}

	float Dist2D(const Vec3& v) const
	{
		return (*this - v).Length2D();
	}

	float Dot(const Vec3& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}

	Vec3 Cross(const Vec3& v) const
	{
		return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	bool IsZero(void) const
	{
		return (x > -0.01f && x < 0.01f &&
				y > -0.01f && y < 0.01f &&
				z > -0.01f && z < 0.01f);
	}

	Vec3 Scale(float fl) {
		return Vec3(x * fl, y * fl, z * fl);
	}

	void Init(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
	}

public:
	float x, y, z;
};

class __declspec(align(16))VectorAligned : public Vec3
{
public:
	inline VectorAligned(void) { };

	inline VectorAligned(float x, float y, float z) {
		Set(x, y, z);
	}

	explicit VectorAligned(const Vec3& othr) {
		Set(othr.x, othr.y, othr.z);
	}

	VectorAligned& operator=(const Vec3& othr) {
		Set(othr.x, othr.y, othr.z);
		return *this;
	}

	float w;
};

typedef float matrix3x4[3][4];

struct cplane_t
{
	Vec3 normal;
	float dist;
	unsigned char	 type;
	unsigned char	 sign_bits;
	unsigned char pad[2];
};

namespace Math
{
	inline void SinCos(float radians, float *sine, float *cosine)
	{
		_asm
		{
			fld		DWORD PTR[radians]
			fsincos

			mov edx, DWORD PTR[cosine]
			mov eax, DWORD PTR[sine]

			fstp DWORD PTR[edx]
			fstp DWORD PTR[eax]
		}
	}

	__inline void SinCos(const float& r, float& s, float& c)
	{
		s = sin(r);
		c = cos(r);
	}

	inline void MatrixCopy(const matrix3x4 &source, matrix3x4 &target)
	{
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				target[i][j] = source[i][j];
			}
		}
	}

	inline void MatrixGetColumn(const matrix3x4 &src, int nCol, Vec3 *pColumn)
	{
		pColumn->x = src[0][nCol];
		pColumn->y = src[1][nCol];
		pColumn->z = src[2][nCol];
	}

	inline void MatrixSetColumn(const Vec3 &in, int column, matrix3x4 &out)
	{
		out[0][column] = in.x;
		out[1][column] = in.y;
		out[2][column] = in.z;
	}


	inline Vec3 VelocityToAngles(const Vec3 &direction)
	{
		auto Magnitude = [&](const Vec3 &v) -> float {
			return sqrtf(v.Dot(v));
		};

		float yaw, pitch;

		if (direction.y == 0.0f && direction.x == 0.0f)
		{
			yaw = 0.0f;

			if (direction.z > 0.0f)
				pitch = 270.0f;

			else pitch = 90.0f;
		}

		else 
		{
			yaw = RAD2DEG(atan2f(direction.y, direction.x));
			pitch = RAD2DEG(atan2f(-direction.z, Magnitude(Vec3(direction))));

			if (yaw < 0.0f)
				yaw += 360.0f;

			if (pitch < 0.0f)
				pitch += 360.0f;
		}

		return { pitch, yaw, 0.0f };
	}

	inline void ConcatTransforms(const matrix3x4 &in1, const matrix3x4 &in2, matrix3x4 &out)
	{
		if (&in1 == &out) {
			matrix3x4 in1b;
			MatrixCopy(in1, in1b);
			ConcatTransforms(in1b, in2, out);
			return;
		}

		if (&in2 == &out) {
			matrix3x4 in2b;
			MatrixCopy(in2, in2b);
			ConcatTransforms(in1, in2b, out);
			return;
		}

		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
			in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
			in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
			in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
			in1[0][2] * in2[2][3] + in1[0][3];
		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
			in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
			in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
			in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
			in1[1][2] * in2[2][3] + in1[1][3];
		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
			in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
			in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
			in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
			in1[2][2] * in2[2][3] + in1[2][3];
	}

	inline void AngleMatrix(const Vec3& angles, matrix3x4& matrix)
	{
		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles.y), &sy, &cy);
		SinCos(DEG2RAD(angles.x), &sp, &cp);
		SinCos(DEG2RAD(angles.z), &sr, &cr);

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = sp * crcy + srsy;
		matrix[1][2] = sp * crsy - srcy;
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}

	inline void GetMatrixOrigin(const matrix3x4& source, Vec3& target) {
		target.x = source[0][3];
		target.y = source[1][3];
		target.z = source[2][3];
	}

	inline void MatrixAngles(const matrix3x4& matrix, Vec3& angles) {
		//Vec3 forward, left, up;

		// extract the basis vectors from the matrix. since we only need the z
		// component of the up vector, we don't get x and y.
		const Vec3 forward = { matrix[0][0], matrix[1][0], matrix[2][0] };
		const Vec3 left = { matrix[0][1], matrix[1][1], matrix[2][1] };
		const Vec3 up = { 0.f, 0.f, matrix[2][2] };

		float len = forward.Length2D();

		// enough here to get angles?
		if (len > 0.001f) {
			angles.x = RAD2DEG(std::atan2(-forward.z, len));
			angles.y = RAD2DEG(std::atan2(forward.y, forward.x));
			angles.z = RAD2DEG(std::atan2(left.z, up.z));
		}

		else {
			angles.x = RAD2DEG(std::atan2(-forward.z, len));
			angles.y = RAD2DEG(std::atan2(-left.x, left.y));
			angles.z = 0.f;
		}
	}

	inline void AngleMatrix(const Vec3& angles, const Vec3& origin, matrix3x4& matrix)
	{
		AngleMatrix(angles, matrix);
		MatrixSetColumn(origin, 3, matrix);
	}

	inline void MatrixMultiply(const matrix3x4 &in1, const matrix3x4 &in2, matrix3x4 &out)
	{
		ConcatTransforms(in1, in2, out);
	}

	inline float DotProduct(const float *v1, const float *v2)
	{
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	}

	inline void VectorRotate(const float *in1, const matrix3x4 &in2, float *out)
	{
		out[0] = DotProduct(in1, in2[0]);
		out[1] = DotProduct(in1, in2[1]);
		out[2] = DotProduct(in1, in2[2]);
	}

	inline void VectorRotate(const Vec3 &in1, const matrix3x4 &in2, Vec3 &out)
	{
		VectorRotate(&in1.x, in2, &out.x);
	}

	inline void VectorRotate(const Vec3 &in1, const Vec3 &in2, Vec3 &out)
	{
		matrix3x4 matRotate;
		AngleMatrix(in2, matRotate);
		VectorRotate(in1, matRotate, out);
	}

	inline double __declspec (naked) __fastcall FastSqrt(double n)
	{
		_asm fld qword ptr[esp + 4]
			_asm fsqrt
		_asm ret 8
	}

	inline float NormalizeAngle(float ang)
	{
		if (!std::isfinite(ang))
			return 0.0f;

		return std::remainder(ang, 360.0f);
	}

	inline float NormalizeYaw(float value)
	{
		while (value > 180)
			value -= 360.f;

		while (value < -180)
			value += 360.f;
		return value;
	}

	inline void ClampAngles(Vec3& v)
	{
		v.x = std::max(-89.0f, std::min(89.0f, NormalizeAngle(v.x)));
		v.y = NormalizeAngle(v.y);
		v.z = 0.0f;
	}

	inline void VectorTransform(const Vec3& input, const matrix3x4& matrix, Vec3& output)
	{
		for (auto i = 0; i < 3; i++)
			output[i] = input.Dot((Vec3&)matrix[i]) + matrix[i][3];
	}


	inline float DistPointToLine(const Vec3 point, const Vec3 origin, const Vec3 direction) {
		const auto pointDirection = point - origin;

		const auto temporaryOffset = pointDirection.Dot(direction) / (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
		if (temporaryOffset < 0.000001f) {
			return FLT_MAX;
		}

		const auto perpendicularPoint = origin + (direction * temporaryOffset);

		return (point - perpendicularPoint).Length();
	}

	inline void AngleVectors(const Vec3& angles, Vec3* forward)
	{
		float sp, sy, cp, cy;

		SinCos(DEG2RAD(angles.x), &sp, &cp);
		SinCos(DEG2RAD(angles.y), &sy, &cy);

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}
	}

	inline void AngleVectors(const Vec3 &angles, Vec3 *forward, Vec3 *right, Vec3 *up)
	{
		float sr, sp, sy, cr, cp, cy;
		SinCos(DEG2RAD(angles.x), &sp, &cp);
		SinCos(DEG2RAD(angles.y), &sy, &cy);
		SinCos(DEG2RAD(angles.z), &sr, &cr);

		if (forward) 
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}

		if (up) 
		{
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}

	inline void AngleNormalize(Vec3& vector)
	{
		for (auto i = 0; i < 3; i++)
		{
			if (vector[i] < -180.0f) vector[i] += 360.0f;
			if (vector[i] > 180.0f) vector[i] -= 360.0f;
		}
	}

	inline float VectorNormalize(Vec3& vector)
	{
		float length = vector.Length();

		if (!length)
			vector.Set();
		else
			vector /= length;

		return length;
	}

	inline Vec3 CalcAngle(const Vec3& source, const Vec3& destination)
	{
		Vec3 angles = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 delta = (source - destination);
		float fHyp = FastSqrt((delta.x * delta.x) + (delta.y * delta.y));

		angles.x = (atanf(delta.z / fHyp) * M_RADPI);
		angles.y = (atanf(delta.y / delta.x) * M_RADPI);
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}

	inline float CalcFov(const Vec3& src, const Vec3& dst)
	{
		Vec3 v_src = Vec3();
		AngleVectors(src, &v_src);

		Vec3 v_dst = Vec3();
		AngleVectors(dst, &v_dst);

		float result = RAD2DEG(acos(v_dst.Dot(v_src) / v_dst.LengthSqr()));

		if (!isfinite(result) || isinf(result) || isnan(result))
			result = 0.0f;

		return result;
	}

	inline void CreateVector(const Vec3& angle, Vec3& vector)
	{
		float pitch, yaw, tmp;

		pitch = float(angle[0] * PI / 180);
		yaw = float(angle[1] * PI / 180);
		tmp = float(cos(pitch));

		vector[0] = float(-tmp * -cos(yaw));
		vector[1] = float(sin(yaw) * tmp);
		vector[2] = float(-sin(pitch));
	}

	inline void VectorAngles(const Vec3& forward, Vec3& angles)
	{
		float tmp, yaw, pitch;

		if (forward.y == 0 && forward.x == 0)
		{
			yaw = 0;

			if (forward.z > 0)
				pitch = 270;
			else
				pitch = 90;
		}

		else
		{
			yaw = RAD2DEG(atan2f(forward.y, forward.x));

			if (yaw < 0)
				yaw += 360;

			tmp = forward.Length2D();
			pitch = RAD2DEG(atan2f(-forward.z, tmp));

			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}

	inline float GetFov(const Vec3& angle, const Vec3& source, const Vec3& destination)
	{
		Vec3 ang, aim;
		float mag, u_dot_v;
		ang = CalcAngle(source, destination);

		CreateVector(angle, aim);
		CreateVector(ang, ang);

		mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
		u_dot_v = aim.Dot(ang);

		return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
	}

	inline Vec3 NormalizedAngle(const Vec3& angles)
	{
		static float sp, sy, cp, cy;

		Math::SinCos(DEG2RAD(angles.x), &sp, &cp);
		Math::SinCos(DEG2RAD(angles.y), &sy, &cy);

		Vec3 forward = { cp * cy, cp * sy, -sp };
		forward *= (1.f / (FLT_EPSILON + forward.Length()));
		return forward;
	}

	inline bool RayToAABB(const Vec3& origin, const Vec3& direction, const Vec3& vmin, const Vec3& vmax)
	{
		static float t[8];

		t[0] = (vmin.x - origin.x) / direction.x;
		t[1] = (vmax.x - origin.x) / direction.x;
		t[2] = (vmin.y - origin.y) / direction.y;
		t[3] = (vmax.y - origin.y) / direction.y;
		t[4] = (vmin.z - origin.z) / direction.z;
		t[5] = (vmax.z - origin.z) / direction.z;
		t[6] = fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
		t[7] = fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));
		return !(t[7] < 0 || t[6] > t[7]);
	}

#define floatCompare(x, y) \
	(fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

	//adapted from https://github.com/gszauer/GamePhysicsCookbook/blob/15810bbf902c1cc19064c176a7e0626eda3b83bd/Code/Geometry3D.cpp#L584
	inline bool RayToOBB(const  Vec3& origin, const  Vec3& direction, const Vec3& position, const Vec3& min, const Vec3& max, const matrix3x4 orientation) {

		Vec3 p = position - origin;

		Vec3 X(orientation[0][0], orientation[0][1], orientation[0][2]);
		Vec3 Y(orientation[1][0], orientation[1][1], orientation[1][2]);
		Vec3 Z(orientation[2][0], orientation[2][1], orientation[2][2]);

		Vec3 f(
			X.Dot(direction),
			Y.Dot(direction),
			Z.Dot(direction)
		);

		Vec3 e(
			X.Dot(p),
			Y.Dot(p),
			Z.Dot(p)
		);

		float t[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		for (int i = 0; i < 3; ++i) {
			if (floatCompare(f[i], 0)) {
				if (-e[i] + min[i] > 0 || -e[i] + max[i] < 0) {
					return false;
				}
				f[i] = 0.00001f; // Avoid div by 0!
			}

			t[i * 2 + 0] = (e[i] + max[i]) / f[i]; // tmin[x, y, z]
			t[i * 2 + 1] = (e[i] + min[i]) / f[i]; // tmax[x, y, z]
		}

		float tmin = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
		float tmax = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

		// if tmax < 0, ray is intersecting AABB
		// but entire AABB is behing it's origin
		if (tmax < 0) {
			return false;
		}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax) {
			return false;
		}

		return true;
	}

	inline float RandFloat(float min, float max)
	{
		return (min + 1) + ((static_cast<float>(rand())) / static_cast<float>(RAND_MAX)) * (max - (min + 1));
	}

	inline float NormalizeRad(float a) noexcept
	{
		return std::isfinite(a) ? std::remainder(a, PI * 2) : 0.0f;
	}

	inline void VectorSubtract(const Vec3& a, const Vec3& b, Vec3& c)
	{
		c.x = a.x - b.x;
		c.y = a.y - b.y;
		c.z = a.z - b.z;
	}

	inline void VectorAdd(const Vec3& a, const Vec3& b, Vec3& c)
	{
		c.x = a.x + b.x;
		c.y = a.y + b.y;
		c.z = a.z + b.z;
	}

	inline void AngleIMatrix(const Vec3& angles, matrix3x4 & matrix)
	{
		float		sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);
		SinCos(DEG2RAD(angles[2]), &sr, &cr);

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[0][1] = cp * sy;
		matrix[0][2] = -sp;
		matrix[1][0] = sr * sp * cy + cr * -sy;
		matrix[1][1] = sr * sp * sy + cr * cy;
		matrix[1][2] = sr * cp;
		matrix[2][0] = (cr * sp * cy + -sr * -sy);
		matrix[2][1] = (cr * sp * sy + -sr * cy);
		matrix[2][2] = cr * cp;
		matrix[0][3] = 0.f;
		matrix[1][3] = 0.f;
		matrix[2][3] = 0.f;
	}

	inline void ComputeCenterIMatrix(const Vec3& origin, const Vec3& angles,
									 const Vec3& mins, const Vec3& maxs, matrix3x4& matrix)
	{
		Vec3 centroid;
		VectorAdd(mins, maxs, centroid);
		centroid *= -0.5f;
		AngleIMatrix(angles, matrix);

		// For the translational component here, note that the origin in world space
		// is T = R * C + O, (R = rotation matrix, C = centroid in local space, O = origin in world space)
		// The IMatrix translation = - transpose(R) * T = -C - transpose(R) * 0
		Vec3 localOrigin;
		VectorRotate(origin, matrix, localOrigin);
		centroid -= localOrigin;
		MatrixSetColumn(centroid, 3, matrix);
	}

	inline void ComputeCenterMatrix(const Vec3& origin, const Vec3 & angles,
									const Vec3& mins, const Vec3& maxs, matrix3x4& matrix)
	{
		Vec3 centroid;
		VectorAdd(mins, maxs, centroid);
		centroid *= 0.5f;
		AngleMatrix(angles, matrix);

		Vec3 worldCentroid;
		VectorRotate(centroid, matrix, worldCentroid);
		worldCentroid += origin;
		MatrixSetColumn(worldCentroid, 3, matrix);
	}

	inline unsigned long& FloatBits(float& f)
	{
		return *reinterpret_cast<unsigned long*>(&f);
	}

	inline unsigned long FloatAbsBits(float f)
	{
		return FloatBits(f) & 0x7FFFFFFF;
	}

	//-----------------------------------------------------------------------------
	static inline void ComputeAbsMatrix(const matrix3x4& in, matrix3x4& out)
	{
		FloatBits(out[0][0]) = FloatAbsBits(in[0][0]);
		FloatBits(out[0][1]) = FloatAbsBits(in[0][1]);
		FloatBits(out[0][2]) = FloatAbsBits(in[0][2]);
		FloatBits(out[1][0]) = FloatAbsBits(in[1][0]);
		FloatBits(out[1][1]) = FloatAbsBits(in[1][1]);
		FloatBits(out[1][2]) = FloatAbsBits(in[1][2]);
		FloatBits(out[2][0]) = FloatAbsBits(in[2][0]);
		FloatBits(out[2][1]) = FloatAbsBits(in[2][1]);
		FloatBits(out[2][2]) = FloatAbsBits(in[2][2]);
	}

	inline void MatrixGetColumn(const matrix3x4& in, int column, Vec3& out)
	{
		out.x = in[0][column];
		out.y = in[1][column];
		out.z = in[2][column];
	}

	inline float MatrixRowDotProduct(const matrix3x4& in1, int row, const Vec3& in2)
	{
		return DotProduct(in1[row], in2.Base());
	}

	inline float FloatMakePositive(float f)
	{
		return fabsf(f);
	}

	inline void VectorCopy(const float* a, float* b)
	{
		b[0] = a[0];
		b[1] = a[1];
		b[2] = a[2];
	}

	inline float MatrixColumnDotProduct(const matrix3x4& in1, int col, const Vec3& in2)
	{
		return in1[0][col] * in2[0] + in1[1][col] * in2[1] + in1[2][col] * in2[2];
	}

	inline void CrossProduct(const float* v1, const float* v2, float* cross)
	{
		cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
		cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
		cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
	}


	inline bool ComputeSeparatingPlane(const matrix3x4& worldToBox1, const matrix3x4& box2ToWorld,
									   const Vec3& box1Size, const Vec3& box2Size, float tolerance, cplane_t* pPlane)
	{
		// The various separating planes can be either
		// 1) A plane parallel to one of the box face planes
		// 2) A plane parallel to the cross-product of an edge from each box

		// First, compute the basis of second box in the space of the first box
		// NOTE: These basis place the origin at the centroid of each box!
		matrix3x4	box2ToBox1;
		ConcatTransforms(worldToBox1, box2ToWorld, box2ToBox1);

		// We're going to be using the origin of box2 in the space of box1 alot,
		// lets extract it from the matrix....
		Vec3 box2Origin;
		MatrixGetColumn(box2ToBox1, 3, box2Origin);

		// Next get the absolute values of these entries and store in absbox2ToBox1.
		matrix3x4 absBox2ToBox1;
		ComputeAbsMatrix(box2ToBox1, absBox2ToBox1);

		// There are 15 tests to make.  The first 3 involve trying planes parallel
		// to the faces of the first box.

		// NOTE: The algorithm here involves finding the projections of the two boxes
		// onto a particular line. If the projections on the line do not overlap,
		// that means that there's a plane perpendicular to the line which separates 
		// the two boxes; and we've therefore found a separating plane.

		// The way we check for overlay is we find the projections of the two boxes
		// onto the line, and add them up. We compare the sum with the projection
		// of the relative center of box2 onto the same line.

		Vec3 tmp;
		float boxProjectionSum;
		float originProjection;

		// NOTE: For these guys, we're taking advantage of the fact that the ith
		// row of the box2ToBox1 is the direction of the box1 (x,y,z)-axis
		// transformed into the space of box2.

		// First side of box 1
		boxProjectionSum = box1Size.x + MatrixRowDotProduct(absBox2ToBox1, 0, box2Size);
		originProjection = FloatMakePositive(box2Origin.x) + tolerance;
		if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
		{
			VectorCopy(worldToBox1[0], pPlane->normal.Base());
			return true;
		}

		// Second side of box 1
		boxProjectionSum = box1Size.y + MatrixRowDotProduct(absBox2ToBox1, 1, box2Size);
		originProjection = FloatMakePositive(box2Origin.y) + tolerance;
		if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
		{
			VectorCopy(worldToBox1[1], pPlane->normal.Base());
			return true;
		}

		// Third side of box 1
		boxProjectionSum = box1Size.z + MatrixRowDotProduct(absBox2ToBox1, 2, box2Size);
		originProjection = FloatMakePositive(box2Origin.z) + tolerance;
		if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
		{
			VectorCopy(worldToBox1[2], pPlane->normal.Base());
			return true;
		}

		// The next three involve checking splitting planes parallel to the
		// faces of the second box.

		// NOTE: For these guys, we're taking advantage of the fact that the 0th
		// column of the box2ToBox1 is the direction of the box2 x-axis
		// transformed into the space of box1.
		// Here, we're determining the distance of box2's center from box1's center
		// by projecting it onto a line parallel to box2's axis

		// First side of box 2
		boxProjectionSum = box2Size.x + MatrixColumnDotProduct(absBox2ToBox1, 0, box1Size);
		originProjection = FloatMakePositive(MatrixColumnDotProduct(box2ToBox1, 0, box2Origin)) + tolerance;
		if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
		{
			MatrixGetColumn(box2ToWorld, 0, pPlane->normal);
			return true;
		}

		// Second side of box 2
		boxProjectionSum = box2Size.y + MatrixColumnDotProduct(absBox2ToBox1, 1, box1Size);
		originProjection = FloatMakePositive(MatrixColumnDotProduct(box2ToBox1, 1, box2Origin)) + tolerance;
		if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
		{
			MatrixGetColumn(box2ToWorld, 1, pPlane->normal);
			return true;
		}

		// Third side of box 2
		boxProjectionSum = box2Size.z + MatrixColumnDotProduct(absBox2ToBox1, 2, box1Size);
		originProjection = FloatMakePositive(MatrixColumnDotProduct(box2ToBox1, 2, box2Origin)) + tolerance;
		if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
		{
			MatrixGetColumn(box2ToWorld, 2, pPlane->normal);
			return true;
		}

		// Next check the splitting planes which are orthogonal to the pairs
		// of edges, one from box1 and one from box2.  As only direction matters,
		// there are 9 pairs since each box has 3 distinct edge directions.

		// Here, we take advantage of the fact that the edges from box 1 are all
		// axis aligned; therefore the crossproducts are simplified. Let's walk through
		// the example of b1e1 x b2e1:

		// In this example, the line to check is perpendicular to b1e1 + b2e2
		// we can compute this line by taking the cross-product:
		//
		// [  i  j  k ]
		// [  1  0  0 ] = - ez j + ey k = l1
		// [ ex ey ez ]

		// Where ex, ey, ez is the components of box2's x axis in the space of box 1,
		// which is == to the 0th column of of box2toBox1

		// The projection of box1 onto this line = the absolute dot product of the box size
		// against the line, which =
		// AbsDot( box1Size, l1 ) = abs( -ez * box1.y ) + abs( ey * box1.z )

		// To compute the projection of box2 onto this line, we'll do it in the space of box 2
		//
		// [  i  j  k ]
		// [ fx fy fz ] = fz j - fy k = l2
		// [  1  0  0 ]

		// Where fx, fy, fz is the components of box1's x axis in the space of box 2,
		// which is == to the 0th row of of box2toBox1

		// The projection of box2 onto this line = the absolute dot product of the box size
		// against the line, which =
		// AbsDot( box2Size, l2 ) = abs( fz * box2.y ) + abs ( fy * box2.z )

		// The projection of the relative origin position on this line is done in the 
		// space of box 1:
		//
		// originProjection = DotProduct( <-ez j + ey k>, box2Origin ) =
		//		-ez * box2Origin.y + ey * box2Origin.z

		// NOTE: These checks can be bogus if both edges are parallel. The if
		// checks at the beginning of each block are designed to catch that case

		// b1e1 x b2e1
		if (absBox2ToBox1[0][0] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.y * absBox2ToBox1[2][0] + box1Size.z * absBox2ToBox1[1][0] +
				box2Size.y * absBox2ToBox1[0][2] + box2Size.z * absBox2ToBox1[0][1];
			originProjection = FloatMakePositive(-box2Origin.y * box2ToBox1[2][0] + box2Origin.z * box2ToBox1[1][0]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 0, tmp);
				CrossProduct(worldToBox1[0], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}

		// b1e1 x b2e2
		if (absBox2ToBox1[0][1] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.y * absBox2ToBox1[2][1] + box1Size.z * absBox2ToBox1[1][1] +
				box2Size.x * absBox2ToBox1[0][2] + box2Size.z * absBox2ToBox1[0][0];
			originProjection = FloatMakePositive(-box2Origin.y * box2ToBox1[2][1] + box2Origin.z * box2ToBox1[1][1]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 1, tmp);
				CrossProduct(worldToBox1[0], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}

		// b1e1 x b2e3
		if (absBox2ToBox1[0][2] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.y * absBox2ToBox1[2][2] + box1Size.z * absBox2ToBox1[1][2] +
				box2Size.x * absBox2ToBox1[0][1] + box2Size.y * absBox2ToBox1[0][0];
			originProjection = FloatMakePositive(-box2Origin.y * box2ToBox1[2][2] + box2Origin.z * box2ToBox1[1][2]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 2, tmp);
				CrossProduct(worldToBox1[0], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}

		// b1e2 x b2e1
		if (absBox2ToBox1[1][0] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.x * absBox2ToBox1[2][0] + box1Size.z * absBox2ToBox1[0][0] +
				box2Size.y * absBox2ToBox1[1][2] + box2Size.z * absBox2ToBox1[1][1];
			originProjection = FloatMakePositive(box2Origin.x * box2ToBox1[2][0] - box2Origin.z * box2ToBox1[0][0]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 0, tmp);
				CrossProduct(worldToBox1[1], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}

		// b1e2 x b2e2
		if (absBox2ToBox1[1][1] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.x * absBox2ToBox1[2][1] + box1Size.z * absBox2ToBox1[0][1] +
				box2Size.x * absBox2ToBox1[1][2] + box2Size.z * absBox2ToBox1[1][0];
			originProjection = FloatMakePositive(box2Origin.x * box2ToBox1[2][1] - box2Origin.z * box2ToBox1[0][1]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 1, tmp);
				CrossProduct(worldToBox1[1], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}

		// b1e2 x b2e3
		if (absBox2ToBox1[1][2] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.x * absBox2ToBox1[2][2] + box1Size.z * absBox2ToBox1[0][2] +
				box2Size.x * absBox2ToBox1[1][1] + box2Size.y * absBox2ToBox1[1][0];
			originProjection = FloatMakePositive(box2Origin.x * box2ToBox1[2][2] - box2Origin.z * box2ToBox1[0][2]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 2, tmp);
				CrossProduct(worldToBox1[1], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}

		// b1e3 x b2e1
		if (absBox2ToBox1[2][0] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.x * absBox2ToBox1[1][0] + box1Size.y * absBox2ToBox1[0][0] +
				box2Size.y * absBox2ToBox1[2][2] + box2Size.z * absBox2ToBox1[2][1];
			originProjection = FloatMakePositive(-box2Origin.x * box2ToBox1[1][0] + box2Origin.y * box2ToBox1[0][0]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 0, tmp);
				CrossProduct(worldToBox1[2], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}

		// b1e3 x b2e2
		if (absBox2ToBox1[2][1] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.x * absBox2ToBox1[1][1] + box1Size.y * absBox2ToBox1[0][1] +
				box2Size.x * absBox2ToBox1[2][2] + box2Size.z * absBox2ToBox1[2][0];
			originProjection = FloatMakePositive(-box2Origin.x * box2ToBox1[1][1] + box2Origin.y * box2ToBox1[0][1]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 1, tmp);
				CrossProduct(worldToBox1[2], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}

		// b1e3 x b2e3
		if (absBox2ToBox1[2][2] < 1.0f - 1e-3f)
		{
			boxProjectionSum =
				box1Size.x * absBox2ToBox1[1][2] + box1Size.y * absBox2ToBox1[0][2] +
				box2Size.x * absBox2ToBox1[2][1] + box2Size.y * absBox2ToBox1[2][0];
			originProjection = FloatMakePositive(-box2Origin.x * box2ToBox1[1][2] + box2Origin.y * box2ToBox1[0][2]) + tolerance;
			if (FloatBits(originProjection) > FloatBits(boxProjectionSum))
			{
				MatrixGetColumn(box2ToWorld, 2, tmp);
				CrossProduct(worldToBox1[2], tmp.Base(), pPlane->normal.Base());
				return true;
			}
		}
		return false;
	}


	inline bool ComputeSeparatingPlane(const Vec3& org1, const Vec3 & angles1, const Vec3& min1, const Vec3& max1,
								const Vec3& org2, const Vec3 & angles2, const Vec3& min2, const Vec3& max2,
								float tolerance, cplane_t* pPlane)
	{
		matrix3x4	worldToBox1, box2ToWorld;
		ComputeCenterIMatrix(org1, angles1, min1, max1, worldToBox1);
		ComputeCenterMatrix(org2, angles2, min2, max2, box2ToWorld);

		// Then compute the size of the two boxes
		Vec3 box1Size, box2Size;
		VectorSubtract(max1, min1, box1Size);
		VectorSubtract(max2, min2, box2Size);
		box1Size *= 0.5f;
		box2Size *= 0.5f;

		return ComputeSeparatingPlane(worldToBox1, box2ToWorld, box1Size, box2Size, tolerance, pPlane);
	}

	inline bool IsOBBIntersectingOBB(const Vec3& vecOrigin1, const Vec3 & vecAngles1, const Vec3& boxMin1, const Vec3& boxMax1,
									 const Vec3& vecOrigin2, const Vec3 & vecAngles2, const Vec3& boxMin2, const Vec3& boxMax2, float flTolerance)
	{
		cplane_t plane;
		bool bFoundPlane = ComputeSeparatingPlane(vecOrigin1, vecAngles1, boxMin1, boxMax1,
												  vecOrigin2, vecAngles2, boxMin2, boxMax2, flTolerance, &plane);
		return (bFoundPlane == false);
	}

	inline float AngleDiffRad(float a1, float a2) noexcept
	{
		double delta = NormalizeRad(a1 - a2);
		if (a1 > a2)
		{
			if (delta >= PI) { delta -= PI * 2; }
		}
		else
		{
			if (delta <= -PI) { delta += PI * 2; }
		}
		return static_cast<float>(delta);
	}

	inline float EaseInBack(float x)
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.0f;
		return c3 * x * x * x - c1 * x * x;
	}

	inline float EaseInOutSine(float x)
	{
		return -(cos(PI * x) - 1.0f) / 2.0f;
	}

	inline float MapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	__inline float RemapValClamped(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return val >= B ? D : C;
		float cVal = (val - A) / (B - A);
		cVal = std::clamp(cVal, 0.0f, 1.0f);

		return C + (D - C) * cVal;
	}

	template < typename t >
	__forceinline void Clamp(t& n, const t& lower, const t& upper) {
		n = std::max(lower, std::min(n, upper));
	}
}
