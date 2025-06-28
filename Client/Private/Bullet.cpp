#include "Bullet.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject{ pGraphicDev }
{
}

CBullet::CBullet(const CBullet& Prototype )
	: CGameObject{ Prototype }
{
}

HRESULT CBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
}

void CBullet::Update(_float fTimeDelta)
{
}

void CBullet::Late_Update(_float fTimeDelta)
{
}

HRESULT CBullet::Render()
{
	return S_OK;
}

HRESULT CBullet::Ready_Components()
{
	return S_OK;
}

HRESULT CBullet::Begin_RenderState()
{
	return S_OK;
}

HRESULT CBullet::End_RenderState()
{
	return S_OK;
}

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBullet* pInstance = new CBullet(pGraphicDev);

	if(FAILED(pInstance->Initialize()))
	return nullptr;
}

CBullet* CBullet::Clone(void* pArg)
{
	return nullptr;
}

void CBullet::Free()
{
}
