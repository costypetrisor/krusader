/***************************************************************************
                          listpanelactions.cpp
                       -------------------
copyright            : (C) 2000 by Shie Erlich & Rafi Yanai
copyright            : (C) 2010 by Jan Lepper
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

#include "listpanelactions.h"

#include "listpanel.h"
#include "panelfunc.h"
#include "krviewfactory.h"
#include "../filemanagerwindow.h"
#include "../Dialogs/krdialogs.h"
#include "../KViewer/krviewer.h"

#include <QSignalMapper>
#include <QActionGroup>
#include <klocale.h>
#include <kactioncollection.h>


ListPanelActions *ListPanelActions::self = 0;

KAction *ListPanelActions::actCompDirs = 0;
KAction *ListPanelActions::actSync = 0;
KAction *ListPanelActions::actProperties = 0;
KAction *ListPanelActions::actPack = 0;
KAction *ListPanelActions::actUnpack = 0;
KAction *ListPanelActions::actTest = 0;
KAction *ListPanelActions::actCopy = 0;
KAction *ListPanelActions::actPaste = 0;
KAction *ListPanelActions::actCalculate = 0;
KAction *ListPanelActions::actCreateChecksum = 0;
KAction *ListPanelActions::actMatchChecksum = 0;
KAction *ListPanelActions::actSelect = 0;
KAction *ListPanelActions::actSelectAll = 0;
KAction *ListPanelActions::actUnselect = 0;
KAction *ListPanelActions::actUnselectAll = 0;
KAction *ListPanelActions::actInvert = 0;
KAction *ListPanelActions::actFTPConnect = 0;
KAction *ListPanelActions::actFTPNewConnect = 0;
KAction *ListPanelActions::actFTPDisconnect = 0;
KAction *ListPanelActions::actAllFilter = 0;
// KAction *ListPanelActions::actExecFilter = 0;
KAction *ListPanelActions::actCustomFilter = 0;
KAction *ListPanelActions::actRoot = 0;
KAction *ListPanelActions::actDirUp = 0;
KAction *ListPanelActions::actSyncBrowse = 0;
KAction *ListPanelActions::actF2 = 0;
KAction *ListPanelActions::actF3 = 0;
KAction *ListPanelActions::actF4 = 0;
KAction *ListPanelActions::actF5 = 0;
KAction *ListPanelActions::actF6 = 0;
KAction *ListPanelActions::actF7 = 0;
KAction *ListPanelActions::actF8 = 0;
KAction *ListPanelActions::actF9 = 0;
KAction *ListPanelActions::actShiftF5 = 0;
KAction *ListPanelActions::actShiftF6 = 0;
KAction *ListPanelActions::actLocationBar = 0;
KAction *ListPanelActions::actJumpBack = 0;
KAction *ListPanelActions::actSetJumpBack = 0;
KAction *ListPanelActions::actHistoryBackward = 0;
KAction *ListPanelActions::actHistoryForward = 0;
KAction *ListPanelActions::actCancelRefresh = 0;


ListPanelActions::ListPanelActions(QObject *parent, FileManagerWindow *mainWindow) :
        ActionsBase(parent, mainWindow)
{
    if(self)
        abort();
    self = this;

    // set view type
    QSignalMapper *mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), SLOT(setView(int)));
    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    QList<KrViewInstance*> views = KrViewFactory::registeredViews();
    for(int i = 0; i < views.count(); i++) {
        KrViewInstance *inst = views[i];
        KAction *action = new KAction(KIcon(inst->icon()), inst->description(), group);
        action->setShortcut(inst->shortcut());
        action->setCheckable(true);
        connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
        mapper->setMapping(action, inst->id());
        _mainWindow->actions()->addAction("view" + QString::number(i), action);
        setViewActions.insert(inst->id(), action);
    }

    // standard actions
    actHistoryBackward = stdAction(KStandardAction::Back, SLOT(historyBackward()));
    actHistoryForward = stdAction(KStandardAction::Forward, SLOT(historyForward()));
    //FIXME: second shortcut for up: see actDirUp
    //   KStandardAction::up( this, SLOT( dirUp() ), actionCollection )->setShortcut(Qt::Key_Backspace);
    /* Shortcut disabled because of the Terminal Emulator bug. */
    actDirUp = stdAction(KStandardAction::Up, SLOT(dirUp()));
    stdAction(KStandardAction::Home, SLOT(home()));
    stdAction(KStandardAction::Cut, SLOT(cut()));
    actCopy = stdAction(KStandardAction::Copy, SLOT(copy()));
    actPaste = stdAction(KStandardAction::Paste, SLOT(paste()));

    // Fn keys
    actF2 = action(i18n("Start Terminal Here"), "terminal", Qt::Key_F2, SLOT(terminal()) , "F2_Terminal");
    actF3 = action(i18n("View File"), 0, Qt::Key_F3, SLOT(view()), "F3_View");
    actF4 = action(i18n("Edit File"), 0, Qt::Key_F4, SLOT(edit()) , "F4_Edit");
    actF5 = action(i18n("Copy..."), 0, Qt::Key_F5, SLOT(copyFiles()) , "F5_Copy");
    actF6 = action(i18n("Move..."), 0, Qt::Key_F6, SLOT(moveFiles()) , "F6_Move");
    actShiftF5 = action(i18n("Copy by queue..."), 0, Qt::SHIFT + Qt::Key_F5, SLOT(copyFilesByQueue()) , "F5_Copy_Queue");
    actShiftF6 = action(i18n("Move by queue..."), 0, Qt::SHIFT + Qt::Key_F6, SLOT(moveFilesByQueue()) , "F6_Move_Queue");
    actF7 = action(i18n("New Directory..."), "folder-new", Qt::Key_F7, SLOT(mkdir()) , "F7_Mkdir");
    actF8 = action(i18n("Delete"), "edit-delete", Qt::Key_F8, SLOT(deleteFiles()) , "F8_Delete");
    actF9 = action(i18n("Rename"), 0, Qt::Key_F9, SLOT(rename()) , "F9_Rename");
    action(i18n("&New Text File..."), "document-new", Qt::SHIFT + Qt::Key_F4, SLOT(editDlg()), "edit_new_file");
    action(i18n("F3 View Dialog"), 0, Qt::SHIFT + Qt::Key_F3, SLOT(viewDlg()), "F3_ViewDlg");

    // filter
    actAllFilter = action(i18n("&All Files"), 0, Qt::SHIFT + Qt::Key_F10, SLOT(allFilter()), "all files");
    //actExecFilter = new KAction( i18n( "&Executables" ), SHIFT + Qt::Key_F11,
    //                             SLOTS, SLOT( execFilter() ), actionCollection(), "exec files" );
    actCustomFilter = action(i18n("&Custom"), 0, Qt::SHIFT + Qt::Key_F12, SLOT(customFilter()), "custom files");

    // selection
    actSelect = action(i18n("Select &Group..."), "kr_select", Qt::CTRL + Qt::Key_Plus, this, SLOT(markGroup()), "select group");
    actSelectAll = action(i18n("&Select All"), "kr_selectall", Qt::ALT + Qt::Key_Plus, this, SLOT(markAll()), "select all");
    actUnselect = action(i18n("&Unselect Group..."), "kr_unselect", Qt::CTRL + Qt::Key_Minus, this, SLOT(unmarkGroup()), "unselect group");
    actUnselectAll = action(i18n("U&nselect All"), "kr_unselectall", Qt::ALT + Qt::Key_Minus, this, SLOT(unmarkAll()), "unselect all");
    actInvert = action(i18n("&Invert Selection"), "kr_invert", Qt::ALT + Qt::Key_Asterisk, this, SLOT(invert()), "invert");

    // file operations
    action(i18n("Right-click Menu"), 0, Qt::Key_Menu, _guiActions, SLOT(rightclickMenu()), "rightclick menu");
    actProperties = action(i18n("&Properties..."), 0, Qt::ALT + Qt::Key_Enter, this, SLOT(properties()), "properties");
    actCompDirs = action(i18n("&Compare Directories"), "view_left_right", Qt::ALT + Qt::SHIFT + Qt::Key_C, this, SLOT(compareDirs()), "compare dirs");
    actCalculate = action(i18n("Calculate &Occupied Space"), "kcalc", 0, this, SLOT(calcSpace()), "calculate");
    actPack = action(i18n("Pac&k..."), "archive-insert", Qt::ALT + Qt::SHIFT + Qt::Key_P, this, SLOT(slotPack()), "pack");
    actUnpack = action(i18n("&Unpack..."), "archive-extract", Qt::ALT + Qt::SHIFT + Qt::Key_U, this, SLOT(slotUnpack()), "unpack");
    actCreateChecksum = action(i18n("Create Checksum..."), "binary", 0, this, SLOT(createChecksum()), "create checksum");
    actMatchChecksum = action(i18n("Verify Checksum..."), "binary", 0, this, SLOT(matchChecksum()), "match checksum");
    action(i18n("New Symlink..."), 0, Qt::CTRL + Qt::ALT + Qt::Key_S, this, SLOT(newSymlink()), "new symlink");
    actTest = action(i18n("T&est Archive"), "utilities-file-archiver", Qt::ALT + Qt::SHIFT + Qt::Key_E, this, SLOT(testArchive()), "test archives");

    // navigation
    actRoot = action(i18n("Root"), "go-top", Qt::CTRL + Qt::Key_Backspace, this, SLOT(root()), "root");
    action(i18n("&Reload"), "view-refresh", Qt::CTRL + Qt::Key_R, this, SLOT(refresh()), "std_redisplay");
    actCancelRefresh = action(i18n("Cancel Refresh of View"), "kr_cancel_refresh", 0, _guiActions, SLOT(inlineRefreshCancel()), "cancel refresh");
    actFTPNewConnect = action(i18n("New Net &Connection..."), "network-connect", Qt::CTRL + Qt::Key_N, this, SLOT(newFTPconnection()), "ftp new connection");
    actFTPDisconnect = action(i18n("Disconnect &from Net"), "network-disconnect", Qt::SHIFT + Qt::CTRL + Qt::Key_F, this, SLOT(FTPDisconnect()), "ftp disconnect");
    action(i18n("Sync Panels"), 0, Qt::ALT + Qt::SHIFT + Qt::Key_O, this, SLOT(syncPanels()), "sync panels");
    actJumpBack = action(i18n("Jump Back"), "kr_jumpback", Qt::CTRL + Qt::Key_J, _guiActions, SLOT(jumpBack()), "jump_back");
    actSetJumpBack = action(i18n("Set Jump Back Point"), "kr_setjumpback", Qt::CTRL + Qt::SHIFT + Qt::Key_J, this, SLOT(slotSetJumpBack()), "set_jump_back");
    actSyncBrowse = action(i18n("S&ynchron Directory Changes"), "kr_syncbrowse_off", Qt::ALT + Qt::SHIFT + Qt::Key_Y, _guiActions, SLOT(toggleSyncBrowse()), "sync browse");
    actLocationBar = action(i18n("Go to Location Bar"), 0, Qt::CTRL + Qt::Key_L, _guiActions, SLOT(editLocation()), "location_bar");
    toggleAction(i18n("Toggle Popup Panel"), 0, Qt::ALT + Qt::Key_Down, _guiActions, SLOT(togglePanelPopup()), "toggle popup panel");
    action(i18n("Bookmarks"), 0, Qt::CTRL + Qt::Key_D, _guiActions, SLOT(openBookmarks()), "bookmarks");
    action(i18n("Left Bookmarks"), 0, 0, this, SLOT(openLeftBookmarks()), "left bookmarks");
    action(i18n("Right Bookmarks"), 0, 0, this, SLOT(openRightBookmarks()), "right bookmarks");
    action(i18n("History"), 0, Qt::CTRL + Qt::Key_H, _guiActions, SLOT(openHistory()), "history");
    action(i18n("Left History"), 0, Qt::ALT + Qt::CTRL + Qt::Key_Left, this, SLOT(openLeftHistory()), "left history");
    action(i18n("Right History"), 0, Qt::ALT + Qt::CTRL + Qt::Key_Right, this, SLOT(openRightHistory()), "right history");
    action(i18n("Media"), 0, Qt::CTRL + Qt::Key_M, _guiActions, SLOT(openMedia()), "media");
    action(i18n("Left Media"), 0, Qt::CTRL + Qt::SHIFT + Qt::Key_Left, this, SLOT(openLeftMedia()), "left media");
    action(i18n("Right Media"), 0, Qt::CTRL + Qt::SHIFT + Qt::Key_Right, this, SLOT(openRightMedia()), "right media");

    // and at last we can set the tool-tips
    actSelect->setToolTip(i18n("Select files using a filter"));
    actSelectAll->setToolTip(i18n("Select all files in the current directory"));
    actUnselectAll->setToolTip(i18n("Unselect all selected files"));
    actRoot->setToolTip(i18n("ROOT (/)"));
}

void ListPanelActions::activePanelChanged(ListPanel *panel)
{
    _guiActions.reconnect(panel);
    _funcActions.reconnect(panel->func);
}

inline KrPanel *ListPanelActions::activePanel()
{
    return static_cast<FileManagerWindow*>(_mainWindow)->activePanel();
}

inline KrPanel *ListPanelActions::leftPanel()
{
    return static_cast<FileManagerWindow*>(_mainWindow)->leftPanel();
}

inline KrPanel *ListPanelActions::rightPanel()
{
    return static_cast<FileManagerWindow*>(_mainWindow)->rightPanel();
}

inline ListPanelFunc *ListPanelActions::activeFunc()
{
    return activePanel()->func;
}

// set view type

void ListPanelActions::setView(int id)
{
    activePanel()->gui->changeType(id);
}

// fn keys

// F2
void ListPanelActions::terminal()
{
    activeFunc()->terminal();
}
// F3
void ListPanelActions::view()
{
    activeFunc()->view();
}
// F4
void ListPanelActions::edit()
{
    activeFunc()->editFile();
}
// F5
void ListPanelActions::copyFiles()
{
    activeFunc()->copyFiles();
}
// F6
void ListPanelActions::moveFiles()
{
    activeFunc()->moveFiles();
}
// SHIFT + F5
void ListPanelActions::copyFilesByQueue()
{
    activeFunc()->copyFiles(true);
}
// SHIFT + F6
void ListPanelActions::moveFilesByQueue()
{
    activeFunc()->moveFiles(true);
}
// F7
void ListPanelActions::mkdir()
{
    activeFunc()->mkdir();
}
// F8
void ListPanelActions::deleteFiles(bool reallyDelete)
{
    activeFunc()->deleteFiles(reallyDelete);
}
// F9
void ListPanelActions::rename()
{
    activeFunc()->rename();
}
// Shift F3
void ListPanelActions::viewDlg()
{
    // ask the user for a url to view
    KUrl dest = KChooseDir::getDir(i18n("Enter a URL to view:"), activePanel()->virtualPath(), activePanel()->virtualPath());
    if (dest.isEmpty()) return ;   // the user canceled

    KrViewer::view(dest);   // view the file
//  }
    // nothing more to it!
}
// Shift F4
void ListPanelActions::editDlg()
{
    activeFunc()->editNewFile();
}

// filter

void ListPanelActions::allFilter()
{
    activePanel()->gui->setFilter(KrViewProperties::All);
}
#if 0
void ListPanelActions::execFilter()
{
    activePanel()->gui->setFilter(KrViewProperties::All);
}
#endif
void ListPanelActions::customFilter()
{
    activePanel()->gui->setFilter(KrViewProperties::Custom);
}

// selection

void ListPanelActions::markAll()
{
    activePanel()->gui->select(true, true);
}

void ListPanelActions::unmarkAll()
{
    activePanel()->gui->select(false, true);
}

void ListPanelActions::markGroup()
{
    activePanel()->gui->select(true, false);
}

void ListPanelActions::markGroup(const QString& mask, bool select)
{
    activePanel()->gui->select(KRQuery(mask), select);
}

void ListPanelActions::unmarkGroup()
{
    activePanel()->gui->select(false, false);
}

void ListPanelActions::invert()
{
    activePanel()->gui->invertSelection();
}

// file operations

void ListPanelActions::cut()
{
    activeFunc()->copyToClipboard(true);
}

void ListPanelActions::copy()
{
    activeFunc()->copyToClipboard(false);
}

void ListPanelActions::paste()
{
    activeFunc()->pasteFromClipboard();
}

void ListPanelActions::createChecksum()
{
    activeFunc()->createChecksum();
}

void ListPanelActions::matchChecksum()
{
    activeFunc()->matchChecksum();
}

void ListPanelActions::compareDirs()
{
    activePanel()->gui->compareDirs();
    activePanel()->otherPanel()->gui->compareDirs();
}

void ListPanelActions::properties()
{
    activeFunc()->properties();
}

void ListPanelActions::rightclickMenu()
{
    activePanel()->gui->rightclickMenu();
}

void ListPanelActions::slotPack()
{
    activeFunc()->pack();
}

void ListPanelActions::slotUnpack()
{
    activeFunc()->unpack();
}

void ListPanelActions::testArchive()
{
    activeFunc()->testArchive();
}

void ListPanelActions::calcSpace()
{
    activeFunc()->calcSpace();
}

void ListPanelActions::newSymlink()
{
    activePanel()->func->krlink(true);
}

// navigation

void ListPanelActions::historyBackward()
{
    activeFunc()->historyBackward();
}

void ListPanelActions::historyForward()
{
    activeFunc()->historyForward();
}

void ListPanelActions::dirUp()
{
    activeFunc()->dirUp();
}

void ListPanelActions::home()
{
    activeFunc()->openUrl(QDir::homePath());
}

void ListPanelActions::root()
{
    activeFunc()->openUrl(KUrl("/"));
}

void ListPanelActions::refresh()
{
    activeFunc()->refresh();
}

void ListPanelActions::FTPDisconnect()
{
    activeFunc()->FTPDisconnect();
}

void ListPanelActions::newFTPconnection()
{
    activeFunc()->newFTPconnection();
}

void ListPanelActions::slotSetJumpBack()
{
    activePanel()->gui->setJumpBack(activePanel()->virtualPath());
}

void ListPanelActions::syncPanels()
{
    activeFunc()->otherFunc()->openUrl(activePanel()->virtualPath());
}

void ListPanelActions::openLeftBookmarks()
{
    leftPanel()->gui->openBookmarks();
}

void ListPanelActions::openRightBookmarks()
{
    rightPanel()->gui->openBookmarks();
}

void ListPanelActions::openLeftHistory()
{
    leftPanel()->gui->openHistory();
}

void ListPanelActions::openRightHistory()
{
    rightPanel()->gui->openHistory();
}

void ListPanelActions::openLeftMedia()
{
    leftPanel()->gui->openMedia();
}

void ListPanelActions::openRightMedia()
{
    rightPanel()->gui->openMedia();
}
