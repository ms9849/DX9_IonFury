#include "Effect_Boss_Die.h"

#include "GameInstance.h"

CEffect_Boss_Die::CEffect_Boss_Die(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CEffect { pGraphic_Device }
{
}

CEffect_Boss_Die::CEffect_Boss_Die(const CEffect_Boss_Die& Prototype) :
	CEffect{ Prototype }
{
}

HRESULT CEffect_Boss_Die::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Boss_Die::Initialize(void* pArg)
{
	return S_OK;
}

void CEffect_Boss_Die::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Boss_Die::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CEffect_Boss_Die::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CEffect_Boss_Die::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CEffect_Boss_Die::Ready_Components()
{
	return E_NOTIMPL;
}

HRESULT CEffect_Boss_Die::Begin_RenderState()
{
	return E_NOTIMPL;
}

HRESULT CEffect_Boss_Die::End_RenderState()
{
	return E_NOTIMPL;
}

CEffect_Boss_Die* CEffect_Boss_Die::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return nullptr;
}

CGameObject* CEffect_Boss_Die::Clone(void* pArg)
{
	return nullptr;
}

void CEffect_Boss_Die::Free()
{
}
