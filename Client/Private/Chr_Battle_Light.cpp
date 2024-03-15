#include "stdafx.h"
#include "Chr_Battle_Light.h"

#include "Chr_Battle_Light_Idle.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "ImGUI_Manager.h"

CChr_Battle_Light::CChr_Battle_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CChr_Battle_Light::CChr_Battle_Light(const CChr_Battle_Light& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CChr_Battle_Light::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CChr_Battle_Light::Initialize(void* pArg)
{
    m_eLevel = g_Level;

    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 10.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);


    //m_pModelCom->Set_Animation(0, false);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 20), 0.f, _float(rand() % 20), 1.f));
    //m_vStartPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    return S_OK;
}

void CChr_Battle_Light::Tick(_float fTimeDelta)
{
    m_pFSMCom->Update(fTimeDelta);
    Update_FSMState(fTimeDelta);

    //if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_UPARROW))
    //    m_pModelCom->Set_Animation(m_pModelCom->Get_CurrentAnimationIndex() + 1, true);
    //if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_DOWNARROW))
    //    m_pModelCom->Set_Animation(m_pModelCom->Get_CurrentAnimationIndex() - 1, true);

    static int i = 0;
    if (!i) {
        ++i;
        m_pTransformCom->Look_At_ForLandObject(((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION));
    }

    

    m_pImGUI_Manager->Tick(fTimeDelta);
    Show_ImGUI();
}

HRESULT CChr_Battle_Light::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    m_pModelCom->Play_Animation(fTimeDelta);

    // 임시
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
    return S_OK;
}

HRESULT CChr_Battle_Light::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i) {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        /* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    m_pImGUI_Manager->Render();

    return S_OK;
}

HRESULT CChr_Battle_Light::Change_State(STATE eState)
{
    m_eState = eState;
    m_pFSMCom->ChangeState(eState);

    return S_OK;
}

void CChr_Battle_Light::Change_Animation(ANIMATION_CHR_BATTLE_LIGHT iAnimationIndex, _bool isLoop)
{
    m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

_float4 CChr_Battle_Light::Get_Look()
{
    // Player의 Look vector를 Y값을 지우고 리턴
    _float4 vChrLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
    vChrLook.y = 0;
    XMStoreFloat4(&vChrLook, XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&vChrLook)), 0.f));

    return vChrLook;
}

HRESULT CChr_Battle_Light::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Model_Light_Battle"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    if (FAILED(Add_Component_FSM()))
        return E_FAIL;

    return S_OK;
}

HRESULT CChr_Battle_Light::Add_Component_FSM()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), (CComponent**)&m_pFSMCom)))
        return E_FAIL;

    m_pFSMCom->Add_State(IDLE, CChr_Battle_Light_Idle::Create(this));

    Change_State(IDLE);
    return S_OK;
}

HRESULT CChr_Battle_Light::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

void CChr_Battle_Light::Update_FSMState(_float fTimeDelta)
{
 /*   if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
        m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
        m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
        m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
        Change_State(MOVE);
    else {
        Change_State(IDLE);
    }*/
}

void CChr_Battle_Light::Show_ImGUI()
{
    _float4x4 worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
    _float vRight[4] = { worldmatrix.m[0][0], worldmatrix.m[0][1] , worldmatrix.m[0][2] , worldmatrix.m[0][3] };
    _float vUp[4] = { worldmatrix.m[1][0], worldmatrix.m[1][1] , worldmatrix.m[1][2] , worldmatrix.m[1][3] };
    _float vLook[4] = { worldmatrix.m[2][0], worldmatrix.m[2][1] , worldmatrix.m[2][2] , worldmatrix.m[2][3] };
    _float vPos[4] = { worldmatrix.m[3][0], worldmatrix.m[3][1] , worldmatrix.m[3][2] , worldmatrix.m[3][3] };

    string str;
    switch (m_eState) {
    case IDLE:
        str = "IDLE";
        break;
    case ATTACK:
        str = "ATTACK";
        break;
    case ITEM:
        str = "ITEM";
        break;
    //case HIT:
    //    str = "HIT";
    //    break;
    case DEAD:
        str = "DEAD";
        break;
    case TP:
        str = "TP";
        break;
    }

    ImGui::Begin("Chr_Field Tool");
    if (ImGui::TreeNode("Transform")) {
        ImGui::InputFloat4("Right", vRight);
        ImGui::InputFloat4("Up", vUp);
        ImGui::InputFloat4("Look", vLook);
        ImGui::InputFloat4("Pos", vPos);

        ImGui::TreePop();
    }
    ImGui::Text("Current State : ");
    ImGui::SameLine();
    ImGui::Text(str.c_str());
    ImGui::Text("Animation Index : %d", m_pModelCom->Get_CurrentAnimationIndex());
    ImGui::Text("Animation Frame : %f", m_pModelCom->Get_CurrentTrackPosition());

    ImGui::End();
}

CChr_Battle_Light* CChr_Battle_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CChr_Battle_Light* pInstance = new CChr_Battle_Light(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CChr_Battle_Light"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CChr_Battle_Light::Clone(void* pArg)
{
    CChr_Battle_Light* pInstance = new CChr_Battle_Light(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CChr_Battle_Light"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChr_Battle_Light::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pFSMCom);

    Safe_Release(m_pImGUI_Manager);
}
