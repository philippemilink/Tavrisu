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

#ifndef UPDATER_H
#define UPDATER_H

#include <QWidget>
#include <QNetworkReply>
#include <QTemporaryFile>
#include <QDomDocument>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QCryptographicHash>


#define UPDATE_PATH QString("http://localhost/Tavrisu/update.xml")
#define UPDATE_ID int(5);

namespace Ui
{
    class Updater;
}


class Updater : public QWidget
{
    Q_OBJECT
    

    public:
    explicit Updater(QWidget *parent = 0);
    ~Updater();
    bool existsNewVersion();


    signals:
    void exportDatabase();


    public slots:
    void display();
    void searchUpdate();


    private slots:
    void errorXMLFile(QNetworkReply::NetworkError);
    void errorUpdateFile(QNetworkReply::NetworkError);
    void saveXMLFile();
    void saveUpdateFile();
    void progressionUpdateDownload(qint64 bytesReceived, qint64 bytesTotal);
    void on_buttonCancel_clicked();
    void on_buttonDownload_clicked();
    

    private:
    void parseXML(QTemporaryFile *xml);
    void setInfosNewVersion();
    void questionSaveData();

    Ui::Updater *ui;
    QString m_updatePath;
    QString m_newVersion;
    std::vector<QString> m_newWhatsnew;
    QString m_newDate;
    QString m_newType;
    bool m_downloadXmlError;
    QNetworkReply *m_networkXml;
    bool m_downloadUpdateError;
    QNetworkReply *m_networkUpdate;
    QString m_destinationPath;
    bool m_downloadFinished;
    QString m_updateMd5;
    bool m_existsNewVersion;
};

#endif // UPDATER_H
