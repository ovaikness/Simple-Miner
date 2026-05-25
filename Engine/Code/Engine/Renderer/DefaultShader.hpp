#pragma once

const char* defaultShaderSource = R"(
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
    Texture2D diffuseTexture : register(t0);
    SamplerState diffuseSampler : register(s0);

    float Interpolate(float start, float end, float t)
    {
	    return start * (1.0f - t) + end * t;
    }

    float GetFractionWithinRange(float start, float end, float value)
    {
	    if (end - start == 0.f)
	    {
		    return 0.5f;
	    }
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
  		return v2p;
  	}
  	
  	float4 PixelMain(v2p_t input) : SV_Target0
  	{
        if(input.color.a < 0.1)
        {
            discard;
        }
        float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
  		return float4(input.color * textureColor);
  	}
  	)";

const char* defaultVertexPCUTBNShaderSource = R"(
  	struct vs_input_t
  	{
  		float3 localPosition : POSITION;
  		float4 color : COLOR;
  		float2 uv : TEXCOORD;
  		float3 tangent : TANGENT;
  		float3 bitangent : BITANGENT;
  		float3 normal : NORMAL;
  	};
  	
  	struct v2p_t
  	{
  		float4 position : SV_Position;
  		float4 color    : COLOR;
  		float2 uv		: TEXCOORD;
        float3 tangent : TANGENT;
  		float3 bitangent : BITANGENT;
  		float3 normal : NORMAL;
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

    cbuffer LightConstants : register(b4)
	{
		float3 sunDirection;
		float  sunIntensity;
		float  ambientIntensity;
	}

    Texture2D diffuseTexture : register(t0);
    SamplerState diffuseSampler : register(s0);

    float Interpolate(float start, float end, float t)
    {
	    return start * (1.0f - t) + end * t;
    }

    float GetFractionWithinRange(float start, float end, float value)
    {
	    if (end - start == 0.f)
	    {
		    return 0.5f;
	    }
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
		v2p.tangent = input.tangent;
		v2p.bitangent = input.bitangent;
		v2p.normal = input.normal;
  		return v2p;
  	}
  	
  	float4 PixelMain(v2p_t input) : SV_Target0
  	{
        clip(input.color.a < 0.1f);
        float directionalFactor = dot(input.normal, sunDirection);
		float sunColor = sunIntensity * RangeMap(directionalFactor, -1.f, 1.f, 0.f, 1.f);
		float ambientColor = ambientIntensity;
		float lighting = sunColor + ambientColor;
        float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
        float4 result = float4(input.color * textureColor) * lighting;
  		return float4(result.rgb,1.f);
  	}
  	)";