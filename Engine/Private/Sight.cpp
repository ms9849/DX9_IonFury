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
	m_pTransform = pDesc->pMonsterTransform;
	m_pPlayerTransform = pDesc->pPlayerTransform;

    return S_OK;
}

int CSight::Check_Sight(_float fTimeDelta)
{
	_float3 fDiff = m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransform->Get_State(STATE::POSITION);
	_float fDist = D3DXVec3Length(&fDiff);

	if (fDist > m_fRange)
		return 0;

	_float3 fPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
	_float3 fMonsterLook = m_pTransform->Get_State(STATE::LOOK);
	_float3 fMonsterUp = m_pTransform->Get_State(STATE::UP);

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
	D3DXVec3Cross(&fCross, &fMonsterLook, &fDiff);

	if (fSign < 0)
		fDegree *= -1;

	/*wchar_t szBuffer[128];
	wchar_t szBuffer_1[128];
	swprintf_s(szBuffer, 128, L"fSign: %.2f도 ", fSign);
	swprintf_s(szBuffer_1, 128, L"fFov: %.2f도\n", fFov);
	OutputDebugString(szBuffer);
	OutputDebugString(szBuffer_1);*/

	if (fSign >= fFov)			// 범위에 들어온 상태
	{
		int a = 10;
		
		m_pTransform->Chase(m_pPlayerTransform->Get_State(STATE::POSITION), fTimeDelta);
		m_pTransform->Turn(fCross, fTimeDelta);
		// 몬스터의 회전도 필요함
		// 추후 특정 state로 전환할수 있도록 할것
		return 1;
	}
	else if (fSign < 0)
	{
		// 뒷모습 출력
		return 2;
	}
	else
	{
		// 시야 밖인 상황 플레이어의 위치를 체크하여 맞는 방향의 모습을 보여줘야 함
		if (fCross.y > 0)		// 플레이어가 몬스터의 오른쪽
		{
			return 3;
		}
		else
		{
			return 4;
		}
	}

	/*wchar_t szBuffer[128];
	swprintf_s(szBuffer, 128, L"플레이어-몬스터 룩벡터 각도: %.2f도\n", fDegree);
	OutputDebugString(szBuffer);*/

	return 5;
}

//void CSight::Set_Transform()
//{
//	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_WorldMatrix);
//}

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
