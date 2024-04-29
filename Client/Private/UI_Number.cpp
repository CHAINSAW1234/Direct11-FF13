#include "stdafx.h"
#include "UI_Number.h"

const _float4 CUI_Number::vTypeColor[TYPE_END] = { {1.f,1.f,1.f,1.f}, { 1.f,0.8f,0.f,1.f }, { 1.f,0.f,0.f,1.f}, {0.f,1.f,0.f, 1.f} };

CUI_Number::CUI_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Number::CUI_Number(const CUI_Number& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Number::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Number::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	UI_NUMBER_DESC* pUI_Number_Desc = (UI_NUMBER_DESC*)pArg;

	m_eType = pUI_Number_Desc->eType;
	m_iNumber = pUI_Number_Desc->iNumber;

	if (m_iNumber <= 0)
		return E_FAIL;


	_float4 vPosition = pUI_Number_Desc->vPosition;
	vPosition.x += Random_Float(2);
	vPosition.y += 1.f;
	vPosition.y += Random_Float(2);
	vPosition.z += Random_Float(2);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
 
	m_eState = LERP;
	m_iCurNumber = (_int)(m_iNumber * 0.5) - rand() % (m_iNumber/10 +1);
	m_iCurNumber = max(m_iCurNumber, 1);
	m_iStartNumber = m_iCurNumber;

	return S_OK;
}

void CUI_Number::Tick(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;

	Update_Position();
	switch (m_eState) {
	case LERP:
		Lerp();
		break;
	case IDLE:
		Idle();
		break;
	case DISAPPEAR:
		Disappear(fTimeDelta);
		break;
	}

}

HRESULT CUI_Number::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Number::Render()
{
	if (FAILED(m_pGameInstance->Render_Font(g_strFontEng32Tag, to_wstring(m_iCurNumber), m_vFontPosition, XMLoadFloat4(&Get_Color(m_eType)), 0.f)))
		return E_FAIL;

	return S_OK;
}

_float4 CUI_Number::Get_Color(TYPE eType)
{
	return vTypeColor[eType];
}

void CUI_Number::Update_Position()
{
	_vector vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	_matrix vViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix vProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
	vPosition = XMVector3TransformCoord(XMVector3TransformCoord(vPosition, vViewMatrix), vProjMatrix);

	_float4 vPos;
	XMStoreFloat4(&vPos, vPosition);

	vPos.x *= g_iWinSizeX / 2;
	vPos.y *= g_iWinSizeY / 2;
	vPos.z = 0.f;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_vFontPosition = { vPos.x + (g_iWinSizeX - m_fSizeX) * 0.5f ,-vPos.y + g_iWinSizeY * 0.5f };
}

void CUI_Number::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
}

void CUI_Number::Lerp()
{
	if (m_fTimeDelta >= 1.f) {
		m_iCurNumber = m_iNumber;
		Change_State(IDLE);
		return;
	}

	m_iCurNumber = m_iStartNumber + (_int)round(m_fTimeDelta * _float(m_iNumber - m_iStartNumber));
}

void CUI_Number::Idle()
{
	if (m_fTimeDelta >= .7f) {
		Change_State(DISAPPEAR);
		return;
	}
}

void CUI_Number::Disappear(_float fTimeDelta)
{

	m_pTransformCom->Go_Up(fTimeDelta * 0.3f);
	Update_Position();

	if (m_fTimeDelta >= 1.f) {
		Set_Dead(true);
	}
}

CUI_Number* CUI_Number::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Number* pInstance = new CUI_Number(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Number"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI_Number* CUI_Number::Clone(void* pArg)
{
	CUI_Number* pInstance = new CUI_Number(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Number"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Number::Free()
{
	__super::Free();

}
