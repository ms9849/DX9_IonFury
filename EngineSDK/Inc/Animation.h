#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CComponent
{
public:
	typedef struct tagFrame
	{
		//_uint		iStart{};		// 몇 번 인덱스부터 출력
		vector<_float3>	Poses{ _float3{0.f, 0.f, 0.f} };
		_uint		iEnd{0};		// 몇 번 인덱스까지 출력
		_uint		iCurrentFrame{0};
		//_uint		iMotion{};	// 몇 번 모션을 출력
		_uint		iFrameSpeed{3}; // 애니메이션 속도 (3프레임 당 1장 재생)
		_float		fTime{};		// 애니메이션 구동 시간
		//_float		fRunTime{1.f};		// 애니메이션 전체 프레임 시간
	}FRAME_DESC;

private:
	CAnimation(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Set_Animation(FRAME_DESC* pFrameDesc);
	FRAME_DESC* Get_Animation();
	_uint Get_Frame_Index();
	void Play_Animation(_float fTimeDelta);
	_bool Check_Animation_Finish();
	void Clear_Animation();

private:
	FRAME_DESC*		m_tFrame{};
	_bool			m_bFinished{ false };

public:
	static CAnimation* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END