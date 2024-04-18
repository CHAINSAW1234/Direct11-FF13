#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{	

}

CTransform::CTransform(const CTransform& rhs)
	: CComponent{ rhs }
	, m_WorldMatrix{ rhs.m_WorldMatrix }
{

}

void CTransform::Set_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State_Vector(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State_Vector(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State_Vector(STATE_LOOK)) * fScaleZ);
		
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC*		pDesc = (TRANSFORM_DESC*)pArg;

		m_fSpeedPerSec = pDesc->fSpeedPerSec;
		m_fRotationPerSec = pDesc->fRotationPerSec;
	}

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
	
}

void CTransform::Set_Position(const _float4& vPosition, CNavigation* pNavigation)
{
	if (nullptr == pNavigation)
		Set_State(STATE_POSITION, vPosition);
	else
	{
		_vector vPos = XMLoadFloat4(&vPosition);

		if (true == pNavigation->isMove(vPos))
			Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vLook = Get_State_Vector(STATE_LOOK);

	_vector		vMovement = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	XMStoreFloat4(&m_vLastMovement, vMovement);

	if (nullptr == pNavigation)
		Set_State(STATE_POSITION, vPosition + vMovement);
	else
	{
		if (true == pNavigation->isMove(vPosition + vMovement))
			Set_State(STATE_POSITION, vPosition + vMovement);
		else {
			// 슬라이딩 백터 처리
			_vector vSliding = XMLoadFloat4(&pNavigation->Sliding(vPosition + vMovement, vMovement));
			Move_To_Direction(vSliding, fTimeDelta, pNavigation);
		}
	}
}



void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vLook = Get_State_Vector(STATE_LOOK);

	_vector		vMovement = -XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	XMStoreFloat4(&m_vLastMovement, vMovement);

	if (nullptr == pNavigation)
		Set_State(STATE_POSITION, vPosition + vMovement);
	else
	{
		if (true == pNavigation->isMove(vPosition + vMovement))
			Set_State(STATE_POSITION, vPosition + vMovement);
		else {
			// 슬라이딩 백터 처리
			_vector vSliding = XMLoadFloat4(&pNavigation->Sliding(vPosition + vMovement, vMovement));
			Move_To_Direction(vSliding, fTimeDelta, pNavigation);
		}
	}
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vRight = Get_State_Vector(STATE_RIGHT);

	_vector		vMovement = -XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	XMStoreFloat4(&m_vLastMovement, vMovement);

	if (nullptr == pNavigation)
		Set_State(STATE_POSITION, vPosition + vMovement);
	else
	{
		if (true == pNavigation->isMove(vPosition + vMovement))
			Set_State(STATE_POSITION, vPosition + vMovement);
		else {
			// 슬라이딩 백터 처리
			_vector vSliding = XMLoadFloat4(&pNavigation->Sliding(vPosition + vMovement, vMovement));
			Move_To_Direction(vSliding, fTimeDelta, pNavigation);
		}
	}
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vRight = Get_State_Vector(STATE_RIGHT);

	_vector		vMovement = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	XMStoreFloat4(&m_vLastMovement, vMovement);

	if (nullptr == pNavigation)
		Set_State(STATE_POSITION, vPosition + vMovement);
	else
	{
		if (true == pNavigation->isMove(vPosition + vMovement))
			Set_State(STATE_POSITION, vPosition + vMovement);
		else {
			// 슬라이딩 백터 처리
			_vector vSliding = XMLoadFloat4(&pNavigation->Sliding(vPosition + vMovement, vMovement));
			Move_To_Direction(vSliding, fTimeDelta, pNavigation);
		}
	}
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vUp = Get_State_Vector(STATE_UP);

	_vector		vMovement = XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;
	XMStoreFloat4(&m_vLastMovement, vMovement);

	vPosition += vMovement;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vUp = Get_State_Vector(STATE_UP);

	_vector		vMovement = -XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;
	XMStoreFloat4(&m_vLastMovement, vMovement);

	vPosition += vMovement;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Set_Look(_fvector vTargetLook)
{
	_vector		vUp = Get_State_Vector(STATE_UP);
	_vector		vLook = XMVector3Normalize(vTargetLook);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, vUp);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Look_At(_fvector vAt)
{
	_vector		vLook = vAt - Get_State_Vector(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Look_At_ForLandObject(_fvector vTargetPosition)
{
	// XMMatrixDecompose();

	_vector		vUp = Get_State_Vector(STATE_UP);
	_vector		vLook = vTargetPosition - Get_State_Vector(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, vUp);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);	
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}


void CTransform::Move_To_Target(_fvector vTargetPos, _float fTimeDelta, _float fMinDistance, CNavigation* pNavigation)
{
	_vector vPos = Get_State_Vector(STATE_POSITION);
	_vector vDir = vTargetPos - vPos;
	_float	fDistance = XMVector3Length(vDir).m128_f32[0];
	_vector vLook = vTargetPos - vPos;

	if (fDistance > fMinDistance)
	{
		vPos += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
		if (nullptr == pNavigation)
			Set_State(STATE_POSITION, XMVectorSetW(vPos, 1.f));
		else
		{
			if (true == pNavigation->isMove(XMVectorSetW(vPos, 1.f)))
				Set_State(STATE_POSITION, XMVectorSetW(vPos, 1.f));
		}
	}
}

void CTransform::Move_To_Direction(_fvector vDirection, _float fTimeDelta, CNavigation* pNavigation)
{
	
	_vector		vMovement = XMVector3Normalize(vDirection) * m_fSpeedPerSec * fTimeDelta;
	XMStoreFloat4(&m_vLastMovement, vMovement);

	if (nullptr == pNavigation)
		Set_State(STATE_POSITION, Get_State_Vector(STATE_POSITION) + vMovement);
	else
	{
		if (true == pNavigation->isMove(Get_State_Vector(STATE_POSITION) + vMovement))
			Set_State(STATE_POSITION, Get_State_Vector(STATE_POSITION) + vMovement);
	}
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix			RotationMatrix  = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	for (size_t i = 0; i < STATE_POSITION; i++)
	{
		Set_State(STATE(i), 
			XMVector4Transform(Get_State_Vector((STATE)i), RotationMatrix));
		/*Set_State(STATE(i),
			XMVector3TransformNormal(Get_State_Vector((STATE)i), RotationMatrix));*/
	}

}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vState[] = {
		XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x,
		XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y, 
		XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z, 		
	};

	// XMConvertToRadians(Degree);
	_matrix			RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	for (size_t i = 0; i < STATE_POSITION; i++)
	{
		Set_State(STATE(i),
			XMVector4Transform(vState[(STATE)i], RotationMatrix));
	}
}

void CTransform::Turn_With_Look_At(_fvector vAxis, _fvector vTargetPosition, _float fDest, _float fTimeDelta, _int iMaxDegree)
{
	_matrix			RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	_vector vLook = XMVector4Transform(Get_State_Vector(STATE_LOOK), RotationMatrix);

	//_float fDifferenceY = vLook.m128_f32[1];
	if (iMaxDegree !=0 && abs(vLook.m128_f32[1]) > sin(XMConvertToRadians((_float)iMaxDegree))) {
		return ;
	}

	for (size_t i = 0; i < STATE_POSITION; i++)
	{
		Set_State(STATE(i),
			XMVector4Transform(Get_State_Vector((STATE)i), RotationMatrix));

	}
	//vLook = Get_State_Vector(STATE_LOOK);
	_vector vNewPos = vTargetPosition - vLook * fDest;
	if (vNewPos.m128_f32[1] <= 0) {
		vNewPos.m128_f32[1] = 0.02f;
	}

	if (vNewPos.m128_f32[1] >= vTargetPosition.m128_f32[1]*2.f) {
		vNewPos.m128_f32[1] = vTargetPosition.m128_f32[1] * 2.f - 0.02f;
	}

	Set_State(STATE_POSITION, vNewPos);
}


CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
