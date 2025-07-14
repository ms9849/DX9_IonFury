#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

NS_BEGIN(Engine)
class CSphereCollider;
class CBoxCollider;
class CTransform;
class CAnimation;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CLandObject
{
private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	typedef struct tagWeapon
	{
		_uint iBulletsMax{}, iCanShootBullets{}, iCurrentBullets{}, iShootBullets{};
		_bool bUseable{ true };
	}WEAPON_INFO;

	typedef struct tagPlayerInfo
	{
		_uint iHp{}, iBullets{}, iShootBullets{}, iArmor{};
		_wstring strWeapon{ TEXT("Pistol") };
		_wstring strAction{ TEXT("Idle") };
		_wstring strItem{ TEXT("CardKey") };
		_wstring strItemAction{ TEXT("Idle") };
		map<const _wstring, WEAPON_INFO> Weapons{};
	}PLAYER_INFO;

private:
	typedef struct tagItem
	{
		_float fCreateTime{};
		_wstring strItemText{};
	}ITEM_DESC;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	PLAYER_INFO Get_Player_Info();
	void Set_Player_Info(PLAYER_INFO tPlayerInfo);

	_bool Get_CanUse_CardKey();
	_bool Get_Can_Open_Door();
	_bool Get_Use_CardKey();

	_bool Get_Active_Elevator();
	void Set_Active_Elevator(_bool bActive);

private:
	CAnimation*				m_pRightHandAnimationCom = { nullptr };
	CAnimation*				m_pLeftHandAnimationCom = { nullptr };	
	CBoxCollider*			m_pBoxColliderCom = { nullptr };
	CSphereCollider*		m_pSphereColliderCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Weapons();
	HRESULT Begin_RenderState();
	HRESULT End_RenderState();
	_float3 Calc_BulletDir(_float3* vOffset);
	void Insert_ItemDesc(const _wstring strItemText);
	void Pop_ItemDesc(_float fTimeDelta);
	_wstring Set_FrameKey(_wstring strDst, _wstring strSrc);


private:
	PLAYER_INFO		m_tInfo{};
	class CPlayer_RightHand*	m_pRightHand{ nullptr };
	class CPlayer_LeftHand*		m_pLeftHand{ nullptr };
	class CDoorLock* m_pDoorLock{nullptr};
	class CLever* m_pLever{nullptr};
	class CButton* m_pButton{nullptr};

	_float			m_fSpeed{ 1.f };
	_bool			m_bSpeedUp{ false };

	_wstring		m_strNextWeapon{};
	deque<ITEM_DESC> m_ItemQueues{};
	_float			m_fTimeStack{0.f};
	_float			m_fColTimeStack{ 0.f };
	_bool			m_bWeaponChange{ false };

	_bool			m_bUseCardKey{ false };
	_bool			m_bCanOpenDoor{ false };
	_bool			m_bCanUseCardKey{ false };

	_bool			m_bActiveElevator{ false };
	_bool			m_bCanActiveElevator{ false };
	_bool			m_bCanActiveDoor{ false };

	_bool			m_bMachinGunBulletCharge{ false };
	_bool			m_bMachinGunShootEnd{ false };
	_float			m_fMachinGunSoundCoolDown = { 0.f };

public:
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
	virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos) override;
	virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);
	_wstring Get_ItemText(size_t iIndex);
	size_t Get_ItemQueue_Length();

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END