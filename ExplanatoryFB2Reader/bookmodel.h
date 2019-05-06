#ifndef BOOKMODEL_H
#define BOOKMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class BookModel : public QAbstractListModel
{
    Q_OBJECT
    
    QStringList m_data;
    
public:
    enum Roles { TextRole = Qt::UserRole + 1, IdRole };
    
    BookModel(QObject *parent = nullptr);
    
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QHash<int, QByteArray> roleNames() const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    void setNewModelData(QStringList newData);
    
};

#endif // BOOKMODEL_H
