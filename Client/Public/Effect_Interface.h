#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Instance.h"

BEGIN(Engine)
class CModel;
class CBone;
END


BEGIN(Client)
class Target_GameObject abstract
{
	typedef struct Target_GameObject_Desc
	{
		CGameObject* pTargetObject = { nullptr };
	} TARGET_GAMEOBJECT_DESC;
protected:
	CGameObject* m_pTargetObject = { nullptr };

public:
	virtual void Free();
};

class Target_Bone abstract
{
public:
	typedef struct Target_Bone_Desc
	{
		CBone* pSocket = { nullptr };
		const _float4x4* pParentMatrix;

	} TARGET_BONE_DESC;

public:
	_float4 Get_Update_Location();

protected:
	CBone* m_pSocket = { nullptr };
	_float4x4 m_WorldMatrix = {};
	const _float4x4* m_pParentMatrix = { nullptr };


public:
	virtual void Free();
};

class Target_Position abstract
{
public:
	typedef struct Target_Position_Desc
	{
		_float4 vPosition;
	}TARGET_POSITION_DESC;
protected:
	_float4 m_vPosition{};
};

class Interface_2D abstract
{
public:
	enum EFFECT { DUST_COLOR, GUN_FIRE, EFFECT_END };
	typedef struct Interface_2D_Desc
	{
		EFFECT eEffect = { EFFECT_END };
	} INTERFACE_2D_DESC;

protected:
	static const Frame EffectImageFrame[EFFECT_END];

protected:
	void	Update_Frame(_float fTimeDelta);
	virtual void Frame_Finish() = 0;		// 프레임 종료시의 동작을 하위 클래스에서 지정할 것

protected:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };

	EFFECT		m_eEffect = {EFFECT_END};
	FRAME		m_tFrame = {};

public:
	virtual void Free();
};

class Interface_3D abstract
{
public:
	typedef struct Interface_3D_Desc
	{
		wstring strModelTag = {};
	} INTERFACE_3D_DESC;

protected:
	CModel* m_pModelCom = { nullptr };
	
	wstring m_strModelTag = {};

public:
	virtual void Free();
};

class Interface_Instance abstract
{
public:
	enum TYPE { RECT, POINT, TYPE_END };
	enum MOVEMENT { SPREAD, GATHER, DIRECTION };
	enum PIVOT_LOOK { PIVOT_UP, PIVOT_UP_REVERSE, PIVOT_RIGHT, PIVOT_RIGHT_REVERSE, PIVOT_LOOK_END };
	typedef struct Interface_Instance_Desc
	{
		CVIBuffer_Instance::INSTANCE_DESC eInstance_Desc;
		TYPE eInstanceType;
		wstring strBufferTag = {};
	} INTERFACE_INSTANCE_DESC;

public:
	TYPE Get_Type() { return m_eInstanceType; }
	PIVOT_LOOK Get_Pivot_Look() { return m_ePivotLook; }
	MOVEMENT Get_Movement() { return m_eMovement; }
	_float4 Get_Direction() { return m_vDirection; }
	_bool Get_Sin() { return m_isSin; }

	void Set_Type(TYPE eInstanceType) { m_eInstanceType = eInstanceType; }
	void Set_Pivot_Look(PIVOT_LOOK ePivotLook) { m_ePivotLook = ePivotLook; }
	void Set_Movement(MOVEMENT eMovement) { m_eMovement = eMovement; }
	void Set_Direction(_float4 vDirection) { m_vDirection = vDirection; }
	void Set_Sin(_bool issin) { m_isSin = issin; }
	
protected:
	CVIBuffer_Instance::INSTANCE_DESC m_eInstance_Desc = {};
	CVIBuffer_Instance* m_pVIBufferCom = { nullptr };
	wstring m_strBufferTag;
	TYPE m_eInstanceType = { TYPE_END };
	PIVOT_LOOK m_ePivotLook = { PIVOT_LOOK_END };
	MOVEMENT m_eMovement = { DIRECTION };
	_float4 m_vDirection = { 0.f,0.f,0.f,0.f };
	_bool m_isSin = { false };					// 안씀 내일까지 sin 해보고 안되면 버림

public:
	virtual void Free();
};

END