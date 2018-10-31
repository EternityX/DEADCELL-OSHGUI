#include <windows.h>
#include <d3dx9.h>
#undef MessageBox
#undef DrawText
//---------------------------------------------------------------------------
//the basic includes for OSHGui
#include <OSHGui.hpp>
#include "Input/WindowsMessage.hpp"
#include "Drawing/Direct3D9/Direct3D9Renderer.hpp"
//---------------------------------------------------------------------------
//optional: make the namespace available
using namespace OSHGui;
using namespace OSHGui::Drawing;
//---------------------------------------------------------------------------
Input::WindowsMessage input;
//-----------------------------------
void InitializeOSHGui(LPDIRECT3DDEVICE9 device)
{
	//1. create our renderer, Direct3D9 in this case
	auto renderer = std::unique_ptr<Direct3D9Renderer>(new Direct3D9Renderer(device));
	
	//2. and initialize the OSHGui with it
	Application::Initialize(std::move(renderer));

	//or 1.+2.
	//Application::Initialize(std::unique_ptr<Direct3D9Renderer>(new Direct3D9Renderer(device)));

	//now we have a valid OSHGui instance, so lets grab it
	auto &app = Application::Instance();

	//3. create a font which will be used by the OSHGui
	auto font = FontManager::LoadFont("Arial", 8.0f, false); //Arial, 8PT, no anti-aliasing
	app.SetDefaultFont(font);

	//4. create our form
	auto form = std::make_shared<Form>();
	form->SetText("Test");
	form->SetLocation( PointI( 6, 10 ) );
	form->SetSize( SizeI( 600, 400 ) );

	CheckBox* check = new CheckBox( );
	check->SetText( "Checkbox" );
	check->SetLocation( PointI( 14, 15 ) );
	check->SetFont( font );

	TrackBar* track = new TrackBar( );
	track->SetPrecision( 1 );
	track->SetLocation( PointI( 30, 40 ) );
	track->SetText( "Slider" );
	track->SetMinimum( 0 );
	track->SetMaximum( 180 );

	ComboBox* combo = new ComboBox( );
	combo->SetLocation( PointI( 34, 65 ) );
	combo->SetFont( font );
	combo->SetMaxShowItems( 6 );
	combo->AddItem( "Item one" );
	combo->AddItem( "Item two" );
	combo->AddItem( "Item three" );
	combo->AddItem( "Item four" );
	combo->AddItem( "Item five" );
	combo->AddItem( "Item six" );
	combo->AddItem( "Item seven" );
	combo->AddItem( "Item eight" );
	combo->AddItem( "Item nine" );

	GroupBox* groupbox = new GroupBox( );
	groupbox->SetLocation( PointI( 17, 12 ) );
	groupbox->SetText( "Groupbox" );
	groupbox->SetSize( 234, 322 );

	HotkeyControl* hotkey = new HotkeyControl( );
	hotkey->SetLocation( 34, 95 );
	hotkey->SetFont( font );

	TextBox* textbox = new TextBox( );
	textbox->SetLocation( 34, 125 );
	textbox->SetFont( font );
	textbox->SetForeColor( Color::FromARGB( 255, 201, 201, 201 ) );

	Button* button = new Button( );
	button->SetLocation( 34, 155 );
	button->SetText( "Button" );
	button->SetFont( font );
	button->SetForeColor( Color::FromARGB( 255, 201, 201, 201 ) );

	TabPage* page = new TabPage( );
	page->SetText( "Legitbot" );

	TabPage* page2 = new TabPage( );
	page2->SetText( "Ragebot" );

	TabPage* page3 = new TabPage( );
	page3->SetText( "Visuals" );

	TabPage* page4 = new TabPage( );
	page4->SetText( "Miscellaneous" );

	TabPage* page5 = new TabPage( );
	page5->SetText( "Config" );

	TabControl* tab = new TabControl( );
	tab->SetSize( 576, 380 );
	tab->SetLocation( PointI( 6, -15 ) );
	tab->AddTabPage( page );
	tab->AddTabPage( page2 );
	tab->AddTabPage( page3 );
	tab->AddTabPage( page4 );
	tab->AddTabPage( page5 );
	tab->SetBackColor( Color::FromARGB( 255, 32, 32, 32 ) );

	page->AddControl( groupbox );
	groupbox->AddControl( check );
	groupbox->AddControl( track );
	groupbox->AddControl( combo );
	groupbox->AddControl( hotkey );
	groupbox->AddControl( textbox );
	groupbox->AddControl( button );

	form->AddControl( tab );

	//5. set this form as our mainform
	app.Run(form);

	//optional: enable the OSHGui drawing
	app.Enable();

	//optional: register a Hotkey with which we can toggle the OSHGui drawing
	app.RegisterHotkey(Hotkey(Key::Insert, []
	{
		Application::Instance().Toggle();
	}));
}
//---------------------------------------------------------------------------
void Render(LPDIRECT3DDEVICE9 device)
{
	device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	device->BeginScene();

	auto &renderer = Application::Instance().GetRenderer();

	//1. let our renderer begin its work
	renderer.BeginRendering();

	//2. render the OSHGui
	Application::Instance().Render();

	//3. end the rendering
	renderer.EndRendering();

	device->EndScene();
	device->Present(0, 0, 0, 0);
}
//---------------------------------------------------------------------------
//wrapper to create a win32 window
//---------------------------------------------------------------------------
class Win32Window
{
public:
	Win32Window(HINSTANCE instance)
	{
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_CLASSDC;
		wc.lpfnWndProc = DefWindowProc;
		wc.hInstance = instance;
		wc.lpszClassName = "D3D";
		wc.hCursor = LoadCursor(0, IDC_ARROW);

		RegisterClass(&wc);

		RECT windowSize = { 0, 0, 1280, 720 };
		AdjustWindowRect(&windowSize, WS_OVERLAPPEDWINDOW, false);
		windowSize.right = std::abs(windowSize.right) + std::abs(windowSize.left);
		windowSize.bottom = std::abs(windowSize.bottom) + std::abs(windowSize.top);

		Window = CreateWindowA("D3D", "D3D", WS_OVERLAPPEDWINDOW, 0, 100, windowSize.right, windowSize.bottom, GetDesktopWindow(), 0, wc.hInstance, 0);
	}
	~Win32Window()
	{
		if (wc.hInstance)
		{
			UnregisterClass("D3D", wc.hInstance);
		}
	}

	HWND Window;

private:
	WNDCLASS wc;
};
//---------------------------------------------------------------------------
//wrapper to setup D3D9
//---------------------------------------------------------------------------
class Direct3DDevice
{
public:
	Direct3DDevice(HWND hwnd)
		: D3D(nullptr),
		Device(nullptr)
	{
		if (!(D3D = Direct3DCreate9(D3D_SDK_VERSION)))
		{
			throw std::runtime_error("can't create Direct3D9");
		}

		RECT rect;
		GetClientRect(hwnd, &rect);

		D3DPRESENT_PARAMETERS pp;
		ZeroMemory(&pp, sizeof(pp));
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pp.BackBufferFormat = D3DFMT_X8R8G8B8;
		pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		pp.Windowed = TRUE;
		pp.BackBufferWidth = rect.right;
		pp.BackBufferHeight = rect.bottom;

		if (FAILED(D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &Device)))
		{
			D3D->Release();

			throw std::runtime_error("can't create device");
		}

		D3DXMATRIX projection;
		D3DXMatrixPerspectiveFovLH(&projection, D3DXToRadian(45.0f), 584.0f / 562.0f, 1.0f, 100.0f);
		Device->SetTransform(D3DTS_PROJECTION, &projection);
	}
	~Direct3DDevice()
	{
		if (Device)
		{
			Device->Release();
		}
		if (D3D)
		{
			D3D->Release();
		}
	}

	LPDIRECT3DDEVICE9 Device;

private:
	LPDIRECT3D9 D3D;
};
//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Win32Window window(hInstance);

	{
		Direct3DDevice device(window.Window);

		InitializeOSHGui(device.Device);

		ShowWindow(window.Window, SW_SHOWDEFAULT);
		UpdateWindow(window.Window);

		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (true)
		{
			if (!IsWindowVisible(window.Window))
			{
				break;
			}

			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				//let the OSHGui handle the input
				if (!input.ProcessMessage(&msg))
				{
					if (msg.message == WM_QUIT)
					{
						break;
					}

					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				Render(device.Device);
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------