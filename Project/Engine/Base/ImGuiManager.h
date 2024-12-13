/**
 * @file ImGuiManager.h
 * @brief ImGuiを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/Application.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/DescriptorHeap.h"
#include "Engine/Externals/imgui/imgui.h"
#include <memory>

class ImGuiManager 
{
public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static ImGuiManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了処理
	/// </summary>
	void ShutDown();

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;

private:
	Application* application_ = nullptr;

	GraphicsCore* graphicsCore_ = nullptr;

	std::unique_ptr<DescriptorHeap> srvDescriptorHeap_ = nullptr;
};

