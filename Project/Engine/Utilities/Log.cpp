/**
 * @file Log.cpp
 * @brief ログ出力や文字列変換をサポートするユーティリティ関数群
 * @author 青木智滉
 * @date
 */

#include "Log.h"

namespace MyUtility
{
    void Log(const std::string& message)
    {
        OutputDebugStringA(message.c_str());
    }

    std::wstring ConvertString(const std::string& str)
    {
        if (str.empty())
        {
            return std::wstring();
        }

        auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
        if (sizeNeeded == 0)
        {
            return std::wstring();
        }
        std::wstring result(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
        return result;
    }

    std::string ConvertString(const std::wstring& str)
    {
        if (str.empty())
        {
            return std::string();
        }

        auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
        if (sizeNeeded == 0)
        {
            return std::string();
        }
        std::string result(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
        return result;
    }
}