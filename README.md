# 前走車検出システム（Depth Camera + YOLO）

## 概要
RealsenseデプスカメラとYOLOv11nを用いて、前走車の検出と距離測定を行います。

## 構成
- `main.cpp`: メインの処理コード
- `docs/flowchart.png`: 処理の流れを示したフローチャート
- `samples/image.png`: 実際のカメラ画像

## 使用技術
- C++ (プログラミング言語)
- OpenCV (ライブラリ)
- Intel RealSense SDK (ライブラリ)
- Visual Studio (統合開発環境)
- YOLO（学習済みモデル）
