#include "Sight.h"
#include "Transform.h"

CSight::CSight(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CComponent { pGraphic_Device }
{
}

CSight::CSight(const CSight& Prototype)
    : CComponent{ Prototype }
	//, m_WorldMatrix { Prototype.m_WorldMatrix }
{
}

HRESULT CSight::Initialize_Prototype()
{
	//D3DXMatrixIdentity(&m_WorldMatrix);
    return S_OK;
}

HRESULT CSight::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	//TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);
	SIGHT_DESC* pDesc = static_cast<SIGHT_DESC*>(pArg);
	m_fRange = pDesc->fRange;
	m_fRotation = pDesc->fRotation;
	m_bTrace = pDesc->bTrace;
	m_pTransform = pDesc->pMonsterTransform;
	m_pPlayerTransform = pDesc->pPlayerTransform;

    return S_OK;
}

_bool CSight::Check_Sight(_float fTimeDelta)
{
	_float3 fDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransform->Get_State(STATE::POSITION);
	_float fDist = D3DXVec3Length(&fDiff);

	if (fDist > m_fRange)
		return false;

	_float3 fPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
	_float3 fMonsterLook = m_pTransform->Get_State(STATE::LOOK);
	_float3 fMonsterUp = _float3{ 0.f, 1.f, 0.f };

	float fFov = cosf(m_fRotation * 0.5f);

	D3DXVec3Normalize(&fPlayerLook, &fPlayerLook);
	D3DXVec3Normalize(&fMonsterLook, &fMonsterLook);
	D3DXVec3Normalize(&fMonsterUp, &fMonsterUp);
	D3DXVec3Normalize(&fDiff, &fDiff);

	float fSign = D3DXVec3Dot(&fMonsterLook, &fDiff);
	_float dot = D3DXVec3Dot(&fPlayerLook, &fMonsterLook);
	float fRadian = acosf(dot);									// 라디안값을 구했으니 이 각도 만큼 회전을 시키자
	float fDegree = D3DXToDegree(fRadian);

	_float3 fCross;
	D3DXVec3Cross(&fCross, &fMonsterLook, &fDiff);				// 해당 값에서 y축 회전만 할수있도록 수정
	if (fSign < 0)
		fDegree *= -1;

	_float3 yAxis = { 0.f, 1.f, 0.f };

	_float3 vDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransform->Get_State(STATE::POSITION);

	if (fSign >= fFov)			// 범위에 들어온 상태
	{
		//m_pTransform->Rotation(yAxis, fRadian);
		//m_pTransform->Turn(yAxis, fTimeDelta);
		//m_pTransform->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fTimeDelta);
		//m_pTransform->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

		return true;
	}
	else if (m_fRange >= D3DXVec3Length(&vDiff) && m_bTrace)
	{
		m_pTransform->Turn(yAxis, fTimeDelta);
		//m_pTransform->Rotation(yAxis, fRadian);
		//m_pTransform->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fTimeDelta);
		m_pTransform->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));
		return true;
	}

	return false;
}

CSight* CSight::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSight* pInstance = new CSight(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CSight::Clone(void* pArg)
{
	CSight* pInstance = new CSight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSight::Free()
{
    __super::Free();

}
