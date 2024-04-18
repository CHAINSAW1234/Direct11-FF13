#include "stdafx.h"
#include "UI_Skill.h"

CUI_Skill::CUI_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_Skill::CUI_Skill(const CUI_Skill& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_Skill::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Skill::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    UI_SKILL_DESC* pUI_Desc = (UI_SKILL_DESC*)pArg;
    
    m_pTargetObject = pUI_Desc->pTargetObject;
    Safe_AddRef(m_pTargetObject);
    m_strSkillName = pUI_Desc->strSkillName;
    m_vColor = pUI_Desc->vColor;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;
   
    m_pTransformBorderCom = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformBorderCom)
        return E_FAIL;

    if (FAILED(m_pTransformBorderCom->Initialize(pArg)))
        return E_FAIL;

    m_Components.emplace(TEXT("Com_TransformBorder"), m_pTransformBorderCom);

    Safe_AddRef(m_pTransformBorderCom);


    Start();

    return S_OK;
}

void CUI_Skill::Tick(_float fTimeDelta)
{
    Update_Position();
}

HRESULT CUI_Skill::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    if (m_pTargetObject->Get_Dead())
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Skill::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pTransformBorderCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(6)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(8)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    if (FAILED(Render_Font()))
        return E_FAIL;

    return S_OK;
}

void CUI_Skill::Start()
{
    m_fSizeX = m_strSkillName.size() * 25.f;
    m_fSizeY = 30.f;
    m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
    m_pTransformBorderCom->Set_Scaled(m_fSizeX + 4.f, m_fSizeY + 4.f, 1.f);
}

HRESULT CUI_Skill::Bind_ShaderResources()
{
    _float fRatio = 1.f;

    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &fRatio, sizeof(_float))))
        return E_FAIL;



    return S_OK;
}

HRESULT CUI_Skill::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

void CUI_Skill::Update_Position()
{
    _vector vPosition = ((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    vPosition.m128_f32[1] += 2.f;
    _matrix vViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
    _matrix vProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
    vPosition = XMVector3TransformCoord(XMVector3TransformCoord(vPosition, vViewMatrix), vProjMatrix);
    _float4 vPos;

    XMStoreFloat4(&vPos, vPosition);

    vPos.x *= g_iWinSizeX / 2;
    vPos.y *= g_iWinSizeY / 2;
    vPos.z = 0.f;

    //vPos.x = clamp(vPos.x, -(_float)g_iWinSizeX / 3.f, (_float)g_iWinSizeX / 3.f);
    //vPos.y = clamp(vPos.y, -(_float)g_iWinSizeY / 3.f, (_float)g_iWinSizeY / 3.f);
    //vPos.y += 10;
    
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
    m_pTransformBorderCom->Set_State(CTransform::STATE_POSITION, vPos);
    m_vFontPosition = { vPos.x + (g_iWinSizeX) * 0.5f ,-vPos.y + (g_iWinSizeY - m_fSizeY) * 0.5f + 2.f };
    m_vFontPosition.x -= (m_strSkillName.size() + 1) * 0.5f * 14.f;

}

HRESULT CUI_Skill::Render_Font()
{
    if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strSkillName, m_vFontPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
        return E_FAIL;

    return S_OK;
}

CUI_Skill* CUI_Skill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Skill* pInstance = new CUI_Skill(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CUI_Skill"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CUI_Skill* CUI_Skill::Clone(void* pArg)
{
    CUI_Skill* pInstance = new CUI_Skill(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CUI_Skill"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Skill::Free()
{
    __super::Free();
    Safe_Release(m_pTransformBorderCom);
    Safe_Release(m_pTargetObject);
}
