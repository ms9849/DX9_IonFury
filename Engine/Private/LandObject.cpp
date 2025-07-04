#include "LandObject.h"

#include "GameInstance.h"

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

void CLandObject::Jump(_float fTimeDelta)
{
	if (m_bJump)
	{
		//4.f는 점프 스피드
		m_fFallSpeed = (4.f * m_fTime - 30.f * m_fTime * m_fTime);

		if (m_fFallSpeed <= -0.2f)
			m_fFallSpeed = -0.2f;

		m_fTime += 0.3f * fTimeDelta;
		_float3 vPosition = m_pTransformCom->Get_State(STATE::POSITION);
		vPosition.y = vPosition.y + m_fFallSpeed;
		m_pTransformCom->Set_State(STATE::POSITION, vPosition);
	}
}

void CLandObject::Change_Land(LANDOBJECT_DESC* pLandDesc)
{
	m_pLandBufferLast = m_pLandVIBuffer;
	m_pLandTransformLast = m_pLandTransform;

	m_pLandVIBuffer = pLandDesc->pLandVIBuffer;
	m_pLandTransform = pLandDesc->pLandTransform;

	Safe_AddRef(m_pLandVIBuffer);
	Safe_AddRef(m_pLandTransform);

	Safe_Release(m_pLandBufferLast);
	Safe_Release(m_pLandTransformLast);
}

/*

*/
void CLandObject::SetUp_OnTerrain(CTransform* pTransform, _float fOffset, _bool* bJump)
{
	if (dynamic_cast<CVIBuffer_Terrain*>(m_pLandVIBuffer) != nullptr)
	{
		_float3 vLocalPos{}, vWorldPos = pTransform->Get_State(STATE::POSITION);

		const _float4x4* matLandWorldInv = m_pLandTransform->Get_WorldMatrixInvPtr();

		D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, matLandWorldInv);

		vLocalPos.y = m_pLandVIBuffer->Compute_Height(vLocalPos, m_pLandTransform);
		if (vLocalPos.y == FLT_MAX)
			return;

		_float3 vWorldHeightPos = {};
		D3DXVec3TransformCoord(&vWorldHeightPos, &vLocalPos, m_pLandTransform->Get_WorldMatrixPtr());

		vWorldHeightPos.y += fOffset;

		if (bJump == nullptr || *bJump == false)
		{
			vWorldPos.y = vWorldHeightPos.y;
			pTransform->Set_State(STATE::POSITION, vWorldPos);
		}
		else if (*bJump == true)
		{
			if (vWorldPos.y < vWorldHeightPos.y)
			{
				vWorldPos.y = vWorldHeightPos.y;
				*bJump = false;
				pTransform->Set_State(STATE::POSITION, vWorldPos);
			}
		}
	}
	else
	{
		_float3 vResultPos{}, vWorldPos = pTransform->Get_State(STATE::POSITION);
		_float3 vWorldHeightPos = vWorldPos;

		const _float4x4* matLandWorldInv = m_pLandTransform->Get_WorldMatrixInvPtr();

		D3DXVec3TransformCoord(&vResultPos, &vWorldPos, matLandWorldInv);

		vWorldHeightPos.y = m_pLandVIBuffer->Compute_Height(vResultPos, m_pLandTransform);
		vWorldHeightPos.y += fOffset;

		if (vWorldHeightPos.y == FLT_MIN)
			return;

		if (bJump == nullptr || *bJump == false)
		{
			vWorldPos.y = vWorldHeightPos.y;
			pTransform->Set_State(STATE::POSITION, vWorldPos);
		}
		else if (*bJump == true)
		{
			if (vWorldPos.y < vWorldHeightPos.y)
			{
				vWorldPos.y = vWorldHeightPos.y;
				*bJump = false;
				pTransform->Set_State(STATE::POSITION, vWorldPos);
			}
		}
	}
}

//void CLandObject::SetUp_OnTerrain(CTransform* pTransform, _float fOffset, _bool* bJump)
//{
//	if (dynamic_cast<CVIBuffer_Terrain*>(m_pLandVIBuffer) != nullptr)
//	{
//		_float3 vLocalPos{}, vWorldPos = pTransform->Get_State(STATE::POSITION);
//
//		const _float4x4* matLandWorldInv = m_pLandTransform->Get_WorldMatrixInvPtr();
//
//		D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, matLandWorldInv);
//
//		vLocalPos.y = m_pLandVIBuffer->Compute_Height(vLocalPos, m_pLandTransform);
//		if (vLocalPos.y == FLT_MAX)
//			return;
//
//		_float3 vWorldHeightPos = {};
//		D3DXVec3TransformCoord(&vWorldHeightPos, &vLocalPos, m_pLandTransform->Get_WorldMatrixPtr());
//
//		vWorldHeightPos.y += fOffset;
//
//		if (bJump == nullptr || *bJump == false)
//		{
//			vWorldPos.y = vWorldHeightPos.y;
//			pTransform->Set_State(STATE::POSITION, vWorldPos);
//		}
//		else if (*bJump == true)
//		{
//			if (vWorldPos.y < vWorldHeightPos.y)
//			{
//				vWorldPos.y = vWorldHeightPos.y;
//				*bJump = false;
//				pTransform->Set_State(STATE::POSITION, vWorldPos);
//			}
//		}
//	}
//	else
//	{
//		_float3 vResultPos{}, vWorldPos = pTransform->Get_State(STATE::POSITION);
//		_float3 vWorldHeightPos = vWorldPos;
//
//		const _float4x4* matLandWorldInv = m_pLandTransform->Get_WorldMatrixInvPtr();
//
//		D3DXVec3TransformCoord(&vResultPos, &vWorldPos, matLandWorldInv);
//
//		vWorldHeightPos.y = m_pLandVIBuffer->Compute_Height(vResultPos, m_pLandTransform);
//		vWorldHeightPos.y += fOffset;
//
//		if (vWorldHeightPos.y == FLT_MIN)
//			return;
//
//		if (bJump == nullptr || *bJump == false)
//		{
//			vWorldPos.y = vWorldHeightPos.y;
//			pTransform->Set_State(STATE::POSITION, vWorldPos);
//		}
//		else if (*bJump == true)
//		{
//			if (vWorldPos.y < vWorldHeightPos.y)
//			{
//				vWorldPos.y = vWorldHeightPos.y;
//				*bJump = false;
//				pTransform->Set_State(STATE::POSITION, vWorldPos);
//			}
//		}
//	}
//}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pLandTransform);
	Safe_Release(m_pLandVIBuffer);
}
