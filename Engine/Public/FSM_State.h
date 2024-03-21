#pragma once
#include "Base.h"


BEGIN(Engine)

class ENGINE_DLL CFSM_State abstract : public CBase
{
protected:
	CFSM_State();
	virtual ~CFSM_State() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() = 0;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) = 0;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() = 0;
	// ���°� �������Ǵ� Ư�� ��쿡 ����ؼ� ���� ����
	virtual void Start();

protected:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END

