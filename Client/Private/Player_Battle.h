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
	void Start();			// 몬스터 리스트 : troup로 따로 관리됨, 플레이어 리스트, 아이템 리스트 전부다 갖고있음
	void Tick(_float fTimeDelta);
	HRESULT Render();		// ImGui 달아서 사용할 용도
	
public:
	CChr_Battle* Get_Leader() { return m_pLeader; }
	vector<CChr_Battle*> Get_Members() { return m_Memebers; }

private:
	CChr_Battle* m_pLeader = { nullptr };			// Leader는 따로 갖고있자
	vector<CChr_Battle*> m_Memebers;				// Leader 이외의 AI들
	vector<CMonster*> m_Monsters;;
	//vector<CItem*> m_Items;
	// 아이템 리스트는 얘가 들고있음
	// 몬스터 리스트도 얘가 들고있음
	// 
public:
	static CPlayer_Battle* Create();
	virtual void Free() override;
};


END
