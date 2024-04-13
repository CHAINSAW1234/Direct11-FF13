#include "stdafx.h"
#include "Body.h"
#include "Bone.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody::CBody(const CBody& rhs)
    : CPartObject{ rhs }
{
}

HRESULT CBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	BODY_DESC* pBody_Desc = (BODY_DESC*)pArg;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pBody_Desc->strModelTag)))
		return E_FAIL;

	return S_OK;
}

void CBody::Tick(_float fTimeDelta)
{
}

HRESULT CBody::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pModelCom->Play_Animation(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return S_OK;
}

HRESULT CBody::Render()
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


	return S_OK;
}

_float4 CBody::Get_BonePosition(const char* pBoneName)
{
	const _float4x4* pMatrix = m_pModelCom->Get_BonePtr(pBoneName)->Get_CombinedTransformationMatrix();

	_vector vScale;
	_vector vRoation;
	_vector vTranspose;


	XMMatrixDecompose(&vScale, &vRoation, &vTranspose, XMLoadFloat4x4(&m_WorldMatrix));

	XMMatrixRotationQuaternion(vRoation);

	/*
		_float4x4			m_WorldMatrix;					// 자신의 월드 행렬
	const _float4x4*	m_pParentMatrix = { nullptr };	// 이 파츠를 보유하고 있는 GameObject == Parent의 월드 행렬을 포인터로 보유
	*/
	_matrix vMatrix = XMMatrixRotationQuaternion(vRoation) * XMLoadFloat4x4(pMatrix);
	_float4x4 fMatrix;
	XMStoreFloat4x4(&fMatrix, vMatrix);

	_float4 vPos = { fMatrix._41 + vTranspose.m128_f32[0] ,	fMatrix._42 + vTranspose.m128_f32[1],	fMatrix._43 + vTranspose.m128_f32[2],	1.f };

	return vPos;
}

HRESULT CBody::Add_Components(const wstring& strModelTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;	*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

CBody* CBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody* pInstance = new CBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody::Clone(void* pArg)
{
	CBody* pInstance = new CBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
