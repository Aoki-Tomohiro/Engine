/**
 * @file MathFunction.h
 * @brief 数学関数群
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include <cmath>

namespace Mathf
{
	/// <summary>
	/// 内積を計算
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns>内積</returns>
	float Dot(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// ベクトルの長さを計算
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>ベクトルの長さ</returns>
	float Length(const Vector3& v);

	/// <summary>
	/// 線形補間（float）
	/// </summary>
	/// <param name="v1">開始値</param>
	/// <param name="v2">終了値</param>
	/// <param name="t">補間係数</param>
	/// <returns>線形補間された値</returns>
	float Lerp(float v1, float v2, float t);

	/// <summary>
	/// 角度を短い経路で線形補間する関数
	/// </summary>
	/// <param name="a">開始角度</param>
	/// <param name="b">終了角度</param>
	/// <param name="t">補間係数</param>
	/// <returns>補間された角度</returns>
	float LerpShortAngle(float a, float b, float t);

	/// <summary>
	/// EaseInSineイージングを適用
	/// </summary>
	/// <param name="x">補間係数</param>
	/// <returns>EaseInSineを適用した値</returns>
	float EaseInSine(float x);

	/// <summary>
	/// EaseInCubicイージングを適用
	/// </summary>
	/// <param name="x">補間係数</param>
	/// <returns>EaseInCubicを適用した値</returns>
	float EaseInCubic(float x);

	/// <summary>
	/// EaseOutSineイージングを適用
	/// </summary>
	/// <param name="x">補間係数</param>
	/// <returns>EaseOutSineを適用した値</returns>
	float EaseOutSine(float x);

	/// <summary>
	/// EaseOutCubicイージングを適用
	/// </summary>
	/// <param name="x">補間係数</param>
	/// <returns>EaseOutCubicを適用した値</returns>
	float EaseOutCubic(float x);

	/// <summary>
	/// EaseInOutSineイージングを適用
	/// </summary>
	/// <param name="x">補間係数</param>
	/// <returns>EaseInOutSineを適用した値</returns>
	float EaseInOutSine(float x);

	/// <summary>
	/// EaseOutExpoイージングを適用
	/// </summary>
	/// <param name="x">補間係数</param>
	/// <returns>EaseOutExpoを適用した値</returns>
	float EaseOutExpo(float x);

	/// <summary>
	/// クォータニオンの大きさを計算
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	/// <returns>クォータニオンの大きさ</returns>
	float Norm(const Quaternion& quaternion);

	/// <summary>
	/// 線形補間（Vector2）
	/// </summary>
	/// <param name="v1">開始値</param>
	/// <param name="v2">終了値</param>
	/// <param name="t">補間係数</param>
	/// <returns>線形補間された値</returns>
	Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);

	/// <summary>
	/// ベクトルを正規化
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>正規化されたベクトル</returns>
	Vector3 Normalize(const Vector3& v);

	/// <summary>
	/// 外積を計算
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns>外積</returns>
	Vector3 Cross(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// ベクトルを変換
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <param name="m">行列</param>
	/// <returns>変換されたベクトル</returns>
	Vector3 Transform(const Vector3& v, const Matrix4x4& m);

	/// <summary>
	/// ベクトルを変換
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <param name="m">行列</param>
	/// <returns>変換されたベクトル</returns>
	Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

	/// <summary>
	/// 線形補間（Vector3）
	/// </summary>
	/// <param name="v1">開始値</param>
	/// <param name="v2">終了値</param>
	/// <param name="t">補間係数</param>
	/// <returns>線形補間された値</returns>
	Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="v1">開始値</param>
	/// <param name="v2">終了値</param>
	/// <param name="t">補間係数</param>
	/// <returns>線形補間された値</returns>
	Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

	/// <summary>
	/// Catmull-Rom曲線を計算
	/// </summary>
	/// <param name="p0">制御点1</param>
	/// <param name="p1">制御点2</param>
	/// <param name="p2">制御点3</param>
	/// <param name="p3">制御点4</param>
	/// <param name="t">補間係数</param>
	/// <returns>Catmull-Rom曲線を適用した値</returns>
	Vector3 CatmullRomSpline(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float t);

	/// <summary>
	/// ベクトルをクォータニオンで回転
	/// </summary>
	/// <param name="vector">ベクトル</param>
	/// <param name="quaternion">クォータニオン</param>
	/// <returns>回転されたベクトル</returns>
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

	/// <summary>
	/// 逆行列を計算
	/// </summary>
	/// <param name="m">行列</param>
	/// <returns>逆行列</returns>
	Matrix4x4 Inverse(const Matrix4x4& m);

	/// <summary>
	/// 行列の転置を計算
	/// </summary>
	/// <param name="m">行列</param>
	/// <returns>転置された行列</returns>
	Matrix4x4 Transpose(const Matrix4x4& m);

	/// <summary>
	/// 単位行列を作成
	/// </summary>
	/// <returns>単位行列</returns>
	Matrix4x4 MakeIdentity4x4();

	/// <summary>
	/// スケーリング行列を作成
	/// </summary>
	/// <param name="scale">スケール</param>
	/// <returns>スケーリング行列</returns>
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	/// <summary>
	/// 平行移動行列を作成
	/// </summary>
	/// <param name="translate">ベクトル</param>
	/// <returns>平行移動行列</returns>
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

	/// <summary>
	/// X軸の回転行列を作成
	/// </summary>
	/// <param name="radian">角度</param>
	/// <returns>X軸の回転行列</returns>
	Matrix4x4 MakeRotateXMatrix(float radian);

	/// <summary>
	/// Y軸の回転行列を作成
	/// </summary>
	/// <param name="radian">角度</param>
	/// <returns>Y軸の回転行列</returns>
	Matrix4x4 MakeRotateYMatrix(float radian);

	/// <summary>
	/// Z軸の回転行列を作成
	/// </summary>
	/// <param name="radian">角度</param>
	/// <returns>Z軸の回転行列</returns>
	Matrix4x4 MakeRotateZMatrix(float radian);

	/// <summary>
	/// 回転行列を作成
	/// </summary>
	/// <param name="rotate">角度</param>
	/// <returns>回転行列</returns>
	Matrix4x4 MakeRotateMatrix(const Vector3& rotate);

	/// <summary>
	/// アフィン行列を作成
	/// </summary>
	/// <param name="scale">スケール</param>
	/// <param name="rotate">回転（オイラー角）</param>
	/// <param name="translate">座標</param>
	/// <returns>アフィン行列</returns>
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	/// <summary>
	/// アフィン行列を作成
	/// </summary>
	/// <param name="scale">スケール</param>
	/// <param name="rotate">回転（クォータニオン）</param>
	/// <param name="translate">座標</param>
	/// <returns>アフィン行列</returns>
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& quaternion, const Vector3& translation);

	/// <summary>
	/// 透視投影行列を作成
	/// </summary>
	/// <param name="fovY">垂直方向の視野角</param>
	/// <param name="aspectRatio">アスペクト比</param>
	/// <param name="nearClip">近距離クリップ面の距離</param>
	/// <param name="farClip">遠距離クリップ面の距離</param>
	/// <returns>透視投影行列</returns>
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	/// <summary>
	/// 正射影行列を作成
	/// </summary>
	/// <param name="left">左クリップ面の座標</param>
	/// <param name="top">上クリップ面の座標</param>
	/// <param name="right">右クリップ面の座標</param>
	/// <param name="bottom">下クリップ面の座標</param>
	/// <param name="nearClip">近距離クリップ面の距離</param>
	/// <param name="farClip">遠距離クリップ面の距離</param>
	/// <returns>正射影行列</returns>
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

	/// <summary>
	/// ビューポート行列を作成
	/// </summary>
	/// <param name="left">ビューポートの左端座標</param>
	/// <param name="top">ビューポートの上端座標</param>
	/// <param name="width">ビューポートの幅</param>
	/// <param name="height">ビューポートの高さ</param>
	/// <param name="minDepth">ビューポートの最小深度</param>
	/// <param name="maxDepth">ビューポートの最大深度</param>
	/// <returns>ビューポート行列</returns>
	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

	/// <summary>
	/// 2つの方向ベクトルを使って、方向変換行列を計算
	/// </summary>
	/// <param name="from">変換元の方向ベクトル</param>
	/// <param name="to">変換先の方向ベクトル</param>
	/// <returns>方向変換を表す行列</returns>
	Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

	/// <summary>
	/// 回転軸と回転角から回転行列を作成
	/// </summary>
	/// <param name="axis">回転軸となるベクトル</param>
	/// <param name="angle">回転角度</param>
	/// <returns>回転行列</returns>
	Matrix4x4 MakeRotateAxisAngle(Vector3 axis, float angle);

	/// <summary>
	/// 回転行列を作成（クォータニオン）
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	/// <returns>回転行列</returns>
	Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

	/// <summary>
	/// 単位クォータニオンを作成
	/// </summary>
	/// <returns>単位クォータニオン</returns>
	Quaternion IdentityQuaternion();

	/// <summary>
	/// クォータニオンの共役を計算
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	/// <returns>クォータニオンの共役</returns>
	Quaternion Conjugate(const Quaternion& quaternion);

	/// <summary>
	/// クォータニオンを正規化
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	/// <returns>正規化されたクォータニオン</returns>
	Quaternion Normalize(const Quaternion& quaternion);

	/// <summary>
	/// 逆クォータニオンを計算
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	/// <returns>逆クォータニオン</returns>
	Quaternion Inverse(const Quaternion& quaternion);

	/// <summary>
	/// 回転軸と回転角からクォータニオンを作成
	/// </summary>
	/// <param name="axis">回転軸となるベクトル</param>
	/// <param name="angle">回転角度</param>
	/// <returns>クォータニオン</returns>
	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

	/// <summary>
	/// 行列からクォータニオンを計算
	/// </summary>
	/// <param name="m">行列</param>
	/// <returns>クォータニオン</returns>
	Quaternion GetRotation(const Matrix4x4& m);

	/// <summary>
	/// 位置からターゲットに向かって回転を求めるクォータニオンを計算
	/// </summary>
	/// <param name="position">回転の起点となる位置</param>
	/// <param name="target">回転の目標となるターゲット位置</param>
	/// <returns>クォータニオン</returns>
	Quaternion LookAt(const Vector3& position, const Vector3& target);

	/// <summary>
	/// 前方ベクトルと上方向ベクトルに基づいてクォータニオンを計算
	/// </summary>
	/// <param name="forward">ターゲットの方向</param>
	/// <param name="up">ワールド空間での上方向ベクトル</param>
	/// <returns>クォータニオン</returns>
	Quaternion LookRotation(const Vector3& forward, const Vector3& up);

	/// <summary>
	/// 球面線形補間（クォータニオン）
	/// </summary>
	/// <param name="q0">クォータニオン1</param>
	/// <param name="q1">クォータニオン2</param>
	/// <param name="t">補間係数</param>
	/// <returns>補間されたクォータニオン</returns>
	Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
}

