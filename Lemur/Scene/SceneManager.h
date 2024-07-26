#pragma once

#include "BaseScene.h"

// �V�[���}�l�[�W���[
namespace Lemur::Scene
{
    class SceneManager
    {
    private:
        SceneManager() {}
        ~SceneManager() {}

    public:
        // �B��̃C���X�^���X�擾
        static SceneManager& Instance()
        {
            static SceneManager instance;
            return instance;
        }

        // �X�V����
        void Update(HWND hwnd, float elapsedTime);

        // �`�揈��
        void Render(float elapsedTime);

        // �V�[���N���A
        void Clear();

        // �V�[���̐؂�ւ�
        void ChangeScene(BaseScene* scene);

        void Finalize() {};

        void SetStageNum(int stage_num_) { stage_num = stage_num_; };
    private:
        BaseScene* currentScene = nullptr;
        BaseScene* nextScene = nullptr;

        int stage_num;

    public:
        bool cont_type = false;//true=�R���g���[���[,false=�L�[�{�[�h
        int set_unit_cont[4] = { 0,1,2,3 };
        int save_units[4] = {-1,-1,-1,-1 };
        bool once_tutorial = true;
    };
}