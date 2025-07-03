#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "GameInstance.h"

NS_BEGIN(Engine)
class CTexture;
class CAnimation;
class CVIBuffer_Rect;
class CSight;
class CBoxCollider;
class CSphereCollider;
NS_END

NS_BEGIN(Client)

class CMonster abstract : public CLandObject
{
protected:
	CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize();
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CAnimation* m_pAnimationCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CSight* m_pSightCom = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	CBoxCollider* m_pBoxColliderCom = { nullptr };
	CSphereCollider* m_pSphereColliderCom = { nullptr };

	_float m_fAttackfCoolTime = 3.f;
	_float m_fSumAttackCoolTime = 3.f;
	_float m_fMoveCoolTime = 0.05f;
	_float m_fSumMoveCoolTime = {};
	_float m_fChaseRange = 8.f;
	_float m_fMaxRange = 10.f;
	_float m_fAttackRange = {};
	_float m_fHp = 100.f;
	_float m_fDamage = {};
	_bool m_bDying = false;

	class CBehaviorNode* m_pRoot = { nullptr };

	_wstring m_strFrameKey;
	map<const _wstring, CTexture*> m_pTextureComs;

protected:
	virtual HRESULT Ready_Animations() = 0;
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT Begin_RenderState();
	virtual HRESULT End_RenderState();
	virtual void Attack() = 0;
	virtual void Move() = 0;

	virtual void Free();

private:
	//_wstring m_strFrameKey{TEXT("Soldier_Front")};
	//m_strFrameKey = TEXT("Soldier_Front");
	// 이니셜라이즈에서 초기화해주는게 좋을듯 최초 값

	/*const _wstring m_strFrameKeys[14] = {
		TEXT("Soldier_Attack_Front"), TEXT("Soldier_Attack_SE"), TEXT("Soldier_Attack_SW"),
		TEXT("Soldier_Die_Default"), TEXT("Soldier_Die_Explosion"), TEXT("Soldier_Die_HeadShot"),
		TEXT("Soldier_Direction_NE"), TEXT("Soldier_Direction_NW"), TEXT("Soldier_Direction_SE"),
		TEXT("Soldier_Direction_SW"), TEXT("Soldier_Front"), TEXT("Soldier_Back"),
		TEXT("Soldier_Left"), TEXT("Soldier_Right")
	};*/
};

NS_END