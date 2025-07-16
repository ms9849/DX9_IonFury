#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "GameInstance.h"

NS_BEGIN(Client)

class CCamera final : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CTransform::TRANSFORM_DESC
	{
		_float3		vAt{};
		_float3		vEye{};

		_float		fFov{};
		_float		fNear{}, fFar{};
	}CAMERA_DESC;

	typedef struct tagCameraSettings
	{
		CGameObject* pTarget{ nullptr };
		bool	isChaseTarget{ false };
		_float3 vLimitDistance{ 0.f, 0.f, 0.f };
		bool	isSyncLook{ false };
		bool	isMouseFixCenter{ false };
		bool	isCanTurn{ false };
	}CAMERA_CONFIG;

private:
	CCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	void Camera_Configure(const CAMERA_CONFIG& settings);
	void Camera_Configure_Clear(CAMERA_CONFIG& settings);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Shaking(_float fIntensity);

private:
	CAMERA_CONFIG					m_pCameraConfig{};

	class CTransform*				m_pTransformCom = { nullptr };
	class CTransform*				m_pTargetTransformCom = { nullptr };
	_float4x4						m_ProjMatrix = { };

	_float							m_fFov = {};
	_float							m_fAspect = {};
	_float							m_fNear = {};
	_float							m_fFar = {};

private:
	_float2							m_vOldMouse = {};
	_float2							m_fMove = {};
	_float							m_fSensor = { 0.2f };
	class CGameObject*				m_pTarget = { nullptr };
	 _float3						m_vOldCameraRight{};
	 _float3						m_vOldCameraUp{};
	 _float3						m_vOldCameraLook{};
	 //Ω¶¿Ã≈∑øÎ
	 _bool							m_bShaking = { false };
	 _float3						m_vOriginPos = {}; 
	 _float							m_fTimeAcc = {};
private:
	//std::function<void()> m_update;
	void Camera_Turn(bool isMouseFixCenter, _float fTimeDelta);
	void Chase_Target(_float3 vLimitDistance, bool isSyncLook, bool isCanTurn, bool isMouseFixCenter, _float fTimeDelta);

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CCamera* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
