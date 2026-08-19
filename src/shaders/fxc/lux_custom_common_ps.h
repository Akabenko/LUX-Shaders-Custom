//===================== File of the LUX Shader Project =====================//

#ifndef LUX_CUSTOM_PS_H_
#define LUX_CUSTOM_PS_H_

#include "lux_common_ps_fxc.h"

//==========================================================================//
//	These are the Constants that will be available to this Shader
//==========================================================================//

// Always available.
const float4 cReg_00 				: register(REGISTER_FLOAT_000);

// c2...c13
// These will be the Model, View and Proj Matrix,
// when using $Shader_Matrices or $Shader_Particle
// Otherwise $psreg_c02 to $psreg_c13
const float4x4 cModelMatrix			: register(REGISTER_FLOAT_002); // c2, c3, c4, c5
#define cReg_02						(cModelMatrix[0])
#define cReg_03						(cModelMatrix[1])
#define cReg_04						(cModelMatrix[2])
#define cReg_05						(cModelMatrix[3])
const float4x4 cViewMatrix			: register(REGISTER_FLOAT_006); // c6, c7, c8, c9
#define cReg_06						(cViewMatrix[0])
#define cReg_07						(cViewMatrix[1])
#define cReg_08						(cViewMatrix[2])
#define cReg_09						(cViewMatrix[3])
const float4x4 cProjMatrix			: register(REGISTER_FLOAT_010); // c10, c11, c12, c13
#define cReg_10						(cProjMatrix[0])
#define cReg_11						(cProjMatrix[1])
#define cReg_12						(cProjMatrix[2])
#define cReg_13						(cProjMatrix[3])

// c14 to c25
// Varies based on the Type of Geometry and Projected Texture usage.
#if PROJTEX
	const float4	cShadowTweaks					: register(REGISTER_FLOAT_014);
	const float4	cFlashlightAttenuationFactors	: register(REGISTER_FLOAT_015);
	const float4	cFlashlightPos					: register(REGISTER_FLOAT_016);
	const float4x4	cFlashlightWorldToTexture		: register(REGISTER_FLOAT_017);
	//				cFlashlightWorldToTexture		: register(REGISTER_FLOAT_018);
	//				cFlashlightWorldToTexture		: register(REGISTER_FLOAT_019);
	//				cFlashlightWorldToTexture		: register(REGISTER_FLOAT_020);
	const float4	cFlashlightColor				: register(REGISTER_FLOAT_021);
	const float4	cFlashlightScreenScale			: register(REGISTER_FLOAT_022); // common_fxc.h - .zw are currently unused
	#define f1FlashlightNoLambertValue			(cFlashlightColor.w) // "This is either 0.0 or 2.0"
#else
	#if LIGHTDATA
		struct PixelShaderLightInfo_t
		{
			float4 color;
			float4 pos;
		};
	
		// "4th light spread across w's"
		// Use Decompress Function below
		PixelShaderLightInfo_t	cLightInfo[3]		: register(REGISTER_FLOAT_014);
		//						cLightInfo[0]		: register(REGISTER_FLOAT_015);
		//						cLightInfo[1]		: register(REGISTER_FLOAT_016);
		//						cLightInfo[1]		: register(REGISTER_FLOAT_017);
		//						cLightInfo[2]		: register(REGISTER_FLOAT_018);
		//						cLightInfo[2]		: register(REGISTER_FLOAT_019);
	
		struct LightData_t
		{
			float3	f3Color;
			float3	f3Pos;
			float3	f3Dir;
			float	f1Attenuation;
		};
	
		void UnpackLight(const int nIndex, float3 f3WorldPos, float4 LightAttens, inout LightData_t Light)
		{
			// Consistent with Stock Shaders
			if(nIndex == 3)
			{
				Light.f3Color = float3(cLightInfo[0].color.w, cLightInfo[0].pos.w, cLightInfo[1].color.w);
				Light.f3Pos = float3(cLightInfo[1].pos.w, cLightInfo[2].color.w, cLightInfo[2].pos.w);
			}
			else
			{
				Light.f3Color = cLightInfo[nIndex].color.rgb;
				Light.f3Pos = cLightInfo[nIndex].pos.xyz;
			}
			
			Light.f3Dir = normalize(Light.f3Pos - f3WorldPos);
			Light.f1Attenuation = LightAttens[nIndex];
		}
	#else
		const float4 cReg_14						: register(REGISTER_FLOAT_014);
		const float4 cReg_15						: register(REGISTER_FLOAT_015);
		
		#if PARALLAX_CORRECTED_CUBEMAP
			const float4x3	g_f4x3CorrectionMatrix			: register(REGISTER_FLOAT_016);
		//					g_f4x3CorrectionMatrix[1]		: register(REGISTER_FLOAT_017);
		//					g_f4x3CorrectionMatrix[2]		: register(REGISTER_FLOAT_018);
			const float3	g_f3CubeMapPos					: register(REGISTER_FLOAT_019);
		#else
			const float4 cReg_16					: register(REGISTER_FLOAT_016);
			const float4 cReg_17					: register(REGISTER_FLOAT_017);
			const float4 cReg_18					: register(REGISTER_FLOAT_018);
			const float4 cReg_19					: register(REGISTER_FLOAT_019);
		#endif	
	#endif	
		
	#if AMBIENTCUBES
		const float3	cAmbientCube[6]				: register(REGISTER_FLOAT_020);
		//				cAmbientCube[1]				: register(REGISTER_FLOAT_021);
		//				cAmbientCube[2]				: register(REGISTER_FLOAT_022);
		//				cAmbientCube[3]				: register(REGISTER_FLOAT_023);
		//				cAmbientCube[4]				: register(REGISTER_FLOAT_024);
		//				cAmbientCube[5]				: register(REGISTER_FLOAT_025);
	#endif
#endif // !PROJTEX

const float4 cReg_31						 : register(REGISTER_FLOAT_031);
// Brushes and Models:
#define g_rcpLightmapRes					(cReg_31.xy)
#define g_LightmapScaleFactor				(cReg_31.z)

// Particles:
#define g_DepthRangeFactor					(cReg_31.x)

// Scale and Offset ( 1.0f / 3.0f ), requires custom Compilers.
#define g_ModelLightmapBumpOffset			(cLightmapData.w)
	
// Only available with sm3.0
// The ShaderCompiler will #define this when you compile the Shader as ps30
#if defined(SHADER_MODEL_PS_3_0)
	#if !AMBIENTCUBES
		const float4 cReg_32				: register(REGISTER_FLOAT_032);
		const float4 cReg_33				: register(REGISTER_FLOAT_033);
		const float4 cReg_34				: register(REGISTER_FLOAT_034);
		const float4 cReg_35				: register(REGISTER_FLOAT_035);
		const float4 cReg_36				: register(REGISTER_FLOAT_036);
		const float4 cReg_37				: register(REGISTER_FLOAT_037);
	#endif
	const float4 cReg_38				: register(REGISTER_FLOAT_038);
	const float4 cReg_39				: register(REGISTER_FLOAT_039);
	const float4 cReg_40				: register(REGISTER_FLOAT_040);
	const float4 cReg_41				: register(REGISTER_FLOAT_041);
	const float4 cReg_42				: register(REGISTER_FLOAT_042);
	const float4 cReg_43				: register(REGISTER_FLOAT_043);
	const float4 cReg_44				: register(REGISTER_FLOAT_044);
	const float4 cReg_45				: register(REGISTER_FLOAT_045);
	const float4 cReg_46				: register(REGISTER_FLOAT_046);
	const float4 cReg_47				: register(REGISTER_FLOAT_047);
	const float4 cReg_48				: register(REGISTER_FLOAT_048);
	const float4 cReg_49				: register(REGISTER_FLOAT_049);
	const float4 cReg_50				: register(REGISTER_FLOAT_050);
	const float4 cReg_51				: register(REGISTER_FLOAT_051);
	const float4 cReg_52				: register(REGISTER_FLOAT_052);
	const float4 cReg_53				: register(REGISTER_FLOAT_053);
	const float4 cReg_54				: register(REGISTER_FLOAT_054);
	const float4 cReg_55				: register(REGISTER_FLOAT_055);
	const float4 cReg_56				: register(REGISTER_FLOAT_056);
	const float4 cReg_57				: register(REGISTER_FLOAT_057);
	const float4 cReg_58				: register(REGISTER_FLOAT_058);
	const float4 cReg_59				: register(REGISTER_FLOAT_059);
	const float4 cReg_60				: register(REGISTER_FLOAT_060);
	const float4 cReg_61				: register(REGISTER_FLOAT_061);
	const float4 cReg_62				: register(REGISTER_FLOAT_062);
	const float4 cReg_63				: register(REGISTER_FLOAT_063);
#endif

// Feel free to rename these in your Shaders!
#define		bReg_00				Bools[0]
#define		bReg_01				Bools[1]
#define		bReg_02				Bools[2]
#define		bReg_03				Bools[3]
#define		bReg_04				Bools[4]
#define		bReg_05				Bools[5]
#define		bReg_06				Bools[6]
#define		bReg_07				Bools[7]
#define		bReg_08				Bools[8]
#define		bReg_09				Bools[9]
#define		bReg_10				Bools[10]
#define		bReg_11				Bools[11]

// Hardcoded this one for Hammer specific Behaviour
// ( To allow accounting for missing Data )
#define		g_bInEditor			Bools[12]

//==========================================================================//
//	Available Samplers
//	Do not change anything here!
//==========================================================================//
sampler		Sampler_Texture0		: register(s0);
sampler		Sampler_Texture1		: register(s1);
sampler		Sampler_Texture2		: register(s2);
sampler		Sampler_Texture3		: register(s3);
sampler		Sampler_Texture4		: register(s4);
sampler		Sampler_Texture5		: register(s5);
sampler		Sampler_Texture6		: register(s6);
sampler		Sampler_Texture7		: register(s7);
sampler		Sampler_Texture8		: register(s8);
sampler		Sampler_Texture9		: register(s9);
sampler		Sampler_Texture10		: register(s10);
sampler		Sampler_Texture11		: register(s11);
sampler		Sampler_Texture12		: register(s12);
sampler		Sampler_Lightmap		: register(s13);
sampler		Sampler_EnvMap			: register(s14);

// s15 is a Gamma LUT when using sRGBWrite, so it is not provided here.

// Only available when using $BumpMap or $BumpMap2
// Using it outside of BUMPMAPPED will spew an error, warning you about undefined behaviour
#if BUMPMAPPED
#define Sampler_BumpMap		(Sampler_Texture1)
#define Sampler_BumpMap2 	(Sampler_Texture2)
#endif

#endif // LUX_CUSTOM_PS_H_
