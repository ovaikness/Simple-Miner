#include "EulerAngles.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/TinyXMLUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

EulerAngles EulerAngles::ZERO = EulerAngles(0.f, 0.f, 0.f);

EulerAngles::EulerAngles()
	: m_yaw(0)
	, m_pitch(0)
	, m_roll(0)
{
}

EulerAngles::EulerAngles(float yaw, float pitch, float roll)
	: m_yaw( yaw )
	, m_pitch( pitch )
	, m_roll( roll )
{
}

void EulerAngles::SetFromText(char const* text)
{
	Strings strings = SplitStringOnDelimiter(text, ',');
	m_yaw = static_cast<float>(atof(strings.at(0).c_str()));
	m_pitch = static_cast<float>(atof(strings.at(1).c_str()));
	m_roll = static_cast<float>(atof(strings.at(2).c_str()));
}

EulerAngles EulerAngles::MakeFromNormal_IFwd_JLeft_KUp(Vec3 const& normal)
{
	EulerAngles angles;
	Vec3 forward = Vec3(1.f, 0.f, 0.f);

	float dot;
	Vec3 cross;
	dot   = DotProduct3D(normal, forward);
	//In the case the normal is exactly opposite choose some reflective rotation on any plane
	if ((dot + 1.f) < 0.0001f)
	{
		return EulerAngles(180.f, 0.f, 0.f);
	}

	cross = CrossProduct3D(forward, normal);

	float XY = cross.z * (dot < 0.f ? -1.f : 1.f);
	float XZ = cross.y;
	float YZ = cross.x;

	float yaw   = asinf(XY) * (180.f / 3.1415f) + (SignFloat(dot) < 0.f ? 180.f : 0.f);
	float roll  = asinf(YZ) * (180.f / 3.1415f);
	float pitch = asinf(XZ) * (180.f / 3.1415f);

	return EulerAngles(yaw, pitch, roll);
}

void EulerAngles::GetAsVectors_IFwd_JLeft_KUp(Vec3& out_iBasis, Vec3& out_jBasis, Vec3& out_kBasis) const
{
	float cosYaw = CosDegrees(m_yaw);
	float sinYaw = SinDegrees(m_yaw);
	float cosPitch = CosDegrees(m_pitch);
	float sinPitch = SinDegrees(m_pitch);
	float cosRoll = CosDegrees(m_roll);
	float sinRoll = SinDegrees(m_roll);

	out_iBasis.x = cosPitch * cosYaw;
	out_iBasis.y = cosPitch * sinYaw;
	out_iBasis.z = -sinPitch;

	out_jBasis.x = (sinRoll * sinPitch * cosYaw) - (cosRoll * sinYaw);
	out_jBasis.y = (sinRoll * sinPitch * sinYaw) + (cosRoll * cosYaw);
	out_jBasis.z = sinRoll * cosPitch;

	out_kBasis.x = (cosRoll * sinPitch * cosYaw) + (sinRoll * sinYaw);
	out_kBasis.y = (cosRoll * sinPitch * sinYaw) - (sinRoll * cosYaw);
	out_kBasis.z = cosRoll * cosPitch;

}

Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp() const
{
	Vec3 iBasis, jBasis, kBasis;
	GetAsVectors_IFwd_JLeft_KUp(iBasis, jBasis, kBasis);
	return Mat44(iBasis, jBasis, kBasis, Vec3(0.f,0.f,0.f));
}
