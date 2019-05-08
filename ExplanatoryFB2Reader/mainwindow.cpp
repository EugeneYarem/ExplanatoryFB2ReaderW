#include "expounders/wikipediaexpounder.h"
#include "fb2reader.h"
#include "keeper.h"
#include "translators/googletranslator.h"
#include "listwidgetitem.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QResizeEvent>
#include <QScrollBar>

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
    ui->tableWidgetTranslations->hide();
    ui->tableWidgetBookmarks->hide();
    ui->tableWidgetBookmarks->setColumnWidth(1, 150);
    ui->tableWidgetBookmarks->setColumnHidden(2, true);
    ui->labelBook->clear();
    ui->labelChapter->clear();
    ui->labelPercent->clear();

    keeper = new Keeper;
    reader = new FB2Reader;
    currentPanelButton = ui->toolButtonContent;
    changeCurrentPanelButtonStyle(PanelButtonStyle::ActivePanelStyle);
    ui->cashingAction->setChecked( keeper->isMustCashing() );

    createConnects();
}

MainWindow::~MainWindow()
{
    delete keeper;
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
                dynamic_cast<ListWidgetItem *>( ui->listWidgetContent->item( currentChapter.first ) )->setSelectedWithIcon(false);
                dynamic_cast<ListWidgetItem *>( ui->listWidgetContent->item( currentChapter.first + index ) )->setSelectedWithIcon(true);
                ui->labelChapter->setText( ui->listWidgetContent->item( currentChapter.first + index )->text() );
                currentChapter = QPair<int, int>(currentChapter.first + index, listItems[currentChapter.first + index]);
                return QMainWindow::eventFilter(watched, event);
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::addTopLevelItemsInTreeW() const
{
    ui->treeWidgetDefinitions->addTopLevelItem(new QTreeWidgetItem(QStringList("Найточніше")));
    ui->treeWidgetDefinitions->addTopLevelItem(new QTreeWidgetItem(QStringList("Інші")));
}

void MainWindow::changeActivePanel(MainWindow::ActivePanel newActivePanel)
{
    currentPanel = newActivePanel;
    changeCurrentPanelButtonStyle(PanelButtonStyle::DisactivatedPanelStyle);

    switch (newActivePanel) {
    case ContentPanel:
        currentPanelButton = ui->toolButtonContent;
        if( !ui->listWidgetContent->isVisible() ) {
            this->hideAllPanels();
            ui->listWidgetContent->show();
        }
        break;
    case BookmarksPanel:
        currentPanelButton = ui->toolButtonBookmarks;
        if( !ui->tableWidgetBookmarks->isVisible() ) {
            this->hideAllPanels();
            ui->tableWidgetBookmarks->show();
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
        if( !ui->tableWidgetTranslations->isVisible() ) {
            this->hideAllPanels();
            ui->tableWidgetTranslations->show();
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
    ui->listWidgetContent->hide();
    ui->tableWidgetBookmarks->hide();
    ui->tableWidgetTranslations->hide();
    ui->treeWidgetDefinitions->hide();
}

void MainWindow::clearBeforeOpening()
{
    listItems.clear();
    ui->listWidgetContent->clear();
    ui->tableWidgetBookmarks->clearContents();
}

void MainWindow::createActionsConnects()
{
    FB2Reader * r = reader;

    connect(ui->closeBookAction, &QAction::triggered, [this] () {
        this->clearBeforeOpening();
        ui->textBrowser->clear();
        ui->labelBook->clear();
        ui->labelChapter->clear();
        ui->labelPercent->clear();
    });
    connect(ui->createBookmarkAction, &QAction::triggered, [this] () {
        bool ok;

        QString text = QInputDialog::getText(this, tr("Створити закладку"),
                                             tr("Назва закладки:"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty()) {
            int percent = ui->labelPercent->text().remove( ui->labelPercent->text().length() - 1, 1 ).toInt();
            int pos = ui->textBrowser->cursorForPosition(QPoint(0, 0)).position();
            if ( this->keeper->saveBookmark(text, percent, pos) ) {
                this->createBookmarkRow(text, percent, pos);
                this->changeActivePanel(MainWindow::ActivePanel::BookmarksPanel);
                QMessageBox::information(this, "Створення закладки", "Закладку було створено.");
            }
            else QMessageBox::warning(this, "Створення закладки", "Помилка!");
        }
    });
    connect(ui->exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(ui->findDefinitionAction, &QAction::triggered, [this] () {
        if(ui->textBrowser->textCursor().selectedText().isEmpty())
            return;

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

            const QMap<unsigned int, QString> & content = r->getContent();
            QMap<unsigned int, QString>::const_iterator i = content.cbegin();
            int row = 0;
            while (i != content.cend()) {
                ListWidgetItem * item = new ListWidgetItem(
                            i.key(),
                            reader->findPositionByChapterId( ui->textBrowser->toPlainText(), i.key() ),
                            i.value());

                ui->listWidgetContent->insertItem( ui->listWidgetContent->count(), item );
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

            keeper->saveBook(r->getBookName(), r->book());
            QVector<Keeper::Bookmark> bookmarks = keeper->readBookmarks( r->getBookName() );
            QVector<Keeper::Bookmark>::const_iterator it = bookmarks.cbegin();
            while (it != bookmarks.cend()) {
                this->createBookmarkRow(it->name, it->percent, it->position);
                ++it;
            }
            ui->tableWidgetBookmarks->sortItems(2);
        }
    });
    connect(ui->translateAction, &QAction::triggered, [this] () {
        if(ui->textBrowser->textCursor().selectedText().isEmpty())
            return;

        this->translate();
        this->changeActivePanel(MainWindow::ActivePanel::TranslationsPanel);
    });
}

void MainWindow::createBookmarkRow(const QString &name, int percent, int position) const
{
    int row = ui->tableWidgetBookmarks->rowCount();
    ui->tableWidgetBookmarks->insertRow( row );
    QTableWidgetItem *newItem1 = new QTableWidgetItem( name );
    QTableWidgetItem *newItem2 = new QTableWidgetItem( QString::number(percent) + "%" );
    QTableWidgetItem *newItem3 = new QTableWidgetItem( QString::number(position) );
    ui->tableWidgetBookmarks->setItem(row, 0, newItem1);
    ui->tableWidgetBookmarks->setItem(row, 1, newItem2);
    ui->tableWidgetBookmarks->setItem(row, 2, newItem3);
}

void MainWindow::createConnects()
{
    ui->textBrowser->verticalScrollBar()->installEventFilter(this);
    connect(ui->cashingAction, &QAction::triggered, keeper, &Keeper::setCashing);

    createToolButtonsConnections();
    createActionsConnects();
    createPanelsConnects();
}

void MainWindow::createPanelsConnects()
{
    connect(ui->listWidgetContent, &QListWidget::currentItemChanged, [this] (QListWidgetItem *current, QListWidgetItem *previous) {
        if (previous)
            dynamic_cast<ListWidgetItem *>( previous )->setSelectedWithIcon(false);
        else dynamic_cast<ListWidgetItem *>( ui->listWidgetContent->item(0) )->setSelectedWithIcon(false);

        moveToPosition(dynamic_cast<ListWidgetItem *>( current )->getChapterPos());

        dynamic_cast<ListWidgetItem *>( ui->listWidgetContent->item(this->currentChapter.first) )->setSelectedWithIcon(false);
        this->currentChapter = QPair<int, int>(ui->listWidgetContent->row(current), dynamic_cast<ListWidgetItem *>( current )->getChapterPos());
        dynamic_cast<ListWidgetItem *>( current )->setSelectedWithIcon(true);
        ui->labelChapter->setText( current->text() );
    });
    connect(ui->treeWidgetDefinitions, &QTreeWidget::doubleClicked, [this] (/*const QModelIndex &index*/) {
        if (this->currentPanel == ActivePanel::DefinitionsPanel) {
            QDesktopServices::openUrl(QUrl(ui->treeWidgetDefinitions->currentItem()->text(2), QUrl::TolerantMode));
        }
    });
    connect(ui->tableWidgetBookmarks, &QTableWidget::itemClicked, [this] (QTableWidgetItem *item) {
        int row = ui->tableWidgetBookmarks->row(item),
            pos = ui->tableWidgetBookmarks->item( row, 2 )->text().toInt(),
            listRow = 0;

        QMap<int, int>::const_iterator i = listItems.cbegin();
        while (i != listItems.cend()) {
            if (i.value() > pos) {
                listRow = i.key() - 1;
                break;
            }
            ++i;
        }
        emit ui->listWidgetContent->currentItemChanged( ui->listWidgetContent->item(listRow),
                                                        ui->listWidgetContent->currentItem());
        moveToPosition( pos );
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

void MainWindow::moveToPosition(int position) const
{
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.setPosition( position );
    ui->textBrowser->setTextCursor(cursor);

    int cursorY = ui->textBrowser->cursorRect().top();
    QScrollBar *vbar = ui->textBrowser->verticalScrollBar();
    vbar->setValue(vbar->value() + cursorY + 30);
}

void MainWindow::detectLanAndFindDef()
{
    GoogleTranslator *translator = new GoogleTranslator;
    connect(translator, &Translator::errorOccurred, [this] () {
        QMessageBox::critical(this, tr("Помилка з'єднання"), tr("Сталася помилка під час з'єднання з Вікіпедією.\n"
                                                                "Можливі причини помилки:\n"
                                                                "\t1) не вдалося встановити зв'язок з Вікіпедією;\n"
                                                                "\t2) Вікіпедія не підтримує мову обраного тексту;\n"
                                                                "\t3) проблеми зв'язку з мережею.\n"));
    });
    connect(translator, &Translator::detected,
            [this, translator] (QMap<QString, double> langAndProbability) {
                if (langAndProbability.isEmpty()) {
                    translator->deleteLater();
                    return;
                }
                findDefinition(langAndProbability.begin().key());
                translator->deleteLater();
            }
    );
    translator->detectLanguage(ui->textBrowser->textCursor().selectedText());
}

void MainWindow::findDefinition(const QString &language)
{
    WikipediaExpounder *wiki = new WikipediaExpounder(language);
    connect(wiki, &Expounder::errorOccurred, [this] () {
        QMessageBox::critical(this, tr("Помилка з'єднання"), tr("Сталася помилка під час з'єднання з Вікіпедією.\n"
                                                                "Можливі причини помилки:\n"
                                                                "\t1) не вдалося встановити зв'язок з Вікіпедією;\n"
                                                                "\t2) Вікіпедія не підтримує мову обраного тексту;\n"
                                                                "\t3) проблеми зв'язку з мережею.\n"));
    });
    connect(wiki, &Expounder::explanationFound, [this, wiki] (QVector<Expounder::Explanation> explanations) {
        if (explanations.isEmpty()) {
            wiki->deleteLater();
            return;
        }
        QVector<Expounder::Explanation>::const_iterator i = explanations.cbegin();
        while (i != explanations.cend()) {
            QStringList row = {
                                i->idea, // name
                                i->brief, // definition
                                i->link, // link
            };
            QTreeWidgetItem *newItem = new QTreeWidgetItem(row);
            newItem->setSizeHint(1, QSize(300, 130));
            if (i == explanations.cbegin())
                ui->treeWidgetDefinitions->topLevelItem(0)->addChild(newItem);
            else ui->treeWidgetDefinitions->topLevelItem(1)->addChild(newItem);

            ++i;
        }
        wiki->deleteLater();
    });
    wiki->findExplanation(ui->textBrowser->textCursor().selectedText());
}

void MainWindow::translate()
{
    int row = ui->tableWidgetTranslations->rowCount();
    GoogleTranslator *translator = new GoogleTranslator;
    connect(translator, &Translator::errorOccurred, [this] () {
        QMessageBox::critical(this, tr("Помилка з'єднання"), tr("Сталася помилка під час з'єднання з Google Translate.\n"
                                                                "Можливі причини помилки:\n"
                                                                "\t1) не вдалося встановити зв'язок з Google Translate;\n"
                                                                "\t2) проблеми зв'язку з мережею.\n"));
    });
    connect(translator, &Translator::translated,
            [this, row, translator] (QVector<Translator::Translation> translations) {
                if (translations.isEmpty()) {
                    translator->deleteLater();
                    return;
                }

                QVector<Translator::Translation>::const_iterator i = translations.cbegin();
                while (i != translations.cend()) {
                    ui->tableWidgetTranslations->insertRow( row );
                    QTableWidgetItem *newItem1 = new QTableWidgetItem( ui->textBrowser->textCursor().selectedText() );
                    QTableWidgetItem *newItem2 = new QTableWidgetItem( i->translation );
                    ui->tableWidgetTranslations->setItem(row, 0, newItem1);
                    ui->tableWidgetTranslations->setItem(row, 1, newItem2);

                    ++i;
                }
                translator->deleteLater();
    });
    translator->translate(ui->textBrowser->textCursor().selectedText());
}

