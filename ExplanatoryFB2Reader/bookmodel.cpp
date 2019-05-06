#include "bookmodel.h"


BookModel::BookModel(QObject *parent)
{
    
}

int BookModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    
    return m_data.size();
}

QHash<int, QByteArray> BookModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[TextRole] = "text";
    roles[IdRole] = "chapterId";
    
    return roles;
}

QVariant BookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    
    switch (role) {
    case TextRole:
        return m_data.at(index.row());
    default:
        return QVariant();
    }
}

void BookModel::setNewModelData(QStringList newData)
{
    m_data = newData;
}



