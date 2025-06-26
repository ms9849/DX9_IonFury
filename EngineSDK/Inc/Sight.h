#pragma once

#include "Component.h"
/* 시야를 체크하기 위해 무엇이 필요한가 
	1. 시야의 범위 각도(라디안)
	2. 시야의 시작점부터 어느 길이까지를 인지할 것인가에 대한 길이
*/

NS_BEGIN(Engine)

class ENGINE_DLL CSight final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float			fRange;
		_float			fRotation;
		class CTransform* pPlayerTransform;
		class CTransform* pMonsterTransform;
	}SIGHT_DESC;
private:
	CSight(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSight(const CSight& Prototype);
	virtual ~CSight() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:	
	int Check_Sight(_float fTimeDelta);

private:
	_float				m_fRadian = {};
	_float				m_fRange = {};
	_float				m_fRotation = {};
	class CTransform*	m_pTransform = { nullptr };
	class CTransform*			m_pPlayerTransform = { nullptr };

public:
	static CSight* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END