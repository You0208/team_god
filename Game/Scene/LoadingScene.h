#pragma once
#include "Lemur/Scene/BaseScene.h"
#include <thread>


class LoadingScene :
    public Lemur::Scene::BaseScene
{
public:
    LoadingScene(Lemur::Scene::BaseScene* next_scene) :nextScene(next_scene) {}
    LoadingScene() {}
    ~LoadingScene() override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

private:
    // ローディングスレッド
    static void LoadingThread(LoadingScene* scene);

private:
    BaseScene* nextScene = nullptr;
    std::thread* thread = nullptr;
    std::unique_ptr<Sprite> spr_back;
    std::unique_ptr<Sprite> loading_text;
    std::unique_ptr<Sprite> point_1_text;
    std::unique_ptr<Sprite> point_2_text;
    std::unique_ptr<Sprite> point_3_text;

    DirectX::XMFLOAT2 poo;

    bool once_only = true;
    bool enable_posint[3] = {};
    float time;
    float time_interval;
};