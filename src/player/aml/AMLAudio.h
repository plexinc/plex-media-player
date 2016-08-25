#ifndef AMLAUDIO_H
#define AMLAUDIO_H

#include <QObject>

class AMLAudio : public QObject
{
  Q_OBJECT

  public:
    AMLAudio(QObject *parent);

    // Digital audio IDs for audio dsp
    // to be set in /sys/class/audiodsp/digital_codec
    enum AMLDigitaAudioType
    {
      AML_DA_NONE = 0,
      AML_DA_AC3 = 2,
      AML_DA_DTS = 3,
      AML_DA_EAC3 = 4,
      AML_DA_8CHPCM = 6,
      AML_DA_TRUEHD = 7,
      AML_DA_DTSHD = 8
    };

    // Digital audio Passthru settings
    // to be set in /sys/class/audiodsp/digital_raw
    enum AMLDigitalAudioPassthrough
    {
      AML_DA_PASSTHROUGH_OFF = 0,
      AML_DA_PASSTHROUGH_ON = 2
    };

  public Q_SLOTS:
    void codecChanged(QString codec, bool passthrough);
};

#endif // AMLAUDIO_H
