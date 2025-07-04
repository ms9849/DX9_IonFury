#pragma once

#include "Client_Defines.h"
#include "Effect.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CEffect_Pistol_Fire final : public CEffect
{
public:

private:
	CEffect_Pistol_Fire(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Pistol_Fire(const CEffect_Pistol_Fire& Prototype);
	virtual ~CEffect_Pistol_Fire() = default;

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
	static CEffect_Pistol_Fire* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
