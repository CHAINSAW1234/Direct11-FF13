#pragma once

#include "FSM.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"
#include "Collider.h"
#include "Collider_Bone.h"
#include "Transform.h"
#include "Navigation.h"
#include "VIBUffer_Line.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Instance_Rect.h"
#include "VIBuffer_Instance_Point.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_SPHERE.h"

#include "Component.h"


/* 1. 컴포넌트들의 원형을 레벨별로 보관한다. */
/* 2. 지정한 원형을 복제하여 사본객체를 생성하고 리턴한다 .*/

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	typedef map<const wstring, class CComponent*>	PROTOTYPES;
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	PROTOTYPES*			m_pPrototypes = { nullptr };
	_uint				m_iNumLevels = { 0 };

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END