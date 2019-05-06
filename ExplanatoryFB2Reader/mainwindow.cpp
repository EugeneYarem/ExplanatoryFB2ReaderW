#include "fb2reader.h"
#include "listwidgetitem.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QProcessEnvironment>
#include <QResizeEvent>
#include <QScrollBar>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeWidget->hide();
    //ui->listWidget->hide();

    reader = new FB2Reader;
    currentPanelButton = ui->toolButtonContent;
    changeCurrentPanelButtonStyle(PanelButtonStyle::ActivePanelStyle);

    createConnects();

    //ui->menuBar->setStyleSheet("QMenuBar::item { height: 30px; max-height: 30px; min-height: 30px; margin: 0px; }");
}

MainWindow::~MainWindow()
{
    delete reader;
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if((watched == ui->listWidget || watched == ui->treeWidget) && event->type() == QEvent::Resize )
    {
        //qDebug() << "h";
        ui->widget->setFixedWidth(dynamic_cast<QResizeEvent *>(event)->size().width());
    }

    if(watched == ui->textBrowser->verticalScrollBar() && event->type() == QEvent::Paint )
    {
        /*qDebug() << event->type();
        qDebug() << ui->textBrowser->cursorRect();
        qDebug() << "textCurosr.Position" << ui->textBrowser->textCursor().position();
        qDebug() << "y" << ui->textBrowser->cursorRect().y();
        qDebug() << "" << ui->textBrowser->cursorForPosition(QPoint(0, ui->textBrowser->cursorRect().y())).position();
        //ui->widget->setFixedWidth(dynamic_cast<QResizeEvent *>(event)->size().width());*/

        if(reader->getNumberBookCharacters() > 0/* && contentLoaded*/) {
            //qDebug() << ui->textBrowser->textCursor().position();
            //qDebug() << reader->getNumberBookCharacters();
            int currentPos = ui->textBrowser->cursorForPosition(QPoint(0, 0)/*bottom_right*/).position();
            int percent = currentPos * 100 / reader->getNumberBookCharacters();
            ui->labelPercent->setText( QString::number(percent) + "%");

            if(currentPos < listItems[0])
                return QMainWindow::eventFilter(watched, event);

            qDebug() << "\t\t\thello " << currentPos;
            //ListWidgetItem * item;
            //if(ui->listWidget->currentItem())
                //item = dynamic_cast<ListWidgetItem *>( ui->listWidget->currentItem() );
            //else item = dynamic_cast<ListWidgetItem *>( ui->listWidget->item(0) );
            //if(currentPos < item->getChapterPos())

            //QMap<int, int>::const_iterator i = listItems.cbegin();
            //while(i != listItems.cend())
            //{
                qDebug() << "\t\t\thello";
                //if(currentPos < i.value() && i.key() > 0 && listItems[i.key() - 1] != item->getChapterPos())

                //if(currentPos > currentChapter.second && currentPos < listItems[ currentChapter.second + 1]
                        //&& i.key() > 0 && listItems[i.key() - 1] != item->getChapterPos())
                if(currentPos < currentChapter.second/* && currentChapter.first < listItems.size()*/)
                {
                    qDebug() << "hello";
                    //item->setSelectedWithIcon(false);
                    dynamic_cast<ListWidgetItem *>( ui->listWidget->item( currentChapter.first ) )->setSelectedWithIcon(false);
                    dynamic_cast<ListWidgetItem *>( ui->listWidget->item( currentChapter.first - 1 ) )->setSelectedWithIcon(true);
                    currentChapter = QPair<int, int>(currentChapter.first - 1, listItems[currentChapter.first - 1]);
                    //ui->listWidget->setCurrentItem( ui->listWidget->item(i.key() - 1) );
                    //dynamic_cast<ListWidgetItem *>( ui->listWidget->item(i.key() - 1) )->setSelectedWithIcon(true);
                    return QMainWindow::eventFilter(watched, event);
                }
                if(currentPos > currentChapter.second && currentChapter.first + 1 < listItems.size() && currentPos > listItems[ currentChapter.first + 1 ])
                {
                    dynamic_cast<ListWidgetItem *>( ui->listWidget->item( currentChapter.first ) )->setSelectedWithIcon(false);
                    dynamic_cast<ListWidgetItem *>( ui->listWidget->item( currentChapter.first + 1 ) )->setSelectedWithIcon(true);
                    currentChapter = QPair<int, int>(currentChapter.first + 1, listItems[currentChapter.first + 1]);
                    //ui->listWidget->setCurrentItem( ui->listWidget->item(i.key() - 1) );
                    //dynamic_cast<ListWidgetItem *>( ui->listWidget->item(i.key() - 1) )->setSelectedWithIcon(true);
                    return QMainWindow::eventFilter(watched, event);
                }

                //++i;
            //}
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::changeActivePanel(MainWindow::ActivePanel newActivePanel)
{
    changeCurrentPanelButtonStyle(PanelButtonStyle::DisactivatedPanelStyle);

    switch (newActivePanel) {
    case ContentPanel:
        currentPanelButton = ui->toolButtonContent;
        if( !ui->listWidget->isVisible() )
            this->changeCurrentPanelWidget();
        break;
    case BookmarksPanel:
        currentPanelButton = ui->toolButtonBookmarks;
        if( !ui->listWidget->isVisible() )
            this->changeCurrentPanelWidget();
        break;
    case DefinitionsPanel:
        currentPanelButton = ui->toolButtonDefinitions;
        if( !ui->treeWidget->isVisible() )
            this->changeCurrentPanelWidget();
        break;
    case TranslationsPanel:
        currentPanelButton = ui->toolButtonTranslations;
        if( !ui->treeWidget->isVisible() )
            this->changeCurrentPanelWidget();
        break;
    }

    changeCurrentPanelButtonStyle(PanelButtonStyle::ActivePanelStyle);
}

void MainWindow::changeCurrentPanelButtonStyle(MainWindow::PanelButtonStyle style)
{
    if(style == PanelButtonStyle::ActivePanelStyle)
    {
        currentPanelButton->setStyleSheet("QToolButton { background-color: rgba(141, 209, 211, 255);"
                                          "border: 2px solid #8DD1D3;"
                                          "border-radius: 7px; }"
                                          ""
                                          "QToolButton:hover { background-color: rgba(141, 209, 211, 100); }");
        currentPanelButton->setFocus();
    }
    else currentPanelButton->setStyleSheet("QToolButton { border: 2px solid #8DD1D3;"
                                           "border-radius: 7px; }"
                                           ""
                                           "QToolButton:hover {background-color: rgba(141, 209, 211, 100); }"
                                           ""
                                           "QToolButton:focus {background-color: rgba(141, 209, 211, 255); }");
}

void MainWindow::changeCurrentPanelWidget()
{
    ui->listWidget->setVisible( !ui->listWidget->isVisible() );
    ui->treeWidget->setVisible( !ui->treeWidget->isVisible() );
}

void MainWindow::createActionsConnects()
{
    FB2Reader * r = reader;
    connect(ui->openBookAction, &QAction::triggered, [r, this] () {
        QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
        QString fileName = QFileDialog::getOpenFileName(this, tr("Будь ласка, оберіть книгу"), QDir::toNativeSeparators(env.value("USERPROFILE")), tr("FB2-книги (*.fb2)"));
        qDebug() << fileName;

        if(!fileName.isEmpty() && !fileName.isNull()) {
            r->parseBook(fileName);
            ui->textBrowser->setHtml(r->book());
            ui->labelBook->setText(r->getBookName());
            ui->labelChapter->setText(r->getFirstChapterName()); // 0 - return the book's name
            this->changeActivePanel(MainWindow::ActivePanel::ContentPanel);
            r->setNumberBookCharacters( ui->textBrowser->toPlainText().length() );

            const QMap<unsigned int, FB2Reader::ChapterData> & content = r->getContent();
            QMap<unsigned int, FB2Reader::ChapterData>::const_iterator i = content.cbegin();
            int row = 0;
            while (i != content.cend())
            {
                ListWidgetItem * item = new ListWidgetItem(
                            i.key(),
                            reader->findPositionByChapterId( ui->textBrowser->toPlainText(), i.key() ),
                            i.value().name);
                ui->listWidget->insertItem( ui->listWidget->count(), item );
                this->listItems.insert(row++, item->getChapterPos());
                if(i == content.cbegin())
                {
                    //ui->listWidget->setCurrentItem(item);
                    currentChapter = QPair<int, int>(0, item->getChapterPos());
                    item->setSelectedWithIcon(true);
                }
                ++i;
            }
            //contentLoaded = true;

            QMap<int, int>::const_iterator j = listItems.cbegin();
            while(j != listItems.cend())
            {
                qDebug() << j.key() << " " << j.value();

                ++j;
            }
        }
    } );
}

void MainWindow::createConnects()
{
    ui->listWidget->installEventFilter(this);
    ui->treeWidget->installEventFilter(this);
    ui->textBrowser->verticalScrollBar()->installEventFilter(this);

    createToolButtonsConnections();
    createActionsConnects();
    createPanelsConnects();

    connect(ui->textBrowser, &QTextBrowser::cursorPositionChanged, [this] () {
        if(reader->getNumberBookCharacters() > 0) {
            qDebug() << ui->textBrowser->textCursor().position();
            qDebug() << reader->getNumberBookCharacters();
            int percent = ui->textBrowser->textCursor().position() * 100 / reader->getNumberBookCharacters();
            ui->labelPercent->setText( QString::number(percent) + "%");
        }
    });

    connect(ui->textBrowser->verticalScrollBar(), &QScrollBar::valueChanged, [this] (int v) {
        //qDebug() << v;
        return;
        QTextCursor cursor = ui->textBrowser->cursorForPosition(QPoint(0, 0));
        QPoint bottom_right(ui->textBrowser->viewport()->x() + 1, ui->textBrowser->viewport()->y() + 1);
        int end_pos = ui->textBrowser->cursorForPosition(QPoint(0, 0)/*bottom_right*/).position();
        if(reader->getNumberBookCharacters() > 0) {
            qDebug() << ui->textBrowser->textCursor().position();
            qDebug() << reader->getNumberBookCharacters();
            int percent = ui->textBrowser->cursorForPosition(QPoint(0, 0)/*bottom_right*/).position() * 100 / reader->getNumberBookCharacters();
            ui->labelPercent->setText( QString::number(percent) + "%");
        }
        qDebug() << end_pos;
        /*cursor.setPosition(end_pos, QTextCursor::KeepAnchor);
        qDebug() << cursor.selectedText();*/
    });
}

void MainWindow::createPanelsConnects()
{
    FB2Reader * r = reader;
    connect(ui->listWidget, &QListWidget::currentItemChanged, [this, r] (QListWidgetItem *current, QListWidgetItem *previous) {
        if(previous)
            dynamic_cast<ListWidgetItem *>( previous )->setSelectedWithIcon(false);
        else dynamic_cast<ListWidgetItem *>( ui->listWidget->item(0) )->setSelectedWithIcon(false);

        unsigned int index = dynamic_cast<ListWidgetItem *>( current )->getChapterId();
        //ui->textBrowser->textCursor().setPosition( r->getPositionByChapterId(index) );

        qDebug() << ui->textBrowser->textCursor().position();
        qDebug() << ui->textBrowser->document()->find("ExplanatoryFB2ReaderChapterId@=" + QString::number(index), 0).isNull();


        QTextCursor cursor = ui->textBrowser->textCursor();
        cursor.setPosition( dynamic_cast<ListWidgetItem *>( current )->getChapterPos() /*r->getPositionByChapterId( ui->textBrowser->toPlainText(), index )*/ ); //ui->textBrowser->toPlainText().indexOf( "ExplanatoryFB2ReaderChapterId@=" + QString::number(index) )); //r->getPositionByChapterId(index));
        ui->textBrowser->setTextCursor(cursor);

        int cursorY = ui->textBrowser->cursorRect().top();
        QScrollBar *vbar = ui->textBrowser->verticalScrollBar();
        vbar->setValue(vbar->value() + cursorY + 30);

        //this->isListCurrentItemChanged = true;
        dynamic_cast<ListWidgetItem *>( ui->listWidget->item(this->currentChapter.first) )->setSelectedWithIcon(false);
        this->currentChapter = QPair<int, int>(ui->listWidget->row(current)/* + 1*/, dynamic_cast<ListWidgetItem *>( current )->getChapterPos());
        dynamic_cast<ListWidgetItem *>( current )->setSelectedWithIcon(true);
    });
}

void MainWindow::createToolButtonsConnections()
{
    connect(ui->toolButtonContent, &QToolButton::pressed, [this] () {
        if(currentPanelButton != ui->toolButtonContent) {
            this->changeActivePanel(MainWindow::ActivePanel::ContentPanel);
        }
    });
    connect(ui->toolButtonBookmarks, &QToolButton::pressed, [this] () {
        if(currentPanelButton != ui->toolButtonBookmarks) {
            this->changeActivePanel(MainWindow::ActivePanel::BookmarksPanel);
        }
    });
    connect(ui->toolButtonDefinitions, &QToolButton::pressed, [this] () {
        if(currentPanelButton != ui->toolButtonDefinitions) {
            this->changeActivePanel(MainWindow::ActivePanel::DefinitionsPanel);
        }
    });
    connect(ui->toolButtonTranslations, &QToolButton::pressed, [this] () {
        if(currentPanelButton != ui->toolButtonTranslations) {
            this->changeActivePanel(MainWindow::ActivePanel::TranslationsPanel);
        }
    });
}

