#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Client)

class CBoss final : public CMonster
{
private:
	CBoss(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBoss(const CBoss& Prototype);
	virtual ~CBoss() = default;

	const _wstring m_strFrameKeys[20] = {
		TEXT("Boss_Attack_Front"), TEXT("Boss_Attack_SE"),TEXT("Boss_Attack_SW"),
		TEXT("Boss_Die"), TEXT("Boss_Direction_NE"), TEXT("Boss_Direction_NW"), 
		TEXT("Boss_Direction_SE"), TEXT("Boss_Direction_SW"), TEXT("Boss_Front"),
		TEXT("Boss_Back"), TEXT("Boss_Left"), TEXT("Boss_Right"),
		TEXT("Boss_Leg_Direction_NE"), TEXT("Boss_Leg_Direction_NW"), TEXT("Boss_Leg_Direction_SE"),
		TEXT("Boss_Leg_Direction_SW"), TEXT("Boss_Front_Leg"), TEXT("Boss_Back_Leg"),
		TEXT("Boss_Left_Leg"), TEXT("Boss_Right_Leg")
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
	HRESULT End_RenderState() override;

	virtual void Attack() override;
	void Move(_float fTimeDelta);
	virtual void Move() override;

private:
	void RotateToPlayer(CTransform* pTranform);
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
	_bool m_bAnimationLock = false;

public:
	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END