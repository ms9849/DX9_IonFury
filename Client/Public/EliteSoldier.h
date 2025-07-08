#pragma once

#include "Client_Defines.h"
#include "Monster.h"

NS_BEGIN(Client)

class CEliteSoldier final : public CMonster
{
private:
	enum class EliteSoldierState
	{
		ONESHOT,
		MASS,
		END
	};

private:
	CEliteSoldier(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEliteSoldier(const CEliteSoldier& Prototype);
	virtual ~CEliteSoldier() = default;

	const _wstring m_strFrameKeys[14] = {
		TEXT("EliteSoldier_Attack_Front"), TEXT("EliteSoldier_Attack_Seat"), TEXT("EliteSoldier_Die_Default"),
		TEXT("EliteSoldier_Direction_NE"), TEXT("EliteSoldier_Direction_NW"), TEXT("EliteSoldier_Direction_SE"),
		TEXT("EliteSoldier_Direction_SW"), TEXT("EliteSoldier_Front"), TEXT("EliteSoldier_Back"),
		TEXT("EliteSoldier_Left"), TEXT("EliteSoldier_Right"), TEXT("EliteSoldier_Die_Idle"),
		TEXT("EliteSoldier_Die_HeadShot"), TEXT("EliteSoldier_Die_HeadShot_Idle"),
	};

	const _float3 m_vShootPosOffset[5] = {
		{0.f, 0.f, 0.f},
		{0.1f,0.05f, 0.25f},
		{-0.1f,-0.05f,-0.25f },
		{0.05f, -0.1f, 0.25f},
		{-0.05f,-0.1f, 0.25f}
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

	void Attack(EliteSoldierState eState);
	void Move(_float fTimeDelta);
	virtual void Move() override;

private:
	_bool m_bAnimationLock = false;
	_uint m_uTempNum = 0;
	EliteSoldierState m_eState = EliteSoldierState::END;

public:
	static CEliteSoldier* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END