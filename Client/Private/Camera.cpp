#include "Camera.h"

#include "GameInstance.h"
#include "Player.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
{
}

void CCamera::Camera_Configure(const CAMERA_CONFIG& settings)
{
	m_pCameraConfig = settings;
	if (m_pCameraConfig.pTarget)
	{
		m_pTarget = m_pCameraConfig.pTarget;
		if (m_pCameraConfig.isChaseTarget)
		{
			m_pTargetTransformCom = dynamic_cast<CTransform*>(m_pTarget->Find_Component(TEXT("Com_Transform")));
		}
	}
}

void CCamera::Camera_Configure_Clear(CAMERA_CONFIG& settings)
{
	settings.pTarget = nullptr;
	settings.isMouseFixCenter = false;
	settings.isSyncLook = false;
	settings.vLimitDistance = _float3{ 0.f, 0.f, 0.f };
	settings.isChaseTarget = false;
	settings.isCanTurn = false;
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
	m_fFov = pDesc->fFov;
	m_fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	/* 카메라의 월드 상태를 트랜스폼 ㅋㅓㅁ포넌트에게 동기화한다.  */
	/* 뷰스페이스 변환행렬을 구한다. */
	/*D3DXMatrixLookAtLH();*/
	m_pTransformCom->Set_State(STATE::POSITION, pDesc->vEye);
	m_pTransformCom->LookAt(pDesc->vAt);

	/* 투영스페이스 변환행렬 */
	D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, m_fFov, m_fAspect, m_fNear, m_fFar);

	/*POINT		ptMouse{};

	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	m_vOldMouse = _float2(ptMouse.x, ptMouse.y);*/

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInvPtr());
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, m_fFov, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Update(_float fTimeDelta)
{
	if (m_pCameraConfig.pTarget)
	{
		if (m_pCameraConfig.isChaseTarget)
		{
			Chase_Target(m_pCameraConfig.vLimitDistance, m_pCameraConfig.isSyncLook, m_pCameraConfig.isCanTurn, m_pCameraConfig.isMouseFixCenter, fTimeDelta);
		}
	}
	else
	{
		if (m_pCameraConfig.isCanTurn)
		{
			Camera_Turn(m_pCameraConfig.isMouseFixCenter, fTimeDelta);
		}
	}

	//if (m_update) m_update();
	/* 카메라의 움직임에 대한 처리를 모두 수행한다. */

	//1000 1011 1101 1111 
	//1000 0000 0000 0000

	//1000 0000 0000 0000
	//0000 0000 0000 0000

	//Camera_Turn(m_pCameraConfig.isMouseFixCenter);
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Camera_Turn(bool isMouseFixCenter, _float fTimeDelta)
{
	POINT		ptMouse{};

	if (isMouseFixCenter)
	{
		ShowCursor(FALSE);
		GetCursorPos(&ptMouse);
		SetCursorPos(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

		m_fMove = { (_float)ptMouse.x - g_iWinSizeX * 0.5f, (_float)ptMouse.y - g_iWinSizeY * 0.5f };
	}
	else
	{
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		m_fMove = { (_float)ptMouse.x - m_vOldMouse.x, (_float)ptMouse.y - m_vOldMouse.y};
	}

	m_pTransformCom->Turn(_float3{0.f, 1.f, 0.f}, fTimeDelta * m_fMove.x * m_fSensor);

	// 카메라 회전 제한
	_float3 vStd{ 0.f, 1.f, 0.f };
	_float3 vCameraLook = m_pTransformCom->Get_State(STATE::LOOK);

	D3DXVec3Normalize(&vCameraLook, &vCameraLook);
	_float fDot = D3DXVec3Dot(&vStd, &vCameraLook);

	_float3 vCross{};
	D3DXVec3Cross(&vCross, &vStd, &vCameraLook);
	D3DXVec3Normalize(&vCross, &vCross);

	if (fDot < 0.8f && fDot > -0.8f)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * m_fMove.y * m_fSensor);
		m_vOldCameraRight = vCross;
		m_vOldCameraLook = vCameraLook;
		D3DXVec3Cross(&m_vOldCameraUp, &vCameraLook, &vCross);
		D3DXVec3Normalize(&m_vOldCameraUp, &m_vOldCameraUp);
	}
	else
	{
		if (fDot > 0.8f && m_fMove.y > 0)
		{
			m_pTransformCom->Set_State(STATE::RIGHT, m_vOldCameraRight);
			m_pTransformCom->Set_State(STATE::UP, m_vOldCameraUp);
			m_pTransformCom->Set_State(STATE::LOOK, m_vOldCameraLook);
		}
		else if (fDot < -0.8f && m_fMove.y < 0)
		{
			m_pTransformCom->Set_State(STATE::RIGHT, m_vOldCameraRight);
			m_pTransformCom->Set_State(STATE::UP, m_vOldCameraUp);
			m_pTransformCom->Set_State(STATE::LOOK, m_vOldCameraLook);
		}
	}

	//// 차원이동
	//if (vCross.y > 0)
	//{
	//	if (vCameraLook.y > 0.9f)
	//	{
	//		vCameraLook.y = 0.8f;
	//		vCameraLook *= vCameraLookLength;
	//		m_pTransformCom->Set_State(STATE::LOOK, vCameraLook);
	//	}
	//	else if (vCameraLook.y < -0.9f)
	//	{
	//		vCameraLook.y = -0.8f;
	//		vCameraLook *= vCameraLookLength;
	//		m_pTransformCom->Set_State(STATE::LOOK, vCameraLook);
	//	}
	//}
		
	
	m_vOldMouse = _float2(ptMouse.x, ptMouse.y);
}

void CCamera::Chase_Target(_float3 vLimitDistance, bool isSyncLook, bool isCanTurn, bool isMouseFixCenter, _float fTimeDelta)
{
	if (isCanTurn)
	{		
		Camera_Turn(isMouseFixCenter, fTimeDelta);

		if (isSyncLook)
		{
			//// 이건 카메라 방향벡터들 플레이어꺼랑 같게 맞추는부분
			//m_pTransformCom->Set_State(STATE::UP, pTargetTransform->Get_State(STATE::UP));
			//m_pTransformCom->Set_State(STATE::LOOK, pTargetTransform->Get_State(STATE::LOOK));
			//m_pTransformCom->Set_State(STATE::RIGHT, pTargetTransform->Get_State(STATE::RIGHT));

			// 이게 카메라에 의한 플레이어 회전
			//m_pTargetTransformCom->Turn(_float3{ 0.f, 1.f, 0.f }, fTimeDelta * m_fMove.x * m_fSensor);
			//m_pTargetTransformCom->Turn(m_pTargetTransformCom->Get_State(STATE::RIGHT), fTimeDelta * m_fMove.y * m_fSensor);

			m_pTargetTransformCom->Set_State(STATE::LOOK, m_pTransformCom->Get_State(STATE::LOOK));
		}
	}

	m_pTransformCom->Set_State(STATE::POSITION,
		m_pTargetTransformCom->Get_State(STATE::POSITION) + vLimitDistance);
}

HRESULT CCamera::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), pArg)))
		return E_FAIL;

	return S_OK;
}

CCamera* CCamera::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera* pInstance = new CCamera(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera::Clone(void* pArg)
{
	CCamera* pInstance = new CCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
