#include "MathFunction.h"
#include <cassert>
#include <numbers>

namespace Mathf
{
	float Dot(const Vector3& v1, const Vector3& v2) 
	{
		float result{};
		result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		return result;
	}


	float Length(const Vector3& v) 
	{
		float result{};
		result = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		return result;
	}


	float Lerp(const float& v1, const float& v2, float t)
	{
		float result{};
		result = v1 + t * (v2 - v1);
		return result;
	}


	float LerpShortAngle(const float& a, const float& b, float t) 
	{
		//角度差分を求める
		float diff = b - a;
		//角度を[-2PI,+2PI]に補正する
		float PI = 3.14159265359f;
		float PI2 = 2.0f * 3.14159265359f;
		float theta = std::fmod(diff, PI2);
		//角度を[-PI,PI]に補正する
		if (theta >= PI)
		{
			theta -= PI2;
		}
		if (theta <= -PI)
		{
			theta += PI2;
		}

		return a + theta * t;
	}


	float EaseInSine(float x)
	{
		return 1.0f - std::cos((x * std::numbers::pi_v<float>) / 2.0f);
	}

	float EaseInCubic(float x)
	{
		return x * x * x;
	}

	float EaseOutCubic(float x)
	{
		return 1.0f - std::pow(1.0f - x, 3.0f);
	}

	float EaseInOutSine(float x)
	{
		return -(std::cos(std::numbers::pi_v<float> *x) - 1.0f) / 2.0f;
	}

	float EaseOutExpo(float x)
	{
		return x == 1 ? 1 : 1 - std::pow(2.0f, -10.0f * x);
	}

	float Norm(const Quaternion& quaternion) 
	{
		float result{};
		result = sqrtf(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
		return result;
	}


	Vector3 Normalize(const Vector3& v)
	{
		Vector3 result{};

		float date = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		if (date != 0) 
		{
			result.x = v.x / date;
			result.y = v.y / date;
			result.z = v.z / date;
		}
		return result;
	}


	Vector3 Cross(const Vector3& v1, const Vector3& v2) 
	{
		Vector3 result{};
		result.x = (v1.y * v2.z) - (v1.z * v2.y);
		result.y = (v1.z * v2.x) - (v1.x * v2.z);
		result.z = (v1.x * v2.y) - (v1.y * v2.x);
		return result;
	}


	Vector3 Transform(const Vector3& v, const Matrix4x4& m)
	{
		Vector3 result{};
		result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] +
			1.0f * m.m[3][0];
		result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] +
			1.0f * m.m[3][1];
		result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] +
			1.0f * m.m[3][2];
		float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] +
			1.0f * m.m[3][3];
		assert(w != 0.0f);
		result.x /= w;
		result.y /= w;
		result.z /= w;

		return result;
	}


	Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix)
	{
		Vector3 result;
		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2];
		return result;
	}


	Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t)
	{
		Vector3 result{};
		result.x = v1.x + t * (v2.x - v1.x);
		result.y = v1.y + t * (v2.y - v1.y);
		result.z = v1.z + t * (v2.z - v1.z);
		return result;
	}


	Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) 
	{
		float theta = std::acos(Dot(v1, v2));
		float sinTheta = std::sin(theta);
		Vector3 result{};
		if (theta != 0.0f) 
		{
			result.x = (std::sin(theta * (1 - t)) * v1.x + std::sin(theta * t) * v2.x) / sinTheta;
			result.y = (std::sin(theta * (1 - t)) * v1.y + std::sin(theta * t) * v2.y) / sinTheta;
			result.z = (std::sin(theta * (1 - t)) * v1.z + std::sin(theta * t) * v2.z) / sinTheta;
		}
		return Normalize(result);
	}


	Vector3 CatmullRomSpline(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float t)
	{
		auto interpolate = [](float p0, float p1, float p2, float p3, float t) {
			return 0.5f * ((2.0f * p1) +
				(-p0 + p2) * t +
				(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
				(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
			};

		return Vector3(
			interpolate(p0.x, p1.x, p2.x, p3.x, t),
			interpolate(p0.y, p1.y, p2.y, p3.y, t),
			interpolate(p0.z, p1.z, p2.z, p3.z, t)
		);
	}


	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion)
	{
		Vector3 result{};
		Quaternion vectorToQuaternion = { vector.x,vector.y,vector.z,0.0f };
		Quaternion conj = Conjugate(quaternion);
		Quaternion rotate = quaternion * vectorToQuaternion * conj;
		result.x = rotate.x;
		result.y = rotate.y;
		result.z = rotate.z;
		return result;
	}


	Matrix4x4 Inverse(const Matrix4x4& m) 
	{
		Matrix4x4 result{};
		float determinant = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
			m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
			m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -

			m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
			m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
			m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -

			m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
			m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
			m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +

			m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
			m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
			m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +

			m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
			m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
			m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -

			m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
			m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
			m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -

			m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
			m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
			m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +

			m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
			m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
			m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
		assert(determinant != 0.0f);
		float determinantRecp = 1.0f / determinant;

		result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) * determinantRecp;
		result.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) * determinantRecp;
		result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) * determinantRecp;
		result.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) * determinantRecp;

		result.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) * determinantRecp;
		result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) * determinantRecp;
		result.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) * determinantRecp;
		result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) * determinantRecp;

		result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) * determinantRecp;
		result.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) * determinantRecp;
		result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) * determinantRecp;
		result.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) * determinantRecp;

		result.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) * determinantRecp;
		result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) * determinantRecp;
		result.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) * determinantRecp;
		result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) * determinantRecp;

		return result;
	}


	Matrix4x4 Transpose(const Matrix4x4& m) 
	{
		Matrix4x4 result{};
		result.m[0][0] = m.m[0][0];
		result.m[0][1] = m.m[1][0];
		result.m[0][2] = m.m[2][0];
		result.m[0][3] = m.m[3][0];

		result.m[1][0] = m.m[0][1];
		result.m[1][1] = m.m[1][1];
		result.m[1][2] = m.m[2][1];
		result.m[1][3] = m.m[3][1];

		result.m[2][0] = m.m[0][2];
		result.m[2][1] = m.m[1][2];
		result.m[2][2] = m.m[2][2];
		result.m[2][3] = m.m[3][2];

		result.m[3][0] = m.m[0][3];
		result.m[3][1] = m.m[1][3];
		result.m[3][2] = m.m[2][3];
		result.m[3][3] = m.m[3][3];

		return result;
	}


	Matrix4x4 MakeIdentity4x4() 
	{
		Matrix4x4 result{};
		result.m[0][0] = 1.0f;
		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;

		result.m[1][0] = 0.0f;
		result.m[1][1] = 1.0f;
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;

		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][2] = 1.0f;
		result.m[2][3] = 0.0f;

		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;

		return result;
	}


	Matrix4x4 MakeScaleMatrix(const Vector3& scale) 
	{
		Matrix4x4 result;
		result.m[0][0] = scale.x;
		result.m[0][1] = 0;
		result.m[0][2] = 0;
		result.m[0][3] = 0;

		result.m[1][0] = 0;
		result.m[1][1] = scale.y;
		result.m[1][2] = 0;
		result.m[1][3] = 0;

		result.m[2][0] = 0;
		result.m[2][1] = 0;
		result.m[2][2] = scale.z;
		result.m[2][3] = 0;

		result.m[3][0] = 0;
		result.m[3][1] = 0;
		result.m[3][2] = 0;
		result.m[3][3] = 1.0f;

		return result;
	}


	Matrix4x4 MakeTranslateMatrix(const Vector3& translate) 
	{
		Matrix4x4 result;
		result.m[0][0] = 1.0f;
		result.m[0][1] = 0;
		result.m[0][2] = 0;
		result.m[0][3] = 0;

		result.m[1][0] = 0;
		result.m[1][1] = 1.0f;
		result.m[1][2] = 0;
		result.m[1][3] = 0;

		result.m[2][0] = 0;
		result.m[2][1] = 0;
		result.m[2][2] = 1.0f;
		result.m[2][3] = 0;

		result.m[3][0] = translate.x;
		result.m[3][1] = translate.y;
		result.m[3][2] = translate.z;
		result.m[3][3] = 1.0f;

		return result;
	}

	Matrix4x4 MakeRotateXMatrix(float radian)
	{
		Matrix4x4 result{};
		result.m[0][0] = 1;
		result.m[0][1] = 0;
		result.m[0][2] = 0;
		result.m[0][3] = 0;

		result.m[1][0] = 0;
		result.m[1][1] = std::cos(radian);
		result.m[1][2] = std::sin(radian);
		result.m[1][3] = 0;

		result.m[2][0] = 0;
		result.m[2][1] = -std::sin(radian);
		result.m[2][2] = std::cos(radian);
		result.m[2][3] = 0;

		result.m[3][0] = 0;
		result.m[3][1] = 0;
		result.m[3][2] = 0;
		result.m[3][3] = 1;

		return result;
	}


	Matrix4x4 MakeRotateYMatrix(float radian) 
	{
		Matrix4x4 result{};
		result.m[0][0] = std::cos(radian);
		result.m[0][1] = 0;
		result.m[0][2] = -std::sin(radian);
		result.m[0][3] = 0;

		result.m[1][0] = 0;
		result.m[1][1] = 1;
		result.m[1][2] = 0;
		result.m[1][3] = 0;

		result.m[2][0] = std::sin(radian);
		result.m[2][1] = 0;
		result.m[2][2] = std::cos(radian);
		result.m[2][3] = 0;

		result.m[3][0] = 0;
		result.m[3][1] = 0;
		result.m[3][2] = 0;
		result.m[3][3] = 1;

		return result;
	}


	Matrix4x4 MakeRotateZMatrix(float radian)
	{
		Matrix4x4 result{};
		result.m[0][0] = std::cos(radian);
		result.m[0][1] = std::sin(radian);
		result.m[0][2] = 0;
		result.m[0][3] = 0;

		result.m[1][0] = -std::sin(radian);
		result.m[1][1] = std::cos(radian);
		result.m[1][2] = 0;
		result.m[1][3] = 0;

		result.m[2][0] = 0;
		result.m[2][1] = 0;
		result.m[2][2] = 1;
		result.m[2][3] = 0;

		result.m[3][0] = 0;
		result.m[3][1] = 0;
		result.m[3][2] = 0;
		result.m[3][3] = 1;

		return result;
	}


	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) 
	{
		Matrix4x4 result{};
		Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
		Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
		Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
		Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
		Matrix4x4 rotateXYZMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
		Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
		result = scaleMatrix * rotateXYZMatrix * translateMatrix;
		return result;
	}


	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& quaternion, const Vector3& translation)
	{
		Matrix4x4 result{};
		Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
		Matrix4x4 rotateMatrix = MakeRotateMatrix(quaternion);
		Matrix4x4 translateMatrix = MakeTranslateMatrix(translation);
		result = scaleMatrix * rotateMatrix * translateMatrix;
		return result;
	}


	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
	{
		Matrix4x4 result{};
		result.m[0][0] = (1.0f / std::tan(fovY / 2)) / aspectRatio;
		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;

		result.m[1][0] = 0.0f;
		result.m[1][1] = 1.0f / std::tan(fovY / 2);
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;

		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][2] = farClip / (farClip - nearClip);
		result.m[2][3] = 1.0f;

		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = -nearClip * farClip / (farClip - nearClip);
		result.m[3][3] = 0.0f;

		return result;
	}


	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
	{
		assert(left != right);
		assert(top != bottom);
		Matrix4x4 result;
		result.m[0][0] = 2.0f / (right - left);
		result.m[0][1] = 0.0f;
		result.m[0][2] = 0.0f;
		result.m[0][3] = 0.0f;

		result.m[1][0] = 0.0f;
		result.m[1][1] = 2.0f / (top - bottom);
		result.m[1][2] = 0.0f;
		result.m[1][3] = 0.0f;

		result.m[2][0] = 0.0f;
		result.m[2][1] = 0.0f;
		result.m[2][2] = 1.0f / (farClip - nearClip);
		result.m[2][3] = 0.0f;

		result.m[3][0] = (left + right) / (left - right);
		result.m[3][1] = (top + bottom) / (bottom - top);
		result.m[3][2] = nearClip / (nearClip - farClip);
		result.m[3][3] = 1.0f;

		return result;
	}


	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) 
	{
		Matrix4x4 result{};
		result.m[0][0] = width / 2;
		result.m[0][1] = 0;
		result.m[0][2] = 0;
		result.m[0][3] = 0;

		result.m[1][0] = 0;
		result.m[1][1] = -height / 2;
		result.m[1][2] = 0;
		result.m[1][3] = 0;

		result.m[2][0] = 0;
		result.m[2][1] = 0;
		result.m[2][2] = maxDepth - minDepth;
		result.m[2][3] = 0;

		result.m[3][0] = left + (width / 2);
		result.m[3][1] = top + (height / 2);
		result.m[3][2] = minDepth;
		result.m[3][3] = 1;

		return result;
	}

	Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to)
	{
		Matrix4x4 result{};
		Vector3 n{};
		if (from.x == -to.x && from.y == -to.y && from.z == -to.z) 
		{
			if (from.x != 0.0f || from.y != 0.0f)
			{
				n = { from.y,-from.x,0.0f };
			}
			else if (from.x != 0.0f || from.z != 0.0f)
			{
				n = { from.z,0.0f,-from.x };
			}
		}
		else 
		{
			n = Normalize(Cross(from, to));
		}
		float cosTheta = Dot(from, to);
		float sinTheta = Length(Cross(from, to));
		result.m[0][0] = n.x * n.x * (1 - cosTheta) + cosTheta;
		result.m[0][1] = n.x * n.y * (1 - cosTheta) + n.z * sinTheta;
		result.m[0][2] = n.x * n.z * (1 - cosTheta) - n.y * sinTheta;
		result.m[0][3] = 0.0f;
		result.m[1][0] = n.x * n.y * (1 - cosTheta) - n.z * sinTheta;
		result.m[1][1] = n.y * n.y * (1 - cosTheta) + cosTheta;
		result.m[1][2] = n.y * n.z * (1 - cosTheta) + n.x * sinTheta;
		result.m[1][3] = 0.0f;
		result.m[2][0] = n.x * n.z * (1 - cosTheta) + n.y * sinTheta;
		result.m[2][1] = n.y * n.z * (1 - cosTheta) - n.x * sinTheta;
		result.m[2][2] = n.z * n.z * (1 - cosTheta) + cosTheta;
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;
		return result;
	}


	Matrix4x4 MakeRotateAxisAngle(Vector3 axis, float angle)
	{
		Matrix4x4 result{};
		result.m[0][0] = axis.x * axis.x * (1 - std::cos(angle)) + std::cos(angle);
		result.m[0][1] = axis.x * axis.y * (1 - std::cos(angle)) + axis.z * std::sin(angle);
		result.m[0][2] = axis.x * axis.z * (1 - std::cos(angle)) - axis.y * std::sin(angle);
		result.m[0][3] = 0.0f;
		result.m[1][0] = axis.x * axis.y * (1 - std::cos(angle)) - axis.z * std::sin(angle);
		result.m[1][1] = axis.y * axis.y * (1 - std::cos(angle)) + std::cos(angle);
		result.m[1][2] = axis.y * axis.z * (1 - std::cos(angle)) + axis.x * std::sin(angle);
		result.m[1][3] = 0.0f;
		result.m[2][0] = axis.x * axis.z * (1 - std::cos(angle)) + axis.y * std::sin(angle);
		result.m[2][1] = axis.y * axis.z * (1 - std::cos(angle)) - axis.x * std::sin(angle);
		result.m[2][2] = axis.z * axis.z * (1 - std::cos(angle)) + std::cos(angle);
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;
		return result;
	}


	Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) 
	{
		Matrix4x4 result{};
		result.m[0][0] = quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z;
		result.m[0][1] = 2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
		result.m[0][2] = 2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
		result.m[0][3] = 0.0f;
		result.m[1][0] = 2.0f * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
		result.m[1][1] = quaternion.w * quaternion.w - quaternion.x * quaternion.x + quaternion.y * quaternion.y - quaternion.z * quaternion.z;
		result.m[1][2] = 2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
		result.m[1][3] = 0.0f;
		result.m[2][0] = 2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
		result.m[2][1] = 2.0f * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
		result.m[2][2] = quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z;
		result.m[2][3] = 0.0f;
		result.m[3][0] = 0.0f;
		result.m[3][1] = 0.0f;
		result.m[3][2] = 0.0f;
		result.m[3][3] = 1.0f;
		return result;
	}


	Quaternion IdentityQuaternion() 
	{
		Quaternion result{};
		result.x = 0.0f;
		result.y = 0.0f;
		result.z = 0.0f;
		result.w = 1.0f;
		return result;
	}


	Quaternion Conjugate(const Quaternion& quaternion)
	{
		Quaternion result{};
		result.x = quaternion.x * -1.0f;
		result.y = quaternion.y * -1.0f;
		result.z = quaternion.z * -1.0f;
		result.w = quaternion.w;
		return result;
	}


	Quaternion Normalize(const Quaternion& quaternion)
	{
		Quaternion result{};
		float norm = Norm(quaternion);
		if (norm != 0.0f)
		{
			result.x = quaternion.x / norm;
			result.y = quaternion.y / norm;
			result.z = quaternion.z / norm;
			result.w = quaternion.w / norm;
		}
		return result;
	}


	Quaternion Inverse(const Quaternion& quaternion) 
	{
		Quaternion result{};
		Quaternion conj = Conjugate(quaternion);
		float norm = Norm(quaternion);
		if (norm != 0.0f) 
		{
			float normSquared = norm * norm;
			result.x = conj.x / normSquared;
			result.y = conj.y / normSquared;
			result.z = conj.z / normSquared;
			result.w = conj.w / normSquared;
		}
		return result;
	}


	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) 
	{
		Quaternion result{};
		float halfAngle = angle / 2.0f;
		float sin = std::sin(halfAngle);
		result.x = axis.x * sin;
		result.y = axis.y * sin;
		result.z = axis.z * sin;
		result.w = std::cos(halfAngle);
		return result;
	}


	Quaternion GetRotation(const Matrix4x4& m)
	{
		float elem[4];
		elem[0] = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.0f;
		elem[1] = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.0f;
		elem[2] = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.0f;
		elem[3] = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.0f;

		int biggestIdx = 0;
		for (int i = 0; i < 4; i++)
		{
			if (elem[i] > elem[biggestIdx])
			{
				biggestIdx = i;
			}
		}

		if (elem[biggestIdx] < 0)
		{
			return IdentityQuaternion();
		}

		float q[4];
		float v = std::sqrtf(elem[biggestIdx]) * 0.5f;
		q[biggestIdx] = v;
		float mult = 0.25f / v;

		switch (biggestIdx)
		{
		case 0:
			q[1] = (m.m[1][0] + m.m[0][1]) * mult;
			q[2] = (m.m[0][2] + m.m[2][0]) * mult;
			q[3] = (m.m[2][1] - m.m[1][2]) * mult;
			break;
		case 1:
			q[0] = (m.m[1][0] + m.m[0][1]) * mult;
			q[2] = (m.m[2][1] + m.m[1][2]) * mult;
			q[3] = (m.m[0][2] - m.m[2][0]) * mult;
			break;
		case 2:
			q[0] = (m.m[0][2] + m.m[2][0]) * mult;
			q[1] = (m.m[2][1] + m.m[1][2]) * mult;
			q[3] = (m.m[1][0] - m.m[0][1]) * mult;
			break;
		case 3:
			q[0] = (m.m[2][1] - m.m[1][2]) * mult;
			q[1] = (m.m[0][2] - m.m[2][0]) * mult;
			q[2] = (m.m[1][0] - m.m[0][1]) * mult;
			break;
		}

		return Quaternion(q[0], q[1], q[2], q[3]);
	}


	Quaternion LookAt(const Vector3& position, const Vector3& target)
	{
		Vector3 z = Normalize(target - position);
		Vector3 x = Normalize(Cross({ 0.0f,1.0f,0.0f }, z));
		Vector3 y = Normalize(Cross(z, x));

		Matrix4x4 m = MakeIdentity4x4();
		m.m[0][0] = x.x;
		m.m[0][1] = y.x;
		m.m[0][2] = z.x;
		m.m[1][0] = x.y;
		m.m[1][1] = y.y;
		m.m[1][2] = z.y;
		m.m[2][0] = x.z;
		m.m[2][1] = y.z;
		m.m[2][2] = z.z;

		Quaternion rot = GetRotation(m);

		return rot;
	}


	Quaternion LookRotation(const Vector3& forward, const Vector3& up)
	{
		Vector3 f = Normalize(forward);
		Vector3 r = Normalize(Cross(up, f));
		Vector3 u = Normalize(Cross(f, r));

		float m00 = r.x, m01 = r.y, m02 = r.z;
		float m10 = u.x, m11 = u.y, m12 = u.z;
		float m20 = f.x, m21 = f.y, m22 = f.z;

		float num8 = (m00 + m11) + m22;
		Quaternion quaternion;
		if (num8 > 0.0f) {
			float num = std::sqrt(num8 + 1.0f);
			quaternion.w = num * 0.5f;
			num = 0.5f / num;
			quaternion.x = (m12 - m21) * num;
			quaternion.y = (m20 - m02) * num;
			quaternion.z = (m01 - m10) * num;
			return quaternion;
		}
		if ((m00 >= m11) && (m00 >= m22)) {
			float num7 = std::sqrt(((1.0f + m00) - m11) - m22);
			float num4 = 0.5f / num7;
			quaternion.x = 0.5f * num7;
			quaternion.y = (m01 + m10) * num4;
			quaternion.z = (m02 + m20) * num4;
			quaternion.w = (m12 - m21) * num4;
			return quaternion;
		}
		if (m11 > m22) {
			float num6 = std::sqrt(((1.0f + m11) - m00) - m22);
			float num3 = 0.5f / num6;
			quaternion.x = (m10 + m01) * num3;
			quaternion.y = 0.5f * num6;
			quaternion.z = (m21 + m12) * num3;
			quaternion.w = (m20 - m02) * num3;
			return quaternion;
		}
		float num5 = std::sqrt(((1.0f + m22) - m00) - m11);
		float num2 = 0.5f / num5;
		quaternion.x = (m20 + m02) * num2;
		quaternion.y = (m21 + m12) * num2;
		quaternion.z = 0.5f * num5;
		quaternion.w = (m01 - m10) * num2;
		return quaternion;
	}

	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) 
	{
		Quaternion result{};
		Quaternion localQ0 = q0;
		Quaternion localQ1 = q1;
		float dot = localQ0.x * localQ1.x + localQ0.y * localQ1.y + localQ0.z * localQ1.z + localQ0.w * localQ1.w;
		if (dot < 0.0f) 
		{
			localQ0 = { -localQ0.x,-localQ0.y,-localQ0.z,-localQ0.w };
			dot = -dot;
		}
		if (dot >= 1.0f - std::numeric_limits<float>::epsilon())
		{
			result.x = (1.0f - t) * localQ0.x + t * localQ1.x;
			result.y = (1.0f - t) * localQ0.y + t * localQ1.y;
			result.z = (1.0f - t) * localQ0.z + t * localQ1.z;
			result.w = (1.0f - t) * localQ0.w + t * localQ1.w;
			return result;
		}
		float theta = std::acos(dot);
		float scale0 = std::sin((1 - t) * theta) / std::sin(theta);
		float scale1 = std::sin(t * theta) / std::sin(theta);
		result.x = scale0 * localQ0.x + scale1 * localQ1.x;
		result.y = scale0 * localQ0.y + scale1 * localQ1.y;
		result.z = scale0 * localQ0.z + scale1 * localQ1.z;
		result.w = scale0 * localQ0.w + scale1 * localQ1.w;
		return result;
	}
}