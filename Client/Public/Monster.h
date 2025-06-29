#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Animation.h"
#include "BehaviorNode.h"

NS_BEGIN(Engine)
class CTexture;
class CAnimation;
class CTransform;
class CVIBuffer_Rect;
class CSight;
NS_END

NS_BEGIN(Client)

enum class MonsterState {
	FRONT,
	BACK,
	ROTATE,
	MOVE,
};

class CMonster final : public CLandObject
{
private:
	CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CAnimation* m_pAnimationCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };	
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CSight* m_pSightCom = { nullptr };
	CTransform* m_pPlayerTransform = { nullptr };
	
	int						m_iNum = 0;
	_float m_fCoolTime = 3.f;
	_float m_fAccumulation = 0.f;
	BehaviorNode* m_pRoot = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animations();
	HRESULT Begin_RenderState();
	HRESULT End_RenderState();
	void Attack();

	map<const _wstring, CTexture*> m_pTextureComs;

	_wstring m_strFrameKey{TEXT("Soldier_Front")};
	map<const _wstring, CAnimation::FRAME_DESC> m_Frames;

	const _wstring m_strFrameKeys[14] = {
		TEXT("Soldier_Attack_Front"), TEXT("Soldier_Attack_SE"), TEXT("Soldier_Attack_SW"),
		TEXT("Soldier_Die_Default"), TEXT("Soldier_Die_Explosion"), TEXT("Soldier_Die_HeadShot"),
		TEXT("Soldier_Direction_NE"), TEXT("Soldier_Direction_NW"), TEXT("Soldier_Direction_SE"),
		TEXT("Soldier_Direction_SW"), TEXT("Soldier_Front"), TEXT("Soldier_Back"),
		TEXT("Soldier_Left"), TEXT("Soldier_Right")
	};

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END