#include "stdafx.h"
#include "Level_Battle.h"

#include "MapObject.h"

#include "Player_Battle.h"
#include "UI.h"

#include "Camera_Field.h"

#include "Chr_Battle_Light.h"
#include "Chr.h"

CLevel_Battle::CLevel_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Battle::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Read_Map()))
        return E_FAIL;

    if (FAILED(Ready_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Grid"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Chr(TEXT("Layer_Chr"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(g_strCameraLayerTag)))
        return E_FAIL;

    m_pPlayer->Start();

    return S_OK;
}

void CLevel_Battle::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
    m_pPlayer->Tick(fTimeDelta);
}

HRESULT CLevel_Battle::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("Battle 레벨입니다."));

    return S_OK;
}

HRESULT CLevel_Battle::Read_Map()
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

HRESULT CLevel_Battle::Ready_UI(const wstring& strLayerTag)
{
    m_pPlayer = CPlayer_Battle::Create();

    if (nullptr == m_pPlayer)
        return E_FAIL;

    CUI::UI_Desc ui_desc = {};
    ui_desc.pObserver_Hander = m_pPlayer;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_UI_ATB"), &ui_desc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevel_Battle::Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix)
{
    CMapObject::MAPOBJECT_DESC MapObjectDesc = {};
    MapObjectDesc.strModelTag = strModelTag;

    CMapObject* pObject = { nullptr };
    if (nullptr == (pObject = dynamic_cast<CMapObject*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_MapObject"), TEXT("Prototype_GameObject_MapObject"), &MapObjectDesc))))
        return E_FAIL;

    dynamic_cast<CTransform*>(pObject->Get_Component(g_strTransformTag))->Set_WorldMatrix(WorldMatrix);

    return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Camera(const wstring& strLayerTag)
{
    CCamera_Field::CAMERA_FIELD_DESC		CameraDesc{};

    CameraDesc.fMouseSensor = 0.1f;
    CameraDesc.fFovy = XMConvertToRadians(60.0f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 1000.0f;
    CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    CameraDesc.fSpeedPerSec = 10.f;
    CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Camera_Field"), &CameraDesc)))
        return E_FAIL;

    dynamic_cast<CCamera_Field*>(m_pGameInstance->Get_GameObject(g_Level, strLayerTag, 0))->Set_Target(
        m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Chr"), 0)
    );
    return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Grid"))))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Chr(const wstring& strLayerTag)
{
    //if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Chr_Field"))))
        //	return E_FAIL;

    CChr_Battle_Light* pInstance = dynamic_cast<CChr_Battle_Light*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Chr_Battle_Light")));

    pInstance->Set_Target(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Monster"), 0));

    return S_OK;
}

HRESULT CLevel_Battle::Ready_Layer_Monster(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Player_Study"))))
        return E_FAIL;

    return S_OK;
}

CLevel_Battle* CLevel_Battle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Battle* pInstance = new CLevel_Battle(pDevice, pContext);

if (FAILED(pInstance->Initialize()))
{
    MSG_BOX(TEXT("Failed To Created : CLevel_Battle"));

    Safe_Release(pInstance);
}

return pInstance;
}

void CLevel_Battle::Free()
{
    __super::Free();
    
    Safe_Release(m_pPlayer);
}
