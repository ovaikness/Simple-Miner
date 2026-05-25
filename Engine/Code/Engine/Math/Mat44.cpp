#include "Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"

size_t const Mat44::Ix = 0;
size_t const Mat44::Iy = 1;
size_t const Mat44::Iz = 2;
size_t const Mat44::Iw = 3;

size_t const Mat44::Jx = 4;
size_t const Mat44::Jy = 5;
size_t const Mat44::Jz = 6;
size_t const Mat44::Jw = 7;

size_t const Mat44::Kx = 8;
size_t const Mat44::Ky = 9;
size_t const Mat44::Kz = 10;
size_t const Mat44::Kw = 11;

size_t const Mat44::Tx = 12;
size_t const Mat44::Ty = 13;
size_t const Mat44::Tz = 14;
size_t const Mat44::Tw = 15;

Mat44 const Mat44::ZERO = 
Mat44(
	0.f, 0.f, 0.f, 0.f, 
	0.f, 0.f, 0.f, 0.f, 
	0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f
);

Mat44 const Mat44::IDENTITY =
Mat44(
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
);

Mat44::Mat44()
	: m_Values
{
	1.f,0.f,0.f,0.f,
	0.f,1.f,0.f,0.f,
	0.f,0.f,1.f,0.f,
	0.f,0.f,0.f,1.f
}
{
}

Mat44::Mat44(float const* floats)
{
	for (int index = 0; index < 16; ++index)
	{
		m_Values[index] = floats[index];
	}
}

Mat44::Mat44(float floats[16])
{
	for (int index = 0; index < 16; ++index)
	{
		m_Values[index] = floats[index];
	}
}

Mat44::Mat44(Mat44 const& other)
	: m_Values
{
	other.m_Values[0], other.m_Values[1], other.m_Values[2], other.m_Values[3],
	other.m_Values[4], other.m_Values[5], other.m_Values[6], other.m_Values[7],
	other.m_Values[8], other.m_Values[9], other.m_Values[10], other.m_Values[11],
	other.m_Values[12], other.m_Values[13], other.m_Values[14], other.m_Values[15]
}
{
}

Mat44::Mat44(Vec4 const& iBasis, Vec4 const& jBasis, Vec4 const& kBasis, Vec4 const& tBasis)
	: m_Values
{
	iBasis.x,iBasis.y,iBasis.z,iBasis.w,
	jBasis.x,jBasis.y,jBasis.z,jBasis.w,
	kBasis.x,kBasis.y,kBasis.z,kBasis.w,
	tBasis.x,tBasis.y,tBasis.z,tBasis.w
}
{
}

Mat44::Mat44(float iBasisX, float iBasisY, float iBasisZ, float iBasisW,
	float jBasisX, float jBasisY, float jBasisZ, float jBasisW,
	float kBasisX, float kBasisY, float kBasisZ, float kBasisW,
	float translationX, float translationY, float translationZ, float translationW)
	: m_Values{ iBasisX, iBasisY, iBasisZ, iBasisW,
	jBasisX, jBasisY, jBasisZ, jBasisW,
	kBasisX, kBasisY, kBasisZ, kBasisW,
	translationX, translationY, translationZ, translationW }
{
}

Mat44::Mat44(Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	*this = IDENTITY;
	SetIBasis2D(iBasis);
	SetJBasis2D(jBasis);
	SetTranslation2D(translation);
}

Mat44::Mat44(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& translation)
{
	*this = IDENTITY;
	SetIBasis3D(iBasis);
	SetJBasis3D(jBasis);
	SetKBasis3D(kBasis);
	SetTranslation3D(translation);
}

Mat44 Mat44::Append(Mat44 const& other)
{
	Mat44 result;

	for (size_t i = 0; i < 4; ++i) {
		for (size_t j = 0; j < 4; ++j) {
			float sum = 0.0f;
			for (size_t k = 0; k < 4; ++k) {
				sum += m_Values[k * 4 + i] * other.m_Values[j * 4 + k];
			}
			result.m_Values[j * 4 + i] = sum;
		}
	}
	

	*this = result;
	return result;
}


Mat44 Mat44::AppendZRotation(float degrees)
{
	Mat44 rotationMat;
	float cosTheta = CosDegrees(degrees);
	float sinTheta = SinDegrees(degrees);

	rotationMat.m_Values[Ix] = cosTheta;
	rotationMat.m_Values[Iy] = sinTheta;
	rotationMat.m_Values[Jx] = -sinTheta;
	rotationMat.m_Values[Jy] = cosTheta;

	return Append(rotationMat);
}

Mat44 Mat44::AppendYRotation(float degrees)
{
	Mat44 rotation = Mat44::CreateYRotationDegrees(degrees);
	return this->Append(rotation);
}

Mat44 Mat44::AppendXRotation(float degrees)
{
	Mat44 rotation = Mat44::CreateXRotationDegrees(degrees);
	return this->Append(rotation);
}

Mat44 Mat44::AppendTranslation2D(Vec2 const& translation)
{
	Mat44 translationMatrix = Mat44::CreateTranslation2D(translation);
	return this->Append(translationMatrix);
}

Mat44 Mat44::AppendTranslation3D(Vec3 const& translation)
{
	Mat44 translationMatrix = Mat44::CreateTranslation3D(translation);
	return this->Append(translationMatrix);
}

Mat44 Mat44::AppendScaleUniform2D(float scale)
{
	Mat44 uniformMatrix = Mat44::CreateUniformScale2D(scale);
	return this->Append(uniformMatrix);
}

Mat44 Mat44::AppendScaleUniform3D(float scale)
{
	Mat44 uniformMatrix = Mat44::CreateUniformScale3D(scale);
	return this->Append(uniformMatrix);
}

Mat44 Mat44::AppendScaleNonUniform2D(Vec2 const& scale)
{
	Mat44 nonUniformMatrix = Mat44::CreateNonUniformScale2D(scale);
	return this->Append(nonUniformMatrix);
}

Mat44 Mat44::AppendScaleNonUniform3D(Vec3 const& scale)
{
	Mat44 nonUniformMatrix = Mat44::CreateNonUniformScale3D(scale);
	return this->Append(nonUniformMatrix);
}

Vec3 Mat44::TransformPoint(Vec3 const& vector) const
{
	Vec3 result;
	result.x = vector.x * m_Values[0] + vector.y * m_Values[4] + vector.z * m_Values[8];
	result.y = vector.x * m_Values[1] + vector.y * m_Values[5] + vector.z * m_Values[9];
	result.z = vector.x * m_Values[2] + vector.y * m_Values[6] + vector.z * m_Values[10];
	return result;
}

Vec2 Mat44::TransformVectorQuantity2D(Vec2 const& vec) const
{
	Vec2 result;
	result.x = vec.x * m_Values[0] + vec.y * m_Values[4];
	result.y = vec.x * m_Values[1] + vec.y * m_Values[5];
	return result;
}

Vec3 Mat44::TransformVectorQuantity3D(Vec3 const& vec) const
{
	Vec3 result;
	result.x = vec.x * m_Values[0] + vec.y * m_Values[4] + vec.z * m_Values[8];
	result.y = vec.x * m_Values[1] + vec.y * m_Values[5] + vec.z * m_Values[9];
	result.z = vec.x * m_Values[2] + vec.y * m_Values[6] + vec.z * m_Values[10];
	return result;
}

Vec2 Mat44::TransformPosition2D(Vec2 const& pos) const
{
	Vec2 result;
	result.x = pos.x * m_Values[0] + pos.y * m_Values[4] + m_Values[12];
	result.y = pos.x * m_Values[1] + pos.y * m_Values[5] + m_Values[13];
	return result;
}

Vec3 Mat44::TransformPosition3D(Vec3 const& pos) const
{
	Vec3 result;
	result.x = pos.x * m_Values[0] + pos.y * m_Values[4] + pos.z * m_Values[8] + m_Values[12];
	result.y = pos.x * m_Values[1] + pos.y * m_Values[5] + pos.z * m_Values[9] + m_Values[13];
	result.z = pos.x * m_Values[2] + pos.y * m_Values[6] + pos.z * m_Values[10] + m_Values[14];
	return result;
}

Vec4 Mat44::TransformHomogeneous3D(Vec4 const& vec) const
{
	Vec4 result;
	result.x = vec.x * m_Values[0] + vec.y * m_Values[4] + vec.z * m_Values[8] +  vec.w * m_Values[12];
	result.y = vec.x * m_Values[1] + vec.y * m_Values[5] + vec.z * m_Values[9] +  vec.w * m_Values[13];
	result.z = vec.x * m_Values[2] + vec.y * m_Values[6] + vec.z * m_Values[10] + vec.w * m_Values[14];
	result.w = vec.x * m_Values[3] + vec.y * m_Values[7] + vec.z * m_Values[11] + vec.w * m_Values[15];
	return result;
}

float* const Mat44::GetAsFloatArray() const
{
	float* matFloats = new float[16];
	for (int i = 0; i < 16; ++i)
	{
		matFloats[i] = m_Values[i];
	}
	return matFloats;
}

Vec2 Mat44::GetIBasis2D() const
{
	return Vec2(m_Values[0], m_Values[1]);
}

Vec2 Mat44::GetJBasis2D() const
{
	return Vec2(m_Values[4], m_Values[5]);
}

Vec2 Mat44::GetTranslation2D() const
{
	return Vec2(m_Values[12], m_Values[13]);
}

Vec3 Mat44::GetIBasis3D() const
{
	return Vec3(m_Values[0], m_Values[1], m_Values[2]);
}

Vec3 Mat44::GetJBasis3D() const
{
	return Vec3(m_Values[4], m_Values[5], m_Values[6]);
}

Vec3 Mat44::GetKBasis3D() const
{
	return Vec3(m_Values[8], m_Values[9], m_Values[10]);
}

Vec3 Mat44::GetTranslation3D() const
{
	return Vec3(m_Values[12], m_Values[13], m_Values[14]);
}

Vec4 Mat44::GetIBasis4D() const
{
	return Vec4(m_Values[0], m_Values[1], m_Values[2], m_Values[3]);
}

Vec4 Mat44::GetJBasis4D() const
{
	return Vec4(m_Values[4], m_Values[5], m_Values[6], m_Values[7]);
}

Vec4 Mat44::GetKBasis4D() const
{
	return Vec4(m_Values[8], m_Values[9], m_Values[10], m_Values[11]);
}

Vec4 Mat44::GetTranslation4D() const
{
	return Vec4(m_Values[12], m_Values[13], m_Values[14], m_Values[15]);
}

void Mat44::SetIJ2D(Vec2 const& iBasis, Vec2 const& jBasis)
{
	SetIBasis2D(iBasis);
	SetJBasis2D(jBasis);
}

void Mat44::SetIJT2D(Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	SetIBasis2D(iBasis);
	SetJBasis2D(jBasis);
	SetTranslation2D(translation);
}

void Mat44::SetIJK3D(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis)
{
	SetIBasis3D(iBasis);
	SetJBasis3D(jBasis);
	SetKBasis3D(kBasis);
}

void Mat44::SetIJKT3D(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis, Vec3 const& translation)
{
	m_Values[0] = iBasis.x; m_Values[4] = jBasis.x; m_Values[8] = kBasis.x;  m_Values[12] = translation.x;
	m_Values[1] = iBasis.y; m_Values[5] = jBasis.y; m_Values[9] = kBasis.y;  m_Values[13] = translation.y;
	m_Values[2] = iBasis.z; m_Values[6] = jBasis.z; m_Values[10] = kBasis.z; m_Values[14] = translation.z;
	m_Values[3] = 0.0f;      m_Values[7] = 0.0f;      m_Values[11] = 0.0f;     m_Values[15] = 1.0f;
}

void Mat44::SetIJKT4D(Vec4 const& iBasis, Vec4 const& jBasis, Vec4 const& kBasis, Vec4 const& translation)
{
	m_Values[0] = iBasis.x; m_Values[4] = jBasis.x; m_Values[8] = kBasis.x;  m_Values[12] = translation.x;
	m_Values[1] = iBasis.y; m_Values[5] = jBasis.y; m_Values[9] = kBasis.y;  m_Values[13] = translation.y;
	m_Values[2] = iBasis.z; m_Values[6] = jBasis.z; m_Values[10] = kBasis.z; m_Values[14] = translation.z;
	m_Values[3] = iBasis.w; m_Values[7] = jBasis.w; m_Values[11] = kBasis.w; m_Values[15] = translation.w;
}

Mat44 Mat44::CreateTranslation2D(Vec2 const& translation)
{
	Mat44 result = Mat44::IDENTITY;
	result.m_Values[12] = translation.x;
	result.m_Values[13] = translation.y;
	return result;
}

Mat44 Mat44::CreateTranslation3D(Vec3 const& translation)
{
	Mat44 result = Mat44::IDENTITY;
	result.m_Values[12] = translation.x;
	result.m_Values[13] = translation.y;
	result.m_Values[14] = translation.z;
	return result;
}

Mat44 Mat44::CreateUniformScale2D(float scale)
{
	Mat44 result = Mat44::IDENTITY;
	result.m_Values[0] = scale;
	result.m_Values[5] = scale;
	return result;
}

Mat44 Mat44::CreateNonUniformScale2D(Vec2 const& scale)
{
	Mat44 result = Mat44::IDENTITY;
	result.m_Values[0] = scale.x;
	result.m_Values[5] = scale.y;
	return result;
}

Mat44 Mat44::CreateUniformScale3D(float scale)
{
	Mat44 result = Mat44::IDENTITY;
	result.m_Values[0] = scale;
	result.m_Values[5] = scale;
	result.m_Values[10] = scale;
	return result;
}

Mat44 Mat44::CreateNonUniformScale3D(Vec3 const& scale)
{
	Mat44 result = Mat44::IDENTITY;
	result.m_Values[0] = scale.x;
	result.m_Values[5] = scale.y;
	result.m_Values[10] = scale.z;
	return result;
}

Mat44 Mat44::CreateZRotationDegrees(float degrees)
{
	Mat44 result = Mat44::IDENTITY;
	float cosVal = CosDegrees(degrees);
	float sinVal = SinDegrees(degrees);

	result.m_Values[0] = cosVal;
	result.m_Values[4] = -sinVal;
	result.m_Values[1] = sinVal;
	result.m_Values[5] = cosVal;

	return result;
}

Mat44 Mat44::CreateYRotationDegrees(float degrees)
{
	Mat44 result = Mat44::IDENTITY;
	float cosVal = CosDegrees(degrees);
	float sinVal = SinDegrees(degrees);

	result.m_Values[0] = cosVal;
	result.m_Values[8] = sinVal;
	result.m_Values[2] = -sinVal;
	result.m_Values[10] = cosVal;

	return result;
}

Mat44 Mat44::CreateXRotationDegrees(float degrees)
{
	Mat44 result = IDENTITY;
	float cosVal = CosDegrees(degrees);
	float sinVal = SinDegrees(degrees);

	result.m_Values[5] = cosVal;
	result.m_Values[9] = -sinVal;
	result.m_Values[6] = sinVal;
	result.m_Values[10] = cosVal;

	return result;
}

void Mat44::SetIBasis2D(Vec2 const& basis)
{
	m_Values[0] = basis.x;
	m_Values[1] = basis.y;
	m_Values[2] = 0.f;
	m_Values[3] = 0.f;
}

void Mat44::SetJBasis2D(Vec2 const& basis)
{
	m_Values[4] = basis.x;
	m_Values[5] = basis.y;
	m_Values[6] = 0.f;
	m_Values[7] = 0.f;
}

void Mat44::SetTranslation2D(Vec2 const& translation)
{
	m_Values[12] = translation.x;
	m_Values[13] = translation.y;
	m_Values[14] = 0.f;
	m_Values[15] = 1.f;
}

void Mat44::SetIBasis3D(Vec3 const& basis)
{
	m_Values[0] = basis.x;
	m_Values[1] = basis.y;
	m_Values[2] = basis.z;
	m_Values[3] = 0.f;
}

void Mat44::SetJBasis3D(Vec3 const& basis)
{
	m_Values[4] = basis.x;
	m_Values[5] = basis.y;
	m_Values[6] = basis.z;
	m_Values[7] = 0.f;
}

void Mat44::SetKBasis3D(Vec3 const& basis)
{
	m_Values[8] = basis.x;
	m_Values[9] = basis.y;
	m_Values[10] = basis.z;
	m_Values[11] = 0.f;
}

void Mat44::SetTranslation3D(Vec3 const& basis)
{
	m_Values[12] = basis.x;
	m_Values[13] = basis.y;
	m_Values[14] = basis.z;
	m_Values[15] = 1.f;
}

void Mat44::SetIBasis4D(Vec4 const& basis)
{
	m_Values[0] = basis.x;
	m_Values[1] = basis.y;
	m_Values[2] = basis.z;
	m_Values[3] = basis.w;
}

void Mat44::SetJBasis4D(Vec4 const& basis)
{
	m_Values[4] = basis.x;
	m_Values[5] = basis.y;
	m_Values[6] = basis.z;
	m_Values[7] = basis.w;
}

void Mat44::SetKBasis4D(Vec4 const& basis)
{
	m_Values[8] = basis.x;
	m_Values[9] = basis.y;
	m_Values[10] = basis.z;
	m_Values[11] = basis.w;
}

void Mat44::SetTranslation4D(Vec4 const& basis)
{
	m_Values[12] = basis.x;
	m_Values[13] = basis.y;
	m_Values[14] = basis.z;
	m_Values[15] = basis.w;
}

void Mat44::Transpose()
{
	Mat44 copy;

	copy.m_Values[Ix] = this->m_Values[Ix];
	copy.m_Values[Iy] = this->m_Values[Jx];
	copy.m_Values[Iz] = this->m_Values[Kx];
	copy.m_Values[Iw] = this->m_Values[Tx];

	copy.m_Values[Jx] = this->m_Values[Iy];
	copy.m_Values[Jy] = this->m_Values[Jy];
	copy.m_Values[Jz] = this->m_Values[Ky];
	copy.m_Values[Jw] = this->m_Values[Ty];

	copy.m_Values[Kx] = this->m_Values[Iz];
	copy.m_Values[Ky] = this->m_Values[Jz];
	copy.m_Values[Kz] = this->m_Values[Kz];
	copy.m_Values[Kw] = this->m_Values[Tz];
	
	copy.m_Values[Tx] = this->m_Values[Iw];
	copy.m_Values[Ty] = this->m_Values[Jw];
	copy.m_Values[Tz] = this->m_Values[Kw];
	copy.m_Values[Tw] = this->m_Values[Tw];

	*this = copy;
}

Mat44 Mat44::GetTranspose() const
{
	Mat44 copy = *this;
	copy.Transpose();
	return copy;
}

Mat44 const Mat44::GetOrthonormalInverse() const
{
	Mat44 copy = Mat44::IDENTITY;

	copy.m_Values[Ix] = this->m_Values[Ix];
	copy.m_Values[Iy] = this->m_Values[Jx];
	copy.m_Values[Iz] = this->m_Values[Kx];

	copy.m_Values[Jx] = this->m_Values[Iy];
	copy.m_Values[Jy] = this->m_Values[Jy];
	copy.m_Values[Jz] = this->m_Values[Ky];

	copy.m_Values[Kx] = this->m_Values[Iz];
	copy.m_Values[Ky] = this->m_Values[Jz];
	copy.m_Values[Kz] = this->m_Values[Kz];

	Vec3 translation = Vec3(-this->m_Values[Tx], -this->m_Values[Ty], -this->m_Values[Tz]);

	copy.m_Values[Tx] = DotProduct3D(this->GetIBasis3D(), translation);
	copy.m_Values[Ty] = DotProduct3D(this->GetJBasis3D(), translation);
	copy.m_Values[Tz] = DotProduct3D(this->GetKBasis3D(), translation);
	copy.m_Values[Tw] = 1.f;
	return copy;
}

float Mat44::GetDeterminant() const
{
	float xy = CrossProduct2D(Vec2(m_Values[Ix], m_Values[Iy]), Vec2(m_Values[Jx], m_Values[Jy]));
	float xz = CrossProduct2D(Vec2(m_Values[Ix], m_Values[Iy]), Vec2(m_Values[Kx], m_Values[Ky]));
	float yz = CrossProduct2D(Vec2(m_Values[Jx], m_Values[Jy]), Vec2(m_Values[Kx], m_Values[Ky]));

	return DotProduct3D(Vec3(m_Values[Ix], m_Values[Iy], m_Values[Iz]), Vec3(xy, xz, yz));
}

Mat44 Mat44::GetInverse() const
{
	float det = GetDeterminant();
	Mat44 transpose = GetTranspose();
	transpose.m_Values[Iy] = -transpose.m_Values[Iy];
	transpose.m_Values[Iz] = -transpose.m_Values[Iz];
	transpose.m_Values[Iw] = -transpose.m_Values[Iw];

	transpose.m_Values[Jx] = -transpose.m_Values[Jx];
	transpose.m_Values[Jz] = -transpose.m_Values[Jz];
	transpose.m_Values[Jw] = -transpose.m_Values[Jw];

	transpose.m_Values[Kx] = -transpose.m_Values[Kz];
	transpose.m_Values[Ky] = -transpose.m_Values[Ky];
	transpose.m_Values[Kw] = -transpose.m_Values[Kw];

	transpose.m_Values[Tx] = -transpose.m_Values[Tx];
	transpose.m_Values[Ty] = -transpose.m_Values[Ty];
	transpose.m_Values[Tz] = -transpose.m_Values[Tz];

	if (det != 0.f)
	{
		for (auto& value : transpose.m_Values)
		{
			value = value / det;
		}

		return transpose;
	}
	return Mat44();
}

Mat44 Mat44::CreateOrthoProjection(float left, float right, float bottom, float top, float near, float far)
{
	Mat44 mat = Mat44::IDENTITY;
	mat.m_Values[Ix] = 2.f / (right - left);
	mat.m_Values[Jy] = 2.f / (top - bottom);
	mat.m_Values[Kz] = 1.f / (far - near);

	mat.m_Values[Tx] = -(right + left) / (right - left);
	mat.m_Values[Ty] = -(top + bottom) / (top - bottom);
	mat.m_Values[Tz] = (1.f + -(far + near) / (far - near)) * 0.5f;
	mat.m_Values[Tw] = 1.f;

	return mat;
}

Mat44 Mat44::CreatePerspectiveProjection(float fovYDegrees, float aspectRatio,float near, float far)
{
	Mat44 mat = Mat44::IDENTITY;

	float scaleY = CosDegrees(fovYDegrees * 0.5f) / SinDegrees(fovYDegrees * 0.5f);
	float scaleX = scaleY / aspectRatio;
	float scaleZ = far / (far - near);
	
	mat.m_Values[Ix] = scaleX;
	mat.m_Values[Jy] = scaleY;
	mat.m_Values[Kz] = scaleZ;
	mat.m_Values[Kw] = 1.f;
	mat.m_Values[Tz] = (near * far) / (near - far);
	mat.m_Values[Tw] = 0.f;
	return mat;
}

void Mat44::Orthonormalize_IFwd_JLeft_KUp()
{
	Vec3 IBasis = GetIBasis3D();
	IBasis.Normalize();
	Vec3 JBasis = GetJBasis3D();
	float dot = DotProduct3D(IBasis, JBasis);
	JBasis -= dot * IBasis;
	JBasis.Normalize();
	Vec3 KBasis = GetKBasis3D();
	dot = DotProduct3D(JBasis, KBasis);
	KBasis -= dot * JBasis;
	dot = DotProduct3D(KBasis, IBasis);
	KBasis -= dot * IBasis;
	KBasis.Normalize();

	SetIBasis3D(IBasis);
	SetJBasis3D(JBasis);
	SetKBasis3D(KBasis);
}

std::string Mat44::ToString() const
{
	std::string result = "Mat44\n\r(";
	for (int index = 0; index < 16; ++index)
	{
		result += std::to_string(m_Values[index]);
		if (index != 15)
		{
			if (index % 4 == 3)
			{
				result += "\n\r";
			}
			else
			{
				result += ", ";
			}
		}
	}
	result += ")";
	return result;
}