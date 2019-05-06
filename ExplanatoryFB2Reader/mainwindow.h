#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class FB2Reader;
class QToolButton;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    //bool contentLoaded = false;
    //bool isListCurrentItemChanged = false;
    FB2Reader * reader;
    QToolButton * currentPanelButton;
    QMap<int, int> listItems;
    QPair<int, int> currentChapter;
    Ui::MainWindow *ui;

    enum ActivePanel {
        ContentPanel,
        BookmarksPanel,
        DefinitionsPanel,
        TranslationsPanel
    };
    enum PanelButtonStyle {
        ActivePanelStyle,
        DisactivatedPanelStyle
    };

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool eventFilter(QObject *watched, QEvent *event);

private:
    void changeActivePanel(ActivePanel newActivePanel);
    void changeCurrentPanelButtonStyle(PanelButtonStyle style);
    void changeCurrentPanelWidget();
    void createActionsConnects();
    void createConnects();
    void createPanelsConnects();
    void createToolButtonsConnections();

};

#endif // MAINWINDOW_H
