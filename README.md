[![DebugBuild](https://github.com/Aoki-Tomohiro/Engine/actions/workflows/DebugBuild.yml/badge.svg)](https://github.com/Aoki-Tomohiro/Engine/actions/workflows/DebugBuild.yml)
[![ReleaseBuild](https://github.com/Aoki-Tomohiro/Engine/actions/workflows/ReleaseBuild.yml/badge.svg)](https://github.com/Aoki-Tomohiro/Engine/actions/workflows/ReleaseBuild.yml)

# アプリケーション

## ゲームタイトル
### 『ファンタズム』

## 概要
このゲームは、プレイヤーがボスと1対1で戦う3Dアクションゲームです。FF16やDMC5を参考にしており、**操作感が良く、遊んでいて気持ちいいと感じられるゲーム**を目指して制作しています。

## 制作環境
- **ジャンル**: 3D近接アクション
- **製作期間**: 2024年1月～現在
- **開発人数**: 1人
- **開発環境**: DirectX12
- **担当箇所**: 全て

<div align="center">
  <img src="https://github.com/user-attachments/assets/e4d74bba-67ad-4ce8-8b0d-91eb06f55ea5" alt="ゲーム画像">
</div>

## こだわっているところ
プレイヤーが**何度も遊びたくなるような気持ちの良いアクションや演出**を目指して制作しています。特に、**スピード感のあるコンボ**や、スキル発動時や落下攻撃時の**カメラワーク**に力を入れています。
また、自作の**エディター**を用いて、実装したいアクションやカメラワークを**効率的に制作**できるようにしています。

### モーションエディター  
このエディターでは、1つのアニメーションを**複数の状態**に分けて、**移動速度**、**攻撃のヒット間隔**、**ダメージ**を調整できるようにしています。これにより、実装したいアクションを**効率的**に作成できるようになりました。  
また、**ノックバックの速度**や**ヒットストップの持続時間**、**カメラシェイクの強さ**も調整可能で、コンボのつながりやアクションの感触を細かく調整できるようになっています。

### カメラアニメーションエディター
このエディターでは、**キーフレームの追加や削除**が可能で、線形補完を用いたカメラアニメーションを実装しています。
回転はQuaternionで管理し、キーフレームごとに**座標**、**回転**、**FOV**を設定できます。
また、ImGuiの「Play Animation」ボタンを押すことで、**現在選択されているアニメーションをすぐに確認**できるようになっています。

## コンボ
<div align="center">
  <img src="https://github.com/user-attachments/assets/155faad7-7a81-4979-8bd5-9526401cd2fd" alt="コンボGIF">
</div>

---

# エンジン

## 機能

### 2D
- スプライト描画

### 3D
- 3Dモデルの読み込みと描画
- アニメーションの読み込みと再生
- ライト: DirectionalLight、PointLight、 SpotLight

### 入力
- キーボードとXboxコントローラー対応

### 音声
- WAV、MP3の音声データの読み込みと再生

### 衝突管理
- ゲームオブジェクトの衝突判定をまとめて処理
- 形状の種類: Sphere、AABB、OBB

### パーティクルシステム
- モデル、テクスチャの設定
- エミッターの追加・削除
- 加速フィールド、重力フィールドの追加・削除

### ポストエフェクト
- Bloom
- DoF
- Fog
- Lens Distortion
- Outline
- Radial Blur
- Vignette

## 外部ライブラリ
- [DirectXTex](https://github.com/microsoft/DirectXTex)
- [imgui](https://github.com/ocornut/imgui)
- [assimp](https://github.com/assimp/assimp)
- [nlohmann/json](https://github.com/nlohmann/json)
