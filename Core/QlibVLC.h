/*************************************************************************
QlibVLC: a C++ wrapper of libVLC for Qt, with audio part only.
    Copyright (C) 2014 - Philippe SWARTVAGHER
    Version 0.1

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
    *****************************************************************************/


#ifndef QLIBVLC_H
#define QLIBVLC_H

#include <vector>

#include <QObject>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

#include <vlc/vlc.h>


struct QlibVLCOutput
{
    QString name;
    QString description;
};


class QlibVLC : public QObject
{
    Q_OBJECT

    public:
    QlibVLC();
    ~QlibVLC();

    // Manage medias:
    void setMedia(QString path);
    void ejectMedia();

    // Main commands:
    void play();
    void pause(bool pause);
    bool isPlaying();

    // Attributes:
    int trackNumber();
    QString album();
    int year();
    QString artist();
    QString title();
    int duration();
    int getDuration(QString path);

    // Outputs:
    std::vector<QlibVLCOutput> getOutputs();
    void setOutput(QString name);

    int getVolume();

    // For signals...
    static void callbackTimeChanged(const libvlc_event_t *event, void* data);
    void emitPositionChanged();
    static void callbackEndTrack(const libvlc_event_t *event, void* data);


    public slots:
    void changePosition(int value);
    void stop();
    void setVolume(int newVolume);


    signals:
    void positionChanged(int newPosition);
    void endTrack();


    private:
    libvlc_instance_t* m_vlc;
    libvlc_media_player_t* m_player;
    libvlc_event_manager_t* m_eventManager;
    libvlc_media_t* m_media;
    bool m_issetMedia;
    QString m_path;
};

#endif // QLIBVLC_H
