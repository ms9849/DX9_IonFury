#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CEffect : public CGameObject
{
public:
	enum class EFFECT_TYPE {
		BOSS_DIE,
		GRENADE_EXPLOSION
	};
public:
	typedef struct tagEffectDesc{
		//포지션 값을 어떻게 받아올 것인가? 는 충분히 고민해야하는 상황
		_float3 vPosition = {};

		//프레임은 객체 내부에서 따로따로 선언해줄지, 받아올지 생각 잘해봐야 한다.
		_float	fFrame = {};
		EFFECT_TYPE eType = {};
		_float  fNumFrame = {};

	} EFFECT_DESC;

protected:
	CEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Pos(const _float3& vPos);

	_float Get_Frame() const {
		return m_fFrame;
	}

protected:
	//직교 투영으로 표현되는 이펙트인지
	class CTexture*				m_pTextureCom = { nullptr };
	class CTransform*				m_pTransformCom = { nullptr };	
	class CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	
	_float					m_fFrame = {};
	_float					m_fNumFrame = {};
	EFFECT_TYPE				m_eType = {};

protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Begin_RenderState();
	virtual HRESULT End_RenderState();


public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END