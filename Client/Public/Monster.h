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
	CBoxCollider* m_pBoxColliderHead = { nullptr };
	CSphereCollider* m_pSphereColliderCom = { nullptr };

	_float m_fAttackCoolTime = 3.f;
	_float m_fSumAttackCoolTime = 3.f;
	_float m_fMoveCoolTime = 0.05f;
	_float m_fSumMoveCoolTime = {};
	_float m_fChaseRange = 8.f;
	_float m_fMaxRange = 10.f;
	_float m_fAttackRange = {};
	_float m_fHp = 100.f;
	_float m_fDamage = {};
	_bool m_bDying = false;
	_bool m_isMove = false;
	_bool m_bFirstEncounter = false;

	_wstring m_strFrameKey;
	map<const _wstring, CTexture*> m_pTextureComs;

protected:
	virtual HRESULT Ready_Animations() = 0;
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT Begin_RenderState();
	virtual HRESULT End_RenderState();
	virtual void Attack();
	virtual void Move() = 0;
	virtual void RotateToPlayer(CTransform* pTranform);

	virtual void Free();
};

NS_END