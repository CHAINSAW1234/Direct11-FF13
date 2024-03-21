#pragma once

#include "Base.h"

/* ������Ʈ �Ŵ����� �������� �ǵ��� ���� ��ü���� ��� �����Ѵ�. */
/* �� ����. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CGameObject*		Get_GameObject(_uint iIndex);
	const class CComponent* Get_Component(const wstring& strComTag, _uint iIndex);
	size_t					Get_IndexCnt() { return m_GameObjects.size(); }

public:
	HRESULT Initialize();
	HRESULT	Add_GameObject(class CGameObject* pGameObject);
	void Tick(_float fTimeDelta);
	void Start();
	void Late_Tick(_float fTimeDelta);

private:
	list<class CGameObject*>		m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END