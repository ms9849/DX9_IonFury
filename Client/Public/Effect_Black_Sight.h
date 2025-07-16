#pragma once
#include "Client_Defines.h"
#include "Effect.h"

NS_BEGIN(Client)

class CEffect_Black_Sight : public CEffect
{
private:
	CEffect_Black_Sight(LPDIRECT3DDEVICE9 pGraphicDev);
	CEffect_Black_Sight(const CEffect_Black_Sight& Prototype);
	virtual ~CEffect_Black_Sight() = default;

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

public:
	virtual void Set_Pos(const _float3& vPos) override;

private:
	_float4x4	m_OldViewMatrix{}, m_OldProjMatrix{};
	_float4x4	m_ViewMatrix, m_ProjMatrix = {};
	_float		m_fWinSizeY, m_fWinSizeX;
	_int		m_iAlpha = { 0 };

public:
	static CEffect_Black_Sight* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
