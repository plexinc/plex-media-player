#include "AMLAudio.h"
#include "PlayerComponent.h"
#include "settings/SettingsComponent.h"
#include "utils/aml/AMLUtils.h"
#include "QsLog.h"
#include <QThread>

// sysfs paths for audiodsp
#define DIGITAL_RAW_PATH    "/sys/class/audiodsp/digital_raw"
#define DIGITAL_CODEC_PATH  "/sys/class/audiodsp/digital_codec"

/////////////////////////////////////////////////////////////////////////////////////////
AMLAudio::AMLAudio(QObject *parent) : QObject(parent)
{
  connect(parent, SIGNAL(onCodecChanged(QString, bool)), this, SLOT(codecChanged(QString, bool)));
}

/////////////////////////////////////////////////////////////////////////////////////////
void AMLAudio::codecChanged(QString codec, bool passthrough)
{
  AMLDigitaAudioType audioType = AML_DA_NONE;

  if (passthrough)
  {
    AMLUtils::sysfsWriteString(DIGITAL_RAW_PATH, QString::number(AML_DA_PASSTHROUGH_ON));

    if (codec == "ac3")
      audioType = AML_DA_AC3;
    else if (codec == "dts")
      audioType = AML_DA_DTS;
    else if (codec == "eac3")
      audioType = AML_DA_EAC3;
    else if (codec == "dtshd")
      audioType = AML_DA_DTSHD;
    else if (codec == "truehd")
      audioType = AML_DA_TRUEHD;

    AMLUtils::sysfsWriteString(DIGITAL_CODEC_PATH, QString::number(audioType));
  }
  else
  {
    AMLUtils::sysfsWriteString(DIGITAL_RAW_PATH, QString::number(AML_DA_PASSTHROUGH_OFF));
    AMLUtils::sysfsWriteString(DIGITAL_CODEC_PATH, QString::number(AML_DA_NONE));
  }

  // This event can come right in the middle of the audio configuration
  // if so, setting the passthru parameters will not work properly if they are
  // not set before alsa is inited, so we reset the audio output
  PlayerComponent::Get().resetAudioOutput();
}
