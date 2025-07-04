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
		_float		fTime{0.f};		// 애니메이션 구동 시간
		_bool		bFinish{ false };	// 끝났는지 체크
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
	void Set_Animation(const _wstring strFrameKey, FRAME_DESC FrameDesc); // 키, 구조체로 애니메이션 초기 세팅 Ready_Animations에서 해야함
	FRAME_DESC* Get_Frame_Desc(const _wstring strFrameKey); // 키값으로 애니메이션 구조체 정보 가져오기
	_uint Get_Frame_Current_Index(const _wstring strFrameKey); // 키값으로 현재 애니메이션 인덱스 가져오기
	void Play_Animation(const _wstring strFrameKey, _float fTimeDelta); // 키값으로 애니메이션 돌리기
	_bool Check_Animation_Finish(); // 모든 애니메이션 끝났는지 체크
	_bool Check_Animation_Finish(const _wstring strFrameKey); // 키값으로 특정 애니메이션 끝났는지 체크
	void Clear_Animation(); // 모든 애니메이션 멈추기
	void Clear_Animation(const _wstring strFrameKey); // 키값으로 특정 애니메이션 멈추기
	_wstring Get_FrameKey();

private:
	map<const _wstring, FRAME_DESC> m_tFrames{};
	_wstring		m_strFrameKey{};
	_float			m_fTimeStack{ 0.f };

public:
	static CAnimation* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END