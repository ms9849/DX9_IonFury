#pragma once

#include "Component.h"
#include "Texture.h"
#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CComponent
{
public:
	typedef struct tagFrame
	{
		//_uint		iStart{};		// 몇 번 인덱스부터 출력
		vector<_float3>	Poses{ _float3{0.f, 0.f, 0.f} };
		_uint		iEnd{0};		// 몇 번 인덱스까지 출력
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

private:
	FRAME_DESC*						m_tFrame{};

public:
	static CAnimation* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END

/*
tag로 애니메이션 구별해서 불러오기
구조체 리스트 반환

구조체 tagFrame
{
	_uint index
	_float3 pos
	_float speed
}, Frame

Ready_Actions 에서 애니메이션 싹 세팅 해주고
세팅 - _int index, _float3 pos, _float speed

업데이트에서 위치, 속도 바꿔주고

렌더에서 Set_Frame(key, index)로 텍스쳐 바꿔주기










<tag, vector<구조체>>

1.	Player에서 구조체 리스트 꺼내다가 반복문 돌려서 사용?
2.	Player에서 구조체 세팅해서 Frame에 저장하고 Animation불러다가 타이밍에 맞춰 애니메이션 실행?

텍스처는 어디서 준비?
loader에서 모든 애니메이션 텍스처 로드
Player/Pistol/Idle, Player/ShootGun/Shoot <- %c - tag
0~... %d - index
/Texture/%c_%d.png

*/