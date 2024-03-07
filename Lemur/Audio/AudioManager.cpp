#include "AudioManager.h"

namespace Lemur::Audio
{

    void AudioManager::LoadAudio()
    {
        {
            bgm[static_cast<int>(BGM::BOSS)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/BGM/Boss.wav");
            bgm[static_cast<int>(BGM::GAMBLE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/BGM/Gambling.wav");
            bgm[static_cast<int>(BGM::CLEAR)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/BGM/GameClear.wav");
            bgm[static_cast<int>(BGM::OVER)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/BGM/GameOver.wav");
            bgm[static_cast<int>(BGM::PLAY)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/BGM/Play.wav");
            bgm[static_cast<int>(BGM::TITLE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/BGM/Title.wav");

            se[static_cast<int>(SE::ARROW)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/SE/arrow_se.wav");
            se[static_cast<int>(SE::AVOID)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/SE/avoid.wav");
            se[static_cast<int>(SE::CONTER)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/SE/counter.wav");
            se[static_cast<int>(SE::DAMAGE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/SE/damage.wav");
            se[static_cast<int>(SE::DECISION)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/SE/decision.wav");
            se[static_cast<int>(SE::HIT)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/SE/Hit.wav");
            se[static_cast<int>(SE::SENI)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/SE/Seni.wav");
            se[static_cast<int>(SE::SWING)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources_2/Sound/SE/swing.wav");
        }
    
    }
    void AudioManager::PlayBgm(const BGM& bgm_, const bool isloop)
    {
        bgm[static_cast<int>(bgm_)]->Play(isloop);
    }

    void AudioManager::PlaySe(const SE& se_, const bool isLoop)
    {
        se[static_cast<int>(se_)]->Play(isLoop);
    }

    void AudioManager::StopBGM(const BGM& bgm_)
    {
        bgm[static_cast<int>(bgm_)]->Stop();
    }

    void AudioManager::StopSE(const SE& se_)
    {
        se[static_cast<int>(se_)]->Stop();
    }

    void AudioManager::StopAllBGM()
    {
        for (std::unique_ptr<Lemur::Audio::Audio>& bgm_ : bgm)
        {
            bgm_->Stop();
        }
    }

    void AudioManager::StopAllSE()
    {
        for (std::unique_ptr<Lemur::Audio::Audio>& se_ : bgm)
        {
            se_->Stop();
        }
    }

    void AudioManager::StopAllAudio()
    {
        for (std::unique_ptr<Lemur::Audio::Audio>& bgm_ : bgm)
        {
            bgm_->Stop();
        }
        for (std::unique_ptr<Lemur::Audio::Audio>& se_ : bgm)
        {
            se_->Stop();
        }
    }

    std::unique_ptr<Lemur::Audio::Audio>& AudioManager::GetBGM(const BGM& bgm_)
    {
        return bgm[static_cast<int>(bgm_)];
    }

    std::unique_ptr<Lemur::Audio::Audio>& AudioManager::GetSE(const SE& se_)
    {
        return se[static_cast<int>(se_)];
    }

}