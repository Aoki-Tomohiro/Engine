/**
 * @file Input.h
 * @brief 入力処理を管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include <memory>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

class Input
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Input* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// キーが押されているかどうか
	/// </summary>
	/// <param name="keyNum">キーの番号</param>
	/// <returns>キーが押されているか</returns>
	bool IsPushKey(uint8_t keyNum);

	/// <summary>
	/// キーが押された瞬間かどうか
	/// </summary>
	/// <param name="keyNum">キーの番号</param>
	/// <returns>キーが押された瞬間か</returns>
	bool IsPushKeyEnter(uint8_t keyNum);

	/// <summary>
	/// キーが離された瞬間かどうか
	/// </summary>
	/// <param name="keyNum">キーの番号</param>
	/// <returns>キーが離された瞬間か</returns>
	bool IsPushKeyExit(uint8_t keyNum);

	/// <summary>
	/// マウスが押されているかどうか
	/// </summary>
	/// <param name="mouseNum">マウスの番号</param>
	/// <returns>マウスが押されているかどうか</returns>
	bool IsPressMouse(int32_t mouseNum);

	/// <summary>
	/// マウスが押されてた瞬間かどうか
	/// </summary>
	/// <param name="mouseNum">マウスの番号</param>
	/// <returns>マウスが押された瞬間かどうか</returns>
	bool IsPressMouseEnter(int32_t mouseNum);

	/// <summary>
	/// マウスが離された瞬間かどうか
	/// </summary>
	/// <param name="mouseNum">マウスの番号</param>
	/// <returns>マウスが離された瞬間かどうか</returns>
	bool IsPressMouseExit(int32_t mouseNum);

	/// <summary>
	/// マウスホイールの入力値を取得
	/// </summary>
	/// <returns>マウスホイールの入力値</returns>
	int32_t GetWheel();

	/// <summary>
	/// コントローラーが接続されているかどうか
	/// </summary>
	/// <returns>コントローラーが接続されているかどうか</returns>
	bool IsControllerConnected();

	/// <summary>
	/// ボタンが押されているかどうか
	/// </summary>
	/// <param name="button">ボタンの種類</param>
	/// <returns>ボタンが押されているかどうか</returns>
	bool IsPressButton(WORD button);

	/// <summary>
	/// ボタンが押されて瞬間かどうか
	/// </summary>
	/// <param name="button">ボタンの種類</param>
	/// <returns>ボタンが押された瞬間かどうか</returns>
	bool IsPressButtonEnter(WORD button);

	/// <summary>
	/// ボタンが離された瞬間かどうか
	/// </summary>
	/// <param name="button">ボタンの種類</param>
	/// <returns>ボタンが離された瞬間かどうか</returns>
	bool IsPressButtonExit(WORD button);

	/// <summary>
	/// 左スティックの横入力値を取得
	/// </summary>
	/// <returns>左スティックの横入力値</returns>
	float GetLeftStickX();

	/// <summary>
	/// 左スティックの縦入力値を取得
	/// </summary>
	/// <returns>左スティックの縦入力値</returns>
	float GetLeftStickY();

	/// <summary>
	/// 右スティックの横入力値を取得
	/// </summary>
	/// <returns>右スティックの横入力値</returns>
	float GetRightStickX();

	/// <summary>
	/// 右スティックの縦入力値を取得
	/// </summary>
	/// <returns>右スティックの縦入力値</returns>
	float GetRightStickY();

	/// <summary>
	/// 左トリガーの入力値を取得
	/// </summary>
	/// <returns>左トリガーの入力値</returns>
	float GetLeftTriggerValue();

	/// <summary>
	/// 右トリガーの入力値を取得
	/// </summary>
	/// <returns>右トリガーの入力値</returns>
	float GetRightTriggerValue();

private:
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	const Input& operator = (const Input&) = delete;

private:
	static Input* instance_;

	ComPtr<IDirectInput8> directInput_ = nullptr;

	ComPtr<IDirectInputDevice8> keyboardDevice_ = nullptr;

	ComPtr<IDirectInputDevice8> mouseDevice_ = nullptr;

	BYTE key_[256] = {};

	BYTE preKey_[256] = {};

	DIMOUSESTATE mouse_ = {};

	DIMOUSESTATE mousePre_ = {};

	XINPUT_STATE state_{};

	XINPUT_STATE preState_{};
};

