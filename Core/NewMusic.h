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

#ifndef NEWMUSIC_H
#define NEWMUSIC_H

#include <QDialog>
#include <QFileDialog>
#include <QCompleter>
#include <QSettings>

#include "QlibVLC.h"
#include "QSqliteCom.h"
#include "NewArtist.h"


namespace Ui
{
    class NewMusic;
}

class NewMusic : public QDialog
{
    Q_OBJECT
    
    public:
    explicit NewMusic(QSqliteCom *base, QSettings* settings, QWidget *parent = 0);
    explicit NewMusic(int id, QSqliteCom *base, QSettings* settings, QWidget *parent = 0); // To edit a track
    ~NewMusic();
    void start();
    static bool deleteMusic(QString name, int id, QSqliteCom* base);


    signals:
    void finish();


    private slots:
    void verifyButtonSubmit(QString value);
    void buttonSubmit_clicked_save();
    void buttonSubmit_clicked_update();
    bool verifyArtist();

    
    private:
    Ui::NewMusic *ui;
    QString m_filePath;
    QSqliteCom* m_base;
    int m_previousId; // Used to edit a track
    QlibVLC* m_vlc;
    //QCompleter* m_completerArtist;
    QSettings* m_settings;
};

#endif // NEWMUSIC_H
