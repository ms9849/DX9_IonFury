#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

/*
SetUp_OnTerrain 함수를 이용해서 지형에 태움..

터레인의 Transform와 VIBuffer를 받아올 때

Change_Terrain을 이용하여 타야할 버퍼와 트랜스폼을 가져오게끔 한다?

-> 큐브들은 어떻게 처리하지?

-> 

*/

class CLand_Manager final: public CBase
{
private:
	CLand_Manager();
	virtual ~CLand_Manager() = default;

public:
	void Check_Landing();

private:


public:
	static CLand_Manager* Create();
	virtual void Free();
};

NS_END
