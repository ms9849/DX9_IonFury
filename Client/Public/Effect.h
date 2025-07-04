#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CEffect abstract : public CAlphaObject
{
public:
	typedef struct tagEffectDesc{
		//포지션 값을 어떻게 받아올 것인가? 는 충분히 고민해야하는 상황
		_float3 vPosition = {};

		//프레임은 객체 내부에서 따로따로 선언해줄지, 받아올지 생각 잘해봐야 한다.
		_float	fFrame = {};
		_bool	bIsOrtho = {};
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
	void Set_Desc(EFFECT_DESC Desc);

	_float Get_Frame() const {
		return m_fFrame;
	}

protected:
	//직교 투영으로 표현되는 이펙트인지
	_bool					m_IsOrtho = {};

	class CTexture*				m_pTextureCom = { nullptr };
	class CTransform*				m_pTransformCom = { nullptr };	
	class CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	
	_float					m_fFrame = {};
	_float					m_fNumFrame = {};

protected:
	virtual HRESULT Ready_Components();
	virtual HRESULT Begin_RenderState();
	virtual HRESULT End_RenderState();


public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

NS_END