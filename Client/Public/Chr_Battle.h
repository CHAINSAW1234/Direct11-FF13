#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CFSM;
END

BEGIN(Client)
class CAbility;

class CChr_Battle abstract : public CGameObject
{
protected:
	CChr_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChr_Battle(const CChr_Battle& rhs);
	virtual ~CChr_Battle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/*============================GET/*============================*/
	_float Get_ATB() { return m_fATB; }
	_float Get_MaxATB()  { return m_fMaxATB; }
	CAbility* Get_Ability() { return m_pAbility; }


	/*============================SET/*============================*/
	void Set_UpdateATB(_bool isUpdateATB) {	m_isUpdateATB = isUpdateATB; }
	void Set_ATB_Full() { m_fATB = m_fMaxATB; }


protected:
	/*===========================생성 관련=====================*/
	virtual HRESULT Add_Ability();
private:
	void Update_ATB(_float fTimeDelta);



protected:
	_bool  m_isUpdateATB = { true };	// 이 값이 참인 경우에만 ATB를 증가시킴
	_float m_fATB = { 0.f };	// ATB 게이지
	_float m_fMaxATB = { 3.f };

	CAbility* m_pAbility = { nullptr };

public:
	virtual void Free() override;

};
END