#include "listwidgetitem.h"


ListWidgetItem::ListWidgetItem(unsigned int chapterId, int chapterPos, const QString &text, QListWidget *parent) :
    QListWidgetItem (parent, QListWidgetItem::ItemType::UserType),
    chapterPos(chapterPos), chapterId (chapterId)
{
    setText(text);
}

unsigned int ListWidgetItem::getChapterId() const
{
    return chapterId;
}

int ListWidgetItem::getChapterPos() const
{
    return chapterPos;
}

void ListWidgetItem::setSelectedWithIcon(bool select)
{
    this->setSelected(select);

    if (select)
        setIcon(QIcon(":/images/chapterPointer.png"));
    else setIcon(QIcon(""));
}
