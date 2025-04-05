/*
*  Copyright (C) 2005-2025 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "DVDCodecs/Video/DVDVideoCodec.h"
#include "DVDMessageQueue.h"
#include "DVDStreamInfo.h"
#include "IVideoPlayer.h"
#include "threads/Thread.h"

namespace ActiveAE
{
class CActiveAEBufferPool;
class CActiveAEBufferPoolResample;
} // namespace ActiveAE

class CAEEncoderFFmpeg;
class CBitstreamConverter;
class CRenderManager;
class CDVDOverlayContainer;
class CDVDAudioCodec;
class StarfishMediaAPIs;

class CMediaPipelineWebOS final : public CThread
{
public:
  explicit CMediaPipelineWebOS(CProcessInfo& processInfo,
                               CRenderManager& renderManager,
                               CDVDClock& clock,
                               CDVDMessageQueue& parent,
                               CDVDOverlayContainer& overlay);
  ~CMediaPipelineWebOS() override;

  static bool Supports(const AVCodecID codec)
  {
    return ms_formatInfoMap.find(codec) != ms_formatInfoMap.cend();
  }

  void FlushVideoMessages();
  void FlushAudioMessages();
  bool OpenAudioStream(CDVDStreamInfo& audioHint);
  bool OpenVideoStream(const CDVDStreamInfo& hint);
  void CloseAudioStream(bool waitForBuffers);
  void CloseVideoStream(bool waitForBuffers);
  void Flush(bool sync);
  bool AcceptsAudioData() const;
  bool AcceptsVideoData() const;
  bool HasAudioData() const;
  bool HasVideoData() const;
  bool IsAudioInited() const;
  bool IsVideoInited() const;
  int GetAudioLevel() const;
  bool IsStalled() const;
  void SendAudioMessage(const std::shared_ptr<CDVDMsg>& msg, int priority);
  void SendVideoMessage(const std::shared_ptr<CDVDMsg>& msg, int priority);
  void SetSpeed(int speed) const;
  double GetCurrentPts() const;
  int GetAudioChannels() const { return m_audioHint.channels; }

  void EnableSubtitle(bool enable);
  bool IsSubtitleEnabled() const;
  double GetSubtitleDelay() const;
  void SetSubtitleDelay(double delay);

protected:
  void Process() override;
  void ProcessAudio();

private:
  void SetHDR(const CDVDStreamInfo& hint) const;
  void FeedVideoData(const std::shared_ptr<CDVDMsg>& msg);
  void ProcessOverlays(double pts) const;
  void FeedAudioData(const std::shared_ptr<CDVDMsg>& msg);
  bool Load(CDVDStreamInfo& videoHint, CDVDStreamInfo& audioHint);

  void PlayerCallback(int32_t type, int64_t numValue, const char* strValue);
  static void PlayerCallback(int32_t type, int64_t numValue, const char* strValue, void* data);
  static void AcbCallback(
      long acbId, long taskId, long eventType, long appState, long playState, const char* reply);

  std::atomic<bool> m_stalled{false};
  std::atomic<bool> m_loaded{false};
  std::atomic<bool> m_flushed{false};
  std::atomic<bool> m_subtitle{false};
  std::atomic<double> m_subtitleDelay{0.0};
  std::atomic<bool> m_needsTranscode{false};
  std::atomic<std::chrono::nanoseconds> m_pts{std::chrono::nanoseconds(0)};
  VideoPicture m_picture{};
  std::unique_ptr<StarfishMediaAPIs> m_mediaAPIs;
  CDVDStreamInfo m_audioHint;
  CDVDStreamInfo m_videoHint;
  std::unique_ptr<CBitstreamConverter> m_bitstream{nullptr};
  std::unique_ptr<CDVDAudioCodec> m_audioCodec{nullptr};
  std::unique_ptr<ActiveAE::CActiveAEBufferPool> m_encoderBuffers{nullptr};
  std::unique_ptr<ActiveAE::CActiveAEBufferPoolResample> m_audioResample{nullptr};
  std::unique_ptr<CAEEncoderFFmpeg> m_audioEncoder{nullptr};
  std::atomic<int> m_bufferLevel{0};
  std::atomic<bool> m_audioFull{false};
  std::atomic<bool> m_videoFull{false};

  std::mutex m_audioCriticalSection;

  CDVDMessageQueue m_messageQueueAudio;
  CDVDMessageQueue m_messageQueueVideo;
  CDVDMessageQueue& m_messageQueueParent;
  CProcessInfo& m_processInfo;
  CRenderManager& m_renderManager;
  CDVDClock& m_clock;
  CDVDOverlayContainer& m_overlayContainer;

  std::thread m_audioThread;

  static constexpr auto ms_formatInfoMap = make_map<AVCodecID, std::string_view>({
      {AV_CODEC_ID_VP8, "starfish-vp8"},
      {AV_CODEC_ID_VP9, "starfish-vp9"},
      {AV_CODEC_ID_AVS, "starfish-h264"},
      {AV_CODEC_ID_CAVS, "starfish-h264"},
      {AV_CODEC_ID_H264, "starfish-h264"},
      {AV_CODEC_ID_HEVC, "starfish-h265"},
      {AV_CODEC_ID_AV1, "starfish-av1"},
  });

  static constexpr auto ms_hdrInfoMap = make_map<AVColorTransferCharacteristic, std::string_view>({
      {AVCOL_TRC_SMPTE2084, "HDR10"},
      {AVCOL_TRC_ARIB_STD_B67, "HLG"},
  });
};

class CVideoPlayerVideoWebOS : public IDVDStreamPlayerVideo
{
public:
  CVideoPlayerVideoWebOS(CMediaPipelineWebOS& mediaPipeline, CProcessInfo& processInfo)
    : IDVDStreamPlayerVideo(processInfo), m_mediaPipeline(mediaPipeline)
  {
  }
  void FlushMessages() override { m_mediaPipeline.FlushVideoMessages(); }
  bool OpenStream(CDVDStreamInfo hint) override { return m_mediaPipeline.OpenVideoStream(hint); }
  void CloseStream(bool bWaitForBuffers) override
  {
    m_mediaPipeline.CloseVideoStream(bWaitForBuffers);
  }
  void Flush(bool sync) override { m_mediaPipeline.Flush(sync); }
  bool AcceptsData() const override { return m_mediaPipeline.AcceptsVideoData(); }
  bool HasData() const override { return m_mediaPipeline.HasVideoData(); }
  bool IsInited() const override { return m_mediaPipeline.IsVideoInited(); }
  void SendMessage(const std::shared_ptr<CDVDMsg> msg, const int priority) override
  {
    m_mediaPipeline.SendVideoMessage(msg, priority);
  }
  void EnableSubtitle(const bool enable) override { m_mediaPipeline.EnableSubtitle(enable); }
  bool IsSubtitleEnabled() override { return m_mediaPipeline.IsSubtitleEnabled(); }
  double GetSubtitleDelay() override { return m_mediaPipeline.GetSubtitleDelay(); }
  void SetSubtitleDelay(const double delay) override { m_mediaPipeline.SetSubtitleDelay(delay); }
  bool IsStalled() const override { return m_mediaPipeline.IsStalled(); }
  double GetCurrentPts() override { return m_mediaPipeline.GetCurrentPts(); }
  double GetOutputDelay() override { return 0.0; };
  std::string GetPlayerInfo() override { return ""; };
  int GetVideoBitrate() override { return 10; }
  void SetSpeed(const int speed) override { m_mediaPipeline.SetSpeed(speed); }

private:
  CMediaPipelineWebOS& m_mediaPipeline;
};

class CVideoPlayerAudioWebOS : public IDVDStreamPlayerAudio
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
  bool AcceptsData() const override { return m_mediaPipeline.AcceptsAudioData(); }
  bool HasData() const override { return m_mediaPipeline.HasAudioData(); }
  int GetLevel() const override { return m_mediaPipeline.GetAudioLevel(); }
  bool IsInited() const override { return m_mediaPipeline.IsAudioInited(); }
  void SendMessage(const std::shared_ptr<CDVDMsg> msg, const int priority) override
  {
    m_mediaPipeline.SendAudioMessage(msg, priority);
  }
  void SetDynamicRangeCompression(long drc) override {}
  std::string GetPlayerInfo() override { return ""; };
  int GetAudioChannels() override { return m_mediaPipeline.GetAudioChannels(); }
  double GetCurrentPts() override { return m_mediaPipeline.GetCurrentPts(); }
  bool IsStalled() const override { return m_mediaPipeline.IsStalled(); }
  bool IsPassthrough() const override { return true; }
  float GetDynamicRangeAmplification() const override { return 0.0f; }

private:
  CMediaPipelineWebOS& m_mediaPipeline;
};
