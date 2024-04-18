#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

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
		CGameObject* pTargetObject = { nullptr };
	} TARGET_BONE_DESC;
protected:
	CBone* m_pSocket = { nullptr };

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
	enum EFFECT { DUST_COLOR, EFFECT_END };
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
	typedef struct Interface_Instance_Desc
	{
		wstring strBufferTag = {};
	} INTERFACE_INSTANCE_DESC;

protected:
	CVIBuffer_Instance_Rect* m_pVIBufferCom = { nullptr };
	wstring m_strBufferTag;

public:
	virtual void Free();
};

END