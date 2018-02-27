#include "mainwindow.h"
#include "ui_mainwindow.h"

QString version = "1.0";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->bDelRasp->setEnabled(false);
    this->setWindowTitle("Auto Configure Database for Kodi");
    Find_Local_Ip();

    connect(ui->bAddRasp,SIGNAL(clicked(bool)),this,SLOT(AddRasp()));
    connect(ui->bDelRasp,SIGNAL(clicked(bool)),this,SLOT(DelRasp()));
    connect(ui->listRasp,SIGNAL(currentRowChanged(int)),this,SLOT(ChangeDelBtn()));
    connect(ui->process,SIGNAL(clicked(bool)),this,SLOT(Process()));
    connect(ui->actionA_Propos_de_Qt,SIGNAL(triggered(bool)),qApp,SLOT(aboutQt()));
    connect(ui->actionA_Propos,SIGNAL(triggered(bool)),this,SLOT(A_Propos()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddRasp()
{
    if(!ui->newRasp->text().isEmpty() && ui->newRasp->text().split(".").count() == 4)
    {
        ui->listRasp->addItem(ui->newRasp->text());
        ui->newRasp->clear();
    }
    else
        QMessageBox::warning(this,"","Format incorrect, uniquement un ip est toléré(ex : 192.168.1.100)");
}

void MainWindow::DelRasp()
{
    if(ui->listRasp->currentRow() >= 0)
    {
        QStringList list;
        for(int i=0;i<ui->listRasp->count();i++)
            if(ui->listRasp->item(i)->text() != ui->listRasp->currentItem()->text())
                list.append(ui->listRasp->item(i)->text());
        ui->listRasp->clear();
        for(int i=0;i<list.count();i++)
            ui->listRasp->addItem(list.at(i));
    }
    ui->bDelRasp->setEnabled(false);
}

void MainWindow::ChangeDelBtn()
{
    if(ui->listRasp->currentRow() >= 0)
        ui->bDelRasp->setEnabled(true);
    else
        ui->bDelRasp->setEnabled(false);
}

void MainWindow::Process()
{
    create_window();
    //Control
    if(ui->loginRasp->text().isEmpty() || ui->passRasp->text().isEmpty())
    {
        QMessageBox::warning(this,"","Login ou mot de passe non saisie, annulation");
        update_windows("",17);
        return;
    }
    if(ui->ipLocal->text() == "0.0.0.0")
    {
        QMessageBox::warning(this,"","Adresse ip local non trouvé, annulation");
        update_windows("",17);
        return;
    }

    //Verif MS C++
    QProcess p;
    QEventLoop pr;
    //2008
    update_windows("Installation de Microsoft visual c++ 2008 x86",1);
    p.start("Data/vcredist_x86_2008.exe /q");
    pr.processEvents();
    p.waitForFinished(300000);
    update_windows("Installation de Microsoft visual c++ 2008 x64",2);
    p.start("Data/vcredist_x64_2008.exe /q");
    pr.processEvents();
    p.waitForFinished(300000);
    //2010
    update_windows("Installation de Microsoft visual c++ 2010 x86",3);
    p.start("Data/vcredist_x86_2010.exe /passive");
    pr.processEvents();
    p.waitForFinished(300000);
    update_windows("Installation de Microsoft visual c++ 2010 x64",4);
    p.start("Data/vcredist_x64_2010.exe /passive");
    pr.processEvents();
    p.waitForFinished(300000);
    //2012
    update_windows("Installation de Microsoft visual c++ 2012 x86",5);
    p.start("Data/vcredist_x86_2012.exe /passive");
    pr.processEvents();
    p.waitForFinished(300000);
    update_windows("Installation de Microsoft visual c++ 2012 x64",6);
    p.start("Data/vcredist_x64_2012.exe /passive");
    pr.processEvents();
    p.waitForFinished(300000);
    //2013
    update_windows("Installation de Microsoft visual c++ 2013 x86",7);
    p.start("Data/vcredist_x86_2013.exe /passive");
    pr.processEvents();
    p.waitForFinished(300000);
    update_windows("Installation de Microsoft visual c++ 2013 x64",8);
    p.start("Data/vcredist_x64_2013.exe /passive");
    pr.processEvents();
    p.waitForFinished(300000);
    //2015
    update_windows("Installation de Microsoft visual c++ 2015 x86",9);
    p.start("Data/vcredist_x86_2015.exe /passive");
    pr.processEvents();
    p.waitForFinished(300000);
    update_windows("Installation de Microsoft visual c++ 2015 x64",10);
    p.start("Data/vcredist_x64_2015.exe /passive");
    pr.processEvents();
    p.waitForFinished(300000);

    //Wamp
    update_windows("Installation de Wamp server",11);
    p.start("Data/wampserver.exe /SILENT /SUPPRESSMSGBOXES");
    pr.processEvents();
    p.waitForFinished(300000);

    //Force Services auto
    update_windows("Configuration du service mysql",12);
    system("sc config wampmysqld start= auto");
    system("sc start wampmysqld");

    //Add User mySQL
    update_windows("Configuration de la base de données",13);
    QEventLoop l;
    QTimer t;
    connect(&t,SIGNAL(timeout()),&l,SLOT(quit()));
    t.start(5000);
    l.exec();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("");
    db.setPort(3306);
    if(!db.open())
    {
        QMessageBox::warning(this,"","Echec de connexion à la base de données\n" + db.lastError().text());
        update_windows("",17);
        return;
    }

    //Control User
    QSqlQuery req;
    db.setDatabaseName("mysql");
    bool test(false);
    if(!req.exec("SELECT * FROM user"))
    {
        QMessageBox::warning(this,"","Lecture de la base de données impossible");
        update_windows("",17);
        return;
    }
    while(req.next())
        if(req.value("").toString().contains(ui->loginRasp->text()))
            test = true;

    //Create User
    if(!test)
    {
        if(!req.exec("CREATE USER '" + ui->loginRasp->text() + "'@'localhost' IDENTIFIED BY '" + ui->passRasp->text() + "';"))
        {
            QMessageBox::warning(this,"",tr("Création de l'utilisateur %0 échoué").arg(ui->loginRasp->text()));
            update_windows("",17);
            return;
        }
        if(!req.exec("GRANT ALL PRIVILEGES ON *.* TO '" + ui->loginRasp->text() + "'@'localhost'"))
        {
            QMessageBox::warning(this,"","Echec de modification des droits d'accès utilisateur");
            update_windows("",17);
            return;
        }
    }

    //Autorisation parefeu windows
    update_windows("Configuration du parefeu windows",14);
    QDesktopServices::openUrl(QUrl("C:/Windows/System32/Firewall.cpl"));
    QMessageBox::information(this,"","Le parefeu windows doit être configuré pour autoriser le port mysql\n"
                                     "pour ce faire allez dans :\n"
                                     "Paramètres avancés->Règles de trafic entrant->Nouvelle règle\n"
                                     "1 - Cochez Port\n"
                                     "2 - TCP et Port locaux -> 3306\n"
                                     "3 - Autoriser la connexion\n"
                                     "4 - Décochez Public\n"
                                     "5 - Nom -> mysql\n"
                                     "Répéter l'opération avec Règles de trafic sortant\n\n"
                                     "Cliquer sur Fait une fois les règles créer","Fait");


    //Upload Settings + Reboot PI
    update_windows("Mise à jour du script",15);
    QFile f("Data/script");
    if(!f.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,"","Ouverture du fichier script échoué");
        update_windows("",17);
        return;
    }
    QTextStream flux(&f);

    QString v = flux.readAll();
    v.replace("{IP}",ui->ipLocal->text());
    v.replace("{USER}",ui->loginRasp->text());
    v.replace("{PASS}",ui->passRasp->text());
    QFile f2("Data/updatedScript");
    f2.resize(0);
    if(!f2.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,"","Ouverture du fichier updatedScript échoué");
        update_windows("",17);
        return;
    }
    QTextStream flux2(&f2);
    flux2 << v;

    for(int i=0;i<ui->listRasp->count();i++)
    {
        update_windows("Injection du script sur " + ui->listRasp->item(i)->text(),16);
        p.start("putty.exe -l " + ui->loginRasp->text() + " -pw " + ui->passRasp->text() + " " + ui->listRasp->item(i)->text() + " -m Data/updatedScript");
        p.waitForFinished();
    }

    //End
    update_windows("",17);
    QMessageBox::information(this,"","Vôtre adresse ip local doit être statique, pensez à attribuer celle-ci dans vôtre routeur/box\n"
                                     "Ip : " + ui->ipLocal->text() + "\n"
                                     "Mac : " + ui->macLocal->text());
}

void MainWindow::Find_Local_Ip()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        if(!(netInterface.flags() & QNetworkInterface::IsLoopBack))
        {
            if(netInterface.interfaceFromIndex(netInterface.index()).addressEntries().count() > 1)
            {
                if(netInterface.interfaceFromIndex(netInterface.index()).addressEntries().at(1).ip().toString().contains("192.168"))
                {
                    ui->ipLocal->setText(netInterface.interfaceFromIndex(netInterface.index()).addressEntries().at(1).ip().toString());
                    ui->macLocal->setText(netInterface.interfaceFromIndex(netInterface.index()).hardwareAddress());
                }
                qDebug() << netInterface.interfaceFromIndex(netInterface.index()).addressEntries().at(1).ip() <<
                            netInterface.interfaceFromIndex(netInterface.index()).hardwareAddress();
            }
        }
    }
}

void MainWindow::A_Propos()
{
    QFormLayout *layout = new QFormLayout;
    QLabel vVersion(version);
    QLabel auteur("Kévin BRIAND");
    QLabel licence("Ce logiciel est sous licence GNU LGPLv3");
    QLabel github("<a href='https://github.com/firedream89/AutoDBKodi'>ici</a>");
    github.setOpenExternalLinks(true);
    layout->addRow("Version",&vVersion);
    layout->addRow("Auteur",&auteur);
    layout->addRow("Licence",&licence);
    layout->addRow("Sources",&github);
    QDialog *fen = new QDialog;
    fen->setLayout(layout);
    fen->setWindowTitle("A Propos d'Auto Configure Database for Kodi");
    fen->exec();
}

void MainWindow::create_window()
{
    QDialog *f = new QDialog(this);
    f->setWindowTitle("Chargement");
    f->setObjectName("Load");
    QFormLayout *l = new QFormLayout(f);
    QLabel *inf = new QLabel;
    inf->setObjectName("Inf");
    l->addRow("Etat : ",inf);
    QProgressBar *p = new QProgressBar;
    p->setObjectName("pLoad");
    p->setValue(0);
    p->setMinimum(0);
    p->setMaximum(17);
    l->addRow(p);
    f->show();
}

void MainWindow::update_windows(QString text,int progress)
{
    if(this->findChild<QDialog*>("Load"))
    {
        this->findChild<QDialog*>("Load")->findChild<QLabel*>("Inf")->setText(text);
        if(progress != -1)
            this->findChild<QDialog*>("Load")->findChild<QProgressBar*>("pLoad")->setValue(progress);
        if(progress == 17)
            this->findChild<QDialog*>("Load")->deleteLater();
    }
}
