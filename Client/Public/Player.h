#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Player_Hand.h"

NS_BEGIN(Engine)
//class CTexture;
class CSphereCollider;
class CBoxCollider;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CLandObject
{
private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

private:
	typedef struct tagPlayerInfo
	{
		_uint iHp{}, iBullets{}, iArmor{};
		_wstring strWeapon{ TEXT("Pistol") };
		_wstring strAction{ TEXT("Idle") };
	}PLAYER_INFO;

	typedef struct tagWeapon
	{
		_uint iBulletsMax{}, iCanShootBullets{}, iCurrentBullets{}, iShootBullets{};
	}WEAPON_INFO;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	PLAYER_INFO Get_Player_Info();

private:
	//CTexture*				m_pTextureCom = { nullptr };
	CAnimation*				m_pRightHandAnimationCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };	
	CBoxCollider* m_pBoxColliderCom = { nullptr };
	CSphereCollider* m_pSphereColliderCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Weapons();
	HRESULT Begin_RenderState();
	HRESULT End_RenderState();
	_float3 Calc_BulletDir(_float3* vOffset);

private:
	PLAYER_INFO		m_tInfo{};
	CPlayer_Hand*	m_pRightHand{ nullptr };
	CPlayer_Hand*	m_pLeftHand{ nullptr };
	_bool			m_bWalk{ false };
	map<const _wstring, WEAPON_INFO> m_Weapons{};

public:
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType) override;
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END