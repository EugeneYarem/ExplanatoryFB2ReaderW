#ifndef LISTWIDGETITEM_H
#define LISTWIDGETITEM_H

#include <QListWidgetItem>

class ListWidgetItem : public QListWidgetItem
{
    unsigned int chapterId;
    int chapterPos;

public:
    ListWidgetItem(unsigned int chapterId, int chapterPos, const QString & text, QListWidget * parent = nullptr);

    unsigned int getChapterId();
    int getChapterPos();
    void setSelectedWithIcon(bool select);

};

#endif // LISTWIDGETITEM_H
