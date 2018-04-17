/*
 * Qt4 bitcoin GUI.
 *
 * W.J. van der Laan 2011-2012
 * The Bitcoin Developers 2011-2012
 */
#include "bitcoingui.h"
#include "transactiontablemodel.h"
#include "addressbookpage.h"
#include "sendcoinsdialog.h"
#include "signverifymessagedialog.h"
#include "optionsdialog.h"
#include "aboutdialog.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "editaddressdialog.h"
#include "optionsmodel.h"
#include "transactiondescdialog.h"
#include "addresstablemodel.h"
#include "transactionview.h"
#include "overviewpage.h"
#include "bitcoinunits.h"
#include "guiconstants.h"
#include "askpassphrasedialog.h"
#include "notificator.h"
#include "guiutil.h"
#include "rpcconsole.h"

#ifdef Q_OS_MAC
#include "macdockiconhandler.h"
#endif

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLocale>
#include <QMessageBox>
#include <QProgressBar>
#include <QStackedWidget>
#include <QDateTime>
#include <QMovie>
#include <QFileDialog>
#include <QDesktopServices>
#include <QTimer>
#include <QDragEnterEvent>
#include <QUrl>
#include <QStyle>
#include <QTextStream>
#include <QButtonGroup>
#include <QDesktopServices>
#include <QSettings>
#include <QUrl>
#include <QProcess>

#include <iostream>

BitcoinGUI::BitcoinGUI(QWidget *parent):
    QMainWindow(parent),CSkinSet(),
    clientModel(0),
    walletModel(0),
    encryptWalletAction(0),
    changePassphraseAction(0),
    aboutQtAction(0),
    trayIcon(0),
    notificator(0),
    rpcConsole(0)
{
    this->setObjectName(QString("mainwindow"));
    resize(850, 550);
    setWindowTitle(tr("MetisCoin") + " - " + tr("Wallet"));
#ifndef Q_OS_MAC
    qApp->setWindowIcon(QIcon(":icons/bitcoin"));
    setWindowIcon(QIcon(":icons/bitcoin"));
#else
    setUnifiedTitleAndToolBarOnMac(true);
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

    //this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMaximizeButtonHint);
    // Accept D&D of URIs
    setAcceptDrops(true);

    // Create actions for the toolbar, menu bar and tray/dock icon
    createActions();

    // Create application menu bar
    //createMenuBar();

    // Create the toolbars
    //createToolBars();

    // Create the tray icon (or setup the dock icon)
    createTrayIcon();

    this->resize(850, 585);

    setFixedSize(this->width(), this->height());

    QWidget *Form = this;

    topwidget = new QWidget(Form);
    topwidget->setObjectName(QString("topwidget"));
    topwidget->setGeometry(QRect(0, 0, 850, 95));
    labellogo = new QLabel(topwidget);
    labellogo->setObjectName(QString("labellogo"));
    labellogo->setGeometry(QRect(10, 13, 70, 70));

    labellogotext = new QLabel(topwidget);
    labellogotext->setObjectName(QString("labellogotext"));
    labellogotext->setGeometry(QRect(80, 13, 135, 52));

    btn_desc = new QPushButton(topwidget);
    btn_desc->setObjectName(QString("btn_desc"));
    btn_desc->setGeometry(QRect(350, 19, 70, 60));

    btn_send = new QPushButton(topwidget);
    btn_send->setObjectName(QString("btn_send"));
    btn_send->setGeometry(QRect(420, 19, 70, 60));

    btn_recv = new QPushButton(topwidget);
    btn_recv->setObjectName(QString("btn_recv"));
    btn_recv->setGeometry(QRect(490, 19, 70, 60));

    btn_logs = new QPushButton(topwidget);
    btn_logs->setObjectName(QString("btn_logs"));
    btn_logs->setGeometry(QRect(560, 19, 70, 60));

    btn_addrs = new QPushButton(topwidget);
    btn_addrs->setObjectName(QString("btn_addrs"));
    btn_addrs->setGeometry(QRect(630, 19, 70, 60));

    btn_export = new QPushButton(topwidget);
    btn_export->setObjectName(QString("btn_export"));
    btn_export->setGeometry(QRect(700, 19, 70, 60));

    //btn_cloudbuy = new QPushButton(topwidget);
    //btn_cloudbuy->setObjectName(QString("btn_cloudbuy"));
    //btn_cloudbuy->setGeometry(QRect(700, 19, 70, 60));

    btn_blockview = new QPushButton(topwidget);
    btn_blockview->setObjectName(QString("btn_blockview"));
    btn_blockview->setGeometry(QRect(770, 19, 70, 60));

    middlewidget = new QWidget(Form);
    middlewidget->setObjectName(QString("middlewidget"));
    middlewidget->setGeometry(QRect(5, 100, 840, 30));
    pushButton_9 = new QPushButton(middlewidget);
    pushButton_9->setObjectName(QString("pushButton_9"));
    pushButton_9->setGeometry(QRect(738, 4, 40, 20));
    pushButton_10 = new QPushButton(middlewidget);
    pushButton_10->setObjectName(QString("pushButton_10"));
    pushButton_10->setGeometry(QRect(688, 4, 40, 20));
    pushButton_11 = new QPushButton(middlewidget);
    pushButton_11->setObjectName(QString("pushButton_11"));
    pushButton_11->setGeometry(QRect(638, 4, 40, 20));
    btn_close = new QPushButton(middlewidget);
    btn_close->setObjectName(QString("btn_close"));
    btn_close->setGeometry(QRect(788, 4, 40, 20));
    btn_skin = new QPushButton(middlewidget);
    btn_skin->setObjectName(QString("btn_skin"));
    btn_skin->setGeometry(QRect(10, 0, 75, 23));
    centralWidget = new QStackedWidget(Form);
    centralWidget->setObjectName(QString("centralWidget"));
    centralWidget->setGeometry(QRect(5, 125, 840, 411));
    page = new QWidget();
    page->setObjectName(QString("page"));
    centralWidget->addWidget(page);
    page_2 = new QWidget();
    page_2->setObjectName(QString("page_2"));
    centralWidget->addWidget(page_2);
    statwidget = new QWidget(Form);
    statwidget->setObjectName(QString("statwidget"));
    statwidget->setGeometry(QRect(5, 535, 840, 30));
    labelConnectionsIcon = new QLabel(statwidget);
    labelConnectionsIcon->setObjectName(QString("labelConnectionsIcon"));
    labelConnectionsIcon->setGeometry(QRect(710, 10, 54, 16));
    progressBarLabel = new QLabel(statwidget);
    progressBarLabel->setObjectName(QString("progressBarLabel"));
    progressBarLabel->setGeometry(QRect(10, 10, 91, 16));
    progressBar = new QProgressBar(statwidget);
    progressBar->setObjectName(QString("progressBar"));
    progressBar->setGeometry(QRect(110, 10, 201, 23));
    progressBar->setValue(24);
    labelEncryptionIcon = new QLabel(statwidget);
    labelEncryptionIcon->setObjectName(QString("labelEncryptionIcon"));
    labelEncryptionIcon->setGeometry(QRect(640, 10, 54, 16));
    labelBlocksIcon = new QLabel(statwidget);
    labelBlocksIcon->setObjectName(QString("labelBlocksIcon"));
    labelBlocksIcon->setGeometry(QRect(770, 10, 54, 16));


    QButtonGroup *btn_group = new QButtonGroup(topwidget);
    btn_group->addButton( btn_desc, 0 );
    btn_group->addButton( btn_send, 1 );
    btn_group->addButton( btn_recv, 2 );
    btn_group->addButton( btn_logs, 3 );
    btn_group->addButton( btn_addrs, 4 );
    //btn_group->addButton( btn_cloudbuy, 5 );
    btn_group->addButton( btn_blockview, 6 );

//    connect(btn_desc, SIGNAL(clicked()), this, SLOT(gotoOverviewPage()));
//    connect(btn_send, SIGNAL(clicked()), this, SLOT(gotoSendCoinsPage()));
    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
//    connect(btn_recv, SIGNAL(clicked()), this, SLOT(gotoReceiveCoinsPage()));
//    connect(btn_logs, SIGNAL(clicked()), this, SLOT(gotoHistoryPage()));
//    connect(btn_addrs, SIGNAL(clicked()), this, SLOT(gotoAddressBookPage()));
    connect(btn_export, SIGNAL(clicked()), this, SLOT(exportClicked()));
    connect(btn_skin, SIGNAL(clicked()), this, SLOT(reloadskin()));

    btn_desc->setCheckable(true);
    btn_send->setCheckable(true);
    btn_recv->setCheckable(true);
    btn_logs->setCheckable(true);
    btn_addrs->setCheckable(true);
    //btn_cloudbuy->setCheckable(true);
    //btn_blockview->setCheckable(true);
    btn_skin->setVisible(false);
    connect(btn_group, SIGNAL(buttonClicked(int)), this, SLOT(onBtnClicked(int)) );
    btn_desc->setChecked(true);
    btn_export->setEnabled(false);

    // Create tabs
    overviewPage = new OverviewPage();

    transactionView = new TransactionView(this);

    addressBookPage = new AddressBookPage(AddressBookPage::ForEditing, AddressBookPage::SendingTab);

    receiveCoinsPage = new AddressBookPage(AddressBookPage::ForEditing, AddressBookPage::ReceivingTab);

    sendCoinsPage = new SendCoinsDialog(this);

    signVerifyMessageDialog = new SignVerifyMessageDialog(this);


    centralWidget->setStyleSheet("background-color: #D9CABB;");
    centralWidget->addWidget(overviewPage);
    centralWidget->addWidget(transactionView);
    centralWidget->addWidget(addressBookPage);
    centralWidget->addWidget(receiveCoinsPage);
    centralWidget->addWidget(sendCoinsPage);


    pushButton_9->setText(tr("帮助"));
    pushButton_10->setText(tr("设置"));
    pushButton_11->setText(tr("文件"));
    btn_close->setText(tr("退出"));
    btn_skin->setText("loadskin");

    // Progress bar and label for blocks download
    progressBarLabel->setVisible(false);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setVisible(false);

    syncIconMovie = new QMovie(":/movies/update_spinner", "mng", this);
	// this->setStyleSheet("background-color: #ceffee;");

    // Clicking on a transaction on the overview page simply sends you to transaction history page
    connect(overviewPage, SIGNAL(transactionClicked(QModelIndex)), this, SLOT(gotoHistoryPage()));
    connect(overviewPage, SIGNAL(transactionClicked(QModelIndex)), transactionView, SLOT(focusTransaction(QModelIndex)));

    // Double-clicking on a transaction on the transaction history page shows details
    connect(transactionView, SIGNAL(doubleClicked(QModelIndex)), transactionView, SLOT(showDetails()));

    rpcConsole = new RPCConsole(this);
    connect(openRPCConsoleAction, SIGNAL(triggered()), rpcConsole, SLOT(show()));

    // Clicking on "Verify Message" in the address book sends you to the verify message tab
    connect(addressBookPage, SIGNAL(verifyMessage(QString)), this, SLOT(gotoVerifyMessageTab(QString)));
    // Clicking on "Sign Message" in the receive coins page sends you to the sign message tab
    connect(receiveCoinsPage, SIGNAL(signMessage(QString)), this, SLOT(gotoSignMessageTab(QString)));

    gotoOverviewPage();


    menu_file = new QMenu();
    menu_file->setObjectName(QString("menu_file"));
    menu_file->addAction(backupWalletAction);
    menu_file->addAction(signMessageAction);
    menu_file->addAction(verifyMessageAction);
    menu_file->addSeparator();
    menu_file->addAction(quitAction);
    pushButton_11->setMenu(menu_file);

    menu_settings = new QMenu();
    menu_settings->setObjectName(QString("menu_settings"));
    menu_settings->addAction(encryptWalletAction);
    menu_settings->addAction(changePassphraseAction);
    //menu_settings->addSeparator();
    //menu_settings->addAction(optionsAction);
    pushButton_10->setMenu(menu_settings);

    menu_help = new QMenu();
    menu_help->setObjectName(QString("menu_help"));
    menu_help->addAction(openRPCConsoleAction);
    //help->addSeparator();
    //help->addAction(aboutAction);
    //help->addAction(aboutQtAction);
    pushButton_9->setMenu(menu_help);

    reloadskin();
    readConfig();
}

BitcoinGUI::~BitcoinGUI()
{
    if(trayIcon) // Hide tray icon, as deleting will let it linger until quit (on Ubuntu)
        trayIcon->hide();
#ifdef Q_OS_MAC
    delete appMenuBar;
#endif
}

void BitcoinGUI::readConfig()
{
    QSettings setting("MetisCoin.conf", QSettings::IniFormat);
    m_cloudyurl = setting.value("urlcloudy").toString();
    m_blockurl = setting.value("urlblock").toString();
}

void BitcoinGUI::reloadskin()
{
    loadQssConfig(this);
    overviewPage->loadQssConfig(overviewPage);
    sendCoinsPage->loadQssConfig(sendCoinsPage);
    addressBookPage->loadQssConfig(addressBookPage);
    receiveCoinsPage->loadQssConfig(receiveCoinsPage);
    transactionView->loadQssConfig(transactionView);
    signVerifyMessageDialog->loadQssConfig(signVerifyMessageDialog);
    rpcConsole->loadQssConfig(rpcConsole);


    QString menustylesheet = "";
    QFile tfile(getQssFilePath("menustyle"));
    if (tfile.open(QFile::ReadOnly)) {
        QTextStream filetext(&tfile);
        menustylesheet = filetext.readAll();
        tfile.close();
    }
    menu_file->setStyleSheet(menustylesheet);
    menu_settings->setStyleSheet(menustylesheet);
    menu_help->setStyleSheet(menustylesheet);
}

/*
void BitcoinGUI::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_Drag = true;
        m_DragPosition = event->globalPos() - this->pos();
        event->accept();
    }
}

void BitcoinGUI::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Drag && (event->buttons() && Qt::LeftButton)) {
        move(event->globalPos() - m_DragPosition);
        event->accept();
    }
}

void BitcoinGUI::mouseReleaseEvent(QMouseEvent *)
{
    m_Drag = false;
}
*/

void BitcoinGUI::createActions()
{
    QActionGroup *tabGroup = new QActionGroup(this);

    quitAction = new QAction(QIcon(":/icons/quit"), tr("E&xit"), this);
    quitAction->setToolTip(tr("Quit application"));
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    quitAction->setMenuRole(QAction::QuitRole);
    aboutAction = new QAction(QIcon(":/icons/bitcoin"), tr("&About MetisCoin"), this);
    aboutAction->setToolTip(tr("Show information about MetisCoin"));
    aboutAction->setMenuRole(QAction::AboutRole);
    aboutQtAction = new QAction(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"), tr("About &Qt"), this);
    aboutQtAction->setToolTip(tr("Show information about Qt"));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
    optionsAction = new QAction(QIcon(":/icons/options"), tr("&Options..."), this);
    optionsAction->setToolTip(tr("Modify configuration options for MetisCoin"));
    optionsAction->setMenuRole(QAction::PreferencesRole);
    toggleHideAction = new QAction(QIcon(":/icons/bitcoin"), tr("&Show / Hide"), this);
    encryptWalletAction = new QAction(QIcon(":/icons/lock_closed"), tr("&Encrypt Wallet..."), this);
    encryptWalletAction->setToolTip(tr("Encrypt or decrypt wallet"));
    encryptWalletAction->setCheckable(true);
    backupWalletAction = new QAction(QIcon(":/icons/filesave"), tr("&Backup Wallet..."), this);
    backupWalletAction->setToolTip(tr("Backup wallet to another location"));
    changePassphraseAction = new QAction(QIcon(":/icons/key"), tr("&Change Passphrase..."), this);
    changePassphraseAction->setToolTip(tr("Change the passphrase used for wallet encryption"));
    signMessageAction = new QAction(QIcon(":/icons/edit"), tr("Sign &message..."), this);
    verifyMessageAction = new QAction(QIcon(":/icons/transaction_0"), tr("&Verify message..."), this);

    openRPCConsoleAction = new QAction(QIcon(":/icons/debugwindow"), tr("&Debug window"), this);
    openRPCConsoleAction->setToolTip(tr("Open debugging and diagnostic console"));

    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutClicked()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(optionsAction, SIGNAL(triggered()), this, SLOT(optionsClicked()));
    connect(toggleHideAction, SIGNAL(triggered()), this, SLOT(toggleHidden()));
    connect(encryptWalletAction, SIGNAL(triggered(bool)), this, SLOT(encryptWallet(bool)));
    connect(backupWalletAction, SIGNAL(triggered()), this, SLOT(backupWallet()));
    connect(changePassphraseAction, SIGNAL(triggered()), this, SLOT(changePassphrase()));
    connect(signMessageAction, SIGNAL(triggered()), this, SLOT(gotoSignMessageTab()));
    connect(verifyMessageAction, SIGNAL(triggered()), this, SLOT(gotoVerifyMessageTab()));
}

void BitcoinGUI::setClientModel(ClientModel *clientModel)
{
    this->clientModel = clientModel;
    if(clientModel)
    {
        // Replace some strings and icons, when using the testnet
        if(clientModel->isTestNet())
        {
            setWindowTitle(windowTitle() + QString(" ") + tr("[testnet]"));
#ifndef Q_OS_MAC
            qApp->setWindowIcon(QIcon(":icons/bitcoin_testnet"));
            setWindowIcon(QIcon(":icons/bitcoin_testnet"));
#else
            MacDockIconHandler::instance()->setIcon(QIcon(":icons/bitcoin_testnet"));
#endif
            if(trayIcon)
            {
                trayIcon->setToolTip(tr("MetisCoin client") + QString(" ") + tr("[testnet]"));
                trayIcon->setIcon(QIcon(":/icons/toolbar_testnet"));
                toggleHideAction->setIcon(QIcon(":/icons/toolbar_testnet"));
            }

        }

        // Keep up to date with client
        setNumConnections(clientModel->getNumConnections());
        connect(clientModel, SIGNAL(numConnectionsChanged(int)), this, SLOT(setNumConnections(int)));

        setNumBlocks(clientModel->getNumBlocks(), clientModel->getNumBlocksOfPeers());
        connect(clientModel, SIGNAL(numBlocksChanged(int,int)), this, SLOT(setNumBlocks(int,int)));

        // Report errors from network/worker thread
        connect(clientModel, SIGNAL(error(QString,QString,bool)), this, SLOT(error(QString,QString,bool)));

        rpcConsole->setClientModel(clientModel);
        addressBookPage->setOptionsModel(clientModel->getOptionsModel());
        receiveCoinsPage->setOptionsModel(clientModel->getOptionsModel());
    }
}

void BitcoinGUI::setWalletModel(WalletModel *walletModel)
{
    this->walletModel = walletModel;
    if(walletModel)
    {
        // Report errors from wallet thread
        connect(walletModel, SIGNAL(error(QString,QString,bool)), this, SLOT(error(QString,QString,bool)));

        // Put transaction list in tabs
        transactionView->setModel(walletModel);

        overviewPage->setModel(walletModel);
        addressBookPage->setModel(walletModel->getAddressTableModel());
        receiveCoinsPage->setModel(walletModel->getAddressTableModel());
        sendCoinsPage->setModel(walletModel);
        signVerifyMessageDialog->setModel(walletModel);

        setEncryptionStatus(walletModel->getEncryptionStatus());
        connect(walletModel, SIGNAL(encryptionStatusChanged(int)), this, SLOT(setEncryptionStatus(int)));

        // Balloon pop-up for new transaction
        connect(walletModel->getTransactionTableModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(incomingTransaction(QModelIndex,int,int)));

        // Ask for passphrase if needed
        connect(walletModel, SIGNAL(requireUnlock()), this, SLOT(unlockWallet()));
    }
}

void BitcoinGUI::createTrayIcon()
{
    QMenu *trayIconMenu;
#ifndef Q_OS_MAC
    trayIcon = new QSystemTrayIcon(this);
    trayIconMenu = new QMenu(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip(tr("MetisCoin client"));
    trayIcon->setIcon(QIcon(":/icons/toolbar"));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
#else
    // Note: On Mac, the dock icon is used to provide the tray's functionality.
    MacDockIconHandler *dockIconHandler = MacDockIconHandler::instance();
    trayIconMenu = dockIconHandler->dockMenu();
#endif

    // Configuration of the tray icon (or dock icon) icon menu
    trayIconMenu->addAction(toggleHideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(signMessageAction);
    trayIconMenu->addAction(verifyMessageAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(optionsAction);
    trayIconMenu->addAction(openRPCConsoleAction);
#ifndef Q_OS_MAC // This is built-in on Mac
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
#endif

    notificator = new Notificator(qApp->applicationName(), trayIcon);
}

#ifndef Q_OS_MAC
void BitcoinGUI::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger)
    {
        // Click on system tray icon triggers show/hide of the main window
        toggleHideAction->trigger();
    }
}
#endif

void BitcoinGUI::optionsClicked()
{
    if(!clientModel || !clientModel->getOptionsModel())
        return;
    OptionsDialog dlg;
    dlg.loadQssConfig(&dlg);
    dlg.setModel(clientModel->getOptionsModel());
    dlg.exec();
}

void BitcoinGUI::aboutClicked()
{
    AboutDialog dlg;
    dlg.setModel(clientModel);
    dlg.exec();
}

void BitcoinGUI::setNumConnections(int count)
{
    QString icon;
    switch(count)
    {
    case 0: icon = ":/icons/connect_0"; break;
    case 1: case 2: case 3: icon = ":/icons/connect_1"; break;
    case 4: case 5: case 6: icon = ":/icons/connect_2"; break;
    case 7: case 8: case 9: icon = ":/icons/connect_3"; break;
    default: icon = ":/icons/connect_4"; break;
    }
    labelConnectionsIcon->setPixmap(QIcon(icon).pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
    labelConnectionsIcon->setToolTip(tr("%n active connection(s) to MetisCoin network", "", count));
}

void BitcoinGUI::setNumBlocks(int count, int nTotalBlocks)
{
    // don't show / hide progress bar and its label if we have no connection to the network
    if (!clientModel || clientModel->getNumConnections() == 0)
    {
        progressBarLabel->setVisible(false);
        progressBar->setVisible(false);

        return;
    }

    QString strStatusBarWarnings = clientModel->getStatusBarWarnings();
    QString tooltip;

    if(count < nTotalBlocks)
    {
        int nRemainingBlocks = nTotalBlocks - count;
        float nPercentageDone = count / (nTotalBlocks * 0.01f);

        if (strStatusBarWarnings.isEmpty())
        {
            progressBarLabel->setText(tr("Synchronizing with network..."));
            progressBarLabel->setVisible(true);
            progressBar->setFormat(tr("~%n block(s) remaining", "", nRemainingBlocks));
            progressBar->setMaximum(nTotalBlocks);
            progressBar->setValue(count);
            progressBar->setVisible(true);
        }

        tooltip = tr("Downloaded %1 of %2 blocks of transaction history (%3% done).").arg(count).arg(nTotalBlocks).arg(nPercentageDone, 0, 'f', 2);
    }
    else
    {
        if (strStatusBarWarnings.isEmpty())
            progressBarLabel->setVisible(false);

        progressBar->setVisible(false);
        tooltip = tr("Downloaded %1 blocks of transaction history.").arg(count);
    }

    // Override progressBarLabel text and hide progress bar, when we have warnings to display
    if (!strStatusBarWarnings.isEmpty())
    {
        progressBarLabel->setText(strStatusBarWarnings);
        progressBarLabel->setVisible(true);
        progressBar->setVisible(false);
    }

	tooltip = tr("Current difficulty is %1.").arg(clientModel->GetDifficulty()) + QString("<br>") + tooltip;

    QDateTime lastBlockDate = clientModel->getLastBlockDate();
    int secs = lastBlockDate.secsTo(QDateTime::currentDateTime());
    QString text;

    // Represent time from last generated block in human readable text
    if(secs <= 0)
    {
        // Fully up to date. Leave text empty.
    }
    else if(secs < 60)
    {
        text = tr("%n second(s) ago","",secs);
    }
    else if(secs < 60*60)
    {
        text = tr("%n minute(s) ago","",secs/60);
    }
    else if(secs < 24*60*60)
    {
        text = tr("%n hour(s) ago","",secs/(60*60));
    }
    else
    {
        text = tr("%n day(s) ago","",secs/(60*60*24));
    }

    // Set icon state: spinning if catching up, tick otherwise
    if(secs < 90*60 && count >= nTotalBlocks)
    {
        tooltip = tr("Up to date") + QString(".<br>") + tooltip;
        labelBlocksIcon->setPixmap(QIcon(":/icons/synced").pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));

        overviewPage->showOutOfSyncWarning(false);
    }
    else
    {
        tooltip = tr("Catching up...") + QString("<br>") + tooltip;
        labelBlocksIcon->setMovie(syncIconMovie);
        syncIconMovie->start();

        overviewPage->showOutOfSyncWarning(true);
    }

    if(!text.isEmpty())
    {
        tooltip += QString("<br>");
        tooltip += tr("Last received block was generated %1.").arg(text);
    }

    // Don't word-wrap this (fixed-width) tooltip
    tooltip = QString("<nobr>") + tooltip + QString("</nobr>");

    labelBlocksIcon->setToolTip(tooltip);
    progressBarLabel->setToolTip(tooltip);
    progressBar->setToolTip(tooltip);
}

void BitcoinGUI::error(const QString &title, const QString &message, bool modal)
{
    // Report errors from network/worker thread
    if(modal)
    {
        QMessageBox::critical(this, title, message, QMessageBox::Ok, QMessageBox::Ok);
    } else {
        notificator->notify(Notificator::Critical, title, message);
    }
}

void BitcoinGUI::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
#ifndef Q_OS_MAC // Ignored on Mac
    if(e->type() == QEvent::WindowStateChange)
    {
        if(clientModel && clientModel->getOptionsModel()->getMinimizeToTray())
        {
            QWindowStateChangeEvent *wsevt = static_cast<QWindowStateChangeEvent*>(e);
            if(!(wsevt->oldState() & Qt::WindowMinimized) && isMinimized())
            {
                QTimer::singleShot(0, this, SLOT(hide()));
                e->ignore();
            }
        }
    }
#endif
}

void BitcoinGUI::closeEvent(QCloseEvent *event)
{
    if(clientModel)
    {
#ifndef Q_OS_MAC // Ignored on Mac
        if(!clientModel->getOptionsModel()->getMinimizeToTray() &&
           !clientModel->getOptionsModel()->getMinimizeOnClose())
        {
            qApp->quit();
        }
#endif
    }
    QMainWindow::closeEvent(event);
}

void BitcoinGUI::askFee(qint64 nFeeRequired, bool *payFee)
{
    QString strMessage =
        tr("This transaction is over the size limit.  You can still send it for a fee of %1, "
          "which goes to the nodes that process your transaction and helps to support the network.  "
          "Do you want to pay the fee?").arg(
                BitcoinUnits::formatWithUnit(BitcoinUnits::BTC, nFeeRequired));
    QMessageBox::StandardButton retval = QMessageBox::question(
          this, tr("Confirm transaction fee"), strMessage,
          QMessageBox::Yes|QMessageBox::Cancel, QMessageBox::Yes);
    *payFee = (retval == QMessageBox::Yes);
}

void BitcoinGUI::incomingTransaction(const QModelIndex & parent, int start, int end)
{
    if(!walletModel || !clientModel)
        return;
    TransactionTableModel *ttm = walletModel->getTransactionTableModel();
    qint64 amount = ttm->index(start, TransactionTableModel::Amount, parent)
                    .data(Qt::EditRole).toULongLong();
    if(!clientModel->inInitialBlockDownload())
    {
        // On new transaction, make an info balloon
        // Unless the initial block download is in progress, to prevent balloon-spam
        QString date = ttm->index(start, TransactionTableModel::Date, parent)
                        .data().toString();
        QString type = ttm->index(start, TransactionTableModel::Type, parent)
                        .data().toString();
        QString address = ttm->index(start, TransactionTableModel::ToAddress, parent)
                        .data().toString();
        QIcon icon = qvariant_cast<QIcon>(ttm->index(start,
                            TransactionTableModel::ToAddress, parent)
                        .data(Qt::DecorationRole));

        notificator->notify(Notificator::Information,
                            (amount)<0 ? tr("Sent transaction") :
                                         tr("Incoming transaction"),
                              tr("Date: %1\n"
                                 "Amount: %2\n"
                                 "Type: %3\n"
                                 "Address: %4\n")
                              .arg(date)
                              .arg(BitcoinUnits::formatWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), amount, true))
                              .arg(type)
                              .arg(address), icon);
    }
}

void BitcoinGUI::onBtnClicked(int btnid)
{
    switch (btnid)
    {
    case 0:
        gotoOverviewPage();
        btn_export->setEnabled(false);
        return;
    case 1:
        gotoSendCoinsPage();
        btn_export->setEnabled(false);
        return;
    case 2:
        gotoReceiveCoinsPage();
        btn_export->setEnabled(true);
        return;
    case 3:
        gotoHistoryPage();
        btn_export->setEnabled(true);
        return;
    case 4:
        gotoAddressBookPage();
        btn_export->setEnabled(true);
        return;
    case 5:
        viewCloudyBuy();
        return;
    case 6:
        viewBlockWebPage();
        return;
    default:
        btn_export->setEnabled(false);
        return;
    }
}

void BitcoinGUI::gotoOverviewPage()
{
//    btn_desc->setChecked(true);
//    btn_send->setChecked(false);
//    btn_recv->setChecked(false);
//    btn_logs->setChecked(false);
//    btn_addrs->setChecked(false);
//    btn_export->setEnabled(false);
    centralWidget->setCurrentWidget(overviewPage);

}

void BitcoinGUI::gotoHistoryPage()
{
//    btn_desc->setChecked(false);
//    btn_send->setChecked(false);
//    btn_recv->setChecked(false);
//    btn_addrs->setChecked(false);
//    btn_export->setEnabled(false);
    centralWidget->setCurrentWidget(transactionView);
}

void BitcoinGUI::gotoAddressBookPage()
{
//    btn_desc->setChecked(false);
//    btn_send->setChecked(false);
//    btn_recv->setChecked(false);
//    btn_logs->setChecked(false);
//    btn_export->setEnabled(true);
    centralWidget->setCurrentWidget(addressBookPage);
    disconnect(btn_export, SIGNAL(clicked()), 0, 0);
    connect(btn_export, SIGNAL(clicked()), addressBookPage, SLOT(exportClicked()));
}

void BitcoinGUI::viewCloudyBuy()
{
    if (m_cloudyurl.length() > 0)
    {
        QDesktopServices::openUrl(QUrl(m_cloudyurl));
    }
}


void BitcoinGUI::viewBlockWebPage()
{
    //if (m_blockurl.length() > 0)
    //{
        if (!QDesktopServices::openUrl(QUrl("http://120.76.194.249:8099")))
        {
            QMessageBox::warning(this, tr("Open False"), tr("Open Brower False."));
        }
    //}
}

void BitcoinGUI::gotoReceiveCoinsPage()
{
//    btn_desc->setChecked(false);
//    btn_send->setChecked(false);
//    btn_logs->setChecked(false);
//    btn_addrs->setChecked(false);
//    btn_export->setEnabled(false);
    centralWidget->setCurrentWidget(receiveCoinsPage);

}

void BitcoinGUI::gotoSendCoinsPage()
{
//    btn_desc->setChecked(false);
//    btn_recv->setChecked(false);
//    btn_logs->setChecked(false);
//    btn_addrs->setChecked(false);
//    btn_export->setEnabled(false);
    centralWidget->setCurrentWidget(sendCoinsPage);

}

void BitcoinGUI::gotoSignMessageTab(QString addr)
{
    btn_export->setEnabled(false);
    // call show() in showTab_SM()
    signVerifyMessageDialog->showTab_SM(true);

    if(!addr.isEmpty())
        signVerifyMessageDialog->setAddress_SM(addr);
}

void BitcoinGUI::gotoVerifyMessageTab(QString addr)
{
    btn_export->setEnabled(false);
    // call show() in showTab_VM()
    signVerifyMessageDialog->showTab_VM(true);

    if(!addr.isEmpty())
        signVerifyMessageDialog->setAddress_VM(addr);
}

void BitcoinGUI::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept only URIs
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void BitcoinGUI::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        int nValidUrisFound = 0;
        QList<QUrl> uris = event->mimeData()->urls();
        foreach(const QUrl &uri, uris)
        {
            if (sendCoinsPage->handleURI(uri.toString()))
                nValidUrisFound++;
        }

        // if valid URIs were found
        if (nValidUrisFound)
            gotoSendCoinsPage();
        else
            notificator->notify(Notificator::Warning, tr("URI handling"), tr("URI can not be parsed! This can be caused by an invalid MetisCoin address or malformed URI parameters."));
    }

    event->acceptProposedAction();
}

void BitcoinGUI::handleURI(QString strURI)
{
    // URI has to be valid
    if (sendCoinsPage->handleURI(strURI))
    {
        showNormalIfMinimized();
        gotoSendCoinsPage();
    }
    else
        notificator->notify(Notificator::Warning, tr("URI handling"), tr("URI can not be parsed! This can be caused by an invalid MetisCoin address or malformed URI parameters."));
}

void BitcoinGUI::setEncryptionStatus(int status)
{
    switch(status)
    {
    case WalletModel::Unencrypted:
        labelEncryptionIcon->hide();
        encryptWalletAction->setChecked(false);
        changePassphraseAction->setEnabled(false);
        encryptWalletAction->setEnabled(true);
        break;
    case WalletModel::Unlocked:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(":/icons/lock_open").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>unlocked</b>"));
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    case WalletModel::Locked:
        labelEncryptionIcon->show();
        labelEncryptionIcon->setPixmap(QIcon(":/icons/lock_closed").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>locked</b>"));
        encryptWalletAction->setChecked(true);
        changePassphraseAction->setEnabled(true);
        encryptWalletAction->setEnabled(false); // TODO: decrypt currently not supported
        break;
    }
}

void BitcoinGUI::encryptWallet(bool status)
{
    if(!walletModel)
        return;
    AskPassphraseDialog dlg(status ? AskPassphraseDialog::Encrypt:
                                     AskPassphraseDialog::Decrypt, this);
    dlg.loadQssConfig(&dlg);
    dlg.setModel(walletModel);
    dlg.exec();

    setEncryptionStatus(walletModel->getEncryptionStatus());
}

void BitcoinGUI::backupWallet()
{
    QString saveDir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    QString filename = QFileDialog::getSaveFileName(this, tr("Backup Wallet"), saveDir, tr("Wallet Data (*.dat)"));
    if(!filename.isEmpty()) {
        if(!walletModel->backupWallet(filename)) {
            QMessageBox::warning(this, tr("Backup Failed"), tr("There was an error trying to save the wallet data to the new location."));
        }
    }
}

void BitcoinGUI::changePassphrase()
{
    AskPassphraseDialog dlg(AskPassphraseDialog::ChangePass, this);
    dlg.loadQssConfig(&dlg);
    dlg.setModel(walletModel);
    dlg.exec();
}

void BitcoinGUI::unlockWallet()
{
    if(!walletModel)
        return;
    // Unlock wallet when requested by wallet model
    if(walletModel->getEncryptionStatus() == WalletModel::Locked)
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::Unlock, this);
        dlg.loadQssConfig(&dlg);
        dlg.setModel(walletModel);
        dlg.exec();
    }
}

void BitcoinGUI::showNormalIfMinimized(bool fToggleHidden)
{
    // activateWindow() (sometimes) helps with keyboard focus on Windows
    if (isHidden())
    {
        show();
        activateWindow();
    }
    else if (isMinimized())
    {
        showNormal();
        activateWindow();
    }
    else if (GUIUtil::isObscured(this))
    {
        raise();
        activateWindow();
    }
    else if(fToggleHidden)
        hide();
}

void BitcoinGUI::toggleHidden()
{
    showNormalIfMinimized(true);
}
