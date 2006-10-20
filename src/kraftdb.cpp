/***************************************************************************
                          KraftDB.cpp  -
                             -------------------
    begin                : Die Feb 3 2004
    copyright            : (C) 2004 by Klaas Freitag
    email                : freitag@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstaticdeleter.h>

#include <qfile.h>
#include <qsqldatabase.h>
#include <qsqlcursor.h>
#include <qsqlquery.h>
#include <qstringlist.h>
#include <qregexp.h>

#include "version.h"
#include "kraftdb.h"
#include "dbids.h"

#include "katalogsettings.h"

#define DB_DRIVER "QMYSQL3"
// #define DB_DRIVER "QSQLITE"

static KStaticDeleter<KraftDB> selfDeleter;

KraftDB* KraftDB::mSelf = 0;

KraftDB::KraftDB()
  :QObject (), m_db( 0 )
{
  bool success = true;

  m_db = QSqlDatabase::addDatabase( DB_DRIVER );
  if ( ! m_db || m_db->isOpenError() )
  {
    kdError() <<  "Failed to connect to the database driver: "
              << m_db->lastError().text() << endl;
    success = false;
  }
  if ( success ) {
    const QString dbFile = KatalogSettings::dbFile();
    if( dbFile.isEmpty() ) {
      kdError() << "Database name is not set!" << endl;
      QSqlDatabase::removeDatabase( m_db );
      success = false;
    } else {
      kdDebug() << "Try to open database " << dbFile << endl;
      m_db->setDatabaseName( dbFile );
      m_db->setUserName( KatalogSettings::dbUser() );
      m_db->setPassword( KatalogSettings::dbPassword() );
      m_db->setHostName( KatalogSettings::dbServerName() );

      if ( m_db->open() ) {
        // Database successfully opened; we can now issue SQL commands.
        kdDebug() << "Database " << dbFile << " opened successfully" << endl;
      } else {
        kdError() << "## Could not open database " << dbFile << ": "
                  << m_db->lastError().text() << endl;
        success = false;
      }
    }
  }
}

KraftDB *KraftDB::self()
{
  if ( !mSelf ) {
    selfDeleter.setObject( mSelf, new KraftDB() );
  }
  return mSelf;
}

QSqlDatabase *KraftDB::getDB()
{
  return m_db;
}

dbID KraftDB::getLastInsertID()
{
    if( ! m_db ) return 0;

    QSqlQuery query("SELECT LAST_INSERT_ID()");
    int id = -1;
    if( query.next() ) {
        id = query.value(0).toInt();
    }

    return dbID(id);
}

QStringList KraftDB::wordList( const QString& selector, StringMap replaceMap )
{
  QStringList re;

  if( ! m_db ) return re;

  QSqlCursor cur( "wordLists" ); // Specify the table/view name
  // cur.setMode( QSqlCursor::ReadOnly );
  cur.select( QString( "category='%1'" ).arg( selector ) );
  while ( cur.next() ) {
    QString w = cur.value( "word" ).toString();

    StringMap::Iterator it;
    for ( it = replaceMap.begin(); it != replaceMap.end(); ++it ) {
      const QString key = it.key().utf8();
      const QString rep = it.data().utf8();
      w.replace( key, rep );
    }
    re << w;
  }
  return re;
}

void KraftDB::checkSchemaVersion()
{
  QSqlQuery q( "SELECT dbSchemaVersion FROM kraftsystem" );
  emit statusMessage( i18n( "Checking Database Schema Version" ) );

  int currentVer = 0;
  if ( q.next() ) {
    currentVer = q.value( 0 ).toInt();
  }

  if ( currentVer < KRAFT_REQUIRED_SCHEMA_VERSION ) {
    kdDebug() << "Kraft Schema Version not sufficient: " << currentVer << endl;

    KStandardDirs stdDirs;

    while ( currentVer < KRAFT_REQUIRED_SCHEMA_VERSION ) {
      ++currentVer;
      const QString migrateFilename = QString( "%1_dbmigrate.sql" ).arg( currentVer );
      kdDebug() << "Searching for file: " << migrateFilename << endl;
      QString findFile = "kraft/dbmigrate/" + migrateFilename;
      QString sqlFile = stdDirs.findResource( "data", findFile );
      if ( ! sqlFile.isEmpty() ) {
        kdDebug() << "Opening migration file " << sqlFile << endl;

        QFile f( sqlFile );
        if ( !f.open( IO_ReadOnly ) ) {
          kdError() << "Could not open " << sqlFile << endl;
        } else {
          QTextStream ts( &f );
          ts.setEncoding( QTextStream::UnicodeUTF8 );

          while ( !ts.atEnd() ) {
            QString sql = ts.readLine();
            if ( !sql.isEmpty() ) {
              QRegExp reg( "\\s*#\\s*message:\\s*" );
              int pos = sql.lower().find( reg );
              if ( pos > -1 ) {
                // QString msg = sql.right( sql.length()-pos );
                QString msg = sql.remove ( reg );
                kdDebug() << "Msg: >" << msg << "<" << endl;
                emit statusMessage( msg );
              } else {
                if ( q.exec( sql ) ) {
                  kdDebug() << "Successfull SQL Command: " << sql << endl;
                } else {
                  kdDebug() << "Failed SQL Command: " << sql << endl;
                }
              }
            }
          }
          f.close();
        }
      } else {
        kdDebug() << "No migrate script for step " << currentVer << endl;
        emit statusMessage( i18n( "Migration Script not found" ) );
      }
    }
    /* Now update to the required schema version */
    q.exec( "UPDATE kraftsystem SET dbSchemaVersion="
            + QString::number( KRAFT_REQUIRED_SCHEMA_VERSION ) );
  } else {
    kdDebug() << "Kraft Schema Version is ok: " << currentVer << endl;
    emit statusMessage( i18n( "Database Schema Version ok" ) );
  }
}


// not yet used.
void KraftDB::checkInit()
{
  kdDebug() << "** Database init **" << endl;
  if( m_db ) {
    kdError() << "global db handle is not zero, can not init!" << endl;
  }

        // The database is not yet open. Thus we can move the file away
  QString dbFile = KatalogSettings::dbFile();
  kdDebug() << "Database file is " << dbFile << endl;
  if( ! dbFile.isEmpty() ) {
            // backup this file
    // dBFileBackup( dbFile );
  } else {
    QString dbName = KatalogSettings::defaultDbName();
    QString dbPath = KatalogSettings::dbPath();
    if( dbPath.isEmpty() ) {
      KStandardDirs stdDirs;
      dbPath = stdDirs.saveLocation( "data" );
    }

    QString dbFile = dbPath + dbName;
    kdDebug() << "Database file: " << dbFile << endl;
    KatalogSettings::setDbFile( dbFile );

  }
}

QString KraftDB::qtDriver()
{
    return DB_DRIVER;
}

KraftDB::~KraftDB(){
}

#include "kraftdb.moc"
