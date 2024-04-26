#include "stdafx.h"
#include "Effect_Instance.h"

CEffect_Instance::CEffect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_Instance::CEffect_Instance(const CEffect& rhs)
	: CEffect{ rhs }
{
}

HRESULT CEffect_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Instance::Initialize(void* pArg)
{
	m_eType = EFFECT_INSTANCE;


	if (nullptr == pArg)
		return E_FAIL;

	EFFECT_INSTANCE_DESC* pDesc = (EFFECT_INSTANCE_DESC*)pArg;
	m_eInstance_Desc = pDesc->eInstance_Desc;
	m_strBufferTag = pDesc->strBufferTag;
	m_eInstanceType = pDesc->eInstanceType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Instance::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pVIBufferCom->Begin();

	m_pVIBufferCom->Compute_LifeTime(fTimeDelta);

	switch (m_eMovement) {
	case SPREAD:
		m_pVIBufferCom->Spread(fTimeDelta);
		break;
	case GATHER:
		m_pVIBufferCom->Gather(fTimeDelta);
		break;
	case DIRECTION:
		m_pVIBufferCom->Move_Dir(XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&m_vDirection)), 0.f), fTimeDelta);
		break;
	}

	m_pVIBufferCom->End();

}

HRESULT CEffect_Instance::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	_matrix vCamMatrix = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	_vector vLook = vCamMatrix.r[2];
	vCamMatrix = XMMatrixMultiply(vCamMatrix, m_pTransformCom->Get_WorldMatrix_Inverse());

	if (m_isCamLook) {
		m_pTransformCom->Set_Look(vLook);
	}

	m_pVIBufferCom->Begin();

	m_pVIBufferCom->Set_Up_Camera(vCamMatrix);

	switch (m_ePivotLook) {
	case PIVOT_UP:
		m_pVIBufferCom->Set_Direction_To_Pivot_Up();
		break;
	case PIVOT_UP_REVERSE:
		m_pVIBufferCom->Set_Direction_To_Pivot_Up_Reverse();
		break;
	case PIVOT_RIGHT:
		m_pVIBufferCom->Set_Direction_To_Pivot_Right();
		break;
	case PIVOT_RIGHT_REVERSE:
		m_pVIBufferCom->Set_Direction_To_Pivot_Right_Reverse();
		break;
	default:
		//m_pVIBufferCom->Set_Direction_To_Pivot_None();
		break;
	}

	m_pVIBufferCom->End();

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
	return S_OK;
}

HRESULT CEffect_Instance::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (999 != m_iDissolveTextureIndex) {
		if (FAILED(m_pShaderCom->Begin(1)))	// disslove
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Instance::Start()
{
}

HRESULT CEffect_Instance::Save_Effect(ofstream& OFS)
{
	__super::Save_Effect(OFS);

	OFS.write(reinterpret_cast<const char*>(&m_eInstance_Desc), sizeof(CVIBuffer_Instance::INSTANCE_DESC));

	size_t istrBufferTagSize = m_strBufferTag.size();
	OFS.write(reinterpret_cast<const char*>(&istrBufferTagSize), sizeof(size_t));
	OFS.write(reinterpret_cast<const char*>(m_strBufferTag.c_str()), sizeof(_tchar) * istrBufferTagSize);

	OFS.write(reinterpret_cast<const char*>(&m_eInstanceType), sizeof(Interface_Instance::TYPE));
	OFS.write(reinterpret_cast<const char*>(&m_ePivotLook), sizeof(PIVOT_LOOK));

	OFS.write(reinterpret_cast<const char*>(&m_eMovement), sizeof(Interface_Instance::MOVEMENT));
	OFS.write(reinterpret_cast<const char*>(&m_vDirection), sizeof(_float4));

	OFS.write(reinterpret_cast<const char*>(&m_isCamLook), sizeof(_bool));

	return S_OK;
}

HRESULT CEffect_Instance::Initialize_Load(ifstream& IFS)
{
	m_eType = EFFECT_INSTANCE;
	if (FAILED(Load_Effect(IFS)))
		return E_FAIL;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Instance::Load_Effect(ifstream& IFS)
{
	__super::Load_Effect(IFS);

	IFS.read(reinterpret_cast<char*>(&m_eInstance_Desc), sizeof(CVIBuffer_Instance::INSTANCE_DESC));

	size_t strBufferTagSize = 0;
	IFS.read(reinterpret_cast<char*>(&strBufferTagSize), sizeof(size_t));
	vector<_tchar> vecBuffer(strBufferTagSize);

	IFS.read(reinterpret_cast<char*>(vecBuffer.data()), sizeof(_tchar) * strBufferTagSize);
	m_strBufferTag = wstring(vecBuffer.begin(), vecBuffer.end());

	IFS.read(reinterpret_cast<char*>(&m_eInstanceType), sizeof(Interface_Instance::TYPE));
	IFS.read(reinterpret_cast<char*>(&m_ePivotLook), sizeof(PIVOT_LOOK));

	IFS.read(reinterpret_cast<char*>(&m_eMovement), sizeof(Interface_Instance::MOVEMENT));
	IFS.read(reinterpret_cast<char*>(&m_vDirection), sizeof(_float4));

	IFS.read(reinterpret_cast<char*>(&m_isCamLook), sizeof(_bool));

	return S_OK;
}

void CEffect_Instance::Reset_Effect()
{
	__super::Reset_Effect();
	
	m_pVIBufferCom->Reset_Instance();
}

HRESULT CEffect_Instance::Add_Components()
{
	/* For.Com_Shader */

	switch (m_eInstanceType) {
	case Interface_Instance::RECT:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	case Interface_Instance::POINT:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	default:
		return E_FAIL;
	}

	/* For.Com_DiffuseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle"),
		TEXT("Com_Texture"), (CComponent**)&m_pDiffuseTextureCom)))
		return E_FAIL;

	///* For.Com_MaskTexture */
	//if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Mask"),
	//	TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
	//	return E_FAIL;

	/* For.Com_DissolveTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
		TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_Level, m_strBufferTag,
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom))) {

		switch (m_eInstanceType) {
		case Interface_Instance::RECT:
			if (FAILED(m_pGameInstance->Add_Prototype(g_Level, m_strBufferTag,
				CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext, m_eInstance_Desc))))
				return E_FAIL;
			break;
		case Interface_Instance::POINT:
			if (FAILED(m_pGameInstance->Add_Prototype(g_Level, m_strBufferTag,
				CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, m_eInstance_Desc))))
				return E_FAIL;
			break;
		}

		if (FAILED(__super::Add_Component(g_Level, m_strBufferTag,
			TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CEffect_Instance::Bind_ShaderResources()
{
	_vector vCamPosition = m_pGameInstance->Get_CamPosition_Vector();

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (nullptr != m_pDiffuseTextureCom && m_iDiffuseTextureIndex != 999) {

		if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iDiffuseTextureIndex)))
			return E_FAIL;
	}
	if (nullptr != m_pDissolveTextureCom && m_iDissolveTextureIndex != 999) {
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", m_iDissolveTextureIndex)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &vCamPosition, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorMagnification", &m_fColorMagnification, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveTime", &m_fDissolveTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CEffect_Instance* CEffect_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Instance* pInstance = new CEffect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Instance"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect_Instance* CEffect_Instance::Clone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& IFS)
{
	CEffect_Instance* pInstance = new CEffect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Load(IFS))) {

		MSG_BOX(TEXT("Failed To Load : CEffect_Instance"));

		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;;
}

CGameObject* CEffect_Instance::Clone(void* pArg)
{
	CEffect_Instance* pInstance = new CEffect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Instance"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Instance::Free()
{
	CEffect::Free();
	Interface_Instance::Free();
}
