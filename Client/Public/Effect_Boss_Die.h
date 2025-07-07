#pragma once

#include "Client_Defines.h"
#include "Effect.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CEffect_Boss_Die : public CEffect
{
private:
	CEffect_Boss_Die(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Boss_Die(const CEffect_Boss_Die& Prototype);
	virtual ~CEffect_Boss_Die() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Begin_RenderState() override;
	virtual HRESULT End_RenderState() override;

private:
	_float4x4	m_OldViewMatrix{}, m_OldProjMatrix{};
	_float4x4	m_ViewMatrix, m_ProjMatrix = {};
	_float		m_fWinSizeY, m_fWinSizeX;

public:
	static CEffect_Boss_Die* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
