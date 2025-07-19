/*
 *  Copyright (C) 2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "MediaPipelineWebOS.h"
#include "VideoPlayerAudio.h"

/**
 * @class CVideoPlayerAudioWebOS
 * @brief Audio stream player adapter forwarding to CMediaPipelineWebOS.
 */
class CVideoPlayerAudioWebOS final : public IDVDStreamPlayerAudio
{
public:
  CVideoPlayerAudioWebOS(CMediaPipelineWebOS& mediaPipeline, CProcessInfo& processInfo)
    : IDVDStreamPlayerAudio(processInfo), m_mediaPipeline(mediaPipeline)
  {
  }
  void FlushMessages() override { m_mediaPipeline.FlushAudioMessages(); };
  bool OpenStream(CDVDStreamInfo hints) override { return m_mediaPipeline.OpenAudioStream(hints); }
  void CloseStream(const bool waitForBuffers) override
  {
    m_mediaPipeline.CloseAudioStream(waitForBuffers);
  }
  void SetSpeed(const int speed) override { m_mediaPipeline.SetSpeed(speed); }
  void Flush(const bool sync) override { m_mediaPipeline.Flush(sync); }
  [[nodiscard]] bool AcceptsData() const override { return m_mediaPipeline.AcceptsAudioData(); }
  [[nodiscard]] bool HasData() const override { return m_mediaPipeline.HasAudioData(); }
  [[nodiscard]] int GetLevel() const override { return m_mediaPipeline.GetAudioLevel(); }
  [[nodiscard]] bool IsInited() const override { return m_mediaPipeline.IsAudioInited(); }
  void SendMessage(const std::shared_ptr<CDVDMsg> msg, const int priority) override
  {
    m_mediaPipeline.SendAudioMessage(msg, priority);
  }
  void SetDynamicRangeCompression(long drc) override {}
  std::string GetPlayerInfo() override { return m_mediaPipeline.GetAudioInfo(); }
  int GetAudioChannels() override { return m_mediaPipeline.GetAudioChannels(); }
  double GetCurrentPts() override { return m_mediaPipeline.GetCurrentPts(); }
  [[nodiscard]] bool IsStalled() const override { return m_mediaPipeline.IsStalled(); }
  [[nodiscard]] bool IsPassthrough() const override { return true; }
  [[nodiscard]] float GetDynamicRangeAmplification() const override { return 0.0f; }

private:
  CMediaPipelineWebOS& m_mediaPipeline;
};
