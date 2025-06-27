#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBoxCollider final : public CComponent
{
public:
	typedef struct tagBoxColliderDesc {

		_float fScaleX;
		_float fScaleY;
		_float fScaleZ;

	} BOXCOLLIDER_DESC;

private:
	CBoxCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoxCollider(const CBoxCollider& rhs);
	virtual ~CBoxCollider() = default;

public:
	const _float3& Get_LocalPos(_uint iIdx) const
	{
		return m_vLocalPos[iIdx];
	}

	const _float Get_Scale(_uint iIdx) const {
		return *(_float *)&m_fScale[iIdx];
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	_float3 m_vLocalPos[8] = {};
	_float3 m_fScale = {};
	_float3 m_vAxis[3] = {};
public:
	static CBoxCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
