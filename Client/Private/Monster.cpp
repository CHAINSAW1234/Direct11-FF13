#include "stdafx.h"
#include "Monster.h"

#include "UI.h"
#include "FSM.h"
#include "Model.h"
#include "Shader.h"
#include "Chr_Battle.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject { pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& rhs)
    : CGameObject { rhs }
    , m_strMonsterName{ rhs.m_strMonsterName }
    , m_iMaxHp{ rhs.m_iMaxHp }
    , m_iHp{ rhs.m_iMaxHp }
    , m_fStagger{ rhs.m_fStagger }
{
}

HRESULT CMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
    m_eLevel = g_Level;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Create_UI_Hp()))
        return E_FAIL;
    return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
    //m_pFSMCom->Update(fTimeDelta);
    Update_Chain(fTimeDelta);
}

HRESULT CMonster::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    m_pModelCom->Play_Animation(fTimeDelta);

    return S_OK;
}

HRESULT CMonster::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        /* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

#ifdef _DEBUG
    if (nullptr != m_pColliderCom)
        m_pColliderCom->Render();

#endif 

    return S_OK;
}

void CMonster::Start()
{
}

_float4 CMonster::Get_TargetPosition()
{
    if (nullptr == m_pTargetObject)
        return _float4(0.f, 0.f, 0.f, 1.f);

    return m_pTargetObject->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION);
}

void CMonster::Set_Target(CChr_Battle* pTargetObject)
{
    if (nullptr != m_pTargetObject)
        Safe_Release(m_pTargetObject);

    m_pTargetObject = pTargetObject;
    Safe_AddRef(m_pTargetObject);
}

void CMonster::Set_isTarget(_bool isTarget)
{
    m_isTarget = isTarget;
    NotifyObserver();
}

_uint CMonster::Get_CurrentAnimationIndex()
{
    if (nullptr == m_pModelCom)
        return INFINITE;

    return m_pModelCom->Get_CurrentAnimationIndex();
}

_float CMonster::Get_CurrentTrackPosition()
{
    if (nullptr == m_pModelCom)
        return INFINITY;
    return m_pModelCom->Get_CurrentTrackPosition();
}

_bool CMonster::Is_Animation_Finished()
{
    if (nullptr == m_pModelCom)
        return false;
    return m_pModelCom->isFinished();
}

void CMonster::Add_Hp(_int iHp)
{
    m_iHp += iHp;
    m_iHp = min(m_iHp, m_iMaxHp);
}

void CMonster::Min_Hp(_int iHp)
{
    m_iHp -= iHp;
    m_iHp = max(m_iHp, 0);
}

void CMonster::Update_Attack_Time(_float fTimeDelta)
{
    m_fAttackTimeDelta += fTimeDelta;
}

void CMonster::Add_Chain(_float fChain)
{
    m_fChain += fChain;
    m_fCurChain = m_fChain;
    m_fMagnification = 1 / (m_fStagger - 100) * 0.1f;
    if (!m_isBreak && m_fChain >= m_fStagger) {
        m_fChain = m_fStagger + 100.f;
        m_isBreak = true;
        m_fBreakTimeDelta = 0.f;
    }
}

_float CMonster::Cal_Degree_Start()
{
    _float4 vLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
    _vector vDir_to_Start = XMLoadFloat4(&m_vStartPosition) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    vDir_to_Start.m128_f32[1] = 0.f;
    vDir_to_Start = XMVector3Normalize(vDir_to_Start);

    _float4 vTargetLook;
    XMStoreFloat4(&vTargetLook, vDir_to_Start);
    return Cal_Degree_From_Directions_Between_Min180_To_180(vLook, vTargetLook);
}

_float CMonster::Cal_Degree_Target()
{
    if (nullptr == m_pTargetObject) {
        return Cal_Degree_Start();
    }

    _float4 vLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
    _vector vDir_to_Target = m_pTargetObject->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    vDir_to_Target.m128_f32[1] = 0.f;
    vDir_to_Target = XMVector3Normalize(vDir_to_Target);

    _float4 vTargetLook;
    XMStoreFloat4(&vTargetLook, vDir_to_Target);
    return Cal_Degree_From_Directions_Between_Min180_To_180(vLook, vTargetLook);
}

HRESULT CMonster::Create_UI_Hp()
{
    CUI::UI_DESC UI_desc = {};
    UI_desc.pObserver_Hander = this;

    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Monster_Hp"), &UI_desc)))
        return E_FAIL;

    return S_OK;
}

void CMonster::Update_Chain(_float fTimeDelta)
{
    if (m_isBreak) {
        m_fBreakTimeDelta += fTimeDelta;
        if (m_fBreakTimeDelta >= 20.f) {
            m_isBreak = false;
            m_fChain = 100.f;
            m_fCurChain = 100.f;
        }
    }
    else {
        m_fMagnification += fTimeDelta / 10;
        m_fCurChain -= fTimeDelta * m_fMagnification;
        if (m_fCurChain <= 100.f) {
            m_fChain = 100.f;
            m_fCurChain = 100.f;
            m_fMagnification = 1 / (m_fStagger -100) * 0.1f;
        }
    }
}

HRESULT CMonster::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    if (FAILED(Add_Component_FSM()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster::Add_Component_FSM()
{
    /* For.Com_FSM */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), (CComponent**)&m_pFSMCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    /*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;	*/

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

void CMonster::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pFSMCom);
    Safe_Release(m_pColliderCom);
    //Safe_Release(m_pTargetObject);
}
