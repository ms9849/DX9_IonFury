#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Client)

class CSpawner;
class CTerrain_Manager;

enum class BossAttackState {
	MASS,
	CHASE_MASS,
	BOOM,
	SUMMON,
	END
};

class CBossUpperBody final : public CMonster
{
private:
	CBossUpperBody(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossUpperBody(const CBossUpperBody& Prototype);
	virtual ~CBossUpperBody() = default;

	const _wstring m_strFrameKeys[11] = {
		TEXT("Boss_Attack_Front"), TEXT("Boss_Attack_SE"),TEXT("Boss_Attack_SW"),
		TEXT("Boss_Direction_NE"), TEXT("Boss_Direction_NW"), TEXT("Boss_Direction_SE"),
		TEXT("Boss_Direction_SW"), TEXT("Boss_Front"), TEXT("Boss_Back"),
		TEXT("Boss_Left"), TEXT("Boss_Right"),
	};

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Ready_Animations() override;
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider);
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);
	virtual void Move() {};

	HRESULT Ready_Components() override;
	HRESULT Begin_RenderTestState();
	HRESULT End_RenderTestState();

	void Attack(_float fTimeDelta, BossAttackState eState);
	void SummonMonster();

private:
	//void RotateToPlayer(CTransform* pTranform);
	void RotateWithParentTransform();
	CTransform* m_pCoreTranform = { nullptr };

private:
	_bool m_bAnimationLock = false;
	_bool m_bAttacking = false;
	_uint m_uMaxBullets = {};
	_uint m_uMaxExplosionBullets = {};
	_uint m_uCurBullets = {};
	_uint m_uCurExplosionBullets = {};
	_uint m_uTempNum = 0;
	_float m_fAttackFailTime = 0.f;
	_float m_fLaunchCoolTime = 0.2f;
	_float m_fSumLaunchCoolTime = 0.f;
	_float3 m_vUpOffset = { 0.f, 4.2f, 0.f };
	_float3 m_vAttackPos = {};
	BossAttackState m_eState = BossAttackState::END;
	CTerrain_Manager* m_pTerrain_Manager = { nullptr };

public:
	static CBossUpperBody* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	const _wstring m_MonsterKeys[4] = {
		TEXT("Prototype_GameObject_Monster_Zombie"), TEXT("Prototype_GameObject_Monster_Spider"),
		TEXT("Prototype_GameObject_Monster_Soldier"), TEXT("Prototype_GameObject_Monster_EliteSoldier")
	};
};

NS_END