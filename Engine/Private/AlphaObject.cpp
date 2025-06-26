#include "AlphaObject.h"

CAlphaObject::CAlphaObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CAlphaObject::CAlphaObject(const CAlphaObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CAlphaObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAlphaObject::Initialize(void* pArg)
{
	return S_OK;
}

void CAlphaObject::Priority_Update(_float fTimeDelta)
{
}

void CAlphaObject::Update(_float fTimeDelta)
{
}

void CAlphaObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CAlphaObject::Render()
{
	return S_OK;
}

void CAlphaObject::Compute_CamDistance(const _float3& vWorldPos)
{
	_float4x4		ViewMatrix = {};
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3			vCamPos = {};
	memcpy(&vCamPos, &ViewMatrix.m[3], sizeof(_float3));

	_float3			vDir = vCamPos - vWorldPos;
	m_fCamDistance = D3DXVec3Length(&vDir);
}

void CAlphaObject::Free()
{
	__super::Free();
}
