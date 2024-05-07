#include "stdafx.h"
#include "UI_Chain.h"
#include "Monster.h"
#include "Player_Battle.h"

CUI_Chain::CUI_Chain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Chain::CUI_Chain(const CUI_Chain& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Chain::Initialize_Prototype()
{
	m_isLateGroup = true;
	return S_OK;
}

HRESULT CUI_Chain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_Chain::Tick(_float fTimeDelta)
{
	m_fMaskMovement += fTimeDelta / 10;
	Check_Target();
	Update_Ratio();
	Update_Position(fTimeDelta);
}

HRESULT CUI_Chain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */

	if (m_isBreak) {
		if (FAILED(m_pShaderCom->Begin(3)))	// inner
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(4)))	// border
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		if (FAILED(Render_BreakChain()))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Begin(3)))	// inner1	: fRatio, fCurRatio 반영	// pass 바꿀것
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(4)))	// Border
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		if (FAILED(Render_Chain()))
			return E_FAIL;
	}

	if (FAILED(Render_Name()))
		return E_FAIL;

	return S_OK;
}

void CUI_Chain::Start()
{
	m_fSizeX = 300;
	m_fSizeY = 10;
	m_fMoveTimeDelta = 1.f;
	m_vTargetPosition = { 400.f, 250.f, 0.f };
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vTargetPosition), 1.f));

	m_vFontPosition = { m_vTargetPosition.x  + (g_iWinSizeX - m_fSizeX) * 0.5f , -m_vTargetPosition.y + g_iWinSizeY * 0.5f + 5.f };
}	

void CUI_Chain::OnNotify()
{
}

void CUI_Chain::Change_Target(CGameObject* pGameObject)
{
	m_isRender = true;
	m_isChasing = true;
	Safe_Release(m_pTarget);
	m_pTarget = pGameObject;
	Safe_AddRef(m_pTarget);

}

HRESULT CUI_Chain::Bind_ShaderResources()
{
	_float4 vColor = { 1.f,0.f,0.f,1.f };
	_float4 vCurColor = { 1.f,.83f,0.f,1.f };
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);
	m_pTextureBorderCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Border", 0);
	m_pTextureMaskCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Mask", 0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurColor", &vCurColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurRatio", &m_fCurRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskMovement", &m_fMaskMovement, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chain::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture_Inner */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Inner"),
		TEXT("Com_Texture_Inner"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture_Border */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Chain_Border"),
		TEXT("Com_Texture_Border"), (CComponent**)&m_pTextureBorderCom)))
		return E_FAIL;

	/* For.Com_Texture_UV */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Mask"),
		TEXT("Com_Texture_UV"), (CComponent**)&m_pTextureMaskCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chain::Render_Name()
{
	_float2 vFontPosition = m_vFontPosition;
	vFontPosition.y -= 40.f;

	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strName, { vFontPosition.x - 1, vFontPosition.y - 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strName, { vFontPosition.x - 1, vFontPosition.y + 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strName, { vFontPosition.x + 1, vFontPosition.y - 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strName, { vFontPosition.x + 1, vFontPosition.y + 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, m_strName, vFontPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chain::Render_BreakChain()
{
	_float4 vColor = { 1.f,0.83f,0.f,1.f };
	_float2 vFontPosition = m_vFontPosition;
	wstring strChain = to_wstring((_int)m_fChain);
	if (FAILED(m_pGameInstance->Render_Font(g_strFontEng32Tag, strChain, vFontPosition, XMLoadFloat4(&vColor), 0.f)))
		return E_FAIL;

	vFontPosition.x += 130.f;

	strChain = TEXT(".") + to_wstring((_int(m_fChain * 10)) % 10);

	if (FAILED(m_pGameInstance->Render_Font(g_strFontNum24Tag, strChain, vFontPosition, XMLoadFloat4(&vColor), 0.f)))
		return E_FAIL;

	return S_OK;;
}

HRESULT CUI_Chain::Render_Chain()
{
	_float2 vFontPosition = m_vFontPosition;
	wstring strChain = to_wstring((_int)m_fChain);
	if (FAILED(m_pGameInstance->Render_Font(g_strFontEng32Tag, strChain, vFontPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
		return E_FAIL;

	vFontPosition.x += 110.f;


	strChain = TEXT(".") + to_wstring((_int(m_fChain * 10)) % 10) + TEXT("%/") + to_wstring((_int)m_fStagger) + TEXT(".") + to_wstring((_int(m_fStagger * 10)) % 10) + TEXT("%");
	
	if (FAILED(m_pGameInstance->Render_Font(g_strFontNum24Tag, strChain, vFontPosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
		return E_FAIL;

	return S_OK;;
}

void CUI_Chain::Set_Movement()
{
	m_isChasing = false;
	m_fMoveTimeDelta = 0.f;
}

void CUI_Chain::Parse_World_to_ViewPort()
{
	_vector vPosition = ((CTransform*)(m_pTarget->Get_Component(g_strTransformTag)))->Get_State_Vector(CTransform::STATE_POSITION);
	vPosition.m128_f32[1] += 2.f;
	_matrix vViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix vProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
	vPosition = XMVector3TransformCoord(XMVector3TransformCoord(vPosition, vViewMatrix), vProjMatrix);
	_float4 vPos;
	XMStoreFloat4(&vPos, vPosition);


	vPos.x *= g_iWinSizeX / 2;
	vPos.y *= g_iWinSizeY / 2;
	vPos.z = 0.f;

	vPos.x = clamp(vPos.x , -(_float)g_iWinSizeX / 3.f, (_float)g_iWinSizeX / 3.f);
	vPos.y = clamp(vPos.y, -(_float)g_iWinSizeY / 3.f, (_float)g_iWinSizeY / 3.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_vFontPosition = { vPos.x + (g_iWinSizeX - m_fSizeX) * 0.5f ,-vPos.y + g_iWinSizeY * 0.5f + 5.f };

}

void CUI_Chain::Update_Position(_float fTimeDelta)
{
	if (m_isChasing) {
		if (nullptr == m_pTarget)
			return;
		Parse_World_to_ViewPort();
	}
	else {
		Move(fTimeDelta);
	}
}

void CUI_Chain::Move(_float fTimeDelta)
{
	if (m_fMoveTimeDelta >= 1.0f) {
		return;
	}

	m_fMoveTimeDelta += fTimeDelta;
	if (m_fMoveTimeDelta >= 1.0f) {
		m_fMoveTimeDelta = 1.0f;
	}

	_float4 vCurPos = {};
	XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), XMLoadFloat3(&m_vTargetPosition), m_fMoveTimeDelta), 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);

	vCurPos.x += (g_iWinSizeX - m_fSizeX) * 0.5f;
	vCurPos.y = -vCurPos.y + g_iWinSizeY * 0.5f + 5.f;
	m_vFontPosition = { vCurPos.x, vCurPos.y };

}

void CUI_Chain::Update_Ratio()
{
	if (m_pTarget == nullptr)
		return;
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pTarget);
	if (nullptr == pMonster) {
		m_isBreak = false;
		m_fRatio = m_fCurRatio = 0.f;
		m_fChain = m_fCurChain = 100.f;
		m_fStagger = INFINITY;
		return;
	}

	m_strName = pMonster->Get_Name();
	m_isBreak = pMonster->Get_Break();
	m_fChain = pMonster->Get_Chain();
	if (m_isBreak) {
		m_fBreakTimeDelta = pMonster->Get_BreakTime();
	}

	else {
		m_fCurChain = pMonster->Get_CurrentChain();
		m_fStagger = pMonster->Get_Stagger();
	}

	if (m_isBreak) {
		m_fRatio = m_fCurRatio = (30.f - m_fBreakTimeDelta) / 30.f;
	}
	else {
  		m_fRatio = (m_fChain - 100.f) / (m_fStagger - 100.f);
		m_fCurRatio = (m_fCurChain - 100.f) / (m_fStagger-100.f);
	}
}

void CUI_Chain::Check_Target()
{
	if (nullptr == m_pTarget)
		return;

	if (true == m_pTarget->Get_Dead()) {
		Safe_Release(m_pTarget);
		m_pTarget = m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, 0);
		if (nullptr == m_pTarget)
			m_isRender = false;
		else 
			Safe_AddRef(m_pTarget);
	}

}

CUI_Chain* CUI_Chain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Chain* pInstance = new CUI_Chain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Chain"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI* CUI_Chain::Clone(void* pArg)
{
	CUI_Chain* pInstance = new CUI_Chain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Chain"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Chain::Free()
{
	__super::Free();

	Safe_Release(m_pTextureBorderCom);
	Safe_Release(m_pTextureMaskCom);
	//Safe_Release(m_pPlayerInfo);
	Safe_Release(m_pTarget);
}
