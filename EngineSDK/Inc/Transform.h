#pragma once

#include "Component.h"

/* 1.특정 객체의 모델을 구성하는 정점들을 로컬스페이스->월드스페이스로 변환시켜주기위한 행렬을 보관한다. */
/* 2.월드행렬이 표현하기위한 상태들을 제어한다? */

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;
	}TRANSFORM_DESC;
private:
	CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	_float3 Get_State(STATE eState) {
		return *reinterpret_cast<_float3*>(&m_WorldMatrix.m[ENUM_CLASS(eState)][0]);
	}

	const _float4x4* Get_WorldMatrixInvPtr() {
		return D3DXMatrixInverse(&m_WorldMatrixInv, nullptr, &m_WorldMatrix);
	}

	const _float4x4* Get_WorldMatrixPtr() {
		return &m_WorldMatrix;
	}

	void Set_State(STATE eState, const _float3& vState) {
		memcpy(&m_WorldMatrix.m[ENUM_CLASS(eState)][0], &vState, sizeof(_float3));
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Add_Pos(const _float3 vSrc);
	void Set_Scale(const _float3& vScale);
	void Set_Transform();
	_float3 Get_Scaled();
	

	void Go_Straight(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);

	/* 항등상태 기준으로 회전시킨다.  */
	void Rotation(const _float3& vAxis, _float fRadian);

	/* 현 상태 기준으로 추가적으로 정해진 속도만큼 회전시킨다.  */
	void Turn(const _float3& vAxis, _float fTimeDelta);

	_bool Chase(const _float3& vTargetPos, _float fTimeDelta, _float fLimitDistance = 0.f);	
	
	void LookAt(const _float3& vTargetPos);

private:
	_float4x4			m_WorldMatrix = {};
	_float4x4			m_WorldMatrixInv = {};

	_float				m_fSpeedPerSec = {};
	_float				m_fRotationPerSec = {};

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END