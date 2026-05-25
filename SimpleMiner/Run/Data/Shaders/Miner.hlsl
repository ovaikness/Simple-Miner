struct vs_input_t
  	{
  		float3 localPosition : POSITION;
  		float4 color : COLOR;
  		float2 uv : TEXCOORD;
  	};
  	
struct v2p_t
{
  	float4 position : SV_Position;
  	float4 color    : COLOR;
  	float2 uv		: TEXCOORD;
    float3 worldPosition : WORLD_POS;
};
  	
cbuffer CameraConstants : register(b2)
{   
    float4x4 ProjectionMatrix;
    float4x4 ViewMatrix;
}
    
cbuffer ModelConstants : register(b3)
{
    float4x4 ModelMatrix;
    float4 ModelColor;
}

cbuffer MinerConstants : register(b5)
{
	float4 CameraPos;
	float4 InteriorLight;
	float4 ExteriorLight;
	float4 SkyColor;
	float FogFar = 0;
	float FogNear = 0;
	float Padding1;
	float Padding2;
}

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

float Interpolate(float start, float end, float t)
{
	return start * (1.0f - t) + end * t;
}

float GetFractionWithinRange(float start, float end, float value)
{
	return (value - start) / (end - start);
}

float RangeMap(float value, float startOrigin, float endOrigin, float startDestination, float endDestination)
{
	float fraction = GetFractionWithinRange(startOrigin, endOrigin, value);
	return Interpolate(startDestination, endDestination, fraction);
}

v2p_t VertexMain(vs_input_t input)
{
    float4 localPosition = float4(input.localPosition, 1);
        
    float4 clipPosition = mul(ProjectionMatrix,mul(ViewMatrix, mul(ModelMatrix, localPosition)));
        
  	v2p_t v2p;
  	v2p.position = clipPosition;
  	v2p.color    = input.color * ModelColor;
  	v2p.uv		 = input.uv;
    v2p.worldPosition = mul(ModelMatrix, localPosition).xyz;
  	return v2p;
}
  	
float4 PixelMain(v2p_t input) : SV_Target0
{
    clip(input.color.a < 0.1f ? -1.0f : 1.0f);
    float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
    float4 outColor = textureColor;
    float distanceToCamera = distance(input.worldPosition.xyz, CameraPos.xyz);
    float frac = saturate(RangeMap(distanceToCamera, FogNear, FogFar, 0.0f, 1.0f));
    float dimAdd = float3(1.f,1.f,1.f) - (float3(1.f,1.f,1.f) - input.color.r * ExteriorLight.rgb) * (float3(1.f,1.f,1.f) - input.color.g * InteriorLight.rgb);
    outColor = lerp(outColor * dimAdd * input.color.b, SkyColor, frac);
    
    return float4(outColor.rgb,1.0);
}