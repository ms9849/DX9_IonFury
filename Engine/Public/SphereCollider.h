#pragma once

#include "Component.h"

NS_BEGIN(Engine)

/*
LocalPos는 기본적으로 0,0,0 세팅
fRadius는 클론해올때 넣어줘야 한다.
*/
class ENGINE_DLL CSphereCollider final : public CComponent
{
public:
	typedef struct tagSphereColliderDesc {
		_float fRadius = {};
		_float3 vLocalPos = {};
	} SPHERECOLLIDER_DESC;

private:
	CSphereCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider() = default;

public:
	const _float& Get_Radius() const {
		return m_fRadius;
	}

	const _float3& Get_LocalPos() const {
		return m_vLocalPos;
	}
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	_float m_fRadius = {};
	_float3 m_vLocalPos = {};

public:
	static CSphereCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END 
