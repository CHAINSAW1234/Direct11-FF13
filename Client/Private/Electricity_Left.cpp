#include "stdafx.h"
#include "Electricity_Left.h"

CElectricity_Left::CElectricity_Left(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CElectricity_Left::CElectricity_Left(const CElectricity_Left& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CElectricity_Left::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElectricity_Left::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10;    // 0.5 초이후 도달
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_vector vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[2];
	_float vTurnTime = 2.f + Random_Float(4.f);

	ELECTRICITY_LEFT_DESC* pDesc = (ELECTRICITY_LEFT_DESC*)pArg;

	_float4 vPosition = pDesc->vPosition;
	vPosition.x += Random_Float(1.f);
	vPosition.y += 1.f +  Random_Float(2.f);
	vPosition.z += Random_Float(1.f);

	if (pDesc->isCamLook) {
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		m_pTransformCom->Set_Look(vCamLook);
	}
	else {
		vPosition.y = 0.f;
		m_pTransformCom->Set_Scaled(1.f, 0.f, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	m_pTransformCom->Turn(vCamLook, vTurnTime);

	m_fTimeMultiplication = 1.f + Random_Float(1.f);

	return S_OK;
}

void CElectricity_Left::Tick(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta * m_fTimeMultiplication;

	if (m_fTimeDelta >= 1.0f)
		Set_Dead(true);
}

HRESULT CElectricity_Left::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	return S_OK;
}

HRESULT CElectricity_Left::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) {
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CElectricity_Left::Start()
{
}

HRESULT CElectricity_Left::Add_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */

	wstring strPrototypeTag = TEXT("Prototype_Component_Model_Boss_Electricity_");
	wstring strComponentTag = TEXT("Com_Model_");

	_int iRand = rand() % 4;

	if (FAILED(__super::Add_Component(m_eLevel, strPrototypeTag + to_wstring(iRand),
		strComponentTag + to_wstring(iRand), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CElectricity_Left::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveTime", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CElectricity_Left* CElectricity_Left::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElectricity_Left* pInstance = new CElectricity_Left(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CElectricity_Left"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElectricity_Left::Clone(void* pArg)
{
	CElectricity_Left* pInstance = new CElectricity_Left(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CElectricity_Left"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElectricity_Left::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
