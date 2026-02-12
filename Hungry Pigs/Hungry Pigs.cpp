#include "framework.h"
#include "Hungry Pigs.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "FCheck.h"
#include "errh.h"
#include "D2BMPLOADER.h"
#include "gifresizer.h"
#include "grideng.h"
#include <chrono>
#include <locale>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "gifresizer.lib")
#pragma comment(lib, "grideng.lib")

constexpr wchar_t bWinClassName[]{ L"PIGS" };
constexpr char tmp_file[]{ ".\\res\\data\\temp.dat" };
constexpr wchar_t Ltmp_file[] { L".\\res\\data\\temp.dat" };
constexpr wchar_t help_file[]{ L".\\res\\data\\help.dat" };
constexpr wchar_t record_file[]{ L".\\res\\data\\record.dat" };
constexpr wchar_t save_file[]{ L".\\res\\data\\save.dat" };
constexpr wchar_t sound_file[]{ L".\\res\\snd\\main.dat" };

constexpr int mNew{ 1001 };
constexpr int mLvl{ 1002 };
constexpr int mExit{ 1003 };
constexpr int mSave{ 1004 };
constexpr int mLoad{ 1005 };
constexpr int mHoF{ 1006 };

constexpr int first_record{ 2001 };
constexpr int no_record{ 2002 };
constexpr int record{ 2003 };

WNDCLASS bWinClass{};
HINSTANCE bIns{ nullptr };
HWND bHwnd{ nullptr };
HICON mainIcon{ nullptr };
HCURSOR mainCursor{ nullptr };
HCURSOR outCursor{ nullptr };
HMENU bBar{ nullptr };
HMENU bMain{ nullptr };
HMENU bStore{ nullptr };
HDC PaintDC{ nullptr };
PAINTSTRUCT bPaint{};
POINT cur_pos{};
MSG bMsg{};
BOOL bRet{ 0 };
UINT bTimer{ 0 };

D2D1_RECT_F b1Rect{ 50.0f, 0, scr_width / 3.0f - 50.0f, 50.0f };
D2D1_RECT_F b2Rect{ scr_width / 3.0f, 0, scr_width * 2.0f / 3.0f - 50.0f, 50.0f };
D2D1_RECT_F b3Rect{ scr_width * 2.0f / 3.0f, 0, scr_width - 50.0f, 50.0f };

D2D1_RECT_F b1TxtRect{ 80.0f, 10.0f, scr_width / 3.0f - 50.0f, 50.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3.0f + 30.0f, 10.0f, scr_width * 2.0f / 3.0f - 50.0f, 50.0f };
D2D1_RECT_F b3TxtRect{ scr_width * 2.0f / 3.0f + 30.0f, 10.0f, scr_width - 50.0f, 50.0f };

bool pause = false;
bool sound = true;
bool show_help = false;
bool in_client = true;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;
bool name_set = false;

bool dizzy = false;

wchar_t current_player[16]{ L"TARLYO" };

int score = 0;
int level = 1;
int mins = 0;
int secs = 0;
int dizzy_counter = 0;

float x_scale{ 0 };
float y_scale{ 0 };

ID2D1Factory* iFactory{ nullptr };
ID2D1HwndRenderTarget* Draw{ nullptr };

ID2D1RadialGradientBrush* b1BckgBrush{ nullptr };
ID2D1RadialGradientBrush* b2BckgBrush{ nullptr }; 
ID2D1RadialGradientBrush* b3BckgBrush{ nullptr };

ID2D1SolidColorBrush* statBrush{ nullptr };
ID2D1SolidColorBrush* txtBrush{ nullptr };
ID2D1SolidColorBrush* hgltBrush{ nullptr };
ID2D1SolidColorBrush* inactBrush{ nullptr };

IDWriteFactory* iWriteFactory{ nullptr };
IDWriteTextFormat* nrmFormat{ nullptr };
IDWriteTextFormat* midFormat{ nullptr };
IDWriteTextFormat* bigFormat{ nullptr };

ID2D1Bitmap* bmpDizzy{ nullptr };
ID2D1Bitmap* bmpLogo{ nullptr };
ID2D1Bitmap* bmpFence{ nullptr };
ID2D1Bitmap* bmpRock{ nullptr };

ID2D1Bitmap* bmpBackon{ nullptr };
ID2D1Bitmap* bmpCheese{ nullptr };
ID2D1Bitmap* bmpFish{ nullptr };
ID2D1Bitmap* bmpPizza{ nullptr };
ID2D1Bitmap* bmpRotten{ nullptr };

ID2D1Bitmap* bmpDirt{ nullptr };
ID2D1Bitmap* bmpForest{ nullptr };
ID2D1Bitmap* bmpLava{ nullptr };
ID2D1Bitmap* bmpPath{ nullptr };
ID2D1Bitmap* bmpWater{ nullptr };

ID2D1Bitmap* bmpIntro[18]{ nullptr };

ID2D1Bitmap* bmpFlyL[2]{ nullptr };
ID2D1Bitmap* bmpFlyR[2]{ nullptr };

ID2D1Bitmap* bmpToughL[5]{ nullptr };
ID2D1Bitmap* bmpToughR[5]{ nullptr };

ID2D1Bitmap* bmpRunnerL[12]{ nullptr };
ID2D1Bitmap* bmpRunnerR[12]{ nullptr };

ID2D1Bitmap* bmpFreakL[11]{ nullptr };
ID2D1Bitmap* bmpFreakR[11]{ nullptr };

ID2D1Bitmap* bmpHeroL[16]{ nullptr };
ID2D1Bitmap* bmpHeroR[16]{ nullptr };

//////////////////////////////////////////////////////////

dll::RANDIT RandIt{};

dll::GRID* GameField{ nullptr };

dll::PIGS* Hero{ nullptr };
std::vector<dll::PIGS*> vEvils;

std::vector<dll::FOOD*>vFoods;
std::vector<dll::OBSTACLES*>vObstacles;

//////////////////////////////////////////////////////////

template<typename T>concept HasRelease = requires(T check)
{
	check.Release();
};
template<HasRelease T>bool ClearMem(T** var)
{
	if ((*var))
	{
		(*var)->Release();
		(*var) = nullptr;
	}
};
void LogErr(const wchar_t* what)
{
	std::wofstream err{ L".\\res\\data\\error.log",std::ios::app };
	err << what << L" time-stamp: " << std::chrono::system_clock::now() << std::endl << std::endl;
	err.close();
}
void ClearResources()
{
	if (!ClearMem(&iFactory))LogErr(L"Error releasing iFactory !");
	if (!ClearMem(&Draw))LogErr(L"Error releasing hwndRenderTarget !");

	if (!ClearMem(&b1BckgBrush))LogErr(L"Error releasing b1BckgBrush !");
	if (!ClearMem(&b2BckgBrush))LogErr(L"Error releasing b2BckgBrush !");
	if (!ClearMem(&b3BckgBrush))LogErr(L"Error releasing b3BckgBrush !");

	if (!ClearMem(&txtBrush))LogErr(L"Error releasing txtBrush !");
	if (!ClearMem(&hgltBrush))LogErr(L"Error releasing hgltBrush !");
	if (!ClearMem(&inactBrush))LogErr(L"Error releasing inactBrush !");
	if (!ClearMem(&statBrush))LogErr(L"Error releasing statBrush !");

	if (!ClearMem(&iWriteFactory))LogErr(L"Error releasing iWriteFactory !");
	if (!ClearMem(&nrmFormat))LogErr(L"Error releasing nrmFormat !");
	if (!ClearMem(&midFormat))LogErr(L"Error releasing midFormat !");
	if (!ClearMem(&bigFormat))LogErr(L"Error releasing bigFormat !");

	if (!ClearMem(&bmpDizzy))LogErr(L"Error releasing bmpDizzy !");
	if (!ClearMem(&bmpLogo))LogErr(L"Error releasing bmpLogo !");
	if (!ClearMem(&bmpFence))LogErr(L"Error releasing bmpFence !");
	if (!ClearMem(&bmpRock))LogErr(L"Error releasing bmpRock !");

	if (!ClearMem(&bmpBackon))LogErr(L"Error releasing bmpBackon !");
	if (!ClearMem(&bmpCheese))LogErr(L"Error releasing bmpCheese !");
	if (!ClearMem(&bmpFish))LogErr(L"Error releasing bmpFish !");
	if (!ClearMem(&bmpPizza))LogErr(L"Error releasing bmpPizza !");
	if (!ClearMem(&bmpRotten))LogErr(L"Error releasing bmpRotten !");

	if (!ClearMem(&bmpDirt))LogErr(L"Error releasing bmpDirt !");
	if (!ClearMem(&bmpForest))LogErr(L"Error releasing bmpForest !");
	if (!ClearMem(&bmpLava))LogErr(L"Error releasing bmpLava !");
	if (!ClearMem(&bmpPath))LogErr(L"Error releasing bmpPath !");
	if (!ClearMem(&bmpWater))LogErr(L"Error releasing bmpWater !");

	for (int i = 0; i < 18; ++i)if (!ClearMem(&bmpIntro[i]))LogErr(L"Error releasing bmpIntro !");

	for (int i = 0; i < 2; ++i)if (!ClearMem(&bmpFlyL[i]))LogErr(L"Error releasing bmpFlyL !");
	for (int i = 0; i < 2; ++i)if (!ClearMem(&bmpFlyR[i]))LogErr(L"Error releasing bmpFlyR !");

	for (int i = 0; i < 5; ++i)if (!ClearMem(&bmpToughL[i]))LogErr(L"Error releasing bmpToughL !");
	for (int i = 0; i < 5; ++i)if (!ClearMem(&bmpToughR[i]))LogErr(L"Error releasing bmpToughR !");

	for (int i = 0; i < 12; ++i)if (!ClearMem(&bmpRunnerL[i]))LogErr(L"Error releasing bmpRunnerL !");
	for (int i = 0; i < 12; ++i)if (!ClearMem(&bmpRunnerR[i]))LogErr(L"Error releasing bmpRunnerR !");

	for (int i = 0; i < 11; ++i)if (!ClearMem(&bmpFreakL[i]))LogErr(L"Error releasing bmpFreakL !");
	for (int i = 0; i < 11; ++i)if (!ClearMem(&bmpFreakR[i]))LogErr(L"Error releasing bmpFreakR !");

	for (int i = 0; i < 16; ++i)if (!ClearMem(&bmpHeroL[i]))LogErr(L"Error releasing bmpHeroL !");
	for (int i = 0; i < 16; ++i)if (!ClearMem(&bmpHeroR[i]))LogErr(L"Error releasing bmpHeroR !");
}
void ErrExit(int what)
{
	MessageBeep(MB_ICONERROR);
	MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

	ClearResources();
	std::remove(tmp_file);
	exit(1);
}

void GameOver()
{
	PlaySound(NULL, NULL, NULL);
	KillTimer(bHwnd, bTimer);



	bMsg.message = WM_QUIT;
	bMsg.message = 0;
}
void InitGame()
{
	name_set = false;
	dizzy = false;

	wcscpy_s(current_player, L"TARLYO");

	score = 0;
	level = 1;
	mins = 0;
	secs = 0;

	dizzy_counter = 0;

	if (GameField)delete GameField;
	GameField = new dll::GRID();
	
	ClearMem(&Hero);
	Hero = dll::PIGS::create(pigs::hero, RandIt(10.0f, 100.0f), RandIt(sky + 10.0f, ground - 90.0f));

	if (!vEvils.empty())for (int i = 0; i < vEvils.size(); ++i)ClearMem(&vEvils[i]);
	if (!vFoods.empty())for (int i = 0; i < vFoods.size(); ++i)ClearMem(&vFoods[i]);
	if (!vObstacles.empty())for (int i = 0; i < vObstacles.size(); ++i)ClearMem(&vObstacles[i]);

	for (int i = 0; i <= 10; ++i)
	{
		bool is_ok = false;
		while (!is_ok)
		{
			is_ok = true;

			dll::OBSTACLES* dummy{ nullptr };

			if (RandIt(0, 8) == 4)dummy = dll::OBSTACLES::create(obstacles::fence, RandIt(-300.0f, scr_width + 300.0f),
				RandIt(-100.0f, ground + 100.0f));
			else dummy = dll::OBSTACLES::create(obstacles::rock, RandIt(-300.0f, scr_width + 300.0f),
				RandIt(-100.0f, ground + 100.0f));

			if (vObstacles.empty())vObstacles.push_back(dummy);
			else
			{
				for (int i = 0; i < vObstacles.size(); ++i)
				{
					FRECT existing{ vObstacles[i]->start.x,vObstacles[i]->start.y,vObstacles[i]->end.x,vObstacles[i]->end.y };
					FRECT new_obst{ dummy->start.x,dummy->start.y,dummy->end.x,dummy->end.y };

					if (dll::Intersect(existing, new_obst))
					{
						is_ok = false;
						break;
					}
				}

				if (is_ok)vObstacles.push_back(dummy);
			}
		}
	}



}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_INITDIALOG:
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
		return true;

	case WM_CLOSE:
		EndDialog(hwnd, IDCANCEL);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;

		case IDOK:
			if (GetDlgItemText(hwnd, IDC_NAME, current_player, 16) < 1)
			{
				wcscpy_s(current_player, L"TARLYO");;
				if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
				MessageBox(bHwnd, L"Ха, ха, хааа ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
				EndDialog(hwnd, IDCANCEL);
				break;
			}
			EndDialog(hwnd, IDOK);
		}
		break;
	}

	return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
	switch (ReceivedMsg)
	{
	case WM_CREATE:
		if (bIns)
		{
			SetTimer(hwnd, bTimer, 1000, NULL);

			bBar = CreateMenu();
			bMain = CreateMenu();
			bStore = CreateMenu();

			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
			AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

			AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
			AppendMenu(bMain, MF_STRING, mLvl, L"Следващо ниво");
			AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bMain, MF_STRING, mExit, L"Изход");

			AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
			AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
			AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
			AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

			SetMenu(hwnd, bBar);
			InitGame();
		}
		break;

	case WM_CLOSE:
		pause = true;
		if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
		if (MessageBox(hwnd, L"Ако излезеш, ще изгубиш тази игра !\n\nНаистина ли излизаш ?",
			L"Изход", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
		{
			pause = false;
			break;
		}
		InitGame();
		break;

	case WM_PAINT:
		PaintDC = BeginPaint(hwnd, &bPaint);
		FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(180, 180, 180)));
		EndPaint(hwnd, &bPaint);
		break;

	case WM_TIMER:
		if (pause)break;
		if (dizzy_counter > 0)dizzy_counter--;
		++secs;
		mins = secs / 60;
		break;

	case WM_SETCURSOR:
		GetCursorPos(&cur_pos);
		ScreenToClient(hwnd, &cur_pos);
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (!in_client)
			{
				in_client = true;
				pause = false;
			}

			if (cur_pos.y * y_scale <= 50)
			{
				if (cur_pos.x * x_scale >= b1Rect.left && cur_pos.x * x_scale <= b1Rect.right)
				{
					if (!b1Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = true;
						b2Hglt = false;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * x_scale >= b2Rect.left && cur_pos.x * x_scale <= b2Rect.right)
				{
					if (!b2Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = true;
						b3Hglt = false;
					}
				}
				else if (cur_pos.x * x_scale >= b3Rect.left && cur_pos.x * x_scale <= b3Rect.right)
				{
					if (!b3Hglt)
					{
						if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
						b1Hglt = false;
						b2Hglt = false;
						b3Hglt = true;
					}
				}
				else
				{
					if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
					b1Hglt = false;
					b2Hglt = false;
					b3Hglt = false;
				}

				SetCursor(outCursor);
				return true;
			}
			else if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}

			SetCursor(mainCursor);
			return true;
		}
		else
		{
			if (in_client)
			{
				in_client = false;
				pause = true;
			}

			if (b1Hglt || b2Hglt || b3Hglt)
			{
				if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
				b1Hglt = false;
				b2Hglt = false;
				b3Hglt = false;
			}

			SetCursor(LoadCursor(NULL, IDC_ARROW));

			return true;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case mNew:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако рестартираш, ще изгубиш тази игра !\n\nНаистина ли рестартираш ?",
				L"Рестарт", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			InitGame();
			break;

		case mLvl:
			pause = true;
			if (sound)mciSendString(L"play .\\res\\snd\\exclamation.wav", NULL, NULL, NULL);
			if (MessageBox(hwnd, L"Ако продължиш, ще изгубиш бонуса !\n\nНаистина ли пропускаш нивото ?",
				L"Следващо ниво", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
			{
				pause = false;
				break;
			}
			//LevelUp();
			break;

		case mExit:
			SendMessage(hwnd, WM_CLOSE, NULL, NULL);
			break;

		}
		break;




	default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
	}

	return (LRESULT)(FALSE);
}

void CreateResources()
{
	int win_x = (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2.0f));
	int win_y = 10;
	if (GetSystemMetrics(SM_CXSCREEN) < win_x + (int)(scr_width) || GetSystemMetrics(SM_CYSCREEN) < win_y
		+ (int)(scr_height))ErrExit(eScreen);

	mainIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 255, LR_LOADFROMFILE));
	if (!mainIcon)ErrExit(eIcon);
	mainCursor = LoadCursorFromFile(L".\\res\\main.ani");
	outCursor = LoadCursorFromFile(L".\\res\\out.ani");
	if (!mainCursor || !outCursor)ErrExit(eCursor);

	bWinClass.lpszClassName = bWinClassName;
	bWinClass.hInstance = bIns;
	bWinClass.lpfnWndProc = &WinProc;
	bWinClass.hbrBackground = CreateSolidBrush(RGB(180, 180, 180));
	bWinClass.hIcon = mainIcon;
	bWinClass.hCursor = mainCursor;
	bWinClass.style = CS_DROPSHADOW;

	if (!RegisterClass(&bWinClass))ErrExit(eClass);

	bHwnd = CreateWindow(bWinClassName, L"ГЛАДНИ ПРАСЕТА !", WS_CAPTION | WS_SYSMENU, win_x, win_y, (int)(scr_width),
		(int)(scr_height), NULL, NULL, bIns, NULL);
	if (!bHwnd)ErrExit(eWindow);
	else
	{
		ShowWindow(bHwnd, SW_SHOWDEFAULT);

		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
		if (hr != S_OK)
		{
			LogErr(L"Error creating D2D1 Factory !");
			ErrExit(eD2D);
		}

		if (iFactory)
		{
			hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
				D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1 HwndRenderTarget !");
				ErrExit(eD2D);
			}
		}

		if (Draw)
		{
			RECT clR{};
			GetClientRect(bHwnd, &clR);

			D2D1_SIZE_F D2D1Size{ Draw->GetSize() };

			x_scale = D2D1Size.width / (clR.right - clR.left);
			y_scale = D2D1Size.height / (clR.bottom - clR.top);

			D2D1_GRADIENT_STOP gStops[2]{};
			ID2D1GradientStopCollection* gColl{ nullptr };

			gStops[0].position = 0;
			gStops[0].color = D2D1::ColorF(D2D1::ColorF::Chocolate);
			gStops[1].position = 1.0f;
			gStops[1].color = D2D1::ColorF(D2D1::ColorF::Maroon);

			hr = Draw->CreateGradientStopCollection(gStops, 2, &gColl);
			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1 GradientStopCollection !");
				ErrExit(eD2D);
			}

			if (gColl)
			{
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F((b1Rect.right -
						b1Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b1Rect.right - b1Rect.left) / 2.0f, 25.0f),
						gColl, &b1BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F((b2Rect.right -
					b2Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b2Rect.right - b2Rect.left) / 2.0f, 25.0f),
					gColl, &b2BckgBrush);
				hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F((b3Rect.right -
					b3Rect.left) / 2.0f, 25.0f), D2D1::Point2F(0, 0), (b3Rect.right - b3Rect.left) / 2.0f, 25.0f),
					gColl, &b3BckgBrush);
				if (hr != S_OK)
				{
					LogErr(L"Error creating D2D1 RadialGradientBrush !");
					ErrExit(eD2D);
				}
				
				ClearMem(&gColl);
			}

			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkKhaki), &statBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Chartreuse), &hgltBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Navy), &txtBrush);
			hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::MediumSpringGreen), &inactBrush);

			if (hr != S_OK)
			{
				LogErr(L"Error creating D2D1 SolidColorBrush !");
				ErrExit(eD2D);
			}

			bmpDizzy = Load(L".\\res\\img\\dizzy.png", Draw);
			if (!bmpDizzy)
			{
				LogErr(L"Error loading bmpDizzy !");
				ErrExit(eD2D);
			}
			bmpLogo = Load(L".\\res\\img\\Logo.png", Draw);
			if (!bmpLogo)
			{
				LogErr(L"Error loading bmpLogo !");
				ErrExit(eD2D);
			}
			bmpFence = Load(L".\\res\\img\\Fence.png", Draw);
			if (!bmpFence)
			{
				LogErr(L"Error loading bmpFence !");
				ErrExit(eD2D);
			}
			bmpRock = Load(L".\\res\\img\\Rock.png", Draw);
			if (!bmpRock)
			{
				LogErr(L"Error loading bmpRock !");
				ErrExit(eD2D);
			}

			bmpBackon = Load(L".\\res\\img\\food\\Bacon.png", Draw);
			if (!bmpBackon)
			{
				LogErr(L"Error loading bmpBackon !");
				ErrExit(eD2D);
			}
			bmpCheese = Load(L".\\res\\img\\food\\Cheese.png", Draw);
			if (!bmpCheese)
			{
				LogErr(L"Error loading bmpCheese !");
				ErrExit(eD2D);
			}
			bmpPizza = Load(L".\\res\\img\\food\\Pizza.png", Draw);
			if (!bmpCheese)
			{
				LogErr(L"Error loading bmpPizza !");
				ErrExit(eD2D);
			}
			bmpFish = Load(L".\\res\\img\\food\\Fish.png", Draw);
			if (!bmpFish)
			{
				LogErr(L"Error loading bmpFish !");
				ErrExit(eD2D);
			}
			bmpRotten = Load(L".\\res\\img\\food\\Rotten.png", Draw);
			if (!bmpRotten)
			{
				LogErr(L"Error loading bmpRotten !");
				ErrExit(eD2D);
			}

			bmpDirt = Load(L".\\res\\img\\tiles\\dirt.png", Draw);
			if (!bmpDirt)
			{
				LogErr(L"Error loading bmpDirt !");
				ErrExit(eD2D);
			}
			bmpForest = Load(L".\\res\\img\\tiles\\Forest.png", Draw);
			if (!bmpForest)
			{
				LogErr(L"Error loading bmpForest !");
				ErrExit(eD2D);
			}
			bmpPath = Load(L".\\res\\img\\tiles\\Path.png", Draw);
			if (!bmpPath)
			{
				LogErr(L"Error loading bmpPath !");
				ErrExit(eD2D);
			}
			bmpLava = Load(L".\\res\\img\\tiles\\Lava.png", Draw);
			if (!bmpLava)
			{
				LogErr(L"Error loading bmpLava !");
				ErrExit(eD2D);
			}
			bmpWater = Load(L".\\res\\img\\tiles\\Water.png", Draw);
			if (!bmpWater)
			{
				LogErr(L"Error loading bmpWater !");
				ErrExit(eD2D);
			}

			for (int i = 0; i < 18; ++i)
			{
				wchar_t name[50]{ L".\\res\\img\\intro\\" };
				wchar_t add[5]{ L"\0" };
				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpIntro[i] = Load(name, Draw);
				if (!bmpIntro[i])
				{
					LogErr(L"Error loading bmpIntro !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 16; ++i)
			{
				wchar_t name[50]{ L".\\res\\img\\hero\\l\\" };
				wchar_t add[5]{ L"\0" };
				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpHeroL[i] = Load(name, Draw);
				if (!bmpHeroL[i])
				{
					LogErr(L"Error loading bmpHeroL !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 16; ++i)
			{
				wchar_t name[50]{ L".\\res\\img\\hero\\r\\" };
				wchar_t add[5]{ L"\0" };
				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpHeroR[i] = Load(name, Draw);
				if (!bmpHeroR[i])
				{
					LogErr(L"Error loading bmpHeroR !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 2; ++i)
			{
				wchar_t name[50]{ L".\\res\\img\\evils\\1\\l\\" };
				wchar_t add[5]{ L"\0" };
				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpFlyL[i] = Load(name, Draw);
				if (!bmpFlyL[i])
				{
					LogErr(L"Error loading bmpFlyL !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 2; ++i)
			{
				wchar_t name[50]{ L".\\res\\img\\evils\\1\\r\\" };
				wchar_t add[5]{ L"\0" };
				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpFlyR[i] = Load(name, Draw);
				if (!bmpFlyR[i])
				{
					LogErr(L"Error loading bmpFlyR !");
					ErrExit(eD2D);
				}
			}

			for (int i = 0; i < 5; ++i)
			{
				wchar_t name[50]{ L".\\res\\img\\evils\\2\\l\\" };
				wchar_t add[5]{ L"\0" };
				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpToughL[i] = Load(name, Draw);
				if (!bmpToughL[i])
				{
					LogErr(L"Error loading bmpToughL !");
					ErrExit(eD2D);
				}
			}
			for (int i = 0; i < 5; ++i)
			{
				wchar_t name[50]{ L".\\res\\img\\evils\\2\\r\\" };
				wchar_t add[5]{ L"\0" };
				wsprintf(add, L"%d", i);

				wcscat_s(name, add);
				wcscat_s(name, L".png");

				bmpToughR[i] = Load(name, Draw);
				if (!bmpToughR[i])
				{
					LogErr(L"Error loading bmpToughR !");
					ErrExit(eD2D);
				}
			}


		}




	}











}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	_wsetlocale(LC_ALL, L"");
	bIns = hInstance;
	if (!bIns)ErrExit(eClass);
	else
	{
		int result = 0;
		CheckFile(Ltmp_file, &result);
		if (result == FILE_EXIST)ErrExit(eStarted);
		std::wofstream start{ Ltmp_file };
		start << L"Game started at: " << std::chrono::system_clock::now();
		start.close();
	}

	CreateResources();
	PlaySound(sound_file, NULL, SND_ASYNC | SND_LOOP);







	ClearResources();
	std::remove(tmp_file);

    return (int) bMsg.wParam;
}