#pragma once

#include "Client_Defines.h"
#include "Effect.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

/*
직교 투영을 위한 특수한 이펙트들은 구현받아 따로 사용하도록 한다.
*/

class CEffect_Screen_Blur final : public CEffect
{
private:
	CEffect_Screen_Blur(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEffect_Screen_Blur(const CEffect_Screen_Blur& Prototype);
	virtual ~CEffect_Screen_Blur() = default;

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
	void Set_TextureNum(_int iTextureNum);
	_float Get_Age() { return m_fAge; }
	void   Set_Age(_float fAge) { m_fAge = fAge; }

private:
	_float4x4	m_OldViewMatrix{}, m_OldProjMatrix{};
	_float4x4	m_ViewMatrix, m_ProjMatrix = {};
	_float		m_fTimeAcc = { 0.f };
	_float		m_fWinSizeY, m_fWinSizeX;
	_int		m_iTextureNum = { 0 };
	_float		m_fAge = { 0.f };
	_float		m_fLifeTime = { 2.f };
	
public:
	static CEffect_Screen_Blur* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END