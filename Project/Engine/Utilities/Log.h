/**
 * @file Log.h
 * @brief ログ出力や文字列変換をサポートするユーティリティ関数群
 * @author 青木智滉
 * @date
 */

#pragma once
#include <Windows.h>
#include <string>
#include <format>

namespace MyUtility
{
	/// <summary>
	/// デバッグコンソールに文字列を出力
	/// </summary>
	/// <param name="message">出力する文字列</param>
	void Log(const std::string& message);

	/// <summary>
	/// std::string型をstd::wstring型に変換
	/// </summary>
	/// <param name="str">変換する文字列</param>
	/// <returns>std::wstring型に変換した文字列</returns>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// std::wstring型をstd::string型に変換
	/// </summary>
	/// <param name="str">変換する文字列</param>
	/// <returns>std::string型に変換した文字列</returns>
	std::string ConvertString(const std::wstring& str);
}

