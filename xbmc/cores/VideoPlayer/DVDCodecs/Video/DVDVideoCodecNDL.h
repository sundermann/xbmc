#pragma once

#include "cores/VideoPlayer/Buffers/VideoBuffer.h"
#include "cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodec.h"
#include "cores/VideoPlayer/DVDStreamInfo.h"

#include <memory>

class CDVDVideoCodecNDL : public CDVDVideoCodec
{
public:
  explicit CDVDVideoCodecNDL(CProcessInfo& processInfo);
  ~CDVDVideoCodecNDL() override;

  static CDVDVideoCodec* Create(CProcessInfo& processInfo);
  static bool Register();

  bool Open(CDVDStreamInfo& hints, CDVDCodecOptions& options) override;
  bool AddData(const DemuxPacket& packet) override;
  void Reset() override;
  bool Reconfigure(CDVDStreamInfo &hints) override;
  CDVDVideoCodec::VCReturn GetPicture(VideoPicture* pVideoPicture) override;
  const char* GetName() override { return m_name.c_str(); }
  unsigned GetAllowedReferences() override { return 5; }

  std::string m_name;
  CDVDStreamInfo m_hints;

  static std::atomic<bool> m_InstanceGuard;
};