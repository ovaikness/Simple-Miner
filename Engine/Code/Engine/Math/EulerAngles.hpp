#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"

class EulerAngles
{
public:
	EulerAngles();
	EulerAngles(float yaw, float pitch, float roll);

	void SetFromText(char const* text);

	static EulerAngles MakeFromNormal_IFwd_JLeft_KUp(Vec3 const& normal);
	void  GetAsVectors_IFwd_JLeft_KUp(Vec3& out_iBasis, Vec3& out_jBasis, Vec3& out_kBasis) const;
	Mat44 GetAsMatrix_IFwd_JLeft_KUp() const;

	float m_yaw;
	float m_pitch;
	float m_roll;

	static EulerAngles ZERO;
};