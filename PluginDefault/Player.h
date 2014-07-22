/*
 * Tavrisu, an easy and modulable audio player.
    Copyright (C) 2014 - Philippe SWARTVAGHER

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QLabel>
#include <QMessageBox>

#define PLUGIN_PATH QString(qApp->applicationDirPath() + "/views/Default/")

QString convertTime(unsigned int time);

namespace Ui
{
    class Player;
}

class Player : public QWidget
{
    Q_OBJECT
    
    public:
    explicit Player(QWidget *parent = 0);
    ~Player();
    void setInfos(QString title, int duration, QString artist, QString albumCover = "", QString album = "");
    void changePosition(int position);
    bool isPlaying();


    private slots:
    void on_buttonPlay_clicked();
    void on_buttonNext_clicked();
    void on_buttonPrevious_clicked();
    void on_sliderTime_sliderMoved(int value);
    void on_sliderTime_sliderReleased();


    signals:
    void play();
    void pause();
    void next();
    void previous();
    void positionChanged(int value);

    
    private:
    Ui::Player *ui;
    int m_currentDuration;
    bool m_isPlaying;
    bool m_isLoaded;
    int m_timeValueSliderDown;
};

#endif // PLAYER_H
