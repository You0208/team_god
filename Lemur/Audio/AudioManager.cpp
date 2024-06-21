#include "AudioManager.h"

namespace Lemur::Audio
{
    void AudioManager::LoadAudio()
    {
        {
            bgm[static_cast<int>(BGM::TITLE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/1_title.wav");
            bgm[static_cast<int>(BGM::SELECT)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/2_stageselect.wav");
            bgm[static_cast<int>(BGM::FORMATION)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/3_select.wav");
            bgm[static_cast<int>(BGM::GAME_MORNING)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/4_game_morning.wav");
            bgm[static_cast<int>(BGM::GAME_MORNING_SE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/5_morningSE.wav");
            bgm[static_cast<int>(BGM::GAME_NOON)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/4_game_noon.wav");
            bgm[static_cast<int>(BGM::GAME_NOON_SE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/5_nonnSE.wav");
            bgm[static_cast<int>(BGM::GAME_NIGHT)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/4_game_night.wav");
            bgm[static_cast<int>(BGM::GAME_NIGHT_SE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/5_nightSE.wav");
            bgm[static_cast<int>(BGM::CLEAR)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/6_gameclear.wav");
            bgm[static_cast<int>(BGM::OVER)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/BGM/7_gameover.wav");

            se[static_cast<int>(SE::CHANGE_IN)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/1_scenechange_up.wav");
            se[static_cast<int>(SE::CHANGE_OUT)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/2_scenechange_down.wav");
            se[static_cast<int>(SE::DECISION)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/3_decision.wav");
            se[static_cast<int>(SE::WORLD_CHANGE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/5_worldchange.wav");
            se[static_cast<int>(SE::CHARACTER_SET)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/6_character_controller.wav");
            se[static_cast<int>(SE::STICK)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/7_stick.wav");
            se[static_cast<int>(SE::DEATH)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/8_death.wav");
            se[static_cast<int>(SE::UNIT_ATTACK)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/9_unit_attack.wav");
            se[static_cast<int>(SE::FENCE_ATTACK)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/10_fence_attack.wav");
            se[static_cast<int>(SE::CLEAR)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/11_clear.wav");
            se[static_cast<int>(SE::CHICKEN)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/12_chicken.wav");
            se[static_cast<int>(SE::BARD)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/13_bard.wav");
            se[static_cast<int>(SE::OWL)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/14_owl.wav");
            se[static_cast<int>(SE::FANCE_BREAK)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/15_fence_break.wav");
            se[static_cast<int>(SE::CHEER)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/16_cheer.wav");
            se[static_cast<int>(SE::SPOWN)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/17_spown.wav");
            se[static_cast<int>(SE::APPEARANCE)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/18_appearance.wav");
            se[static_cast<int>(SE::STANDUP)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/19_standup .wav");
            se[static_cast<int>(SE::Shishito)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/Shishito.wav");
            se[static_cast<int>(SE::OrangePumpkin)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/OrangePumpkin.wav");
            se[static_cast<int>(SE::GreenPumpkin)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/GreenPumpkin.wav");
            se[static_cast<int>(SE::Broccoli)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/Broccoli.wav");
            se[static_cast<int>(SE::Cauliflower)] = std::make_unique<Lemur::Audio::Audio>(xAudio2.Get(), L"./resources/Sound/SE/Cauliflower.wav");
        }
    }
    void AudioManager::PlayBgm(const BGM& bgm_, const bool isloop)
    {
        bgm[static_cast<int>(bgm_)]->Volume(0.0f);
        bgm[static_cast<int>(bgm_)]->Play(isloop);
    }

    void AudioManager::PlaySe(const SE& se_, const bool isLoop)
    {
        se[static_cast<int>(se_)]->Volume(0.0f);
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