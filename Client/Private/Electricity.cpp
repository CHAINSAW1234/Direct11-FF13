#include "stdafx.h"
#include "Electricity.h"
#include "Effect.h"
#include "Chr_Battle.h"
#include "Electricity_Left.h"

CElectricity::CElectricity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CElectricity::CElectricity(const CElectricity& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CElectricity::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElectricity::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    m_eLevel = g_Level;

    ELECTRICITY_DESC* pDesc = (ELECTRICITY_DESC*)pArg;
    _float vDist = XMVector3Length(XMLoadFloat4(&pDesc->fTargetPosition) - XMLoadFloat4(&pDesc->fStartPosition)).m128_f32[0];

    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 10;    // 0.5 초이후 도달
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fTargetPosition = pDesc->fTargetPosition;
    m_iDamage = pDesc->iDamage;
    m_pTargetObject = pDesc->pTargetObject;
    Safe_AddRef(m_pTargetObject);

    Set_Memeber(pDesc->fStartPosition, pDesc->fTargetPosition);
    m_pTransformCom->Set_Position(pDesc->fStartPosition);
    m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
    m_pTransformCom->Look_At(XMLoadFloat4(&pDesc->fTargetPosition));
    m_pTransformCom->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK), pDesc->iTurnDist / XMConvertToRadians(90.0f));
    m_iNumModel = (_uint)m_pModelComs.size();

    return S_OK;
}

void CElectricity::Tick(_float fTimeDelta)
{
    // 30 프레임동안 보이게 처리 하는 방법
   // 틱 30회 이후 사망 
    if (m_iCurPosition < (_int)m_fDist) {
        m_fTimeDelta += fTimeDelta * 80;
        _int iPrevPositionIndex = m_iCurPosition;
        m_iCurPosition = (_int)floor(m_fTimeDelta);

        if (m_iCurPosition >= (_int)m_fDist) {
            m_iCurPosition = (_int)m_fDist;

            m_pTargetObject->Set_Hit(m_iDamage);
            m_pGameInstance->PlaySoundDuplicate(TEXT("Boss_Attack_Maic_Attack_0.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
            Create_Electricity_Left();
        }

        if (m_iCurPosition != iPrevPositionIndex) {
            vector<CEffect*> pEffects;
            CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Particle_Boss_Electricity.dat", m_pGameInstance, m_pDevice, m_pContext, m_fPositions[m_iCurPosition], &pEffects);
            
            for (auto& pEffect : pEffects) {
                pEffect->Get_Transform()->Look_At(XMLoadFloat4(&m_fTargetPosition));
            }
        }
    }
    else {
        m_fDissolveTimeDelta += fTimeDelta;
    }

    if (m_fDissolveTimeDelta >= 1.f) {
        Set_Dead(true);
    }
}

HRESULT CElectricity::Late_Tick(_float fTimeDelta)
{
    if (FAILED(__super::Late_Tick(fTimeDelta)))
        return E_FAIL;

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
    return S_OK;
}

HRESULT CElectricity::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _int iPass;
    if (m_fDissolveTimeDelta == 0)
        iPass = 5;
    else
        iPass = 6;

    for (_int i = 0; i < m_iCurPosition; ++i) {

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_fPositions[i]);
        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        _uint iNumMeshes = m_pModelComs[i % m_iNumModel]->Get_NumMeshes();

        for (_uint j = 0; j < iNumMeshes; ++j) {
            m_pModelComs[i % m_iNumModel]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", j, aiTextureType_DIFFUSE);

            if (FAILED(m_pShaderCom->Begin(iPass)))
                return E_FAIL;

            m_pModelComs[i % m_iNumModel]->Render(j);
        }
    }

    return S_OK;
}

void CElectricity::Start()
{
}

void CElectricity::Set_Memeber(_float4 fStartPosition, _float4 fTargetPosition)
{
    m_fDist = XMVector3Length(XMLoadFloat4(&fTargetPosition) - XMLoadFloat4(&fStartPosition)).m128_f32[0];
    XMStoreFloat4(&m_fDir, XMVector3Normalize(XMLoadFloat4(&fTargetPosition) - XMLoadFloat4(&fStartPosition)));
    
    _vector vPosition = XMLoadFloat4(&fStartPosition);
    _float4 fPosition;
    for (_int i = 0; i < (_int)ceil(m_fDist); ++i) {
        XMStoreFloat4(&fPosition, vPosition);
        m_fPositions.push_back(fPosition);
        vPosition += XMLoadFloat4(&m_fDir);
    }
}

void CElectricity::Create_Electricity_Left()
{
    CElectricity_Left::ELECTRICITY_LEFT_DESC pDesc = {};
    pDesc.vPosition = m_fTargetPosition;
    pDesc.isCamLook = rand()%3;
    
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity_Left"), &pDesc)))
        return;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity_Left"), &pDesc)))
        return;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity_Left"), &pDesc)))
        return;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity_Left"), &pDesc)))
        return;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity_Left"), &pDesc)))
        return;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity_Left"), &pDesc)))
        return;

    CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Particle_Boss_Electricity_Left.dat", m_pGameInstance, m_pDevice, m_pContext, m_fTargetPosition);


}

HRESULT CElectricity::Add_Components()
{
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Corpse"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */

    wstring strPrototypeTag = TEXT("Prototype_Component_Model_Boss_Electricity_");
    wstring strComponentTag = TEXT("Com_Model_");

    for (int i = 0; i < 4; ++i) {
        CModel* pModel = { nullptr };
        if (FAILED(__super::Add_Component(m_eLevel, strPrototypeTag + to_wstring(i),
            strComponentTag + to_wstring(i), (CComponent**)&pModel)))
            return E_FAIL;

        m_pModelComs.push_back(pModel);
    }

    return S_OK;
}

HRESULT CElectricity::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fColorMagnification = 2.f;
    _float4 vColor = { 0.f,1.f,1.f,1.f };
    _float2 fTextureMovement = { 0.f, 0.f };

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorMagnification", &fColorMagnification, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_TextureMovement", &fTextureMovement, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveTime", &m_fDissolveTimeDelta, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CElectricity* CElectricity::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CElectricity* pInstance = new CElectricity(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CElectricity"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CElectricity::Clone(void* pArg)
{
    CElectricity* pInstance = new CElectricity(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CElectricity"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CElectricity::Free()
{
    __super::Free();

    Safe_Release(m_pTargetObject);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);

    for (auto& pModelCom : m_pModelComs)
        Safe_Release(pModelCom);
    m_pModelComs.clear();
}
