/**
 * @file HitStop.cpp
 * @brief ヒットストップ処理を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "HitStop.h"

void HitStop::Start(const float duration)
{
    //ヒットストップがすでにアクティブでない場合のみ処理
    if (!isActive_ && duration != 0.0f)
    {
        //ヒットストップをアクティブにする
        isActive_ = true;
        //残り時間を設定
        remainingTime_ = duration;
        //ゲームタイムスケールを0にしてゲームの進行を一時停止
        GameTimer::SetTimeScale(0.0f);
    }
}

void HitStop::Stop()
{
    //アクティブフラグをリセット
    isActive_ = false;
    //残り時間をリセット
    remainingTime_ = 0.0f;
    //ゲームタイムスケールを通常に戻してゲームの進行を再開
    GameTimer::SetTimeScale(1.0f);
}

void HitStop::Update()
{
    //ヒットストップが有効の場合のみ処理
    if (isActive_)
    {
        //残り時間を減少
        const float kHitStopDeltaTime = 1.0f / 60.0f;
        remainingTime_ -= kHitStopDeltaTime;

        //残り時間が0以下になった場合ヒットストップを終了
        if (remainingTime_ <= 0.0f)
        {
            //ヒットストップを無効にする
            isActive_ = false;
            //残り時間をリセット
            remainingTime_ = 0.0f;
            //ゲームタイムスケールを通常に戻してゲームの進行を再開
            GameTimer::SetTimeScale(1.0f);
        }
    }
}