#include "LandObject.h"
#include "Transform.h"
#include "VIBuffer.h"

CLandObject::CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CLandObject::CLandObject(const CLandObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	LANDOBJECT_DESC* pDesc = static_cast<LANDOBJECT_DESC*>(pArg);

	m_pLandVIBuffer = pDesc->pLandVIBuffer;
	m_pLandTransform = pDesc->pLandTransform;

	Safe_AddRef(m_pLandVIBuffer);
	Safe_AddRef(m_pLandTransform);

	return S_OK;
}

void CLandObject::Priority_Update(_float fTimeDelta)
{
}

void CLandObject::Update(_float fTimeDelta)
{
}

void CLandObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

void CLandObject::Change_Land(LANDOBJECT_DESC* pLandDesc)
{
}

void CLandObject::SetUp_OnTerrain(CTransform* pTransform, _float fOffset)
{
/*
	_float3		vWorldPos = pTransform->Get_State(STATE::POSITION);

	_float3		vLocalPos = {};

	D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, m_pLandTransform->Get_WorldMatrixInvPtr());

	vLocalPos.y = m_pLandVIBuffer->Compute_Height(vLocalPos);

	D3DXVec3TransformCoord(&vWorldPos, &vLocalPos, m_pLandTransform->Get_WorldMatrixPtr());

	vWorldPos.y += fOffset;

	pTransform->Set_State(STATE::POSITION, vWorldPos);
*/

	_float3 vWorldPos = pTransform->Get_State(STATE::POSITION);  // 현재 월드 위치

	_float3 vLocalPos = {};
	D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, m_pLandTransform->Get_WorldMatrixInvPtr()); // 월드 → 로컬

	// 로컬 좌표계에서 높이 계산
	vLocalPos.y = m_pLandVIBuffer->Compute_Height(vLocalPos);
	
	// 다시 로컬 → 월드로 Y만 변환
	_float3 vWorldHeightPos = {};
	D3DXVec3TransformCoord(&vWorldHeightPos, &vLocalPos, m_pLandTransform->Get_WorldMatrixPtr());

	// 기존 X/Z 유지하고 Y만 갱신
	vWorldPos.y = vWorldHeightPos.y + fOffset;

	// 최종 위치 설정
	pTransform->Set_State(STATE::POSITION, vWorldPos);
}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pLandTransform);
	Safe_Release(m_pLandVIBuffer);
}
