/**
 * @file Audio.h
 * @brief オーディオを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include <array>
#include <vector>
#include <set>
#include <fstream>
#include <wrl.h>
#include <xaudio2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib,"Mf.lib")
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"Mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")

class Audio 
{
private:
	//エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//基本のディレクトリパス
	static const std::string kBaseDirectory;

public:
	//音声データの最大数
	static const int kMaxSoundData = 256;

	//オーディオコールバック
	class XAudio2VoiceCallback : public IXAudio2VoiceCallback 
	{
	public:
		//ボイス処理パスの開始時
		STDMETHOD_(void, OnVoiceProcessingPassStart)
			([[maybe_unused]] THIS_ UINT32 BytesRequired) {
		};
		//ボイス処理パスの終了時
		STDMETHOD_(void, OnVoiceProcessingPassEnd)(THIS) {};
		//バッファストリームの再生が終了した時
		STDMETHOD_(void, OnStreamEnd)(THIS) {};
		//バッファの使用開始時
		STDMETHOD_(void, OnBufferStart)([[maybe_unused]] THIS_ void*) {};
		//バッファの末尾に達した時
		STDMETHOD_(void, OnBufferEnd)(THIS_ void* pBufferContext);
		//再生がループ位置に達した時
		STDMETHOD_(void, OnLoopEnd)([[maybe_unused]] THIS_ void*) {};
		//ボイスの実行エラー時
		STDMETHOD_(void, OnVoiceError)
			([[maybe_unused]] THIS_ void*, [[maybe_unused]] HRESULT) {
		};
	};

	//チャンクヘッダ
	struct ChunkHeader {
		char id[4];//チャンク毎のID
		int32_t size;//チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader {
		ChunkHeader chunk;//"RIFF"
		char type[4];//"WAVE"
	};

	//FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt;//波形フォーマット
	};

	//音声データ
	struct SoundData {
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		std::vector<BYTE> pBuffer;
		//バッファのサイズ
		size_t bufferSize;
		//名前
		std::string name;
		//オーディオハンドル
		uint32_t audioHandle;
	};

	//ボイスデータ
	struct Voice {
		uint32_t handle = 0;
		IXAudio2SourceVoice* sourceVoice = nullptr;
	};

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Audio* GetInstance();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 音声データを読み込み
	/// </summary>
	/// <param name="filename">ファイルの名前</param>
	/// <returns>オーディオハンドル</returns>
	uint32_t LoadAudioFile(const std::string& filename);

	/// <summary>
	/// サウンドデータを解放
	/// </summary>
	/// <param name="soundData">サウンドデータ</param>
	void SoundUnload(SoundData* soundData);

	/// <summary>
	/// 音声データの再生
	/// </summary>
	/// <param name="audioHandle">オーディオハンドル</param>
	/// <param name="loopFlag">ループフラグ</param>
	/// <param name="volume">音声の大きさ</param>
	/// <returns>ボイスハンドル</returns>
	uint32_t PlayAudio(uint32_t audioHandle, bool loopFlag, float volume);

	/// <summary>
	/// 音声データの停止
	/// </summary>
	/// <param name="voiceHandle">ボイスハンドル</param>
	void StopAudio(uint32_t voiceHandle);

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator = (const Audio&) = delete;

private:
	ComPtr<IXAudio2> xAudio2_ = nullptr;

	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	std::array<SoundData, kMaxSoundData> soundDatas_{};

	std::set<Voice*> sourceVoices_{};

	int32_t audioHandle_ = -1;

	int32_t voiceHandle_ = -1;

	XAudio2VoiceCallback voiceCallback_;
};

