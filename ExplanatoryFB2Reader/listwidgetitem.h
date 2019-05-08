#ifndef LISTWIDGETITEM_H
#define LISTWIDGETITEM_H

#include <QListWidgetItem>

class ListWidgetItem : public QListWidgetItem
{
    int chapterPos;
    unsigned int chapterId;

public:
    ListWidgetItem(unsigned int chapterId, int chapterPos, const QString &text, QListWidget *parent = nullptr);

    int getChapterPos() const;
    unsigned int getChapterId() const;
    void setSelectedWithIcon(bool select);

};

#endif // LISTWIDGETITEM_H
