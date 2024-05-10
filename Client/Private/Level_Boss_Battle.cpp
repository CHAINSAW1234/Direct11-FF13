#include "stdafx.h"
#include "Level_Boss_Battle.h"

#include "Camera_Battle.h"
#include "MapObject.h"

#include "UI.h"
#include "Player_Battle.h"

#include "Chr_Battle_Light.h"
#include "Monster.h"

CLevel_Boss_Battle::CLevel_Boss_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Boss_Battle::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Read_Map()))
        return E_FAIL;

    if (FAILED(Ready_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    //if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Grid"))))
    //    return E_FAIL;

    if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(g_strMonsterLayerTag)))
        return E_FAIL;

    if (FAILED(Ready_Layer_Chr(g_strChrLayerTag)))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(g_strCameraLayerTag)))
        return E_FAIL;

    // shadow 카메라 gara 세팅
    m_pGameInstance->Set_Shadow_Transform(CPipeLine::D3DTS_VIEW, XMMatrixLookAtLH(XMVectorSet(0.f, 30.f, 10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
    m_pGameInstance->Set_Shadow_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 2000.f));

    m_pPlayer->Start();
    Set_Object_Position();

    return S_OK;
}

void CLevel_Boss_Battle::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
    m_pPlayer->Tick(fTimeDelta);

    if (0 == m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag)) {
        if (m_fTimeDelta == 0) {
            m_pGameInstance->StopSound(CSound_Manager::BGM);
            m_pGameInstance->PlayBGM(TEXT("BGM_Battle_End.wav"));
        }

    }

}

HRESULT CLevel_Boss_Battle::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("Boss Battle 레벨입니다."));

    return S_OK;
}

HRESULT CLevel_Boss_Battle::Read_Map()
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

HRESULT CLevel_Boss_Battle::Ready_UI(const wstring& strLayerTag)
{
    m_pPlayer = CPlayer_Battle::Create();

    if (nullptr == m_pPlayer)
        return E_FAIL;

    CUI::UI_Desc ui_desc = {};
    ui_desc.pObserver_Hander = m_pPlayer;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_UI_ATB"), &ui_desc)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_UI_Cursor"), &ui_desc)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Inventory"), TEXT("Prototype_GameObject_Inventory"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, g_strBackGroundLayerTag, TEXT("Prototype_GameObject_UI_Warning"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, g_strBackGroundLayerTag, TEXT("Prototype_GameObject_LoadingOutBlur"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss_Battle::Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix)
{
    CMapObject::MAPOBJECT_DESC MapObjectDesc = {};
    MapObjectDesc.strModelTag = strModelTag;

    CMapObject* pObject = { nullptr };
    if (nullptr == (pObject = dynamic_cast<CMapObject*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_MapObject"), TEXT("Prototype_GameObject_MapObject"), &MapObjectDesc))))
        return E_FAIL;

    dynamic_cast<CTransform*>(pObject->Get_Component(g_strTransformTag))->Set_WorldMatrix(WorldMatrix);
    pObject->Tick_Navigation();
    return S_OK;
}

HRESULT CLevel_Boss_Battle::Ready_Layer_Camera(const wstring& strLayerTag)
{
    CCamera::CAMERA_DESC		CameraDesc{};

    CameraDesc.fFovy = XMConvertToRadians(60.0f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 1000.0f;
    CameraDesc.vEye = _float4(5.f, .5f, -2.f, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    CameraDesc.fSpeedPerSec = 10.f;
    CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    CCamera_Battle* pCamera = (CCamera_Battle*)m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Camera_Battle"), &CameraDesc);
    pCamera->Set_YOffset(4.f);

    return S_OK;
}

HRESULT CLevel_Boss_Battle::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Grid"))))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Boss_Battle::Ready_Layer_Chr(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Chr_Battle_Light"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Chr_Battle_Sazh"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Chr_Battle_Vanila"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss_Battle::Ready_Layer_Monster(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Boss"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss_Battle::Ready_Layer_Sky(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Boss_Battle::Set_Object_Position()
{
    for (_int i = 1; i < 4; ++i) {
        CChr_Battle* pChr_Battle = (CChr_Battle*)m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, i - 1);
        pChr_Battle->Set_StartPosition(_float4({ (i / 2) * _float(pow(-1, (i % 2))) * -3.f, 0.f, ((i / 2) + 1) * -3.f, 1.f }));
    }
}

CLevel_Boss_Battle* CLevel_Boss_Battle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Boss_Battle* pInstance = new CLevel_Boss_Battle(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Created : CLevel_Boss_Battle"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Boss_Battle::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    m_pGameInstance->StopSound(CSound_Manager::BGM);
}
