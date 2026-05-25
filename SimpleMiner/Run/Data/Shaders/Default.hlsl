// Default shader for C30 SD Engine (circa SD2 Spring 2021)

// a struct containing what inputs our vertex shader is expecting
struct vs_input_t
{
	// variables being passes between stages have the following declaration style
	// [qualifier] type localName [: semanticName]; 
	float3	a_position	: POSITION;
	float4	a_color		: TINT; 
	float2	a_uv		: TEXCOORDS; 
	
	// Built-in / automatic attributes (not part of incoming VBO data)
	uint	a_vertexID	: SV_VertexID;
	
	// localName -> what the shader will reference it as
	// semanticName -> how D3D links up variables between different stages (including initial GPU to CPU)
	// 				   Semenatics with `SV_` are special, and stand for `System Variable`, and have special meaning/function within the pipeline
	// 				   SV_VertexID will cause this variable to be filled with the index of the vertex we're drawing when this is used as an input to a vertex shader; 
};

// struct of data that I'm passing from the vertex stage for the fragment stage.  `v2f_t` is the name used in Unity for this, 
// but you can call it whatever you want, such as `VertexToPixel` or `VertexToFragment` if you prefer a different naming scheme
struct v2p_t // vertex to fragment
{
	float4 v_position : SV_Position; // SV_Position is always required for a vertex shader, and denotes the renering location of this vertex.  As an input to the pixel shader, it gives the pixel location of the fragment
	float4 v_color : VertexColor; // This semantic is NOT `SV_`, and is just whatever name we want to call it.  If a pixel stage has an input marked as `VertexColor`, it will link the two up.
	float2 v_uv : TextureCoords;
};


// b0 = system constants (e.g. toggle debug mode) - rare
// b1 = frame constants (e.g. time) - once per frame
// b2 = camera constants (e.g. view/proj matrices) - per camera begin
// b3 = model constants (e.g. model matrix & tint) - per draw call
// b4-7 = other engine-reserved slots
// b8-15 = game-specific / free slots
// NOTE: constant buffers MUST be 16B-aligned (size is multiple of 16B)
// Also, primitives may not cross 16B boundaries (unless they are 16B aligned, like Mat44)
cbuffer CameraConstants : register(b2)
{
	float4x4 b_projectionMatrix;	// gameCamera-to-clip (includes gameCamera->screenCamera axis swaps)
	float4x4 b_viewMatrix;			// world-to-gameCamera
};


//------------------------------------------------------------------------------------------------
// Main Entry Point for the vertex stage
// which for graphical shaders is usually a main entry point
// and will get information from the game
v2p_t VertexMain( vs_input_t input )
{
	v2p_t v2f;
	
	float4 worldPos = float4( input.a_position, 1 );
	float4 cameraPos = mul( b_viewMatrix, worldPos );
	float4 clipPos = mul( b_projectionMatrix, cameraPos );

	// we defined the position as a 3 dimensional coordinate, but SV_Position expects a clip/perspective space coordinate (4D).  More on this later.  For now, just pass 1 for w; 
	v2f.v_position = clipPos;
	v2f.v_color = input.a_color;
	v2f.v_uv = input.a_uv;

	return v2f; // pass it on to the raster stage
}


Texture2D<float4>	t_diffuseTexture : register(t0);	// Texture bound in texture constant slot #0
SamplerState		s_diffuseSampler : register(s0);				// Sampler is bound in sampler constant slot #0

//------------------------------------------------------------------------------------------------
// Main Entry Point for the Pixel Stage
// This returns only one value (the output color)
// so instead of using a struct, we'll just label the return value
//
// This determines the color of a single pixel or fragment in the output
// the input may vary certain variables in the raster stage, and we'll get those varied 
// inputs passed to us. 
//
// Note, system variables such as `SV_Position` have special rules, and the one output may
// have no or very little relation to the one you got into the pixel shader; 
float4 PixelMain(v2p_t input) : SV_Target0
{
	float2 uvCoords = input.v_uv;
	float4 diffuseColor = t_diffuseTexture.Sample( s_diffuseSampler, uvCoords );
	
	float4 tint = input.v_color;
	float4 finalColor = tint * diffuseColor;
	if( finalColor.a <= 0.001 )
		discard;
	
	return finalColor;
}

