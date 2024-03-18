#pragma once

#include "Client_Defines.h"
#include "Observer_Handler.h"

BEGIN(Client)
class CChr_Battle_Light;
class CChr_Battle;
class CMonster;
//class CItem;
class CPlayer_Battle final : public CBase, public CObserver_Handler
{
private:
	CPlayer_Battle();
	~CPlayer_Battle() = default;

public:
	HRESULT Initialize();
	void Start();			// ���� ����Ʈ : troup�� ���� ������, �÷��̾� ����Ʈ, ������ ����Ʈ ���δ� ��������
	void Tick(_float fTimeDelta);
	HRESULT Render();		// ImGui �޾Ƽ� ����� �뵵
	
public:
	CChr_Battle* Get_Leader() { return m_pLeader; }
	vector<CChr_Battle*> Get_Members() { return m_Memebers; }

private:
	CChr_Battle* m_pLeader = { nullptr };			// Leader�� ���� ��������
	vector<CChr_Battle*> m_Memebers;				// Leader �̿��� AI��
	vector<CMonster*> m_Monsters;;
	//vector<CItem*> m_Items;
	// ������ ����Ʈ�� �갡 �������
	// ���� ����Ʈ�� �갡 �������
	// 
public:
	static CPlayer_Battle* Create();
	virtual void Free() override;
};


END
