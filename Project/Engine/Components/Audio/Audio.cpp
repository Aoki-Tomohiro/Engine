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

//uint32_t Audio::SoundLoadWave(const std::string& filename)
//{
//	//同じ音声データがないか探す
//	for (SoundData& soundData : soundDatas_) 
//	{
//		if (soundData.name == filename) 
//		{
//			return soundData.audioHandle;
//		}
//	}
//
//	audioHandle_++;
//	//ファイル入力ストリームのインスタンス
//	std::ifstream file;
//	std::string filePath = kBaseDirectory + "/" + filename;
//	if (filename.find("Application/Resources/Sounds") != std::string::npos)
//	{
//		filePath = filename;
//	}
//	//.wavファイルをバイナリモードで開く
//	file.open(filePath, std::ios_base::binary);
//	//ファイルオープン失敗を検出する
//	assert(file.is_open());
//
//	//RIFFヘッダーの読み込み
//	RiffHeader riff{};
//	file.read((char*)&riff, sizeof(riff));
//	//ファイルがRIFFかチェック
//	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
//	{
//		assert(0);
//	}
//	//タイプがWAVEかチェック
//	if (strncmp(riff.type, "WAVE", 4) != 0) 
//	{
//		assert(0);
//	}
//
//	//Formatチャンクの読み込み
//	FormatChunk format = {};
//	//チャンクヘッダーの確認
//	file.read((char*)&format, sizeof(ChunkHeader));
//	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
//	{
//		assert(0);
//	}
//
//	//チャンク本体の読み込み
//	assert(format.chunk.size <= sizeof(format.fmt));
//	file.read((char*)&format.fmt, format.chunk.size);
//
//	//Dataチャンクの読み込み
//	ChunkHeader data{};
//	file.read((char*)&data, sizeof(data));
//	//JUNKチャンクを検出した場合
//	if (strncmp(data.id, "JUNK", 4) == 0) 
//	{
//		//読み取り位置をJUNKチャンクの終わりまで進める
//		file.seekg(data.size, std::ios_base::cur);
//		//再読み込み
//		file.read((char*)&data, sizeof(data));
//	}
//
//	//LISTチャンクを検出した場合
//	if (strncmp(data.id, "LIST", 4) == 0) 
//	{
//		//読み取り位置をLISTチャンクの終わりまで進める
//		file.seekg(data.size, std::ios_base::cur);
//		//再読み込み
//		file.read((char*)&data, sizeof(data));
//	}
//
//	if (strncmp(data.id, "data", 4) != 0)
//	{
//		assert(0);
//	}
//
//	//Dataチャンクのデータ部(波形データ)の読み込み
//	char* pBuffer = new char[data.size];
//	file.read(pBuffer, data.size);
//
//	//Waveファイルを閉じる
//	file.close();
//
//	//returnする為の音声データ
//	soundDatas_[audioHandle_].wfex = format.fmt;
//	soundDatas_[audioHandle_].pBuffer = reinterpret_cast<BYTE*>(pBuffer);
//	soundDatas_[audioHandle_].bufferSize = data.size;
//	soundDatas_[audioHandle_].name = filename;
//	soundDatas_[audioHandle_].audioHandle = audioHandle_;
//
//	return audioHandle_;
//}

void Audio::SoundUnload(SoundData* soundData)
{
	//バッファのメモリを解放
	//delete[] soundData->pBuffer;
	soundData->pBuffer.clear();
	//soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

uint32_t Audio::SoundPlayWave(uint32_t audioHandle, bool loopFlag, float volume)
{
	HRESULT result;
	//voiceHandleをインクリメント
	voiceHandle_++;

	//波形フォーマットを元にSourceVoiceの作成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatas_[audioHandle].wfex);
	assert(SUCCEEDED(result));

	//コンテナに追加
	Voice* voice = new Voice();
	voice->handle = audioHandle;
	voice->sourceVoice = pSourceVoice;
	sourceVoices_.insert(voice);

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[audioHandle].pBuffer.data();
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
	HRESULT result;
	auto it = sourceVoices_.begin();
	while (it != sourceVoices_.end()) 
	{
		if ((*it)->handle == voiceHandle)
		{
			result = (*it)->sourceVoice->Stop();
			delete (*it);
			it = sourceVoices_.erase(it);
		}
		else 
		{
			++it;
		}
	}
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

	audioHandle_++;
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
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
	pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

	WAVEFORMATEX* waveFormat{ nullptr };
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

	std::vector<BYTE> mediaData;
	while (true)
	{
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

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

	soundDatas_[audioHandle_].wfex = *waveFormat;
	soundDatas_[audioHandle_].pBuffer = mediaData;
	soundDatas_[audioHandle_].bufferSize = mediaData.size();
	soundDatas_[audioHandle_].name = filePath;
	soundDatas_[audioHandle_].audioHandle = audioHandle_;
	CoTaskMemFree(waveFormat);
	pMFMediaType->Release();
	pMFSourceReader->Release();

	return audioHandle_;
}