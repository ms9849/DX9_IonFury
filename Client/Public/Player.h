#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Player_Hand.h"

NS_BEGIN(Engine)
//class CTexture;
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
		_uint iHp{}, iBullets{};
	}PLAYER_INFO;

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
	CTransform*				m_pTransformCom = { nullptr };	
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CBoxCollider*			m_pBoxColliderCom = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Begin_RenderState();
	HRESULT End_RenderState();
	_float3 Calc_BulletDir(_float3* vOffset);

private:
	PLAYER_INFO		m_tInfo{};
	CPlayer_Hand*	m_pRightHand{ nullptr };
	CPlayer_Hand*	m_pLeftHand{ nullptr };
	_uint			m_iCurrentAnimation{};

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END