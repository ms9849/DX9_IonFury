#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Client)

class CBossLowerBody final : public CMonster
{
private:
	CBossLowerBody(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBossLowerBody(const CBossLowerBody& Prototype);
	virtual ~CBossLowerBody() = default;

	const _wstring m_strFrameKeys[10] = {
		TEXT("Boss_Leg_Direction_NE"), TEXT("Boss_Leg_Direction_NW"), TEXT("Boss_Leg_Direction_SE"),
		TEXT("Boss_Leg_Direction_SW"), TEXT("Boss_Front_Leg"), TEXT("Boss_Back_Leg"),
		TEXT("Boss_Left_Leg"), TEXT("Boss_Right_Leg"), TEXT("Boss_LeftLeg"), 
		TEXT("Boss_RightLeg")
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

	HRESULT Ready_Components() override;
	HRESULT Begin_RenderState() override;
	HRESULT End_RenderState() override;

	virtual void Attack() override;
	void Move(_float fTimeDelta);
	virtual void Move() override;

private:
	//void RotateToPlayer(CTransform* pTranform);
	void RotateWithParentTransform();

private:
	_wstring m_strLegFrameKey = {};
	_bool m_bAnimationLock = false;
	_uint m_uTempNum = 0;
	_float m_fSafeDistance = {};
	_float m_fStopMoveTime = {};
	_float m_fSumStopMoveTime = 0.f;
	CTransform* m_pCoreTranform = { nullptr };

public:
	static CBossLowerBody* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END