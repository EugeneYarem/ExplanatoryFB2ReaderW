#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class FB2Reader;
class Keeper;
class QNetworkReply;
class QToolButton;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    FB2Reader *reader;
    Keeper *keeper;
    QMap<int, int> listItems;
    QPair<int, int> currentChapter;
    QToolButton *currentPanelButton;
    Ui::MainWindow *ui;

    enum ActivePanel {
        ContentPanel,
        BookmarksPanel,
        DefinitionsPanel,
        TranslationsPanel
    } currentPanel;
    enum PanelButtonStyle {
        ActivePanelStyle,
        DisactivatedPanelStyle
    };

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QNetworkReply *setQueryToGoogleTranlate(const QString &textToTranslate, const QString &errorMessage);
    void addTopLevelItemsInTreeW() const;
    void createActionsConnects();
    void changeActivePanel(ActivePanel newActivePanel);
    void changeCurrentPanelButtonStyle(PanelButtonStyle style) const;
    void hideAllPanels() const;
    void clearBeforeOpening();
    void createConnects();
    void createPanelsConnects();
    void createToolButtonsConnections();

private slots:
    void detectLanAndFindDef();
    void findDefinition(const QString &language);
    void translate();

};

#endif // MAINWINDOW_H
