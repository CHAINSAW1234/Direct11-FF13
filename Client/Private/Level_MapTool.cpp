#include "stdafx.h"
#include "Level_MapTool.h"

#include "GameInstance.h"
#include "ImGUI_Manager.h"
#include "Camera_Free.h"
#include "MapTool.h"

CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MapTool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_IMGUI(TEXT("Layer_IMGUI"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_MapTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

HRESULT CLevel_MapTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Map_Tool �����Դϴ�."));

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, strLayerTag, TEXT("Prototype_GameObject_Grid"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_IMGUI(const wstring& strLayerTag)
{
	LEVEL eLevel = LEVEL_MAPTOOL;
	m_pGameInstance->Add_Clone(LEVEL_MAPTOOL, strLayerTag, TEXT("Prototype_GameObject_MapTool"), &eLevel);
	return S_OK;
}

CLevel_MapTool* CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MapTool* pInstance = new CLevel_MapTool(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("��! CLevel_MapTool �����Ѱ��...."));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MapTool::Free()
{
	__super::Free();
}
