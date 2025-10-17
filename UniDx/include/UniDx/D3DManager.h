﻿#pragma once

// Direct3Dのライブラリを使用できるようにする
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3dcompiler.h>

#include "UniDxDefine.h"
#include "Singleton.h"


constexpr UINT UNIDX_PS_SLOT_LIGHTS = 0;  // t0
constexpr UINT UNIDX_PS_SLOT_LIGHT_INDICES = 1;  // t1
constexpr UINT UNIDX_PS_SLOT_ALBEDO = 4;  // t4
constexpr UINT UNIDX_PS_SLOT_NORMAL = 5;  // t5


namespace UniDx{

// ----------------------------------------------------------
// D3DManagerクラス
// ----------------------------------------------------------
class D3DManager : public Singleton<D3DManager>
{
public:

	//--------------------------------------------
	// Direct3Dを初期化し、使用できるようにする
	// hWnd		: ウィンドウハンドル
	// width	: 画面の幅
	// height	: 画面の高さ
	//--------------------------------------------
	bool Initialize(HWND hWnd, int width, int height);

	const ComPtr<ID3D11Device>&			GetDevice() const { return m_device; }
	const ComPtr<ID3D11DeviceContext>&	GetContext() const { return m_context; }

	// バックバッファレンダーターゲットをクリア
	void Clear(float r, float g, float b, float a);

	// バックバッファの内容を画面に表示
	void Present()
	{
		m_swapChain->Present(1, 0);
	}

	const Vector2& getScreenSize() const { return screenSize; }

private:
	Vector2                         screenSize;
	ComPtr<ID3D11Device>			m_device; // Direct3Dデバイス
	ComPtr<ID3D11DeviceContext>		m_context; // Direct3Dデバイスコンテキスト
	ComPtr<IDXGISwapChain>			m_swapChain; // スワップチェイン
	ComPtr<ID3D11RenderTargetView>	m_renderTarget; // バックバッファーのRTビュー

	ComPtr<ID3D11Texture2D> m_depthStencilBuffer; // デプス&ステンシルバッファ
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;
};

} // UniDx
