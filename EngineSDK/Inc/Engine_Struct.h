#pragma once

namespace Engine
{
	typedef struct Engine_Desc
	{
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX, iWinSizeY;
	}ENGINE_DESC;


	typedef struct ENGINE_DLL VTXPosTex
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOSTEX;

	


}