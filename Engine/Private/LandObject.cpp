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
	if (m_pLandVIBuffer == pLandDesc->pLandVIBuffer)
		return;

	CVIBuffer* pBufferLast = m_pLandVIBuffer;
	CTransform* pTransformLast = m_pLandTransform;

	m_pLandVIBuffer = pLandDesc->pLandVIBuffer;
	m_pLandTransform = pLandDesc->pLandTransform;

	Safe_AddRef(m_pLandVIBuffer);
	Safe_AddRef(m_pLandTransform);

	Safe_Release(pBufferLast);
	Safe_Release(pTransformLast);
}

void CLandObject::SetUp_OnTerrain(CTransform* pTransform, _float fOffset, _bool* bJump)
{
    _float3 vWorldPos = pTransform->Get_State(STATE::POSITION);

    _float3 vLocalPos = {};
    D3DXVec3TransformCoord(&vLocalPos, &vWorldPos, m_pLandTransform->Get_WorldMatrixInvPtr());

    vLocalPos.y = m_pLandVIBuffer->Compute_Height(vLocalPos);
    if (vLocalPos.y == FLT_MIN)
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

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pLandTransform);
	Safe_Release(m_pLandVIBuffer);
}
