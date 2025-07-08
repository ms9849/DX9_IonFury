#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Client)

enum class BossAttackState {
	MASS,
	CHASE_MASS,
	BOOM,
	END
};

class CBoss final : public CMonster
{
private:
	CBoss(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss(const CBoss& Prototype);
	virtual ~CBoss() = default;

	const _wstring m_strFrameKeys[22] = {
		TEXT("Boss_Attack_Front"), TEXT("Boss_Attack_SE"),TEXT("Boss_Attack_SW"),
		TEXT("Boss_Die"), TEXT("Boss_Direction_NE"), TEXT("Boss_Direction_NW"), 
		TEXT("Boss_Direction_SE"), TEXT("Boss_Direction_SW"), TEXT("Boss_Front"),
		TEXT("Boss_Back"), TEXT("Boss_Left"), TEXT("Boss_Right"),
		TEXT("Boss_Leg_Direction_NE"), TEXT("Boss_Leg_Direction_NW"), TEXT("Boss_Leg_Direction_SE"),
		TEXT("Boss_Leg_Direction_SW"), TEXT("Boss_Front_Leg"), TEXT("Boss_Back_Leg"),
		TEXT("Boss_Left_Leg"), TEXT("Boss_Right_Leg"),
		TEXT("Boss_LeftLeg"), TEXT("Boss_RightLeg")
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
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

	HRESULT Ready_Components() override;
	HRESULT Begin_RenderState() override;
	HRESULT Begin_RenderTestState();
	HRESULT End_RenderState() override;
	HRESULT End_RenderTestState();

	void Attack(_float fTimeDelta, BossAttackState eState);

	virtual void Attack() override;
	void Move(_float fTimeDelta);
	virtual void Move() override;

private:
	//void RotateToPlayer(CTransform* pTranform);
	void RotateWithParentTransform();

private:
	CVIBuffer_Rect* m_pVIBufferCom_Up = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom_Down = { nullptr };
	CTransform* m_pTransformCom_Up = { nullptr };
	CTransform* m_pTransformCom_Down = { nullptr };
	CAnimation* m_pAnimationCom_Up = { nullptr };
	CAnimation* m_pAnimationCom_Down = { nullptr };
	_wstring m_strUpFrameKey = {};
	_wstring m_strDownFrameKey = {};
	_wstring m_strLegFrameKey = {};
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
	_float m_fSafeDistance = {};
	_float m_fStopMoveTime = {};
	_float m_fSumStopMoveTime = 0.f;
	_float3 m_vUpOffset = { 0.f, 4.2f, 0.f };
	_float3 m_vAttackPos = {};
	BossAttackState m_eState = BossAttackState::END;

public:
	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END