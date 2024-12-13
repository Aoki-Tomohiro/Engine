/**
 * @file Audio.cpp
 * @brief オーディオを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "Audio.h"
#include "Engine/Utilities/Log.h"
#include <cassert>

const std::string Audio::kBaseDirectory = "Application/Resources/Sounds";

Audio* Audio::GetInstance()
{
	static Audio instance;
	return &instance;
}

void Audio::Finalize()
{
	//ボイスデータ開放
	for (const Voice* voice : sourceVoices_)
	{
		if (voice->sourceVoice != nullptr)
		{
			voice->sourceVoice->DestroyVoice();
		}
		delete voice;
	}
	sourceVoices_.clear();

	//XAudio2解放
	xAudio2_.Reset();
	//音声データ開放
	for (int i = 0; i < soundDatas_.size(); i++)
	{
		SoundUnload(&soundDatas_[i]);
	}

	//MediaFoundationの終了
	MFShutdown();
}

void Audio::Initialize()
{
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	HRESULT result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

uint32_t Audio::LoadAudioFile(const std::string& filename)
{
	//同じ音声データがないか探す
	for (SoundData& soundData : soundDatas_)
	{
		if (soundData.name == filename)
		{
			return soundData.audioHandle;
		}
	}

	//ファイルパスの設定
	std::string filePath = kBaseDirectory + "/" + filename;
	if (filename.find("Application/Resources/Sounds") != std::string::npos)
	{
		filePath = filename;
	}

	IMFSourceReader* pMFSourceReader{ nullptr };
	MFCreateSourceReaderFromURL(MyUtility::ConvertString(filePath).c_str(), NULL, &pMFSourceReader);

	IMFMediaType* pMFMediaType{ nullptr };
	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	pMFSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

	WAVEFORMATEX* waveFormat{ nullptr };
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

	std::vector<BYTE> mediaData;
	while (true)
	{
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		pMFSourceReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer{ nullptr };
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	audioHandle_++;
	soundDatas_[audioHandle_].wfex = *waveFormat;
	soundDatas_[audioHandle_].pBuffer = mediaData;
	soundDatas_[audioHandle_].bufferSize = mediaData.size();
	soundDatas_[audioHandle_].name = filename;
	soundDatas_[audioHandle_].audioHandle = audioHandle_;
	CoTaskMemFree(waveFormat);
	pMFMediaType->Release();
	pMFSourceReader->Release();

	return audioHandle_;
}

void Audio::SoundUnload(SoundData* soundData)
{
	//バッファのメモリを解放
	soundData->pBuffer.clear();
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

uint32_t Audio::PlayAudio(uint32_t audioHandle, bool loopFlag, float volume)
{
	//voiceHandleをインクリメント
	voiceHandle_++;

	//波形フォーマットを元にSourceVoiceの作成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatas_[audioHandle].wfex, 0, 1.0f, &voiceCallback_);
	assert(SUCCEEDED(result));

	//コンテナに追加
	Voice* voice = new Voice();
	voice->handle = voiceHandle_;
	voice->sourceVoice = pSourceVoice;
	sourceVoices_.insert(voice);

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[audioHandle].pBuffer.data();
	buf.pContext = voice;
	buf.AudioBytes = UINT32(soundDatas_[audioHandle].bufferSize);
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (loopFlag)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	pSourceVoice->SetVolume(volume);
	result = pSourceVoice->Start();

	return voiceHandle_;
}

void Audio::StopAudio(uint32_t voiceHandle)
{
	//sourceVoices_からvoiceHandleに一致するVoiceを検索
	for (auto it = sourceVoices_.begin(); it != sourceVoices_.end(); ++it)
	{
		//現在のVoiceポインタを取得
		Voice* voice = *it;

		//Voiceのhandleが引数のvoiceHandleと一致するか確認
		if (voice->handle == voiceHandle)
		{
			//一致した場合、音声を停止
			if (voice->sourceVoice)
			{
				voice->sourceVoice->Stop();
			}

			//ソースボイスを削除
			voice->sourceVoice->DestroyVoice();

			//停止したVoiceをsourceVoices_から削除
			sourceVoices_.erase(it);

			//解放
			delete voice;

			//ループを抜ける
			break;
		}
	}
}

STDMETHODIMP_(void __stdcall) Audio::XAudio2VoiceCallback::OnBufferEnd(void* pBufferContext)
{
	//Voiceポインタを取得
	Voice* voice = static_cast<Voice*>(pBufferContext);

	//オーディオのインスタンスを取得し音声を止める
	Audio::GetInstance()->StopAudio(voice->handle);
}