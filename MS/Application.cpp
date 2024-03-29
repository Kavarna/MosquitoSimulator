#include "Application.h"


CApplication::CApplication( )
{
}

bool CApplication::InitWindow( HINSTANCE hInstance, bool bFullscreen )
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof( wndClass );
	wndClass.hbrBackground = ( HBRUSH ) ( GetStockObject( LTGRAY_BRUSH ) );
	wndClass.hIcon = ( HICON ) LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
	wndClass.hIconSm = ( HICON ) LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = CApplication::WndProc;
	wndClass.lpszClassName = ENGINE_NAME;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	if ( !RegisterClassEx( &wndClass ) )
	{
		throw std::exception( "Couldn't register the window class" );
		return false;
	}

#if _DEBUG || DEBUG
	m_WindowWidth = 800;
	m_WindowHeight = 600;
#else
	m_WindowWidth = GetSystemMetrics( SM_CXSCREEN );
	m_WindowHeight = GetSystemMetrics( SM_CYSCREEN );
#endif // _DEBUG || DEBUG
	m_hWnd = CreateWindowEx( WS_EX_CLIENTEDGE, ENGINE_NAME, GAME_NAME, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, m_WindowWidth, m_WindowHeight, nullptr, nullptr, hInstance, nullptr );
	if ( !m_hWnd )
	{
		throw std::exception( "Couldn't create window" );
		return false;
	}
	UpdateWindow( m_hWnd );
	ShowWindow( m_hWnd, SW_SHOWNORMAL );
	SetFocus( m_hWnd );
	return true;
}

bool CApplication::Initialize( HINSTANCE hInstance, bool bFullscreen )
{
	if ( !InitWindow( hInstance, bFullscreen ) )
		return false;

	m_Input = new CInput( );
	if ( !m_Input->Initialize( hInstance, m_hWnd ) )
		return false;
	m_Input->addSpecialKey( DIK_ESCAPE );

	m_Graphics = new CGraphics( );
	if ( !m_Graphics->Initialize( m_hWnd, m_WindowWidth, m_WindowHeight, bFullscreen, m_Input ) )
		return false;

	m_Physics = new CPhysics( );
	if ( !m_Physics->Initialize( m_Graphics, m_Input ) )
		return false;

	return true;
}

void CApplication::Run( )
{
	MSG Message;
	m_Timer.StartTimer( );
	while ( true )
	{
		if ( PeekMessage( &Message, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( Message.message == WM_QUIT )
				break;
			TranslateMessage( &Message );
			DispatchMessage( &Message );
		}
		else
		{
			if ( m_Timer.GetTime( ) > 1.0f )
			{
				m_FPS = m_Timer.GetFramesPerSecond( );
				m_Timer.StartTimer( );
			}
			m_Timer.Frame( );
			m_Input->Frame( );
			if ( m_Input->isSpecialKeyPressed( DIK_ESCAPE ) && m_bGameRunning )
			{
				if ( m_bShowMenu )
					break;
				m_bShowMenu = true;
				m_Graphics->BeginScene( );
				m_Physics->Frame( 0.0f );
				m_Graphics->EndScene( false );
				m_Graphics->ApplyBlurToMenuImage( );
			}
			if ( m_bShowMenu )
			{
				m_Graphics->BeginFrame( );
				m_Graphics->RenderMenu( m_Timer.GetFrameTime( ), m_FPS );
				m_Graphics->EndFrame( );
				if ( m_Input->isKeyPressed( DIK_RETURN ) ||
					m_Input->isLeftKeyPressed( ) )
				{
					CGraphics::EMenuOption Option = m_Graphics->GetMenuSelection( );
					if ( Option == CGraphics::EMenuOption::StartGame )
					{
						m_bShowMenu = false;
						m_Timer.Frame( );
					}
					else if ( Option == CGraphics::EMenuOption::QuitGame )
					{
						break;
					}
				}
			}
			else if ( m_bGameRunning )
			{
				m_Graphics->BeginScene( );
				bool bFrameResult = m_Physics->Frame( m_Timer.GetFrameTime( ) );
				m_Graphics->Update( m_Timer.GetFrameTime( ), m_FPS );
				if ( bFrameResult )
					m_Graphics->EndScene( );
				else
				{
					m_Graphics->EndScene( false );
					m_Graphics->ApplyBlurToMenuImage( );
					m_bGameRunning = false;
					m_ReleaseTime = std::chrono::system_clock::now( ) + std::chrono::seconds( SecondsToWait );
					m_Physics->ResetGame( );
				}
			}
			else
			{
				m_Graphics->BeginFrame( );
				m_Graphics->RenderDeathMenu( m_Timer.GetFrameTime( ), m_FPS );
				m_Graphics->EndFrame( );
				std::chrono::system_clock::time_point now = std::chrono::system_clock::now( );
				if ( now > m_ReleaseTime )
				{
					m_bGameRunning = true;
					m_bShowMenu = true;
				}
			}
		}
	}
}

void CApplication::Shutdown( )
{
	DestroyWindow( m_hWnd );
	UnregisterClass( ENGINE_NAME, m_hInstance );
	if ( m_Graphics )
		delete m_Graphics;
	if ( m_Input )
		delete m_Input;
	if ( m_Physics )
		delete m_Physics;
}

CApplication::~CApplication( )
{
	Shutdown( );
}

LRESULT CALLBACK CApplication::WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch ( Message )
	{
	case WM_QUIT:
	DestroyWindow( hWnd );
	break;
	case WM_DESTROY:
	PostQuitMessage( 0 );
	break;
	}
	return DefWindowProc( hWnd, Message, wParam, lParam );
}