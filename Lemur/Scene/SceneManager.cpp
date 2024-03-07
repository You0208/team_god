#include "SceneManager.h"

namespace Lemur::Scene
{
    // 更新処理
    void SceneManager::Update(HWND hwnd, float elapsedTime)
    {
        if (nextScene != nullptr)
        {
            // 古いシーンを終了処理
            Clear();

            // 新しいシーンを設定
            currentScene = nextScene;
            nextScene = nullptr;

            // シーン初期化処理
            if (!currentScene->IsReady())
            {
                currentScene->Initialize();
            }
        }

        if (currentScene != nullptr)
        {
            currentScene->Update(hwnd, elapsedTime);
        }
    }

    // 描画処理
    void SceneManager::Render(float elapsedTime)
    {
        if (currentScene != nullptr)
        {
            currentScene->Render(elapsedTime);
        }
    }

    void SceneManager::Clear()
    {
        if (currentScene != nullptr)
        {
            currentScene->Finalize();
            delete currentScene;
            currentScene = nullptr;
        }
    }

    // シーン切り替え
    void SceneManager::ChangeScene(BaseScene* scene)
    {
        nextScene = scene;
    }
}