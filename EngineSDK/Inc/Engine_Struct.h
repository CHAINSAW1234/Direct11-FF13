#pragma once

namespace Engine
{
	typedef struct Engine_Desc
	{
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX, iWinSizeY;
	}ENGINE_DESC;

	typedef struct Light_Desc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };
		TYPE		eType;

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

	}LIGHT_DESC;



	typedef struct Mesh_Material
	{
		_tchar			MaterialPath[AI_TEXTURE_TYPE_MAX][MAX_PATH];
		class CTexture* MaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	typedef struct
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vTranslation;
		_float		fTime;
	}KEYFRAME;

	typedef struct ENGINE_DLL VTXPos
	{
		XMFLOAT3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];
	}VTXPOS;

	typedef struct ENGINE_DLL VTXPosTex
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL VTXCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXCUBE;

	typedef struct ENGINE_DLL VTXNorTex
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];
	}VTXNORTEX;

	typedef struct ENGINE_DLL VTXMatrix
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
		bool			isLived;

	}VTXMATRIX;

	typedef struct ENGINE_DLL VTXInstance_Rect
	{
		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_RECT;

	typedef struct ENGINE_DLL VTXInstance_Point
	{
		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];
	}VTXINSTANCE_POINT;

	typedef struct ENGINE_DLL VTXMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int	iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];

	}VTXMESH;

	typedef struct ENGINE_DLL VTXAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndices;	// 이 정점에게 영향을 주는 뼈의 모델에서의 인덱스
		XMFLOAT4		vBlendWeights;	// 각각의 뼈가 영향을 주는 정도(가중치)

		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];

	}VTXANIMMESH;

	typedef struct ENGINE_DLL Frame
	{
		_uint					iMaxFrame = 0;			// 최대 프레임 수
		_float					fFrameSpeed = 0.2f;	// 프레임 속도	
		_uint					iCurFrame = 0;		// 현재 프레임
		_float					fTime = 0.f;		// 현재 누적 시간
		_uint					iStartFrame = 0;	// 시작 프레임
	}FRAME;


}