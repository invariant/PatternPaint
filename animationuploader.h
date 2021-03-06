#include <QObject>
#include <QTimer>
#include <iostream>
#include "animation.h"
#include "avrprogrammer.h"
#include "blinkytape.h"

#ifndef ANIMATIONUPLOADER_H
#define ANIMATIONUPLOADER_H


// This is an re-entreant version of an animation uploader.
// Each task in the upload process is broken into a single state, and the state
// machine is driven forward by receiving events from the connected serial port,
// or in the case of a timeout, from the timeout timer.
//
// This seems convoluted, but the goal is to avoid ever waiting on a serial read
// event. For some reason, doing so seems to block the main thread even if we are
// in a different thread; it's possible serial sits on the main lock?
//
// While the upload process is underway, it will send periodic progress updates
// via the progressUpdate() signal.
class AnimationUploader : public QObject
{
    Q_OBJECT
public:
    AnimationUploader(QObject *parent=0);

    // TODO: Destructor that gets rid of stuff

    // Start an upload, using the passed blinkytape as a launching point
    // Note that the blinkytape will be disconnected during the upload process.
    void startUpload(BlinkyTape& tape, Animation animation);

signals:
    // Sends an update about the upload progress, from 0 to 1
    void progressChanged(int progress);

    // Sends a signal at end of upload to report the result.
    void finished(bool result);

private slots:
    void doWork();  // Handle the next section of work, whatever it is

    void handleProgrammerError(QString error);

    void handleProgrammerCommandFinished(QString command, QByteArray returnData);

    // Delay timer, lets us wait some time between receiving a finished command, and
    // passing the message along (to give the serial device some time to reset itself).
    void handleResetTimer();

private:
    enum State {
        State_Ready, ///< Nothing to do
        State_WaitForBootloaderPort, ///< We are waiting for the bootloader port to show up.
        State_WaitForDeviceSignature,
        State_WaitForSketchWritten,
        State_WaitForMetaDataWritten,
        State_Finished,
    };

    /// Timer that fires
    QTimer *processTimer;

    float progress;        // TODO: deleteme?

    /// Time that we transitioned into the current state
    QDateTime stateStartTime;

    /// Current command state
    State state;

    // Update any listeners with the latest progress
    void updateProgress(int newProgress);

    AvrProgrammer programmer;

    QByteArray sketch;
    QByteArray metadata;

    QByteArray responseData;    // Response data left over.

    // TODO: Delete me?
    bool waitOneMore; // BS variable to delay connection to bootloader by 1 timer cycle
};

#endif // ANIMATIONUPLOADER_H
