/**
 * @file Application.h
 * @brief アプリケーションを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include <Windows.h>
#include <cstdint>

class Application
{
public:
	//画面の幅
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Application* GetInstance();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ</param>
	/// <param name="wparam">メッセージの追加情報</param>
	/// <param name="lparam">メッセージの追加情報</param>
	/// <returns>メッセージの処理結果</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// ゲームウィンドウを生成
	/// </summary>
	/// <param name="title">タイトルの名前</param>
	/// <param name="clientWidth">画面の横幅</param>
	/// <param name="clientHeight">画面の縦幅</param>
	void CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

	/// <summary>
	/// ゲームウィンドウを閉じる
	/// </summary>
	void CloseGameWindow();

	/// <summary>
	/// メッセージ処理
	/// </summary>
	/// <returns>終了メッセージが来たかどうか</returns>
	bool ProcessMessage();

	//ウィンドウハンドルを取得
	HWND GetHwnd() const { return hwnd_; };

	//インスタンスハンドルを取得
	HINSTANCE GetHInstance() { return wc_.hInstance; };

private:
	Application() = default;
	~Application() = default;
	Application(const Application&) = delete;
	const Application& operator=(const Application&) = delete;

private:
	WNDCLASS wc_{};

	RECT wrc_{};

	HWND hwnd_{};
};

