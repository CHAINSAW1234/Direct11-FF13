#include "stdafx.h"
#include "Level_Field.h"
#include "Camera_Free.h"
#include "MapObject.h"
CLevel_Field::CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Field::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;
    
	if (FAILED(Read_Map()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;


	return S_OK;

}

void CLevel_Field::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

HRESULT CLevel_Field::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("필드 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Field::Read_Map()
{
	ifstream IFS{ strMapTag, ios::in | ios::binary };

	size_t iModelNum = 0;
	IFS.read(reinterpret_cast<char*>(&iModelNum), sizeof(size_t));

	for (size_t i = 0; i < iModelNum; ++i) {
		size_t strModelTagLength;
		wstring strModelTag;

		IFS.read(reinterpret_cast<char*>(&strModelTagLength), sizeof(size_t));
		vector<_tchar> vecBuffer(strModelTagLength);

		IFS.read(reinterpret_cast<char*>(vecBuffer.data()), sizeof(_tchar) * strModelTagLength);
		strModelTag = wstring(vecBuffer.begin(), vecBuffer.end());

		_float4x4 WorldMatrix;
		IFS.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));
		if (FAILED(Create_MapObject(strModelTag, WorldMatrix))) {
			return E_FAIL;
		}
	}
	IFS.close();
	return S_OK;
}

HRESULT CLevel_Field::Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix)
{
	CMapObject::MAPOBJECT_DESC MapObjectDesc = {};
	MapObjectDesc.strModelTag = strModelTag;

	CMapObject* pObject = { nullptr };
	if (nullptr == (pObject = dynamic_cast<CMapObject*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_MapObject"), TEXT("Prototype_GameObject_MapObject"), &MapObjectDesc))))
		return E_FAIL;

	dynamic_cast<CTransform*>(pObject->Get_Component(g_strTransformTag))->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Camera(const wstring& strLayerTag)
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

	if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Field* CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Field* pInstance = new CLevel_Field(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Field::Free()
{
	__super::Free();
}
