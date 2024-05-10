#include "stdafx.h"
#include "Sphere.h"

#include "Model.h"
#include "Shader.h"

#include "Effect.h"
#include "Effect_Instance.h"
#include "Effect_Camera_Look.h"
#include "Monster.h"
#include "Chr_Battle.h"

CSphere::CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSphere::CSphere(const CSphere& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CSphere::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSphere::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	SPHERE_DESC* pSphere_Desc = (SPHERE_DESC*)pArg;

	m_isTargetMonster = pSphere_Desc->isTargetMonster;
	m_iDamage = pSphere_Desc->iDamage;
	m_fChain = pSphere_Desc->fChain;
	m_eSkill = pSphere_Desc->eSkill;
	m_pTargetObject = pSphere_Desc->pTargetObject;
	Safe_AddRef(m_pTargetObject);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pSphere_Desc->vStartPosition);

	m_fStartPosition = pSphere_Desc->vStartPosition;
	for (_int i = 0; i < 3; ++i) {
		m_fOBBDirection[i].x = Random_Float(2.f);
		m_fOBBDirection[i].y = Random_Float(2.f);
		m_fOBBDirection[i].z = Random_Float(2.f);
	}
	for (_int i = 0; i < 16; ++i) {
		m_fTrailPosition[i] = m_fStartPosition;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	string strEffectFilePath;
	wstring strSoundTag;

	switch (m_eSkill) {
	case CRole::RUIN:	
		strEffectFilePath = "../Bin/Resources/Effect/Aero_Projectile.dat";
		strSoundTag = TEXT("Ruin_Start.wav");
		break;
	case CRole::AERO:
		strEffectFilePath = "../Bin/Resources/Effect/Aero_Projectile.dat";
		strSoundTag = TEXT("Aero_Start.wav");
		break;
	case CRole::WATER:
		strEffectFilePath = "../Bin/Resources/Effect/Water_Projectile.dat";
		strSoundTag = TEXT("Water_Start.wav");
		break;
	case CRole::FIRE:
		strEffectFilePath = "../Bin/Resources/Effect/Fire_Projectile.dat";
		strSoundTag = TEXT("Fire_Start.wav");
		break;
	case CRole::CURE:
		strEffectFilePath = "../Bin/Resources/Effect/Heal_Projectile.dat";
		strSoundTag = TEXT("Cure_Start.wav");
		break;
	}

	if(FAILED(CEffect::Read_File_Loop(strEffectFilePath, m_pGameInstance, m_pDevice, m_pContext, &m_Effects)))
		return E_FAIL;
	m_pGameInstance->PlaySoundDuplicate(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);


	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	for (auto& pEffect : m_Effects) {
		pEffect->Set_Position(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		pEffect->Tick(0);
	}

	return S_OK;
}

void CSphere::Tick(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	m_fParticleCreateTimeDelta += fTimeDelta;

	Move(fTimeDelta);
	Update_Trail();
	for (auto& pEffect : m_Effects)
		pEffect->Set_Position(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));


	if (m_fParticleCreateTimeDelta >= 0.2) {
		vector<CEffect*> Effects;
		CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Particle_Projectile.dat", m_pGameInstance, m_pDevice, m_pContext, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), &Effects);

		_float4 fDirection = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);

		for (auto& pEffect : Effects) {
			((CEffect_Instance*)pEffect)->Set_Direction(fDirection);
			Safe_Release(pEffect);
		}
		m_fParticleCreateTimeDelta = 0.f;
	}


	if (m_pTargetObject->Get_Dead()) {
		Set_Dead(true);
		for (auto& pEffect : m_Effects)
			pEffect->Set_Dead(true);
		return;
	}

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	Check_Collision();
}

HRESULT CSphere::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CSphere::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSphere::Start()
{
}

HRESULT CSphere::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderSphereDesc = {};
	ColliderSphereDesc.fRadius = .1f;
	ColliderSphereDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderSphereDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Trail"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSphere::Bind_ShaderResources()
{
	_float fSizeY = .5f;
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vPosition", &m_fTrailPosition, sizeof(_float4) * 16 )))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &fSizeY, sizeof(_float))))
		return E_FAIL;

	if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	return S_OK;
}

void CSphere::Move(_float fTimeDelta)
{
	CTransform* pTransformCom = (CTransform*)m_pTargetObject->Get_Component(g_strTransformTag);
	_vector vTargetPos = pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	if (m_isTargetMonster) {
		vTargetPos.m128_f32[1] += ((CMonster*)m_pTargetObject)->Get_ColliderSize().y * 0.5f;
	}
	else {
		vTargetPos.m128_f32[1] += ((CChr_Battle*)m_pTargetObject)->Get_ColliderSize().y * 0.5f;
	}

	m_pTransformCom->Look_At(vTargetPos);
	//m_pTransformCom->Go_Straight(fTimeDelta);	// 임시

	m_pTransformCom->Set_Look(XMVectorSetW(XMVector3Normalize(vTargetPos - XMLoadFloat4(&m_fStartPosition)), 0.f));

	_vector vRight = m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT);
	_vector vUp = m_pTransformCom->Get_State_Vector(CTransform::STATE_UP);
	_vector vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);

	m_fDist = XMVector3Length(vTargetPos - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION)).m128_f32[0];
	// 1. 처음에 저장된 점의 비율을 통해서 점의 위치를 결정
	_vector vPoint[5] = {};
	vPoint[0] = XMLoadFloat4(&m_fStartPosition);
	vPoint[4] =  vTargetPos;

	for (_int i = 1; i < 4; ++i) {
		_vector vPosition = vPoint[0] + vLook * m_fDist * 0.5f;

		vPosition += m_fOBBDirection[i-1].x * vRight;
		vPosition += m_fOBBDirection[i-1].y * vUp;
		vPosition += m_fOBBDirection[i-1].z * vLook * m_fDist * 0.5f;

		vPoint[i] = vPosition;
	}

	// 2. Quartic Bezier 곡선을 적용해서 새로운 위치를 계산후 적용 
	// B(t) = (1-t)^4 * P0 + 4(1-t)^3 * t * P1 + 6(1-t)^2 * t^2 * P2 + 4(1-t) * t^3 * P3 + t^4 * P4
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		pow((1 - m_fTimeDelta), 4) * vPoint[0] +
		4 * pow((1 - m_fTimeDelta), 3) * m_fTimeDelta * vPoint[1] +
		6 * pow((1 - m_fTimeDelta), 2) * pow(m_fTimeDelta, 2) * vPoint[2] +
		4 * (1 - m_fTimeDelta) * pow(m_fTimeDelta, 3) * vPoint[3] +
		pow(m_fTimeDelta, 4) * vPoint[4]);

}

void CSphere::Check_Collision()
{
	CCollider* pColliderCom = (CCollider*)m_pTargetObject->Get_Component(TEXT("Com_Collider"));
	if (nullptr == pColliderCom)
		return;

	if (m_pColliderCom->Intersect(pColliderCom)) {

		if (m_isTargetMonster) {
			((CMonster*)m_pTargetObject)->Set_Hit(m_iDamage, m_fChain);
		}
		else {
			((CChr_Battle*)m_pTargetObject)->Set_Hit(m_iDamage);
		}

		Set_Dead(true);
		for (auto& pEffect : m_Effects)
			pEffect->Set_Dead(true);

		string strEffectFilePath;
		wstring strSoundTag;
		switch (m_eSkill) {
		case CRole::RUIN:
			strEffectFilePath = "../Bin/Resources/Effect/Ruin_Hit_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Ruin_Hit.wav");
			break;
		case CRole::AERO:
			strEffectFilePath = "../Bin/Resources/Effect/Aero_Hit_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Aero_Hit.wav");
			break;
		case CRole::WATER:
			strEffectFilePath = "../Bin/Resources/Effect/Water_Hit_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Water_Hit.wav");
			break;
		case CRole::FIRE:
			strEffectFilePath = "../Bin/Resources/Effect/Fire_Hit_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Fire_Hit.wav");
			break;
		case CRole::CURE:
			strEffectFilePath = "../Bin/Resources/Effect/Heal_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Cure_Hit.wav");
			break;
		}

		if (FAILED(CEffect::Read_File_NoLoop(strEffectFilePath, m_pGameInstance, m_pDevice, m_pContext, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION))))
			return ;
		m_pGameInstance->PlaySoundDuplicate(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);

	}
}

void CSphere::Update_Trail()
{
	for (_int i = 16 - 1; i >=1; --i) {
		m_fTrailPosition[i] = m_fTrailPosition[i - 1];
	}
	m_fTrailPosition[0] = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

}

CSphere* CSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphere* pInstance = new CSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSphere::Clone(void* pArg)
{
	CSphere* pInstance = new CSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSphere::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTargetObject);

	for (auto& pEffect : m_Effects)
		Safe_Release(pEffect);
	m_Effects.clear();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}