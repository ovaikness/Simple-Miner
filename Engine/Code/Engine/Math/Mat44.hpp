#pragma once
#include "Engine/Math/Vec4.hpp"

class Mat44
{
public:
	alignas(32) float m_Values[16];

	static size_t const Ix;
	static size_t const Iy;
	static size_t const Iz;
	static size_t const Iw;

	static size_t const Jx;
	static size_t const Jy;
	static size_t const Jz;
	static size_t const Jw;

	static size_t const Kx;
	static size_t const Ky;
	static size_t const Kz;
	static size_t const Kw;

	static size_t const Tx;
	static size_t const Ty;
	static size_t const Tz;
	static size_t const Tw;

	static Mat44 const IDENTITY;
	static Mat44 const ZERO;
public:
	Mat44();
	Mat44(float const* floats);
	Mat44(float floats[16]);
	Mat44(Mat44 const& other);
	Mat44(Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
	Mat44(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& translation);
	Mat44(Vec4 const& iBasis, Vec4 const& jBasis, Vec4 const& kBasis, Vec4 const& translation);
	Mat44(float iBasisX, float iBasisY, float iBasisZ, float iBasisW,
		float jBasisX, float jBasisY, float jBasisZ, float jBasisW,
		float kBasisX, float kBasisY, float kBasisZ, float kBasisW,
		float translationX, float translationY, float translationZ, float translationW);

	Mat44 Append(Mat44 const& other);
	Mat44 AppendZRotation(float degrees);
	Mat44 AppendYRotation(float degrees);
	Mat44 AppendXRotation(float degrees);

	Mat44 AppendTranslation2D(Vec2 const& translation);
	Mat44 AppendTranslation3D(Vec3 const& translation);
	Mat44 AppendScaleUniform2D(float scale);
	Mat44 AppendScaleUniform3D(float scale);
	Mat44 AppendScaleNonUniform2D(Vec2 const& scale);
	Mat44 AppendScaleNonUniform3D(Vec3 const& scale);

	Vec3 TransformPoint(Vec3 const& point) const;
	Vec2 TransformVectorQuantity2D(Vec2 const& vec) const;
	Vec3 TransformVectorQuantity3D(Vec3 const& vec) const;
	Vec2 TransformPosition2D(Vec2 const& pos) const;
	Vec3 TransformPosition3D(Vec3 const& vec) const;
	Vec4 TransformHomogeneous3D(Vec4 const& vec) const;
	float* const GetAsFloatArray() const;

	Vec2 GetIBasis2D() const;
	Vec2 GetJBasis2D() const;
	Vec2 GetTranslation2D() const;
	Vec3 GetIBasis3D() const;
	Vec3 GetJBasis3D() const;
	Vec3 GetKBasis3D() const;
	Vec3 GetTranslation3D() const;
	Vec4 GetIBasis4D() const;
	Vec4 GetJBasis4D() const;
	Vec4 GetKBasis4D() const;
	Vec4 GetTranslation4D() const;

	void SetIJ2D(Vec2 const& iBasis, Vec2 const& jBasis);
	void SetIJT2D(Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
	void SetIJK3D(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis);
	void SetIJKT3D(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& translation);
	void SetIJKT4D(Vec4 const& iBasis, Vec4 const& jBasis, Vec4 const& kBasis, Vec4 const& translation);

	static Mat44 CreateTranslation2D(Vec2 const& translation);
	static Mat44 CreateTranslation3D(Vec3 const& translation);
	static Mat44 CreateUniformScale2D(float scale);
	static Mat44 CreateNonUniformScale2D(Vec2 const& scale);
	static Mat44 CreateUniformScale3D(float scale);
	static Mat44 CreateNonUniformScale3D(Vec3 const& scale);
	static Mat44 CreateZRotationDegrees(float degrees);
	static Mat44 CreateYRotationDegrees(float degrees);
	static Mat44 CreateXRotationDegrees(float degrees);

	void SetIBasis2D(Vec2 const& basis);
	void SetJBasis2D(Vec2 const& basis);
	void SetTranslation2D(Vec2 const& translation);
	void SetIBasis3D(Vec3 const& basis);
	void SetJBasis3D(Vec3 const& basis);
	void SetKBasis3D(Vec3 const& basis);
	void SetTranslation3D(Vec3 const& basis);
	void SetIBasis4D(Vec4 const& basis);
	void SetJBasis4D(Vec4 const& basis);
	void SetKBasis4D(Vec4 const& basis);
	void SetTranslation4D(Vec4 const& basis);

	void Transpose();
	Mat44 GetTranspose() const;
	Mat44 const GetOrthonormalInverse() const;

	float GetDeterminant() const;
	Mat44 GetInverse() const;

	static Mat44 CreateOrthoProjection(float left, float right, float bottom, float top, float near, float far);
	static Mat44 CreatePerspectiveProjection(float fovYDegrees, float aspectRatio, float near, float far);

	void Orthonormalize_IFwd_JLeft_KUp();

	std::string ToString() const;
};