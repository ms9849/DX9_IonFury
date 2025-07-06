#pragma once

#include "AlphaObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CParticleSystem abstract : public CAlphaObject
{
public:
	/* 
	FVF값은 미리 지정해두기. 이것 말곤 안쓸테니까..
	설령 쓴다고 하더라도 추후 바깥에서 따로 세팅해주면 될 일이다.	
	*/
	
	/*
	Particle ? -> VTXPOSTEX와 같은 역할을 하는, 실제 파티클 정점 하나의 정보
	*/
	typedef struct Particle {
		D3DXVECTOR3 vPosition = {};         // 파티클의 위치를 저장한다
		D3DCOLOR    Color = {};        // 파티클 색을 저장한다
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;    // FVF값.
	} PARTICLE;

	/*
	Particle Attribute? -> 파티클의 속성들을 나타내는 값. 
	모든 속성을 가지고 있을 필요는 없다.
	*/
	typedef struct ParticleAttribute {
		D3DXVECTOR3     vPosition = {};     // 월드스페이스 내 파티클 위치
		D3DXVECTOR3     vVelocity = {};     // 초당 이동 속도
		D3DXVECTOR3     vAcceleration = {}; // 초당 가속 속도
		_float			fSize = {};			// 파티클 사이즈
		_float          fLifeTime = {};     // 파티클 생명시간
		_float          fAge = {};          // 파티클의 현재 나이
		D3DXCOLOR       Color = {};        // 파티클의 색
		D3DXCOLOR       ColorFade = {};    // 파티클 색이 퇴색하는 방법
		_bool           bisAlive = { true };      // 파티클 생존 여부
	} PARTICLE_ATTRIBUTE;
	/*
	Particle System을 상속받은 실제 파티클들을 세팅하기 위한 Desc 구조체
	*/
	typedef struct tagParticleSystemDesc
	{
		_uint iNumParticles;
		_float3 vBoundaryMin;
		_float3 vBoundaryMax;
		_float3 vWorldPosition;
	} PARTICLESYSTEM_DESC;

protected:
	CParticleSystem(LPDIRECT3DDEVICE9 pGraphic_Device);
	CParticleSystem(const CParticleSystem& Prototype);
	virtual ~CParticleSystem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_bool isPlaying() const { return m_bIsPlaying; }
	void Set_Playing(_bool bFlag) { m_bIsPlaying = bFlag; }

public:
	virtual void Reset();
	// 파티클의 각 입자를 초기화하는 방법은 객체에 따라 달라진다
	// 상속받는 하위 객체에서 구현하도록 추상 메서드로 선언하자
	virtual void ResetParticle(ParticleAttribute* attribute) = 0;
	virtual void AddParticle();
	_bool isEmpty();

protected:
	virtual void removeDeadParticles();

protected:
	virtual HRESULT Begin_RenderState();
	virtual HRESULT End_RenderState();


protected:
	// 파티클 매니저의 위치 (파티클 생성 위치)
	_float3                 m_vOrigin;

	// 파티클 생성 경계 범위
	_float3                 m_vBoundaryMin;
	_float3                 m_vBoundaryMax;

	// 초당 생성 파티클 입자 수
	_float                   m_fEmitRate;
	// 파티클 크기
	_float                   m_fSize;
	// 파티클 버텍스 버퍼
	LPDIRECT3DVERTEXBUFFER9  m_pVertexBuffer;
	// 파티클 속성 리스트
	list<PARTICLE_ATTRIBUTE>  m_Particles;
	// 한 화면에서 시스템이 가질 수 있는 최대 파티클 수
	_uint                    m_iMaxParticles;

	// 버텍스 버퍼가 보관할 수 있는 파티클 수
	DWORD                    m_vertexBufferSize;
	// 렌더링에 이용되는 칭구들이다
	// 복사를 시작할 다음 파티클의 인덱스
	DWORD                    m_vertexBufferOffset;
	// 한 파티클 드로잉 단계에 정의된 파티클의 수
	DWORD                    m_vertexBufferBatchSize;

	// 파티클이 하나라도 재생 중이라면 TRUE
	_bool					 m_bIsPlaying = { false };
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END
