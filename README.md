#Tavrisu
Tavrisu is an easy and modulable audio player. 

Easy because you become the master of your musical library: you manage tracks attributes: artist, album, year, track number... Tavrisu is not based on ID3 Tags. 

Modulable because you can create your own interface (however you need to know using C++ and Qt).

![Screenshot](http://tavrisu.files.wordpress.com/2013/07/0-1b1-plugin-default.png)

Nowadays this project is not maintened.

##Coding
This project is coded in C++ with the Qt framework version 4.8.2. It uses [QlibVLC](http://philippemilink.github.io/QlibVLC/) as sound library and SQLite as database system. The class QSQLiteCom makes the communication between the software and the database.

The folder *Core* contains the core of the software: how it works. It manages the sound system (QlibVLC), the database communication (QSqliteCom), the musical library (NewArtist, NewMusic, AlbumDialog...), the initialization of the database (Init*), the export and the import of the database.

The folder *PluginDefault* contains the interface used by default (you can see it on the screenshot). This plugin is separeted in two classes (but it isn't an obligation !): Default manages the list of musics and Player represents the player at left. The plugin needs the QSqliteCom library, to get the list of tracks and necessary informations.

##Known issues
* the export in XML format can be problematic because values are not escaped.
* when a music ends, the software doesn't always respect the order as it is defined in the list. 
* we can run two instances of Tavrisu, and it is not a feature !
* the communication with the database can be very slow for modifications (three seconds to edit an album !). I tested performances of the Qt SQLite driver and of the native library: and I think it is better to use the native library: to change !

##More informations
* [The blog of the project](http://tavrisu.wordpress.com/) (in french): I shared the progession of the project.
* [Download a beta setup](http://tavrisu.wordpress.com/2013/10/23/lancement-de-la-premiere-beta/), for Windows only.

