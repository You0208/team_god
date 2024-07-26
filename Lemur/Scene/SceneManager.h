#pragma once

#include "BaseScene.h"

// シーンマネージャー
namespace Lemur::Scene
{
    class SceneManager
    {
    private:
        SceneManager() {}
        ~SceneManager() {}

    public:
        // 唯一のインスタンス取得
        static SceneManager& Instance()
        {
            static SceneManager instance;
            return instance;
        }

        // 更新処理
        void Update(HWND hwnd, float elapsedTime);

        // 描画処理
        void Render(float elapsedTime);

        // シーンクリア
        void Clear();

        // シーンの切り替え
        void ChangeScene(BaseScene* scene);

        void Finalize() {};

        void SetStageNum(int stage_num_) { stage_num = stage_num_; };
    private:
        BaseScene* currentScene = nullptr;
        BaseScene* nextScene = nullptr;

        int stage_num;

    public:
        bool cont_type = false;//true=コントローラー,false=キーボード
        int set_unit_cont[4] = { 0,1,2,3 };
        int save_units[4] = {-1,-1,-1,-1 };
        bool once_tutorial = true;
    };
}