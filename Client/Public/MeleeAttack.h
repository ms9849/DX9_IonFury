#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
/*
final은 아닐 수 있음
*/

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CSphereCollider;
NS_END

NS_BEGIN(Client)

class CMeleeAttack final : public CGameObject
{
public:
	typedef struct tagMeleeAttackDesc
	{
		_float3 vDir = {};
		_float3 vPos = {};
		_float fDamage = {};
		_float fDurationTime = {};
	} MELEEATTACK_DESC;

private:
	CMeleeAttack(LPDIRECT3DDEVICE9 pGraphicDev);
	CMeleeAttack(const CMeleeAttack& rhs);
	virtual ~CMeleeAttack() = default;

public:
	_float Get_Damage()
	{
		return m_fDamage;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);


private:
	_float3 m_vDir = {};
	_float m_fDamage = { 0.f };
	_float m_fDurationTime = { 0.f };
	_float m_fAccumulationTime = { 0.f };

	CTransform* m_pTransformCom = { nullptr };
	CSphereCollider* m_pSphereColliderCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CMeleeAttack* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
