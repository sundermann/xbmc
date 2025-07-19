/*
 *  Copyright (C) 2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "MediaPipelineWebOS.h"
#include "VideoPlayerVideo.h"

#include <memory>

/**
 * @class CVideoPlayerVideoWebOS
 * @brief Video stream player adapter forwarding to CMediaPipelineWebOS.
 */
class CVideoPlayerVideoWebOS final : public IDVDStreamPlayerVideo
{
public:
  CVideoPlayerVideoWebOS(CMediaPipelineWebOS& mediaPipeline, CProcessInfo& processInfo)
    : IDVDStreamPlayerVideo(processInfo), m_mediaPipeline(mediaPipeline)
  {
  }
  void FlushMessages() override { m_mediaPipeline.FlushVideoMessages(); }
  bool OpenStream(const CDVDStreamInfo hint) override
  {
    return m_mediaPipeline.OpenVideoStream(hint);
  }
  void CloseStream(const bool waitForBuffers) override
  {
    m_mediaPipeline.CloseVideoStream(waitForBuffers);
  }
  void Flush(const bool sync) override { m_mediaPipeline.Flush(sync); }
  [[nodiscard]] bool AcceptsData() const override { return m_mediaPipeline.AcceptsVideoData(); }
  [[nodiscard]] bool HasData() const override { return m_mediaPipeline.HasVideoData(); }
  [[nodiscard]] bool IsInited() const override { return m_mediaPipeline.IsVideoInited(); }
  void SendMessage(const std::shared_ptr<CDVDMsg> msg, const int priority) override
  {
    m_mediaPipeline.SendVideoMessage(msg, priority);
  }
  void EnableSubtitle(const bool enable) override { m_mediaPipeline.EnableSubtitle(enable); }
  bool IsSubtitleEnabled() override { return m_mediaPipeline.IsSubtitleEnabled(); }
  double GetSubtitleDelay() override { return m_mediaPipeline.GetSubtitleDelay(); }
  void SetSubtitleDelay(const double delay) override { m_mediaPipeline.SetSubtitleDelay(delay); }
  [[nodiscard]] bool IsStalled() const override { return m_mediaPipeline.IsStalled(); }
  double GetCurrentPts() override { return m_mediaPipeline.GetCurrentPts(); }
  double GetOutputDelay() override { return 0.0; }
  std::string GetPlayerInfo() override { return m_mediaPipeline.GetVideoInfo(); }
  int GetVideoBitrate() override { return m_mediaPipeline.GetVideoBitrate(); }
  void SetSpeed(const int speed) override { m_mediaPipeline.SetSpeed(speed); }

private:
  CMediaPipelineWebOS& m_mediaPipeline;
};
