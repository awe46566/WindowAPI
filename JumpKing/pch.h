#pragma once

#include <iostream>

#define NOMINMAX 
#include <windows.h>	// min/max 유틸함수가 또 구현.
// Direct2D: 2D 그리기, DirectWrite: 텍스트, WIC: PNG 디코딩
#include <d2d1.h>
#include <dwrite.h>
#include <dwrite_3.h>
#include <wincodec.h>
// ComPtr: COM 객체의 AddRef/Release를 자동 관리
#include <wrl/client.h>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>
#include <algorithm>
#include <windowsx.h>
#include <queue>

// C++ 20 std::format
#include <format>
#include <filesystem>
namespace fs = std::filesystem;

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
// 사운드
#include <MMSystem.h>
#include <dsound.h>
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dsound.lib")

// Json
#include <fstream>

// 정수형을 관리하기 편한 용도로, 별칭을 만들자.
// -> 언리얼이 이런방식이라 미리 익숙해지자.

using int8 = char;		// 1byte 정수형
using int16 = short;	// 2byte 정수형
using int32 = int;		// 4byte 정수형
using int64 = long long;// 8byte 정수형

using uint8 = unsigned char;		// 1byte 정수형
using uint16 = unsigned short;	// 2byte 정수형
using uint32 = unsigned int;		// 4byte 정수형
using uint64 = unsigned long long;// 8byte 정수형

using namespace std;

