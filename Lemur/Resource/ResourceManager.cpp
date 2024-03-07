#include "ResourceManager.h"
#include <locale>
#include <string>

const std::shared_ptr<SkinnedMesh> ResourceManager::LoadModelResource(
    ID3D11Device* device,
    const char* const       fbx_filename,
    const bool              triangulate,
    const float             sampling_rate)
{
    // ���f��������
    const ModelMap::iterator it = models.find(fbx_filename);
    if (it != models.end())
    {
        // �����N(����)���؂�Ă��Ȃ����m�F
        if (it->second.expired() == false)
        {
            // �����������f���Ɠ������f�������������炻���Ԃ�
            return it->second.lock();
        }
    }

    // ������Ȃ�������V�K���f�����\�[�X���쐬�E�ǂݍ���
    const std::shared_ptr<SkinnedMesh> model = {
        std::make_shared<SkinnedMesh>(
            device,
            fbx_filename,
            triangulate,
            sampling_rate)
    };

    // �}�b�v�ɓo�^
    models[fbx_filename] = model;

    return model;
}

const std::shared_ptr<Sprite> ResourceManager::load_sprite_resource(ID3D11Device* device, const wchar_t* const filename)
{
    // ���̕�������g�p���đ��̃^�C�v�̕�������쐬���܂��B
    const wchar_t* orig = filename;

    // wchar_t ������� char* ������ɕϊ����܂��B �L�^
    // ���̕�����̒����� 1 �������܂��B
    // �I�[�� null �������l�����܂��B
    size_t origsize = wcslen(orig) + 1;
    size_t convertedChars = 0;

    // �}���`�o�C�g��������g�p���ĕ�����̎�ނ�ǉ����܂�
    // ���ʂ�\������O�ɐV����������ɕϊ����܂�
    char strConcat[] = " (char *)";
    size_t strConcatsize = (strlen(strConcat) + 1) * 2;

    // �}���`�o�C�g�o�͕�����Ƀ��C�h���Ƃ� 2 �o�C�g�����蓖�Ă܂��B
    // ���͕�������̕��� (���C�h�������܂�)
    // �k���j�B �}���`�o�C�g������ 1 �o�C�g�܂��� 2 �o�C�g�ł��邽�߁A
    // �e������ 2 �o�C�g�����蓖�Ă�K�v������܂��B �]���Ɏ����Ă���
    // �V����������̃X�y�[�X�̓G���[�ł͂���܂��񂪁A
    // �X�y�[�X�s���͐��ݓI�ȃZ�L�����e�B��̖��ł��B
    const size_t newsize = origsize * 2;
    // �V����������ɂ́A���̕�����̕ϊ����ꂽ�R�s�[���܂܂�܂��B
         // ������Ƃ���ɒǉ�����镶����̃^�C�v�B
    char* nstring = new char[newsize + strConcatsize];

    // �ϊ����ꂽ������̃R�s�[�� nstring �ɔz�u���܂�
    wcstombs_s(&convertedChars, nstring, newsize, orig, _TRUNCATE);


    const sprite_map::iterator it = sprites.find(nstring);
    if (it != sprites.end())
    {
        // �����N(����)���؂�Ă��Ȃ����m�F
        if (it->second.expired() == false)
        {
            // �����������f���Ɠ������f�������������炻���Ԃ�
            return it->second.lock();
        }
    }

    // ������Ȃ�������V�K���f�����\�[�X���쐬�E�ǂݍ���
    const std::shared_ptr<Sprite> sprite_ = {
        std::make_shared<Sprite>(
            device,
            filename)
    };

    // �}�b�v�ɓo�^
    sprites[nstring] = sprite_;

    return sprite_;
}


const std::shared_ptr<SkinnedMesh> ResourceManager::LoadModelResource(
    ID3D11Device* device,
    const char* const                  fbx_filename,
    std::vector<std::string> animation_filenames,
    const bool                         triangulate,
    const float                        sampling_rate)
{
    // ���f��������
    const ModelMap::iterator it = models.find(fbx_filename);
    if (it != models.end())
    {
        // �����N(����)���؂�Ă��Ȃ����m�F
        if (it->second.expired() == false)
        {
            // �����������f���Ɠ������f�������������炻���Ԃ�
            return it->second.lock();
        }
    }

    // ������Ȃ�������V�K���f�����\�[�X���쐬�E�ǂݍ���
    const std::shared_ptr<SkinnedMesh> model = {
        std::make_shared<SkinnedMesh>(
            device,
            fbx_filename,
            animation_filenames,
            triangulate,
            sampling_rate)
    };

    // �}�b�v�ɓo�^
    models[fbx_filename] = model;

    return model;
}

