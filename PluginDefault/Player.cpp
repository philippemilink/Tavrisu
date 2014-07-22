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

#include "Player.h"
#include "ui_Player.h"

Player::Player(QWidget *parent) : QWidget(parent), ui(new Ui::Player), m_isPlaying(false)
{
    ui->setupUi(this);

    ui->buttonPrevious->setIcon(QIcon(PLUGIN_PATH + "previous.png"));
    ui->buttonPrevious->setIconSize(QSize(32, 23));

    ui->buttonPlay->setIcon(QIcon(PLUGIN_PATH + "play.png"));
    ui->buttonPlay->setIconSize(QSize(32, 32));

    ui->buttonNext->setIcon(QIcon(PLUGIN_PATH + "next.png"));
    ui->buttonNext->setIconSize(QSize(32, 23));

    m_isLoaded = false;
}


Player::~Player()
{
    delete ui;
}


void Player::setInfos(QString title, int duration, QString artist, QString albumCover, QString album)
{
    m_currentDuration = duration;

    ui->labelInfos->setText("<strong style=\"font-size: 12px;\">" + title + "</strong><br /><br />" + album + "<br /><br /><strong>" + artist + "</strong>");

    if(!albumCover.isEmpty())
        ui->labelAlbumCover->setPixmap(QPixmap(QPixmap(albumCover).scaled(180, 180, Qt::KeepAspectRatio)));
    else
        ui->labelAlbumCover->setPixmap(QPixmap());

    ui->labelTimeTotal->setText(convertTime(duration));
    ui->sliderTime->setMaximum(duration);

    ui->buttonPlay->setIcon(QIcon(PLUGIN_PATH + "pause.png"));

    m_isPlaying = true;
    m_isLoaded = true;
}


void Player::changePosition(int position)
{
    if(!ui->sliderTime->isSliderDown()) {
        ui->sliderTime->setValue(position);
        ui->labelTimePast->setText(convertTime(position));
    }
}


void Player::on_buttonPlay_clicked()
{
    if(m_isLoaded)
    {
        if(m_isPlaying) // Set pause
        {
            m_isPlaying = false;
            ui->buttonPlay->setIcon(QIcon(PLUGIN_PATH + "play.png"));
        }
        else // Unset pause
        {
            m_isPlaying = true;
            ui->buttonPlay->setIcon(QIcon(PLUGIN_PATH + "pause.png"));
        }
        emit pause();
    }
    else
        emit play();
}


void Player::on_buttonNext_clicked()
{
    emit next();
}


void Player::on_buttonPrevious_clicked()
{
    emit previous();
}

/*
 * Show the time currently focused (where the playing will restart after releasing)
 */
void Player::on_sliderTime_sliderMoved(int value)
{
    m_timeValueSliderDown = value;
    ui->labelTimePast->setText(convertTime(m_timeValueSliderDown));
}


void Player::on_sliderTime_sliderReleased()
{
    emit positionChanged(ui->sliderTime->value());
}


QString convertTime(unsigned int time)
{
    unsigned int minutes = 0;
    unsigned int secondes = 0;
    unsigned int secondesTotal = 0;
    QString string;

    secondesTotal = time / 1000;
    minutes = secondesTotal / 60;
    secondes = secondesTotal % 60;

    if(secondes < 10)
    {
         string.sprintf("%d:0%d", minutes, secondes);
    }
    else
    {
         string.sprintf("%d:%d", minutes, secondes);
    }

    return string;
}


bool Player::isPlaying()
{
    return m_isPlaying;
}
