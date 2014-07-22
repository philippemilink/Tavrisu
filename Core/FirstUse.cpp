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

#include "FirstUse.h"



FirstUse::FirstUse(QWidget *parent, QSqliteCom *base, QSettings *settings): m_base(base), m_settings(settings)
{
    m_wizard = new QWizard(parent);
    QObject::connect(m_wizard, SIGNAL(rejected()), this, SIGNAL(cancel()));
    QObject::connect(m_wizard, SIGNAL(accepted()), this, SLOT(createDatabase()));


    m_path = QDir::homePath();


    // The size of the font used in this wizard:
    QFont police;
    police.setPixelSize(12);


    // Creation of the first page of the wizard: welcome !
    m_pageWelcome = new QWizardPage(m_wizard);
    m_pageWelcome->setTitle(QObject::tr("Bienvenue dans l'univers de Tavrisu"));

    QLabel* label = new QLabel(QObject::tr("C'est visiblement la première fois que vous utilisez Tavrisu.<br />Cet assistant va vous guider pour finaliser son installation."));
    label->setFont(police);
    label->setWordWrap(true);

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(label);

    m_pageWelcome->setLayout(vbox);
    m_wizard->addPage(m_pageWelcome);


    // Creation of the second page of the wizard: GPL is amazing !
    m_pageLicense = new QWizardPage(m_wizard);
    m_pageLicense->setTitle(QObject::tr("Tavrisu, un logiciel libre"));

    label = new QLabel(QObject::tr("<strong>Tavrisu</strong> est un logiciel libre distribué sous la <strong>licence GPL</strong>.<br />Cela signifie que vous êtes libres:<br/>- <strong>d'utiliser</strong> Tavrisu comme vous voulez, où vous voulez, quelqu'en soit le but<br />- de <strong>redistribuer</strong> des copies de Tavrisu<br />- de <strong>consulter</strong> le code source<br />- de <strong>modifier</strong> ce code source et de l'adapter à vos besoins<br />- de <strong>distribuer</strong> vos modifications de Tavrisu sous licence GPL et en citant l'auteur original (moi :-p)<br />- de vous <strong>éclater</strong> en utilisant Tavrisu !<br /><br />Génial, n'est-ce pas ?"));
    label->setFont(police);
    label->setWordWrap(true);

    vbox = new QVBoxLayout();
    vbox->addWidget(label);

    m_pageLicense->setLayout(vbox);
    m_pageLicense->setButtonText(QWizard::NextButton, QObject::tr("Génial !"));
    m_wizard->addPage(m_pageLicense);


    // Creation of the third page of the wizard: user must choose where he want save the dabase file.
    m_pageSelectFolder = new QWizardPage(m_wizard);
    m_pageSelectFolder->setTitle(QObject::tr("Choisissez où placer votre musicothèque"));
    m_pageSelectFolder->setButtonText(QWizard::FinishButton, tr("Créer !"));

    label = new QLabel(QObject::tr("Afin de gérer pour le mieux votre musicothèque, Tavrisu utilise un unique fichier où il stocke les références de tous vos morceaux.<br />Choisissez où enregistrer ce fichier:"));
    label->setFont(police);
    label->setWordWrap(true);

    QHBoxLayout* hbox = new QHBoxLayout;

    QLabel* labelChemin = new QLabel(QObject::tr("Chemin: "));
    hbox->addWidget(labelChemin);

    m_lineEdit = new QLineEdit(m_path);
    m_lineEdit->setReadOnly(true);
    hbox->addWidget(m_lineEdit);

    QPushButton* button = new QPushButton("...");
    button->setFixedWidth(30);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(openDialogSelectFolder()));
    hbox->addWidget(button);

    vbox = new QVBoxLayout();
    vbox->addWidget(label);
    vbox->addLayout(hbox);

    m_pageSelectFolder->setLayout(vbox);
    m_wizard->addPage(m_pageSelectFolder);

    m_wizard->show();
}


void FirstUse::openDialogSelectFolder()
{
    QString selectedPath;

    selectedPath = QFileDialog::getExistingDirectory(m_pageSelectFolder, QObject::tr("Selectionnez un dossier"), m_path);

    if(!selectedPath.isEmpty())
        m_path = selectedPath;

    m_lineEdit->setText(m_path);
}


void FirstUse::createDatabase()
{
    if(m_base->create(m_path + "/Tavrisu.db")) // The database has been created
    {
        if(m_base->open(m_path + "/Tavrisu.db"))
        {
            if(m_base->simpleQuery("CREATE TABLE artistes (`id` int(11), `nom` varchar(100), `type` varchar(10), `photo` text, PRIMARY KEY(id))")
                    && m_base->simpleQuery("CREATE TABLE albums (`id` int(11), `titre` varchar(100), `artiste_id` int(11), `publication` int(11), `photo` text, PRIMARY KEY(id))")
                    && m_base->simpleQuery("CREATE TABLE pistes (`id` int(11), `titre` varchar(100), `artiste_id` int(11), `album_id` int(11), `numero` int(11), `path` text, `duration` int(11), PRIMARY KEY(id))")
                    && m_base->simpleQuery("INSERT INTO artistes VALUES(0, \"\", \"\", \"\")")
                    && m_base->simpleQuery("INSERT INTO albums VALUES(0, \"\", 0, 0, \"\")"))
            {
                QMessageBox::information(0, tr("Création du fichier"), tr("La création du fichier a réussi.<br />Bonne utilisation de Tavrisu !"));
                saveNewPath();
                emit finish();
            }
            else
            {
                m_base->close();
                QFile::remove(m_path + "/Tavrisu.db");
                QMessageBox::critical(0, tr("Création du fichier"), tr("<strong>Erreur:</strong><br />Erreur lors de l'initialisation de la base de données: <i>") + m_base->lastError() + tr("</i>"));
                emit cancel();
            }
        }
        else
        {
            m_base->close();
            QFile::remove(m_path + "/Tavrisu.db");
            QMessageBox::critical(0, tr("Création du fichier"), tr("<strong>Erreur:</strong><br />Erreur lors de l'ouverture: <i>") + m_base->lastError() + tr("</i>"));
            emit cancel();
        }
    }
    else
    {
        QMessageBox::critical(0, tr("Création du fichier"), tr("<strong>Erreur:</strong><br />Erreur lors de la création: <i>") + m_base->lastError() + tr("</i>"));
        emit cancel();
    }
}


void FirstUse::saveNewPath()
{
    m_settings->setValue("Chemin", m_path + "/Tavrisu.db");
    m_settings->setValue("FirstUse", false);
    m_settings->setValue("Version", "0.1b4");
    m_settings->setValue("lastArtistId", 0);
    m_settings->setValue("lastAlbumId", 0);
    m_settings->setValue("lastTrackId", 0);
    m_settings->setValue("interface", "Default");
    m_settings->setValue("searchUpdate", true);
}

