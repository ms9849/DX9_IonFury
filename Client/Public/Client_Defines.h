#pragma once

#include <process.h>
#include "../Default/framework.h"
#include "../Json/json.hpp"

#define JSON nlohmann::json

namespace Client
{
	//enum class OBJECTID { BACKGROUND, CAMERA, TERRAIN, SKY, CUBEOBJECT, END };

	const unsigned int			g_iWinSizeX = 1280;
	const unsigned int			g_iWinSizeY = 720;


	const unsigned int DefaultFontSet[39] = {
		32, 45, 47, 48, 49, 50, 51, 52, 53, 54,
		55, 56, 57, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
		82, 83, 84, 85, 86, 87, 88, 89, 90
	};

	const unsigned int GrayFontSet[37] = {
		32, 48, 49, 50, 51, 52, 53, 54, 55, 56,
		57, 65, 66, 67, 68, 69, 70, 71, 72, 73,
		74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
		84, 85, 86, 87, 88, 89, 90
	};

	const unsigned int PrimaryFontSet[94] ={
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
		42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
		62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
		82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
		92, 93, 94, 95, 97, 98, 99, 100, 101, 102,
		103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
		113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
		123, 124, 125, 126
	};

	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, JUSIN, BOSSFIGHT, END };
}

using namespace Client;

extern HWND g_hWnd;

/* 프레임워크 */
/* 배경(맵) */
/* 이펙트(파티클 인스턴싱, 스프라이트) */
/* 유아이 */

/* 컨텐츠작업 */
/* 플레이어담당, 몬스터 담당. 게임 컨텐츠 작업. */
