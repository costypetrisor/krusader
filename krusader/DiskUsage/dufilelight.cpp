/***************************************************************************
                       dufilelight.cpp  -  description
                             -------------------
    copyright            : (C) 2004 by Csaba Karai
    e-mail               : krusader@users.sourceforge.net
    web site             : http://krusader.sourceforge.net
 ---------------------------------------------------------------------------
  Description
 ***************************************************************************

  A

     db   dD d8888b. db    db .d8888.  .d8b.  d8888b. d88888b d8888b.
     88 ,8P' 88  `8D 88    88 88'  YP d8' `8b 88  `8D 88'     88  `8D
     88,8P   88oobY' 88    88 `8bo.   88ooo88 88   88 88ooooo 88oobY'
     88`8b   88`8b   88    88   `Y8b. 88~~~88 88   88 88~~~~~ 88`8b
     88 `88. 88 `88. 88b  d88 db   8D 88   88 88  .8D 88.     88 `88.
     YP   YD 88   YD ~Y8888P' `8888Y' YP   YP Y8888D' Y88888P 88   YD

                                                     S o u r c e    F i l e

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dufilelight.h"
#include "radialMap/radialMap.h"
#include <kpopupmenu.h>
#include <klocale.h>
#include <kinputdialog.h>

#define SCHEME_POPUP_ID    6730

DUFilelight::DUFilelight( DiskUsage *usage, const char *name )
  : RadialMap::Widget( usage, name ), diskUsage( usage ), currentDir( 0 )
{
   connect( diskUsage, SIGNAL( enteringDirectory( Directory * ) ), this, SLOT( slotDirChanged( Directory * ) ) );
   connect( diskUsage, SIGNAL( clearing() ), this, SLOT( clear() ) );
   connect( diskUsage, SIGNAL( changed( File * ) ), this, SLOT( slotChanged( File * ) ) );
   connect( diskUsage, SIGNAL( deleted( File * ) ), this, SLOT( slotChanged( File * ) ) );
   connect( this, SIGNAL( activated( const KURL& ) ), this, SLOT( slotActivated( const KURL& ) ) );
}

void DUFilelight::slotDirChanged( Directory *dir )
{
  if( currentDir != dir )
  {
    currentDir = dir;
    
    File::setBaseURL( diskUsage->getBaseURL() );
    invalidate( false );
    create( dir );
    refreshNeeded = false;
  }
}

void DUFilelight::clear()
{
  invalidate( false );
  currentDir = 0;
}

void DUFilelight::slotActivated( const KURL& url )
{
  KURL baseURL = diskUsage->getBaseURL();
  
  if( !baseURL.path().endsWith("/") )
    baseURL.setPath( baseURL.path() + "/" );
    
  QString relURL = KURL::relativeURL( baseURL, url );
  
  if( relURL.endsWith( "/" ) )
    relURL.truncate( relURL.length() - 1 );
  
  Directory * dir = diskUsage->getDirectory( relURL );
  if( dir != 0 && dir != currentDir )
  {
    currentDir = dir;
    diskUsage->changeDirectory( dir );  
  }
}

void DUFilelight::mousePressEvent( QMouseEvent *event )
{
   if( event->button() == Qt::RightButton )
   {
     File * file = 0;
     
     const RadialMap::Segment * focus = focusSegment();
     
     if( focus && !focus->isFake() && focus->file() != currentDir )
       file = (File *)focus->file();

     KPopupMenu filelightPopup;
     filelightPopup.insertItem( i18n("Zoom In"),  this, SLOT( zoomIn() ) );
     filelightPopup.insertItem( i18n("Zoom Out"), this, SLOT( zoomOut() ) );
     
     KPopupMenu schemePopup;           
     schemePopup.insertItem( i18n("Rainbow"),       this, SLOT( schemeRainbow() ) );
     schemePopup.insertItem( i18n("High Contrast"), this, SLOT( schemeHighContrast() ) );
     schemePopup.insertItem( i18n("KDE"),           this, SLOT( schemeKDE() ) );

     filelightPopup.insertItem( QPixmap(), &schemePopup, SCHEME_POPUP_ID );
     filelightPopup.changeItem( SCHEME_POPUP_ID, i18n( "Scheme" ) );     

     filelightPopup.insertItem( i18n("Increase contrast"), this, SLOT( increaseContrast() ) );
     filelightPopup.insertItem( i18n("Decrease contrast"), this, SLOT( decreaseContrast() ) );
          
     int aid = filelightPopup.insertItem( i18n("Use antialiasing" ), this, SLOT( changeAntiAlias() ) );
     filelightPopup.setItemChecked( aid, Filelight::Config::antiAliasFactor > 1 );
     
     int sid = filelightPopup.insertItem( i18n("Show small files" ), this, SLOT( showSmallFiles() ) );
     filelightPopup.setItemChecked( sid, Filelight::Config::showSmallFiles );
     
     int vid = filelightPopup.insertItem( i18n("Vary label font sizes" ), this, SLOT( varyLabelFontSizes() ) );
     filelightPopup.setItemChecked( vid, Filelight::Config::varyLabelFontSizes );

     filelightPopup.insertItem( i18n("Minimum font size"), this, SLOT( minFontSize() ) );     
          
     diskUsage->rightClickMenu( file, &filelightPopup, i18n( "Filelight" ) );
     return;     
   }
   RadialMap::Widget::mousePressEvent( event );
}
  
void DUFilelight::setScheme( Filelight::MapScheme scheme )
{
  Filelight::Config::scheme = scheme;
  Filelight::Config::write();    
  slotRefresh();
}

void DUFilelight::increaseContrast()
{
  if( ( Filelight::Config::contrast += 10 ) > 100 )
    Filelight::Config::contrast = 100;
  
  Filelight::Config::write();    
  slotRefresh();
}

void DUFilelight::decreaseContrast()
{
  if( ( Filelight::Config::contrast -= 10 ) > 100 )
    Filelight::Config::contrast = 0;
  
  Filelight::Config::write();
  slotRefresh();
}

void DUFilelight::changeAntiAlias()
{
  Filelight::Config::antiAliasFactor = 1 + ( Filelight::Config::antiAliasFactor == 1 );
  Filelight::Config::write();    
  slotRefresh();
}

void DUFilelight::showSmallFiles()
{
  Filelight::Config::showSmallFiles = !Filelight::Config::showSmallFiles;
  Filelight::Config::write();    
  slotRefresh();
}

void DUFilelight::varyLabelFontSizes()
{
  Filelight::Config::varyLabelFontSizes = !Filelight::Config::varyLabelFontSizes;
  Filelight::Config::write();    
  slotRefresh();
}

void DUFilelight::minFontSize()
{
  bool ok = false;
  
  int result = KInputDialog::getInteger( i18n( "Krusader::Filelight" ),
    i18n( "Minimum font size" ), (int)Filelight::Config::minFontPitch, 1, 100, 1, &ok );

  if ( ok )
  {
    Filelight::Config::minFontPitch = (uint)result;
    
    Filelight::Config::write();    
    slotRefresh();
  }
}

void DUFilelight::slotRefresh() 
{ 
  refreshNeeded = false;
  if( currentDir )
  {
    invalidate( false );
    create( currentDir );
  }
}

void DUFilelight::slotChanged( File * )
{
   if( !refreshNeeded )
   {
     invalidate( false );
     refreshNeeded = true;
     QTimer::singleShot( 0, this, SLOT( slotRefresh() ) );
   }
}

#include "dufilelight.moc"
