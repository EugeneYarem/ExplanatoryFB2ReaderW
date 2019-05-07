#include "fb2reader.h"
#include "listwidgetitem.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QResizeEvent>
#include <QScrollBar>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrlQuery>

static QStringList WikiSupportedLan = { "id", "de", "en", "eo", "nl", "kab", "vi",
                                        "tr", "az", "da", "es", "fr", "it", "lt",
                                        "oc", "pl", "pt", "pt-br", "ro", "cs", "bg",
                                        "ru", "uk", "ar", "sd", "fa", "ps", "mr",
                                        "hi", "bn", "mni", "kn", "th", "zh", "lzh",
                                        "ja", "yue", "ko" };

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeWidgetDefinitions->hide();
    ui->tableWidgeTranslations->hide();
    //ui->listWidget->hide();

    reader = new FB2Reader;
    currentPanelButton = ui->toolButtonContent;
    changeCurrentPanelButtonStyle(PanelButtonStyle::ActivePanelStyle);

    createConnects();
}

MainWindow::~MainWindow()
{
    delete reader;
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->textBrowser->verticalScrollBar() && event->type() == QEvent::Paint ) {
        if (reader->getNumberBookCharacters() > 0) {
            int currentPos = ui->textBrowser->cursorForPosition(QPoint(0, 0)).position();
            int percent = currentPos * 100 / reader->getNumberBookCharacters();
            ui->labelPercent->setText( QString::number(percent) + "%");

            if (currentPos < listItems[0])
                return QMainWindow::eventFilter(watched, event);

            int index = 0;
            if ( (currentPos < currentChapter.second && (index = -1))
                 || (currentPos > currentChapter.second && currentChapter.first + 1 < listItems.size()
                     && currentPos > listItems[ currentChapter.first + 1 ] && (index = 1)) ) {
                dynamic_cast<ListWidgetItem *>( ui->listWidget->item( currentChapter.first ) )->setSelectedWithIcon(false);
                dynamic_cast<ListWidgetItem *>( ui->listWidget->item( currentChapter.first + index ) )->setSelectedWithIcon(true);
                ui->labelChapter->setText( ui->listWidget->item( currentChapter.first + index )->text() );
                currentChapter = QPair<int, int>(currentChapter.first + index, listItems[currentChapter.first + index]);
                return QMainWindow::eventFilter(watched, event);
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

QNetworkReply *MainWindow::setQueryToGoogleTranlate(const QString &str, const QString &errorMessage)
{
//----- Create URL
    QUrl myurl;
    myurl.setScheme("https"); //https also applicable
    myurl.setHost("translate.googleapis.com"/*"ws.detectlanguage.com"*/); // https://translate.googleapis.com/translate_a/single?client=gtx&sl=auto&tl=uk&dt=t&q=Tiu%20frazo%20estas%20skribita%20en%20Esperanto.
    myurl.setPath("/translate_a/single"/*"/0.2/detect"*/);

//----- Create URL query
    QUrlQuery querystr;
    querystr.addQueryItem("client", "gtx");
    querystr.addQueryItem("sl", "auto");
    querystr.addQueryItem("tl", "uk");
    querystr.addQueryItem("dt", "t");
    querystr.addQueryItem("q", str);
    //querystr.addQueryItem("key", "demo");

    myurl.setQuery(querystr); // Add query to URL

    qDebug() << "\n-------------- Translation";
    qDebug() << str;
    qDebug() << myurl.toString();

//----- Create network request
    QNetworkRequest request;
    request.setUrl(myurl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

//----- Create network access manager
    QNetworkAccessManager * restclient;
    restclient = new QNetworkAccessManager(this);
    connect(restclient, &QNetworkAccessManager::destroyed, [] () {qDebug() << "\n-------------- Translation manager deleted";});

//----- Create and read reply
    QNetworkReply * reply = restclient->get(request);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [this, errorMessage] (/*QNetworkReply::NetworkError code*/) {
                QMessageBox::critical(this, tr("Помилка з'єднання"), errorMessage);
    });
    connect(reply, &QNetworkReply::destroyed, [] () {qDebug() << "\n-------------- Translation reply deleted";});
    return reply;
}

void MainWindow::addTopLevelItemsInTreeW() const
{
    ui->treeWidgetDefinitions->addTopLevelItem(new QTreeWidgetItem(QStringList("Найточніше")));
    ui->treeWidgetDefinitions->addTopLevelItem(new QTreeWidgetItem(QStringList("Інші")));
}

void MainWindow::detectLanAndFindDef()
{
    QNetworkReply * reply = setQueryToGoogleTranlate(ui->textBrowser->textCursor().selectedText(),
                                                     tr("Сталася помилка під час з'єднання з Вікіпедією.\n"
                                                        "Можливі причини помилки:\n"
                                                        "\t1) не вдалося встановити зв'язок з Вікіпедією;\n"
                                                        "\t2) Вікіпедія не підтримує мову обраного тексту;\n"
                                                        "\t3) проблеми зв'язку з мережею.\n"));

    connect(reply, &QNetworkReply::finished, [reply, this] () {

    //----- Read data from reply and close reply with manager
        QByteArray arr = reply->readAll();
        reply->close();
        reply->manager()->clearAccessCache();
        reply->manager()->deleteLater();
        reply->deleteLater();

    //----- Parse server answer
        QJsonDocument doc = QJsonDocument::fromJson(arr);
        if (doc.isEmpty() && !doc.isArray())
            return;

        qDebug() << doc.toJson();

        QJsonArray jArr = doc.array();
        if (jArr.isEmpty())
            return;

        this->findDefinition(jArr.at(2).toString());
    });    
}

void MainWindow::changeActivePanel(MainWindow::ActivePanel newActivePanel)
{
    currentPanel = newActivePanel;
    changeCurrentPanelButtonStyle(PanelButtonStyle::DisactivatedPanelStyle);

    switch (newActivePanel) {
    case ContentPanel:
        currentPanelButton = ui->toolButtonContent;
        if( !ui->listWidget->isVisible() ) {
            this->hideAllPanels();
            ui->listWidget->show();
        }
        break;
    case BookmarksPanel:
        currentPanelButton = ui->toolButtonBookmarks;
        if( !ui->listWidget->isVisible() ) {
            this->hideAllPanels();
            ui->listWidget->show();
        }
        break;
    case DefinitionsPanel:
        currentPanelButton = ui->toolButtonDefinitions;
        if( !ui->treeWidgetDefinitions->isVisible() ) {
            this->hideAllPanels();
            ui->treeWidgetDefinitions->show();
        }
        break;
    case TranslationsPanel:
        currentPanelButton = ui->toolButtonTranslations;
        if( !ui->tableWidgeTranslations->isVisible() ) {
            this->hideAllPanels();
            ui->tableWidgeTranslations->show();
        }
        break;
    }

    changeCurrentPanelButtonStyle(PanelButtonStyle::ActivePanelStyle);
}

void MainWindow::changeCurrentPanelButtonStyle(MainWindow::PanelButtonStyle style) const
{
    if (style == PanelButtonStyle::ActivePanelStyle) {
        currentPanelButton->setStyleSheet("QToolButton { background-color: rgba(141, 209, 211, 255);"
                                          "border: 2px solid #8DD1D3;"
                                          "border-radius: 7px; }"
                                          ""
                                          "QToolButton:hover { background-color: rgba(141, 209, 211, 100); }");
        currentPanelButton->setFocus();
    } else currentPanelButton->setStyleSheet("QToolButton { border: 2px solid #8DD1D3;"
                                           "border-radius: 7px; }"
                                           ""
                                           "QToolButton:hover {background-color: rgba(141, 209, 211, 100); }"
                                           ""
                                           "QToolButton:focus {background-color: rgba(141, 209, 211, 255); }");
}

void MainWindow::hideAllPanels() const
{
    ui->listWidget->hide();
    ui->tableWidgeTranslations->hide();
    ui->treeWidgetDefinitions->hide();
}

void MainWindow::clearBeforeOpening()
{
    listItems.clear();
    ui->listWidget->clear();
}

void MainWindow::createActionsConnects()
{
    FB2Reader * r = reader;

    connect(ui->findDefinitionAction, &QAction::triggered, [this] () {
        ui->treeWidgetDefinitions->clear();
        this->addTopLevelItemsInTreeW();
        this->detectLanAndFindDef();
        this->changeActivePanel(MainWindow::ActivePanel::DefinitionsPanel);
        ui->treeWidgetDefinitions->expandAll();
    });
    connect(ui->openBookAction, &QAction::triggered, [r, this] () {
        QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
        QString fileName = QFileDialog::getOpenFileName(this, tr("Будь ласка, оберіть книгу"), QDir::toNativeSeparators(env.value("USERPROFILE")), tr("FB2-книги (*.fb2)"));
        qDebug() << fileName;

        if (!fileName.isEmpty() && !fileName.isNull()) {
            r->parseBook(fileName);
            this->clearBeforeOpening();
            ui->textBrowser->setHtml(r->book());
            ui->labelBook->setText(r->getBookName());
            ui->labelChapter->setText(r->getFirstChapterName()); // 0 - return the book's name
            this->changeActivePanel(MainWindow::ActivePanel::ContentPanel);
            r->setNumberBookCharacters( ui->textBrowser->toPlainText().length() );

            const QMap<unsigned int, FB2Reader::ChapterData> & content = r->getContent();
            QMap<unsigned int, FB2Reader::ChapterData>::const_iterator i = content.cbegin();
            int row = 0;
            while (i != content.cend()) {
                ListWidgetItem * item = new ListWidgetItem(
                            i.key(),
                            reader->findPositionByChapterId( ui->textBrowser->toPlainText(), i.key() ),
                            i.value().name);

                ui->listWidget->insertItem( ui->listWidget->count(), item );
                this->listItems.insert(row++, item->getChapterPos());
                if (i == content.cbegin()) {
                    currentChapter = QPair<int, int>(0, item->getChapterPos());
                    item->setSelectedWithIcon(true);
                }
                ++i;
            }

            QMap<int, int>::const_iterator j = listItems.cbegin();
            while (j != listItems.cend()) {
                qDebug() << j.key() << " " << j.value();
                ++j;
            }
        }
    });
    connect(ui->translateAction, &QAction::triggered, [this] () {
        this->translate();
        this->changeActivePanel(MainWindow::ActivePanel::TranslationsPanel);
    });
}

void MainWindow::createConnects()
{
    ui->textBrowser->verticalScrollBar()->installEventFilter(this);

    createToolButtonsConnections();
    createActionsConnects();
    createPanelsConnects();
}

void MainWindow::createPanelsConnects()
{
    //FB2Reader * r = reader;
    connect(ui->listWidget, &QListWidget::currentItemChanged, [this/*, r*/] (QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous)
            dynamic_cast<ListWidgetItem *>( previous )->setSelectedWithIcon(false);
        else dynamic_cast<ListWidgetItem *>( ui->listWidget->item(0) )->setSelectedWithIcon(false);

        QTextCursor cursor = ui->textBrowser->textCursor();
        cursor.setPosition( dynamic_cast<ListWidgetItem *>( current )->getChapterPos() );
        ui->textBrowser->setTextCursor(cursor);

        int cursorY = ui->textBrowser->cursorRect().top();
        QScrollBar *vbar = ui->textBrowser->verticalScrollBar();
        vbar->setValue(vbar->value() + cursorY + 30);

        dynamic_cast<ListWidgetItem *>( ui->listWidget->item(this->currentChapter.first) )->setSelectedWithIcon(false);
        this->currentChapter = QPair<int, int>(ui->listWidget->row(current), dynamic_cast<ListWidgetItem *>( current )->getChapterPos());
        dynamic_cast<ListWidgetItem *>( current )->setSelectedWithIcon(true);
        ui->labelChapter->setText( current->text() );
    });
    connect(ui->treeWidgetDefinitions, &QTreeWidget::doubleClicked, [this] (/*const QModelIndex &index*/) {
        if (this->currentPanel == ActivePanel::DefinitionsPanel) {
            QDesktopServices::openUrl(QUrl(ui->treeWidgetDefinitions->currentItem()->text(2), QUrl::TolerantMode));
        }
    });
}

void MainWindow::createToolButtonsConnections()
{
    connect(ui->toolButtonContent, &QToolButton::pressed, [this] () {
        if (currentPanelButton != ui->toolButtonContent) {
            this->changeActivePanel(MainWindow::ActivePanel::ContentPanel);
        }
    });
    connect(ui->toolButtonBookmarks, &QToolButton::pressed, [this] () {
        if (currentPanelButton != ui->toolButtonBookmarks) {
            this->changeActivePanel(MainWindow::ActivePanel::BookmarksPanel);
        }
    });
    connect(ui->toolButtonDefinitions, &QToolButton::pressed, [this] () {
        if (currentPanelButton != ui->toolButtonDefinitions) {
            this->changeActivePanel(MainWindow::ActivePanel::DefinitionsPanel);
        }
    });
    connect(ui->toolButtonTranslations, &QToolButton::pressed, [this] () {
        if (currentPanelButton != ui->toolButtonTranslations) {
            this->changeActivePanel(MainWindow::ActivePanel::TranslationsPanel);
        }
    });
}

void MainWindow::findDefinition(const QString &language)
{
    qDebug() << "\n-------------- Language: " << language;
    /*if (!WikiSupportedLan.contains(language)) {
        QMessageBox::critical(this, tr("Помилка пошуку"), tr("Значення слова не може бути отримане через відсутність підтримки Вікіпедією мови виділеного тексту."));
    }*/

    QString str = ui->textBrowser->textCursor().selectedText();

//----- Create URL
    QUrl myurl;
    myurl.setScheme("http"); //https also applicable
    myurl.setHost(language + ".wikipedia.org");
    myurl.setPath("/w/api.php");

//----- Create URL query
    QUrlQuery querystr;
    querystr.addQueryItem("action", "opensearch");
    querystr.addQueryItem("search", str);
    querystr.addQueryItem("prop", "info");
    querystr.addQueryItem("format", "json");
    querystr.addQueryItem("inprop", "url");

    myurl.setQuery(querystr); // Add query to URL

    qDebug() << "\n-------------- Definition";
    qDebug() << str;
    qDebug() << myurl.toString();

//----- Create network request
    QNetworkRequest request;
    request.setUrl(myurl);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

//----- Create network access manager
    QNetworkAccessManager *restclient;
    restclient = new QNetworkAccessManager(this);
    connect(restclient, &QNetworkAccessManager::destroyed, [] () { qDebug() << "\n-------------- Definition manager deleted"; });

//----- Create and read reply
    QNetworkReply *reply = restclient->get(request);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [this] (/*QNetworkReply::NetworkError code*/) {
                QMessageBox::critical(this, tr("Помилка з'єднання"), tr("Сталася помилка під час з'єднання з Вікіпедією.\n"
                                                                        "Можливі причини помилки:\n"
                                                                        "\t1) не вдалося встановити зв'язок з Вікіпедією;\n"
                                                                        "\t2) Вікіпедія не підтримує мову обраного тексту;\n"
                                                                        "\t3) проблеми зв'язку з мережею.\n"));
    });
    connect(reply, &QNetworkReply::finished, [reply, this] () {

    //----- Read data from reply and close reply with manager
        QByteArray arr = reply->readAll();
        reply->close();
        reply->manager()->clearAccessCache();
        reply->manager()->deleteLater();
        reply->deleteLater();

    //----- Parse server answer
        QJsonDocument doc = QJsonDocument::fromJson(arr);
        if (doc.isEmpty() && !doc.isArray())
            return;

        qDebug() << doc.toJson();

        QJsonArray jArr = doc.array();
        if (jArr.isEmpty())
            return;

        QVector<QJsonArray> columns;

        QJsonArray::iterator i = jArr.begin();
        int curColumn = 0;
        while (i != jArr.end() && curColumn < 3) {
            if (i->isArray()) {
                columns.push_back(i->toArray());
                ++curColumn;
            }
            ++i;
        }

        for (int j = 0; j < columns.at(0).count(); ++j) {
            QStringList row = {
                                columns.at(0).at(j).toString(), // name
                                columns.at(1).at(j).toString(), // definition
                                columns.at(2).at(j).toString(), // link
            };

            QTreeWidgetItem *newItem = new QTreeWidgetItem(row);
            newItem->setSizeHint(1, QSize(300, 130));
            if (j == 0)
                ui->treeWidgetDefinitions->topLevelItem(0)->addChild(newItem);
            else ui->treeWidgetDefinitions->topLevelItem(1)->addChild(newItem);
        }
    });

    connect(reply, &QNetworkReply::destroyed, [] () { qDebug() << "\n-------------- Definition reply deleted"; });
}

void MainWindow::translate()
{
    QNetworkReply * reply = setQueryToGoogleTranlate(ui->textBrowser->textCursor().selectedText(),
                                                     tr("Сталася помилка під час з'єднання з Google Translate.\n"
                                                        "Можливі причини помилки:\n"
                                                        "\t1) не вдалося встановити зв'язок з Google Translate;\n"
                                                        "\t2) проблеми зв'язку з мережею.\n"));

    connect(reply, &QNetworkReply::finished, [reply, this] () {

    //----- Read data from reply and close reply with manager
        QByteArray arr = reply->readAll();
        reply->close();
        reply->manager()->clearAccessCache();
        reply->manager()->deleteLater();
        reply->deleteLater();

    //----- Parse server answer
        QJsonDocument doc = QJsonDocument::fromJson(arr);
        if (doc.isEmpty() && !doc.isArray())
            return;

        qDebug() << doc.toJson();

        QJsonArray jArr = doc.array();
        if (jArr.isEmpty())
            return;

        int row = ui->tableWidgeTranslations->rowCount();
        ui->tableWidgeTranslations->insertRow( row );
        QTableWidgetItem *newItem1 = new QTableWidgetItem( jArr.at(0).toArray().at(0).toArray().at(1).toString() );
        QTableWidgetItem *newItem2 = new QTableWidgetItem( jArr.at(0).toArray().at(0).toArray().at(0).toString() );
        ui->tableWidgeTranslations->setItem(row, 0, newItem1);
        ui->tableWidgeTranslations->setItem(row, 1, newItem2);
    });
}

