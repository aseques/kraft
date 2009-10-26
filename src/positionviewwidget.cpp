/***************************************************************************
        positionviewwidget - inherited class for doc position views.
                             -------------------
    begin                : 2006-02-20
    copyright            : (C) 2006 by Klaas Freitag
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

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QColor>
#include <QLayout>
#include <QToolTip>
#include <QPaintEvent>
#include <QRegExp>
#include <QPainter>
#include <QPixmap>
#include <qdrawutil.h>

//Added by qt3to4:
#include <Q3PtrList>
#include <Q3PopupMenu>
#include <q3widgetstack.h>

#include <kglobal.h>
#include <klocale.h>
#include <knuminput.h>
#include <ktextedit.h>
#include <kmenu.h>
#include <kiconloader.h>

#include "docposition.h"
#include "positionviewwidget.h"
#include "unitmanager.h"
#include "geld.h"
#include "kraftsettings.h"
#include "defaultprovider.h"
#include "kraftdb.h"
#include "positiontagdialog.h"
#include "tagman.h"


PositionViewWidget::PositionViewWidget()
    :QWidget(), Ui_positionWidget(),
   mModified( false ),
   m_skipModifiedSignal( false ),
   mToDelete(false),
   mOrdNumber(0),
   mPositionPtr( 0 ),
   mExecPopup( new KMenu( this ) ) ,
   mStateSubmenu( 0 ),
   mState( Active ),
   mKind( Normal ),
   mPositionPriceValid( false ),
   mLocale( 0 )
{
  setupUi( this );
  m_sbUnitPrice->setMinimum( -99999.99 );
  m_sbUnitPrice->setMaximum( 99999.99 );
  m_sbUnitPrice->setDecimals( 2 );

  m_sbAmount->setMinimum( -99999.99 );
  m_sbAmount->setMaximum( 99999.99 );
  m_sbAmount->setDecimals( 2 );

  mDiscountPercent->setMinimum( -9999.99 );
  mDiscountPercent->setMaximum( 9999.99 );
  mDiscountPercent->setDecimals( 2 );

  pbExec->setCheckable( false );
  pbTagging->setCheckable( false );
  pbTagging->setIcon( SmallIcon( "flag" ) );

  connect( m_sbAmount, SIGNAL( valueChanged( double )),
             this, SLOT( slotRefreshPrice( ) ) );
  connect( m_sbUnitPrice, SIGNAL( valueChanged( double )),
             this, SLOT( slotRefreshPrice( ) ) );
  connect( mDiscountPercent, SIGNAL( valueChanged( double ) ),
           this, SLOT( slotRefreshPrice() ) );

  connect( pbExec, SIGNAL( pressed() ),     this,  SLOT( slotExecButtonPressed() ) );
  connect( pbTagging,  SIGNAL( pressed() ), this,  SLOT( slotTaggingButtonPressed() ) );


  /* modified signals */
  connect( m_cbUnit,      SIGNAL( activated(int) ), this,      SLOT( slotModified() ) );
  connect( m_teFloskel,   SIGNAL( textChanged() ), this,       SLOT( slotModified() ) );

  connect( m_sbAmount,    SIGNAL( valueChanged(double)), this, SLOT( slotModified() ) );
  connect( m_sbUnitPrice, SIGNAL( valueChanged(double)), this, SLOT( slotModified() ) );
  connect( mDiscountPercent, SIGNAL( valueChanged( double ) ), this, SLOT( slotModified() ) );
  connect( mDiscountTag,  SIGNAL( activated( int ) ), this,    SLOT( slotModified() ) );

  // mExecPopup->insertTitle( i18n("Position Actions") );

  // state submenu:
  mStateSubmenu = mExecPopup->addMenu(i18n( "Position Kind" ));
  mStateSubmenu->addAction( i18n( "Normal" ), this, SIGNAL( positionStateNormal() ) );
  mStateSubmenu->addAction( KIcon( "alternative" ),
                            i18n( "Alternative" ), this, SIGNAL( positionStateAlternative() ) );
  mStateSubmenu->addAction( KIcon( "demand" ),
                            i18n( "On Demand" ), this, SIGNAL( positionStateDemand() ) );

  mExecPopup->addSeparator();


  mExecPopup->addSeparator();

  mExecPopup->addAction(  KIcon("up"),
                           i18n("Move Up"),         this, SIGNAL( moveUp() ) );
  mExecPopup->addAction(  KIcon("down"),
                           i18n("Move Down"),       this, SIGNAL( moveDown() ) );
  mLockId = mExecPopup->addAction(  KIcon("encrypted"),
                           i18n("Lock Position"),   this, SIGNAL( lockPosition() ) );
  mUnlockId = mExecPopup->addAction(  KIcon("decrypted"),
                           i18n("Unlock Position"), this, SIGNAL( unlockPosition() ) );
  mDeleteId = mExecPopup->addAction(  KIcon("remove"),
                           i18n("Delete Position"), this, SIGNAL( deletePosition() ) );


  connect( this, SIGNAL( positionStateNormal() ), this, SLOT( slotSetPositionNormal() ) );
  connect( this, SIGNAL( positionStateAlternative() ), this, SLOT( slotSetPositionAlternative() ) );
  connect( this, SIGNAL( positionStateDemand() ), this, SLOT( slotSetPositionDemand() ) );


  connect( this, SIGNAL( lockPosition() ),   this, SLOT( slotLockPosition() ) );
  connect( this, SIGNAL( unlockPosition() ), this, SLOT( slotUnlockPosition() ) );

  connect( mExecPopup, SIGNAL( aboutToShow() ), this, SLOT( slotMenuAboutToShow() ) );
  connect( mExecPopup, SIGNAL( aboutToHide() ), this, SLOT( slotMenuAboutToHide() ) );

  mUnlockId->setEnabled(false);
  lStatus->setPixmap( QPixmap() );
  lKind->setPixmap( QPixmap() );
}

void PositionViewWidget::setDocPosition( DocPositionBase *dp, KLocale* loc )
{
  if( ! dp ) {
    kError() << "setDocPosition got empty position!" << endl;
    return;
  }

  DocPosition *pos = static_cast<DocPosition*>(dp);

  mPositionPtr = pos;

  m_skipModifiedSignal = true;

  m_teFloskel->setText( pos->text() );

  lStatus->hide();
  lKind->hide();

  setLocale( loc );
  AttributeMap amap = dp->attributes();

  if( dp->type() == DocPositionBase::Position ) {
    positionDetailStack->raiseWidget( positionPage );

    m_sbAmount->blockSignals( true );
    m_sbAmount->setValue( pos->amount() );
    m_sbAmount->blockSignals( false );
    m_cbUnit->setCurrentIndex(m_cbUnit->findText( pos->unit().einheitSingular() ));

    m_sbUnitPrice->blockSignals( true );
    m_sbUnitPrice->setValue( pos->unitPrice().toDouble() );
    m_sbUnitPrice->blockSignals( false );

    if ( amap.contains( DocPosition::Kind ) ) {
      Attribute kind = amap[DocPosition::Kind];
      const QString kindStr = kind.value().toString();
      if ( kindStr == kindString( Alternative ) ) {
        slotSetPositionAlternative();
      } else if ( kindStr == kindString( Demand ) ) {
        slotSetPositionDemand();
      } else {
        kDebug() << "Unknown position kind!" << endl;
      }
    }
    kDebug() << "Setting position ptr. in viewwidget: " << pos << endl;
  } else if ( dp->type() == DocPositionBase::ExtraDiscount ) {
    positionDetailStack->raiseWidget( discountPage );
    // kDebug() << " " << dp->type()<< endl;
    Attribute discount = amap[DocPosition::Discount];
    mDiscountPercent->setValue( discount.value().toDouble() );

    QString selTag;
    if ( amap.contains( DocPosition::ExtraDiscountTagRequired ) ) {
      Attribute tagSelector = amap[DocPosition::ExtraDiscountTagRequired];
      selTag = tagSelector.value().toString();
    }

    /* Fill and set the extra discount selection combo */
    const QString allPos = i18n( "all items" );
    mDiscountTag->insertItem( 0, allPos ); // , i18n( "Overall Position Discount" ) );
    QStringList taglist = TagTemplateMan::self()->allTagTemplates();
    QString currentEntry = allPos;

    for ( QStringList::Iterator tagIt = taglist.begin(); tagIt != taglist.end(); ++tagIt ) {
      QString tagger;
      TagTemplate tmpl = TagTemplateMan::self()->getTagTemplate( *tagIt );
      QPixmap pix( 16, 12 );
      pix.fill( tmpl.color() );
      tagger = i18n( "%1-tagged items" ).arg( *tagIt );
      mDiscountTag->insertItem( 255, tagger );
      // ExtendedComboItem *item = mDiscountTag->insertEntry( tagger, i18n( "sum up only items marked with '%1'" ).arg( *tagIt ) );
      // item->setPixmap( pix );
      if ( selTag == *tagIt ) {
        currentEntry = tagger;
      }
    }
    mDiscountTag->setCurrentIndex(mDiscountTag->findText( currentEntry ));
  } else {
    kDebug() << "unknown doc position type " << dp->type()<< endl;
  }
  slotSetOverallPrice( currentPrice() );

  // set tags marked
  mTags = dp->tags();
  slotUpdateTagToolTip();

  m_skipModifiedSignal = false;
}

void PositionViewWidget::slotUpdateTagToolTip()
{
  QString tip;
  bool first = true;

  if ( mTags.count() == 1 ) {
    tip = i18n( "Tag: %1" ).arg( mTags.first() );
  } else if ( mTags.count() > 1 ) {
    tip = i18n( "Tags:<ul>" );
    for ( QStringList::Iterator it = mTags.begin(); it != mTags.end(); ++it ) {
      if ( first ) {
        tip += QString( "<li>%1</li>" ).arg( *it );
        first = false;
      } else {
        tip += QString( "<li>%1</li>" ).arg( *it );
      }
    }
    tip += "</ul>";
  } else {
    tip = i18n( "No tags assigned yet." );
  }

  pbTagging->setToolTip( tip );
}

QString PositionViewWidget::extraDiscountTagRestriction()
{
  QStringList taglist = TagTemplateMan::self()->allTagTemplates();

  uint currentItem = mDiscountTag->currentIndex();
  if ( currentItem > 0 && currentItem <= taglist.count() ) {
    // subtract one for the "all items" entry in the combo box at first position
    currentItem -= 1;
    return taglist[currentItem];
  } else {
    kDebug() << "taglist index possibly out of range!";
  }
  return QString();
}

void PositionViewWidget::setLocale( KLocale *loc )
{
  mLocale = loc;
  const QString currSymbol = mLocale->currencySymbol();
  m_sbUnitPrice->setPrefix( currSymbol + " " );
  slotSetOverallPrice( currentPrice() );
}

void PositionViewWidget::slotTaggingButtonPressed()
{
  kDebug() << "opening tagging dialog" << endl;

  PositionTagDialog dia( 0 );

  dia.setPositionTags( mTags );
  if ( dia.exec() ) {
    mTags = dia.getSelectedTags();
    slotUpdateTagToolTip();
    slotModified();
    update();
    kDebug() << "Selected tags: " << mTags.join( ", " ) << endl;
  }
}


void PositionViewWidget::slotExecButtonPressed()
{
  kDebug() << "Opening Context Menu over exec button" << endl;

  // set bg-color
  mExecPopup->popup( QWidget::mapToGlobal( pbExec->pos() ) );

}

void PositionViewWidget::slotMenuAboutToShow()
{
  QPalette palette;
  palette.setCurrentColorGroup(QPalette::Disabled);
  setPalette( palette );
  m_sumLabel->setPalette( palette );
  m_labelPosition->setPalette( palette );
  m_sumLabel->setPalette( palette );
  m_sbAmount->setPalette( palette );
  m_cbUnit->setPalette( palette );
  m_sbUnitPrice->setPalette( palette );
  discountPage->setPalette( palette );
  mDiscountPercent->setPalette( palette );
  mDiscountLabel->setPalette( palette );
  discountPage->setPalette( palette );
  positionPage->setPalette( palette );
}

void PositionViewWidget::slotMenuAboutToHide()
{
  kDebug() << "Set normal again" << endl;
  QPalette palette;
  palette.setCurrentColorGroup(QPalette::Active);
  setPalette( palette );
  m_sumLabel->setPalette( palette );
  m_labelPosition->setPalette( palette );
  m_sumLabel->setPalette( palette );
  m_sbAmount->setPalette( palette );
  m_cbUnit->setPalette( palette );
  m_sbUnitPrice->setPalette( palette );
  discountPage->setPalette( palette );
  mDiscountPercent->setPalette( palette );
  mDiscountLabel->setPalette( palette );
  discountPage->setPalette( palette );
  positionPage->setPalette( palette );

}

void PositionViewWidget::slotLockPosition( )
{
  slotSetState( Locked );
}

void PositionViewWidget::slotUnlockPosition( )
{
  slotSetState( Active );
}

void PositionViewWidget::slotEnableKindMenu( bool s )
{
  mStateSubmenu->setEnabled( s );
}

QString PositionViewWidget::stateString( const State& state ) const
{
  QString str;

  if( state == Active ) {
    str = i18n( "Active" );
  } else if( state == New ) {
    str = i18n( "New" );
  } else if( state == Deleted ) {
    str = i18n( "Deleted" );
  } else if( state == Locked ) {
    str = i18n( "Locked" );
  } else {
    str = i18n( "Unknown" );
  }
  return str;
}

void PositionViewWidget::slotSetState( State state )
{
  mState = state;
  kDebug() << "Setting new widget state " << stateString( state ) << endl;
  if( state == Active ) {
    mLockId->setEnabled( true );
    mUnlockId->setEnabled( false );

    lStatus->hide();
    lStatus->setPixmap( QPixmap() );
    mToDelete = false;
    slotSetEnabled( true );
  } else if( state == New ) {
    lStatus->setPixmap( SmallIcon( "filenew" ) );
    lStatus->show();
  } else if( state == Deleted ) {
    lStatus->setPixmap( SmallIcon( "remove" ) );
    lStatus->show();
    mToDelete = true;
    slotSetEnabled( false );
  } else if( state == Locked ) {
    mLockId->setEnabled( false );
    mUnlockId->setEnabled( true );
    slotSetEnabled( false );
    lStatus->setPixmap( SmallIcon( "encrypted" ) );
    lStatus->show();
  }
}

void PositionViewWidget::setOrdNumber( int o )
{
  mOrdNumber = o;
  m_labelPosition->setText( QString("%1.").arg( mOrdNumber ) );
}

void PositionViewWidget::slotSetEnabled( bool doit )
{
  if( !doit ) {
    m_sbAmount->setEnabled( false );
    m_sbUnitPrice->setEnabled( false );
    m_labelPosition->setEnabled( false );
    m_teFloskel->setEnabled( false );
    m_sumLabel->setEnabled( false );
    m_cbUnit->setEnabled( false );
  } else {
    m_sbAmount->setEnabled( true );
    m_sbUnitPrice->setEnabled( true );
    m_labelPosition->setEnabled( true );
    m_teFloskel->setEnabled( true );
    m_sumLabel->setEnabled( true );
    m_cbUnit->setEnabled( true );
  }
}

bool PositionViewWidget::priceValid()
{
  bool isValid = true;

  if ( position()->type() == DocPosition::ExtraDiscount ) {
    isValid = mPositionPriceValid;
  }

  return isValid;
}

void PositionViewWidget::setCurrentPrice( Geld g )
{
  // do nothing for normal positions
  if ( position()->type() == DocPosition::ExtraDiscount ) {
    mPositionPrice = g;
    mPositionPriceValid = true;
  }
}

Geld PositionViewWidget::currentPrice()
{
  Geld sum;
  if ( mKind == Normal ) {
    if ( position()->type() == DocPosition::ExtraDiscount ) {
      sum = mPositionPrice;
      if ( ! mPositionPriceValid ) {
        kWarning() << "Asking for price of Discount item, but invalid!" << endl;
      }
    } else {
      double amount = m_sbAmount->value();
      Geld g( m_sbUnitPrice->value() );
      sum = g * amount;
    }
  }
  return sum;
}

Geld PositionViewWidget::unitPrice()
{
  Geld p(  m_sbUnitPrice->value() );
  return p;
}

void PositionViewWidget::slotRefreshPrice()
{
  const Geld sum = currentPrice();
  slotSetOverallPrice( sum );
  emit priceChanged( sum );
}

void PositionViewWidget::slotSetOverallPrice( Geld g )
{
  // if ( mPositionPtr->type() == DocPosition::ExtraDiscount ) {
  //   m_sumLabel->setText( "--" );
  // } else {
    m_sumLabel->setText( g.toString( mLocale ) );
    // }
}

void PositionViewWidget::slotModified()
{
  // if( mModified ) return;
  if( m_skipModifiedSignal ) return;
  kDebug() << "Modified Position!" << endl;
  QColor c( "red" );
  QPalette palette;
  palette.setColor(m_labelPosition->foregroundRole(), c);
  mModified = true;
  emit positionModified();
}

PositionViewWidget::~PositionViewWidget()
{
}

PositionViewWidgetList::PositionViewWidgetList()
  : Q3PtrList<PositionViewWidget>()
{
  setAutoDelete( true );
}

PositionViewWidget* PositionViewWidgetList::widgetFromPosition( DocPositionGuardedPtr ptr)
{
  PositionViewWidget *pvw = 0;

  for( pvw = first(); pvw; pvw = next() ) {
    if( pvw->position() == ptr ) return pvw;
  }
  return 0;
}

Geld PositionViewWidgetList::nettoPrice()
{
  PositionViewWidget *pvw = 0;
  Geld res;

  for( pvw = first(); pvw; pvw = next() ) {
    res += pvw->currentPrice();
  }
  return res;
}

void PositionViewWidget::slotSetPositionNormal()
{
  lKind->hide();
  lKind->setPixmap( QPixmap() );
  mKind = Normal;

  cleanKindString();
  slotRefreshPrice();
  emit positionModified();
}

void PositionViewWidget::cleanKindString()
{
  QString current = m_teFloskel->text();
  bool touched = false;

  if ( current.startsWith( kindLabel( Alternative ) ) ) {
    current.remove( 0, QString( kindLabel( Alternative ) ).length() );
    touched = true;
  } else if ( current.startsWith( kindLabel( Demand ) ) ) {
    current.remove( 0, QString( kindLabel( Demand ) ).length() );
    touched = true;
  }

  if ( touched ) {
    m_teFloskel->setText( current );
  }
}

void PositionViewWidget::slotSetPositionAlternative()
{
  lKind->show();
  lKind->setToolTip( i18n( "This is an alternative position."
                              " Use the position toolbox to change." ) );
  lKind->setPixmap( SmallIcon( "alternative" ) );
  mKind = Alternative;
  slotRefreshPrice();

  cleanKindString();

  m_teFloskel->insertAt( kindLabel( Alternative ), 0, 0 );

  emit positionModified();
}

void PositionViewWidget::slotSetPositionDemand()
{
  lKind->show();
  lKind->setToolTip( i18n( "This is a as required position. "
                              "Use the position toolbox to change." ) );
  lKind->setPixmap( SmallIcon( "demand" ) );
  mKind = Demand;
  slotRefreshPrice();

  cleanKindString();
  m_teFloskel->insertAt( kindLabel( Demand ), 0, 0 );

  emit positionModified();
}

// The technical label
QString PositionViewWidget::kindString( Kind k ) const
{
  Kind kind = k;

  if ( kind == Invalid ) kind = mKind;

  if ( kind == Normal )      return QString::fromLatin1( "Normal" );
  if ( kind == Demand )      return QString::fromLatin1( "Demand" );
  if ( kind == Alternative ) return QString::fromLatin1( "Alternative" );

  return QString::fromLatin1( "unknown" );
}

// The label that is prepended to a positions text
QString PositionViewWidget::kindLabel( Kind k ) const
{
  Kind kind = k;

  if ( kind == Invalid ) kind = mKind;

  QString re;
  if ( kind == Normal ) {
    re = KraftSettings::self()->self()->normalLabel();
    if ( re.isEmpty() ) re = i18n( "Normal" );
  }
  if ( kind == Demand ) {
    re = KraftSettings::self()->self()->demandLabel();
    if ( re.isEmpty() ) re = i18n( "Demand" );
  }
  if ( kind == Alternative ) {
    re = KraftSettings::self()->self()->alternativeLabel();
    if ( re.isEmpty() ) re = i18n( "Alternative" );
  }

  if ( ! re.endsWith( ": " ) ) {
    re += QString::fromLatin1( ": " );
  }
  return re;
}

void PositionViewWidget::paintEvent ( QPaintEvent *pe )
{
  QPainter *painter;
  painter = new QPainter( this );

  // visualise the tags
  QStringList taglist = tagList();
  if ( taglist.count() ) {
    int share = ( height() - 24 ) / taglist.count();
    int cnt = 0;

    for ( QStringList::Iterator it = taglist.begin(); it != taglist.end(); ++it ) {
      QString tag = *it;
      TagTemplate tagTemplate = TagTemplateMan::self()->getTagTemplate( tag );

      // QColor c = tagTemplate.color();
      // kDebug() << "color: " << c.red() << ", " << c.green() << ", " << c.blue() << endl;
      // painter->setBrush( c );

      int starty = cnt*share;
      qDrawShadeLine( painter, 3, starty, 3, starty+share-1, tagTemplate.colorGroup(), false, 1, 4 );
      cnt++;
    }
  }
  delete painter;
  // check again: Ui_positionWidget::paintEvent( pe );
}

#include "positionviewwidget.moc"

