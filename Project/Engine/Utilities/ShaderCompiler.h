#pragma once
#include "Log.h"
#include <dxcapi.h>
#include <string>
#include <wrl.h>
/**
 * @file ShaderCompiler.h
 * @brief シェーダーをコンパイルするクラス
 * @author 青木智滉
 * @date
 */

#pragma comment(lib,"dxcompiler.lib")

class ShaderCompiler
{
public:
    //基本のディレクトリパス
    static const std::wstring& kBaseDirectory;

    /// <summary>
    /// 初期化
    /// </summary>
    static void Initialize();

    /// <summary>
    /// シェーダーをコンパイルする
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    /// <param name="profile">設定</param>
    /// <returns>コンパイル済みシェーダーのバイナリデータ</returns>
    static Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

private:
    static Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;

    static Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;

    static Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
};

