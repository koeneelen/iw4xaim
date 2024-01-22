// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#define _USE_MATH_DEFINES
#include <math.h>


struct RGBA_COLOR
{
    float r, g, b, a;
};

typedef void* (*GetFontType_)(char* FontName, int Unknown1);
GetFontType_ GetFontType = (GetFontType_)0x505670;

typedef int (*DrawEngineText_)(char* Text, int Unknown1, void* Font, float X, float Y, float Unknown2, float Unknown3, float Unknown4, RGBA_COLOR* Color, int Unknown5);
DrawEngineText_ DrawEngineText = (DrawEngineText_)0x509D80;

typedef void* (*EndFrame_)();
EndFrame_ EndFrame = (EndFrame_)0x580C20;

void* Font = GetFontType((char*)"fonts/smalldevfont", 0);



void DrawTextWithEngine(float x, float y, RGBA_COLOR* Color, const char* Text, ...)
{
    char buf[300] = "";
    va_list va_alist;

    va_start(va_alist, Text);
    vsnprintf(buf, sizeof(buf), Text, va_alist);
    va_end(va_alist);

    DrawEngineText(buf, 0x7FFFFFFF, Font, x, y, 1.0f, 1.0f, 0.0f, Color, 0);
}


typedef void(__cdecl* tEndScene) (int a1, void* Src, int a3, int a4, float a5, float a6, int a7, int a8, float a9, int a10, int a11);
tEndScene oEndScene;
RGBA_COLOR ColorWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
RGBA_COLOR ColorCyan = { 0.0f, 1.0f, 1.0f, 1.0f };
RGBA_COLOR ColorGreen = { 0.0f, 1.0f, 0.0f, 1.0f };
RGBA_COLOR ColorYellow = { 1.0f, 1.0f, 0.0f, 1.0f };
RGBA_COLOR ColorRed = { 1.0f, 0.0f, 0.0f, 1.0f };
RGBA_COLOR ColorPink = { 1.0f, 0.75f, 0.80f, 1.0f };


struct Vector2
{
    float x, y;
};

struct Vector3
{
    float x, y, z;
};

struct Vector4
{
    float x, y, z, w;
};


void Subtract(Vector3 src, Vector3 dst, Vector3 &vResult)
{
    Vector3 diff;
    diff.x = src.x - dst.x;
    diff.y = src.y - dst.y;
    diff.z = src.z - dst.z;

    vResult = diff;
}

inline Vector3 Subtract(Vector3 src, Vector3 dst)
{
    Vector3 diff;
    diff.x = src.x - dst.x;
    diff.y = src.y - dst.y;
    diff.z = src.z - dst.z;

    return diff;
}


inline float DotProduct(Vector3 src, Vector3 dst)
{
    return src.x * dst.x + src.y * dst.y + src.z * dst.z;
}


//structs & shit
class CRefdef
{
public:
    __int32 x; //0x0000 
    __int32 y; //0x0004 
    __int32 iWidth; //0x0008 
    __int32 iHeight; //0x000C 
    float fovX; //0x0010 
    float fovY; //0x0014 
    Vector3 vOrigin; //0x0018 
    Vector3 vViewAxis[3]; //0x0024 
    char _0x0048[4];
    Vector3 vViewAngles; //0x004C 
    __int32 iTime; //0x0058 
    __int32 iMenu; //0x005C 
    char _0x0060[16128];
    Vector3 refdefViewAngles; //0x3F60 
    char _0x3F6C[276];

};//Size=0x4080
CRefdef * refdef = (CRefdef*)0x85B6F0;

class CEntity
{
public:
    char _0x0000[24];
    Vector3 vOrigin; //0x0018 
    char _0x0024[72];
    __int32 IsZooming; //0x006C 
    char _0x0070[12];
    Vector3 vOldOrigin; //0x007C 
    char _0x0088[84];
    __int32 clientNum; //0x00DC 
    __int32 eType; //0x00E0 
    __int32 eFlags; //0x00E4 
    char _0x00E8[12];
    Vector3 vNewOrigin; //0x00F4 
    char _0x0100[108];
    __int32 pickupItemID; //0x016C
    __int32 clientNum2; //0x0170 
    char _0x0174[52];
    BYTE weaponID; //0x01A8 
    char _0x01A9[51];
    __int32 iAlive; //0x01DC 
    char _0x01E0[32];
    __int32 clientNum3; //0x0200 

};//Size=0x0204

class CClient
{
public:
    __int32 IsValid; //0x0000 
    __int32 IsValid2; //0x0004 
    __int32 clientNum; //0x0008 
    char szName[16]; //0x000C 
    __int32 iTeam; //0x001C 
    __int32 iTeam2; //0x0020 
    __int32 iRank; //0x0024 rank+1 
    char _0x0028[4];
    __int32 iPerk; //0x002C 
    __int32 iKills; //0x0030
    __int32 iScore; //0x0034 
    char _0x0038[968];
    Vector3 vAngles; //0x0400 
    char _0x040C[136];
    __int32 IsShooting; //0x0494 
    char _0x0498[4];
    __int32 IsZoomed; //0x049C 
    char _0x04A0[68];
    __int32 weaponID; //0x04E4 
    char _0x04E8[24];
    __int32 weaponID2; //0x0500 
    char _0x0504[40];
};//Size=0x052C

//W2S
inline bool WorldToScreen(Vector3 world, Vector2& screen)
{
    auto Position = Subtract(world, refdef->vOrigin);
    Vector3 Transform;

    // get our dot products from viewAxis
    Transform.x = DotProduct(Position, refdef->vViewAxis[1]);
    Transform.y = DotProduct(Position, refdef->vViewAxis[2]);
    Transform.z = DotProduct(Position, refdef->vViewAxis[0]);

    // make sure it is in front of us
    if (Transform.z < 0.1f)
        return false;

    // get the center of the screen
    Vector2 Center;
    Center.x = (float)(refdef->iWidth * 0.5f);
    Center.y = (float)(refdef->iHeight * 0.5f);

    screen.x = Center.x * (1 - (Transform.x / refdef->fovX / Transform.z));
    screen.y = Center.y * (1 - (Transform.y / refdef->fovY / Transform.z));
    return true;
}

// ESP TESTING
#define dwDrawRotatedPic 0x510320 //CG_DrawRotatedPic func

typedef void(__cdecl* CG_DrawRotatedPic_t)(float x, float y, float width, float height, float, float, float, float, float angle, RGBA_COLOR* color, int shader);
CG_DrawRotatedPic_t CG_DrawRotatedPic = (CG_DrawRotatedPic_t)dwDrawRotatedPic;
static void Engine_DrawLine(float x1, float y1, float x2, float y2, RGBA_COLOR* color)
{
    float l1 = x2 - x1, h = y2 - y1, l2 = sqrt(l1 * l1 + h * h);
    CG_DrawRotatedPic(x1 + ((l1 - l2) / 2.f), y1 + (h / 2.f), l2, 1.f, 1.f, 1.f, 1.f, 1.f, (180.f / M_PI * atan(h / l1)), color, 1.0f);
}

bool IsVisibleEntity(CEntity* pEnt)
{
    DWORD dwCall = 0x582000;
    __asm mov esi, pEnt;
    __asm xor edi, edi;
    __asm mov eax, dwCall;
    __asm call eax;
}

const char * bones[] =
{
    "j_helmet", "j_head", "j_neck",
    "j_shoulder_le", "j_elbow_le", "j_wrist_le", "j_elbow_le", "j_shoulder_le", "j_neck",
    "j_shoulder_ri", "j_elbow_ri", "j_wrist_ri", "j_elbow_ri", "j_shoulder_ri", "j_neck",
    "j_spineupper", "j_spinelower",
    "j_hip_le", "j_knee_le", "j_ankle_le", "j_knee_le", "j_hip_le",
    "j_spinelower",
    "j_hip_ri", "j_knee_ri", "j_ankle_ri"
};
WORD Bones[ARRAYSIZE(bones)];
#define sBone   ARRAYSIZE(bones)

typedef unsigned short(__cdecl* tRegisterTag)(char* szName);
tRegisterTag RegisterTag_ = (tRegisterTag)0x004BC450;
unsigned short RegisterTag(char* szName)
{
    return RegisterTag_(szName);
}

int GetPlayerTag(unsigned short Tag, CEntity* pEnt, Vector3 &vOut)
{
    DWORD dwCall = 0x0056A020;
    //std::cout << "step3.6" << std::endl;
    //std::cout << vOut.x << std::endl;
    __asm movzx edi, Tag;
    __asm push vOut;
    __asm mov esi, pEnt;
    __asm call dwCall;
    __asm add esp, 0x4;
}

CEntity* GetEntity(int idx)
{
    return (CEntity*)(0x008F3CA8 + (0x204 * idx));
}

CClient* GetClient(int idx)
{
    return (CClient*)(0x008E77B0 + (0x52C * idx));
}

CEntity* GetLocalEntity()
{
    return (CEntity*)(0x008F3CA8 + (0x204 * *(int*)0x7F42C8));
}

CClient* GetLocalClient()
{
    return (CClient*)(0x008E77B0 + (0x52C * *(int*)0x7F42C8));
}




void BoneESP()
{
    for (int i = 0; i < 18;i++)//looping through players
    {
        CEntity* temp = GetEntity(i);
        Vector2* screen = new Vector2;
        Vector3* vOut = new Vector3;

        if (temp->iAlive == 17 || temp->iAlive == 49)
        {
            //for (int I = 0; I < sBone;I++)//looping through bones
            //{
                int I = 1; // just select j_head
                GetPlayerTag(Bones[I], temp, *vOut);
                if (WorldToScreen(*vOut, *screen))
                {
                    if (GetClient(i)->iTeam == GetLocalClient()->iTeam)
                    {
                        if (IsVisibleEntity(temp))
                            DrawTextWithEngine(screen->x, screen->y, &ColorCyan, ".");
                        else
                            DrawTextWithEngine(screen->x, screen->y, &ColorGreen, ".");
                    }
                    else
                    {
                        if (IsVisibleEntity(temp))
                            DrawTextWithEngine(screen->x, screen->y, &ColorYellow, ".");
                        else
                            DrawTextWithEngine(screen->x, screen->y, &ColorRed, ".");
                    }

                    if ((GetLocalClient()->iTeam && GetClient(i)->iTeam) == 0)
                    {
                        if (IsVisibleEntity(GetEntity(i)))
                            DrawTextWithEngine(screen->x, screen->y, &ColorWhite, ".");
                        else
                            DrawTextWithEngine(screen->x, screen->y, &ColorPink, ".");
                    }
                }
            //}
        }

    }
}
//aim
void VectoAngles(Vector3 &vVector, Vector3 &vAngles)
{
    float forward, yaw, pitch;
    if (vVector.y == 0 && vVector.x == 0)
    {
        yaw = 0;
        if (vVector.z > 0)
            pitch = 90;
        else
            pitch = 270;
    }
    else
    {
        if (vVector.x)
            yaw = (atan2(vVector.y, vVector.x) * 180 / M_PI);
        else if (vVector.y > 0)
            yaw = 90;
        else
            yaw = 270;

        if (yaw < 0)
            yaw += 360;

        forward = sqrt(vVector.x * vVector.x + vVector.y * vVector.y);
        pitch = (atan2(vVector.z, forward) * 180 / M_PI);

        if (pitch < 0)
            pitch += 360;
    }
    vAngles.x = -pitch;
    vAngles.y = yaw;
    vAngles.z = 0;
}

void GetAngleToOrigin(Vector3 vOrigin, float& flOutX, float& flOutY)
{
    Vector3* vEntity = new Vector3;
    Vector3* vAngle = new Vector3;
    Subtract(vOrigin, refdef->vOrigin, *vEntity);
    VectoAngles(*vEntity, *vAngle);

    if (vAngle->x > 180.0f)
        vAngle->x -= 360.0f;
    else if (vAngle->x < -180.0f)
        vAngle->x += 360.0f;

    if (vAngle->y > 180.0f)
        vAngle->y -= 360.0f;
    else if (vAngle->y < -180.0f)
        vAngle->y += 360.0f;

    vAngle->x -= refdef->refdefViewAngles.x; //refdef angles?
    vAngle->y -= refdef->refdefViewAngles.y;

    if (vAngle->x > 180.0f)
        vAngle->x -= 360.0f;
    else if (vAngle->x < -180.0f)
        vAngle->x += 360.0f;

    if (vAngle->y > 180.0f)
        vAngle->y -= 360.0f;
    else if (vAngle->y < -180.0f)
        vAngle->y += 360.0f;

    flOutX = vAngle->x;
    flOutY = vAngle->y;
}

float Length(Vector3 vect)
{
    return (vect.x) * (vect.x) + (vect.y) * (vect.y) + (vect.z) * (vect.z);
}

void Aim()
{
    CEntity* closest = nullptr;
    float distance = 0;
    for (int i = 0; i < 18;i++)//looping through players
    {
        CEntity* temp = GetEntity(i);
        Vector2* screen = new Vector2;
        Vector3* vOut = new Vector3;
        if (temp->iAlive == 17 || temp->iAlive == 49)
        {

            if (GetClient(i)->iTeam != GetLocalClient()->iTeam)
            //if (true)
            {
                if (IsVisibleEntity(temp))
                {
                    Vector3 headPos;
                    GetPlayerTag(Bones[1], temp, headPos);
                    Vector3* vDist = new Vector3;
                    Subtract(headPos, refdef->vOrigin, *vDist);
                    float iDist = Length(*vDist);
                    if (iDist > distance)
                    {
                        distance = iDist;
                        closest = temp;
                    }
                }
            }
        }
    }
    if (closest != nullptr)
    {
        Vector3 headPos;
        GetPlayerTag(Bones[1], closest, headPos);

        float flAngleX, flAngleY;
        GetAngleToOrigin(headPos, flAngleX, flAngleY);

        *(float*)0xB2F8D4 += flAngleY * 0.1;
        *(float*)0xB2F8D0 += flAngleX * 0.1;
    }
}


bool init = false, bBoneESP = false, bAim = false;
void __cdecl hEndScene(int a1, void* Src, int a3, int a4, float a5, float a6, int a7, int a8, float a9, int a10, int a11)
{
    if (!init)
    {
        std::cout << "Hook Succesfull" << std::endl;
        init = !init;
    }
    DrawTextWithEngine(20, 20, &ColorWhite, "flikkerBot");


    if (GetAsyncKeyState(VK_NUMPAD1) & 1)
    {
        bBoneESP = !bBoneESP;
    }

    if (bBoneESP)
    {
        BoneESP();
        DrawTextWithEngine(20, 40, &ColorWhite, "ESP ON");
    }
    else
    {
        DrawTextWithEngine(20, 40, &ColorWhite, "ESP OFF");
    }

    if (GetAsyncKeyState(VK_NUMPAD2) & 1)
    {
        bAim = !bAim;
    }


    if (bAim)
    {
        Aim();
        DrawTextWithEngine(20, 60, &ColorWhite, "AIM ON");
    }
    else
    {
        DrawTextWithEngine(20, 60, &ColorWhite, "AIM OFF");
    }
    return oEndScene(a1, Src, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

DWORD WINAPI HackThread(HMODULE hModule)
{
    //create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    //std::cout << "step1" << std::endl;
    for (int i = 0; i < sBone;i++)//looping through bones
    {
        Bones[i] = RegisterTag((char*)bones[i]);
    }

    bool b1 = false, b2 = false, b3 = false;
    oEndScene = (tEndScene)(uintptr_t)0x49C0D0;
    oEndScene = (tEndScene)TrampHook((char*)oEndScene, (char*)hEndScene, 7);
    return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

