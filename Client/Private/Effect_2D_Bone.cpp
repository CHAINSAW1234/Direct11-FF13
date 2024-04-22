#include "stdafx.h"
#include "Effect_2D_Bone.h"
#include "Bone.h"

CEffect_2D_Bone::CEffect_2D_Bone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_2D_Bone::CEffect_2D_Bone(const CEffect& rhs)
	: CEffect{ rhs }
{
}

HRESULT CEffect_2D_Bone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_2D_Bone::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	EFFECT_2D_BONE_DESC* pDesc = (EFFECT_2D_BONE_DESC*)pArg;
	m_eEffect = pDesc->eEffect;
	m_tFrame = EffectImageFrame[m_eEffect];
	m_pSocket = pDesc->pSocket;
	m_pParentMatrix = pDesc->pParentMatrix;

	Safe_AddRef(m_pSocket);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, Get_Update_Location());
	//m_pTransformCom->Set_Scaled(0.3f, 0.3f, 0.3f);


	return S_OK;
}

void CEffect_2D_Bone::Tick(_float fTimeDelta)
{

	Update_Frame(fTimeDelta);
}

HRESULT CEffect_2D_Bone::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	
	_matrix vMatrix = XMMatrixMultiply(XMLoadFloat4x4(m_pSocket->Get_CombinedTransformationMatrix()), XMLoadFloat4x4(m_pParentMatrix));
	m_pTransformCom->Set_WorldMatrix(vMatrix);
	m_pTransformCom->Set_Scaled(0.3f, 0.3f, 0.3f);
	_vector vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[2];

	m_pTransformCom->Set_Look(vCamLook);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return S_OK;
}

HRESULT CEffect_2D_Bone::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))		//color
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_2D_Bone::Start()
{
}

HRESULT CEffect_2D_Bone::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_tFrame.iCurFrame);
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_2D_Bone::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Gun_Fire"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

void CEffect_2D_Bone::Frame_Finish()
{
	Set_Dead(true);
}

CEffect_2D_Bone* CEffect_2D_Bone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_2D_Bone* pInstance = new CEffect_2D_Bone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_2D_Bone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_2D_Bone::Clone(void* pArg)
{
	CEffect_2D_Bone* pInstance = new CEffect_2D_Bone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_2D_Bone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_2D_Bone::Free()
{
	CEffect::Free();
	Interface_2D::Free();
	Target_Bone::Free();
}
