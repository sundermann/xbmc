#include "DVDVideoCodecNDL.h"

#include "Application.h"
#include "DVDCodecs/DVDFactoryCodec.h"
#include "ServiceBroker.h"
#include "cores/VideoPlayer/Buffers/VideoBuffer.h"
#include "cores/VideoPlayer/Interface/DemuxCrypto.h"
#include "cores/VideoPlayer/Interface/TimingConstants.h"
#include "media/decoderfilter/DecoderFilterManager.h"
#include "messaging/ApplicationMessenger.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "utils/CPUInfo.h"
#include "utils/log.h"

#include <vector>

#include <NDL_directmedia_v2.h>

using namespace KODI::MESSAGING;

/*****************************************************************************/
/*****************************************************************************/
CDVDVideoCodecNDL::CDVDVideoCodecNDL(CProcessInfo &processInfo)
  : CDVDVideoCodec(processInfo)
    , m_name("NDL")
{
  int ret = NDL_DirectMediaInit("org.xbmc.kodi", NULL);
  CLog::Log(LOGDEBUG, "NDL NDL_DirectMediaInit ret = %d", ret);
}

CDVDVideoCodecNDL::~CDVDVideoCodecNDL()
{
  NDL_DirectMediaUnload();
  NDL_DirectMediaQuit();
}

CDVDVideoCodec* CDVDVideoCodecNDL::Create(CProcessInfo &processInfo)
{
  return new CDVDVideoCodecNDL(processInfo);
}

bool CDVDVideoCodecNDL::Register()
{
  CDVDFactoryCodec::RegisterHWVideoCodec("ndl_dec", &CDVDVideoCodecNDL::Create);
  return true;
}

static void LoadCallback(int type, long long numValue, const char *strValue) {
  CLog::Log(LOGERROR, "NDL MediaLoadCallback type=%d, numValue=%llx, strValue=%p\n",
            type, numValue, strValue);
}

std::atomic<bool> CDVDVideoCodecNDL::m_InstanceGuard(false);

bool CDVDVideoCodecNDL::Open(CDVDStreamInfo &hints, CDVDCodecOptions &options)
{
  // allow only 1 instance here
  if (m_InstanceGuard.exchange(true))
  {
    CLog::Log(LOGERROR, "CDVDVideoCodecNDL::Open - InstanceGuard locked");
    return false;
  }

  NDL_VIDEO_TYPE format = NDL_VIDEO_TYPE_MAX;
  NDL_DIRECTMEDIA_DATA_INFO info;

  // mediacodec crashes with null size. Trap this...
  if (!hints.width || !hints.height)
  {
    CLog::Log(LOGERROR, "CDVDVideoCodecNDL::Open - %s", "null size, cannot handle");
    goto FAIL;
  }
  else if (!CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_VIDEOPLAYER_USENDL))
    goto FAIL;

  CLog::Log(
      LOGDEBUG,
      "CDVDVideoCodecNDL::Open hints: Width %d x Height %d, Fpsrate %d / Fpsscale "
      "%d, CodecID %d, Level %d, Profile %d, PTS_invalid %d, Tag %d, Extradata-Size: %d",
      hints.width, hints.height, hints.fpsrate, hints.fpsscale, hints.codec, hints.level,
      hints.profile, hints.ptsinvalid, hints.codec_tag, hints.extrasize);

  m_hints = hints;
  switch(m_hints.codec)
  {
    case AV_CODEC_ID_MPEG2VIDEO:
      break;
    case AV_CODEC_ID_MPEG4:
      break;
    case AV_CODEC_ID_H263:
      break;
    case AV_CODEC_ID_VP6:
    case AV_CODEC_ID_VP6F:
      break;
    case AV_CODEC_ID_VP8:
      break;
    case AV_CODEC_ID_VP9:
      format = NDL_VIDEO_TYPE_VP9;
    case AV_CODEC_ID_AVS:
    case AV_CODEC_ID_CAVS:
    case AV_CODEC_ID_H264:
      format = NDL_VIDEO_TYPE_H264;
      break;
    case AV_CODEC_ID_HEVC:
      format = NDL_VIDEO_TYPE_H265;
      break;
    case AV_CODEC_ID_WMV3:
      break;
    case AV_CODEC_ID_VC1:
    {
      break;
    }
    default:
      CLog::Log(LOGDEBUG, "CDVDVideoCodecNDL::Open Unknown hints.codec(%d)", hints.codec);
      goto FAIL;
      break;
  }

  info.video.type = format;
  info.video.height = m_hints.height;
  info.video.width = m_hints.width;
  info.video.unknown1 = 0;
  return NDL_DirectMediaLoad(&info, LoadCallback) == 0;

FAIL:
  m_InstanceGuard.exchange(false);
  return false;
}

bool CDVDVideoCodecNDL::AddData(const DemuxPacket &packet)
{
  double pts(packet.pts), dts(packet.dts);

  if (CServiceBroker::GetLogging().CanLogComponent(LOGVIDEO))
    CLog::Log(LOGDEBUG, "CDVDVideoCodecNDL::AddData dts:%0.2lf pts:%0.2lf sz:%d indexBuffer:%d current state (%d)", dts, pts, packet.iSize);

  if (m_hints.ptsinvalid)
    pts = DVD_NOPTS_VALUE;

  uint8_t *pData(packet.pData);
  size_t iSize(packet.iSize);

  int ret = NDL_DirectVideoPlay(pData, iSize, pts);

  CLog::Log(LOGDEBUG, "CDVDVideoCodecNDL::NDL_DirectVideoPlay ret %d", ret);

  return true;
}

void CDVDVideoCodecNDL::Reset()
{
  CLog::Log(LOGDEBUG, "CDVDVideoCodecNDL::Reset: not implemented");
}

bool CDVDVideoCodecNDL::Reconfigure(CDVDStreamInfo &hints)
{
  m_hints = hints;
  CLog::Log(LOGDEBUG, "CDVDVideoCodecNDL::Reconfigure: false");
  return false;
}

CDVDVideoCodec::VCReturn CDVDVideoCodecNDL::GetPicture(VideoPicture* pVideoPicture)
{
  return VC_PICTURE;
}