#pragma once
#include "Effect_3D.h"
class CEffect_3D_Target_Object final : public CEffect_3D, public Target_GameObject
{
public:
	typedef struct Effect_3D_Target_Object_Desc : public CEffect::EFFECT_DESC, public Interface_3D::INTERFACE_3D_DESC, public Target_GameObject
	{} EFFECT_3D_TARGET_OBJECT_DESC;
};

