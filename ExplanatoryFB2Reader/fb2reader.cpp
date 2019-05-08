#include "bookmodel.h"
#include "fb2reader.h"
#include <algorithm>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>


const QString & FB2Reader::book() const
{
    return m_book;
}

int FB2Reader::getNumberBookCharacters()
{
    return numberBookCharacters;
}

int FB2Reader::findPositionByChapterId(const QString &plainText, unsigned int index)
{
    return plainText.indexOf( chapterIdPrefix + QString::number(index) )
            + QString("ExplanatoryFB2ReaderChapterId@=1").length();
}

QString FB2Reader::getBookName() const
{
    return m_content.value(0);
}

QString FB2Reader::getChapterName(unsigned int index) const
{
    return m_content.value(index);
}

QString FB2Reader::getFirstChapterName() const
{
    QMap<unsigned int, QString>::const_iterator i = m_content.cbegin();
    ++i;
    return i.value();
}

void FB2Reader::setNumberBookCharacters(int number)
{
    numberBookCharacters = number;
}

void FB2Reader::clearBeforeParsing()
{
    numberBookCharacters = 0;
    m_book.clear();
    m_content.clear();
}

const QMap<unsigned int, QString> & FB2Reader::getContent()
{
    return m_content;
}

void FB2Reader::removeLineBreaksFromContent()
{
    QString::size_type pos = 0;
    for (QString & item : m_content) {
        pos = 0;
        for (QString::size_type i = 0; i < item.size(); ++i) {
            if (item.at(i).isSpace() && item.at(i + 1).isSpace()) {
                pos = i;
                while (item.at(pos).isSpace())
                    ++pos;
                item.replace(i, pos - i, ". ");
            }
        }
    }
}

void FB2Reader::parseBook(const QString &fileName)
{
    QFile f(fileName);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "файл не открыт";
        return;
    }

    clearBeforeParsing();

    //QString tokenIdPrefix = "ExplanatoryFB2ReaderId@=";
    //QString chapterIdPrefix = "ExplanatoryFB2ReaderChapterId@=";
    unsigned int tokenId = 0, chapterId = 0;
    QString special;
    QString description; // описание видео
    //  настройки отображения
    int fontSize = 20;
    if( QSysInfo::productType() == "android" )
        fontSize *= 1.8;

    QXmlStreamReader sr(&f);
    QString rId;
    QString rType;
    QString opt;
    QString chapterSpan = "";

    QStringList thisToken;

    while( !sr.atEnd() )
    {
        switch( sr.readNext() )
        {
        case QXmlStreamReader::NoToken:
            qDebug() << "QXmlStreamReader::NoToken";
            break;
        case QXmlStreamReader::StartDocument:
            m_book = "<!DOCTYPE HTML><html><body style=\"font-size:%1px; font-family:Sans, Times New Roman;\">";
            m_book = m_book.arg(fontSize);
            break;
        case QXmlStreamReader::EndDocument:
            m_book.append("</body></html>");
            break;
        case QXmlStreamReader::StartElement:
            thisToken.append( sr.name().toString() );

            if(thisToken.contains("description")) // ОПИСАНИЕ КНИГИ
            {
                if( thisToken.back() != "image" ) //пропускаем всё кроме обложки
                    break; // не выводим
            }

            if(sr.name().toString() == "title")
            {
                m_content.insert(chapterId, ""); // добавляем пункт содержания
                break;
            }

            if( sr.name().toString() == "body" )
                if( !sr.attributes().isEmpty() && sr.attributes().first().value().toString() == "notes")
                    special = "notes";  // режим примечаний

            if(special == "notes")
            {
                if( sr.name().toString() == "section" )
                {
                    if( sr.attributes().count() > 0 )
                    {
                        rId = sr.attributes().at(0).value().toString(); // ссылка на текст
                        rType = "";
                    }
                }
            }

            opt = " align=\"justify\"";
            if(thisToken.contains("title") )
            {
                chapterSpan = "<span style=\"color: transparent; display: block; height: 0px; width: 0px;\">" + chapterIdPrefix + QString::number(chapterId) + "</span>";
                opt = " id=\"" + chapterIdPrefix + QString::number(chapterId) + "\" align=\"center\" style=\"font-size:" + QString::number(int(fontSize * 1.5)) + "px\" ";
                ++chapterId;

                if(special == "notes")
                {
                    opt += (" id=\"" + rId + "\"");
                }
            }
            if(thisToken.contains("subtitle") )
            {
                opt = " align=\"center\" style=\"font-size:" + QString::number(int(fontSize * 1.2)) + "px\" ";
            }
            if(thisToken.contains("annotation") )
            {
                opt = " align=\"left\" ";
            }

            if(sr.name().toString() == "p"
                    || sr.name().toString() == "subtitle")
            {
                if(!chapterSpan.isEmpty())
                {
                    m_book.append(chapterSpan);
                    chapterSpan = "";
                }
                m_book.append("<p " + opt + " >");
                break;
            }

            if( sr.name().toString() == "table" )
            {
                QString text;
                for(int i = 0; i < sr.attributes().count(); ++i)
                {
                    if(sr.attributes().at(i).name() == "id")
                        qDebug() << sr.attributes().at(i).value().toString();
                    if(sr.attributes().at(i).name() == "style")
                        text.append( "style=\"" +sr.attributes().at(i).value().toString()+ ";\"" );
                }
                m_book.append("<table border=1 align=\"center\" style=\"border:solid;\" " + text + ">");
                break;
            }
            if( sr.name().toString() == "tr" )
            {
                QString text;
                if(!thisToken.contains("table"))
                    qDebug() << "ошибка в таблице";
                for(int i = 0; i < sr.attributes().count(); ++i)
                {
                    if(sr.attributes().at(i).name() == "align") // aling
                        text.append( "align=\"" + sr.attributes().at(i).value().toString() + "\"" );
                    else
                        qDebug() << "<tr>" << sr.attributes().at(i).name() << sr.attributes().at(i).value().toString();
                }
                m_book.append("<tr " + text + ">");
                break;
            }            //
            if( sr.name().toString() == "td" || sr.name().toString() == "th" )
            {
                if(!thisToken.contains("table"))
                    qDebug() << "ошибка в таблице";
                QString text;
                for(int i = 0; i < sr.attributes().count(); ++i)
                {
                    if(sr.attributes().at(i).name() == "align") // aling
                        text.append( "align=\"" + sr.attributes().at(i).value().toString() + "\" " );
                    else if(sr.attributes().at(i).name() == "valign") // valing
                        text.append( "valign=\"" + sr.attributes().at(i).value().toString() + "\" " );
                    else if(sr.attributes().at(i).name() == "colspan")
                        text.append( "colspan=" + sr.attributes().at(i).value().toString() + " " );
                    else if(sr.attributes().at(i).name() == "rowspan")
                        text.append( "rowspan=" + sr.attributes().at(i).value().toString() + " " );
                    else
                        qDebug() << "<td th>" << sr.attributes().at(i).name() << sr.attributes().at(i).value().toString();
                }
                m_book.append( "<" + sr.name().toString() + " " + text + ">" );
                break;
            }
            if( sr.name().toString() == "empty-line" )
            {
                m_book.append("<br/>");
                break;
            }
            if(sr.name().toString() == "strong"
                    || sr.name().toString() == "sup"
                    || sr.name().toString() == "sub"
                    || sr.name().toString() == "code"
                    || sr.name().toString() == "cite")
            {
                m_book.append( "<" + sr.name().toString() + ">");
                break;
            }
            if(sr.name().toString() == "emphasis")
            {
                m_book.append( "<i>" );
                break;
            }
            if( sr.name().toString() == "v" )
            {
                m_book.append("<p align=\"left\" style=\"margin-left:25px;\">");
                break;
            }
            if(sr.name().toString() == "strikethrough")
            {
                m_book.append( "<strike>" );
                break;
            }

            if( sr.name().toString() == "a" ) // метка примечания
            {
                rId = "";
                for(int i = 0; i < sr.attributes().count(); ++i)
                {
                    if(sr.attributes().at(i).name() == "type" )
                    {
                        //rType = sr.attributes().at(i).value().toString();
                    }
                    if(sr.attributes().at(i).name() == "href")
                    {
                        rId = sr.attributes().at(i).value().toString();
                    }
                }
                m_book.append("<a href=\"" + rId + "\"> ");
                //qDebug() << "a" << rId;
            }

            if(sr.name().toString() == "poem"
                    || sr.name().toString() == "stanza"
                    || sr.name().toString() == "epigraph")
            {
                break;
            }

            if(sr.name().toString() == "text-author" ) // автор текстта
            {
                m_book.append( "<p align=\"justify\" style=\"margin-left:45px;\">" );
                break;
            }
            if(sr.name().toString() == "date" ) // автор текстта
            {
                m_book.append( "<p align=\"justify\" style=\"margin-left:45px;\">" );
                break;
            }

            if( sr.name().toString() == "image" ) // расположение рисунков
            {
                if(sr.attributes().count() > 0)
                    m_book.append("<p align=\"center\">" + sr.attributes().at(0).value().toString() + "#" + "</p>");
            }
            if(sr.name() == "binary") // хранилище рисунков
            {
                if(sr.attributes().at(0).name() == "id")
                {
                    rId = sr.attributes().at(0).value().toString();
                    rType = sr.attributes().at(1).value().toString();
                }
                if(sr.attributes().at(1).name() == "id")
                {
                    rId = sr.attributes().at(1).value().toString();
                    rType = sr.attributes().at(0).value().toString();
                }
            }
            break;
        case QXmlStreamReader::EndElement:
            if( thisToken.last() == sr.name().toString() )
            {
                thisToken.removeLast();
            }
            else
                qDebug() << "error token";

            if(thisToken.contains("description")) // ОПИСАНИЕ КНИГИ
            {
                break; // не выводим
            }

            if( sr.name().toString() == "p"
                    || sr.name().toString() == "subtitle"
                    || sr.name().toString() == "v"
                    || sr.name().toString() == "date"
                    || sr.name().toString() == "text-author")
            {
                m_book.append("</p>");
                break;
            }

            if(sr.name().toString() == "td"
                    || sr.name().toString() == "th"
                    || sr.name().toString() == "tr"
                    || sr.name().toString() == "table"
                    || sr.name().toString() == "sup"
                    || sr.name().toString() == "sub"
                    || sr.name().toString() == "strong"
                    || sr.name().toString() == "code"
                    || sr.name().toString() == "cite")
            {
                m_book.append( "</" + sr.name().toString() + ">" );
                break;
            }

            if( sr.name().toString() == "a" )
            {
                rId.remove("#");
                m_book.append( "</a><span id=\"" + rId + "___" + "\"></span>" );
                qDebug() << "id" << rId + "___";
                break;
            }

            if(sr.name().toString() == "emphasis")
            {
                m_book.append( "</i>" );
                break;
            }
            if(sr.name().toString() == "strikethrough")
            {
                m_book.append( "</strike>" );
                break;
            }

            if(sr.name().toString() == "stanza") // конец строфы
            {
                //m_book.append("<br/>");
                break;
            }
            if(sr.name().toString() == "epigraph" || sr.name().toString() == "poem")
            {
                break;
            }

            if(special == "notes") // режим извлечения примечаний
            {
                if( sr.name().toString() == "body" )
                {
                    special = "";
                }
                if( sr.name().toString() == "section" )
                {
                    m_book.insert(m_book.lastIndexOf("<"), "<a href=\"#" + rId + "___" + "\"> назад</a>");
                }
            }
            break;
        case QXmlStreamReader::Characters:
            if( sr.text().toString() == "" )
            {
                //qDebug() << "isEmpty";
                break;
            }
            if( sr.text().toString() == "\n" )
            {
                //qDebug() << "isEmpty";
                break;
            }

            if(thisToken.contains("description")) // ОПИСАНИЕ КНИГИ
            {
                description.append(sr.text().toString() + " "); // не выводим
                break;
            }

            if(thisToken.contains( "binary" ) ) // для рисунков
            {
                QString image = "<img src=\"data:"
                        + rType + ";base64,"
                        + sr.text().toString()
                        + "\"/>";
                m_book.replace("#" + rId + "#", image);
                rId = "";
                rType = "";

                break;
            }
            if(thisToken.contains("div"))
            {
                qDebug() << "div" << sr.text().toString();
                break;
            }
            if(thisToken.back() == "Fictionm_book")
            {
                qDebug() << "Fictionm_book" << sr.text().toString();
                break;
            }

            if( thisToken.contains("title") ) // формируем содержание
            {
                m_content.last()/*.back()*/ += " " + sr.text().toString();//content->back()=="" ? "" : " " +
                //  qDebug() << "title" << sr.text().toString();
            }

            if(special == "notes" && !thisToken.contains("title") )  // добавление текста примечания
            {
                rType += " ";
                rType += sr.text().toString();
                //break;
            }

            if(thisToken.back() == "p"
                    || thisToken.back() == "subtitle"
                    || thisToken.back() == "v"
                    || thisToken.back() == "emphasis"
                    || thisToken.back() == "strong"
                    || thisToken.back() == "strikethrough"
                    || thisToken.back() == "sup"
                    || thisToken.back() == "sub"
                    || thisToken.back() == "td"
                    || thisToken.back() == "th"
                    || thisToken.back() == "code"
                    || thisToken.back() == "cite"
                    || thisToken.back() == "text-author"  // ??
                    || thisToken.back() == "date"
                    )
            {
                m_book.append( sr.text().toString() );
                break;
            }

            if(thisToken.back() == "section")
            {
                break;
            }
            if(thisToken.back() == "body")
            {
                break;
            }
            if(thisToken.back() == "table"
                    || thisToken.back() == "tr"
                    || thisToken.back() == "title"
                    || thisToken.back() == "poem"
                    || thisToken.back() == "stanza")
            {
                //m_book.append( sr.text().toString() );
                break;
            }
            if(thisToken.back() == "annotation")
            {
                qDebug() << "annotation" << sr.text().toString();
                break;
            }

            if(thisToken.back() == "a")
            {
                m_book.append( sr.text().toString() );
                break;
            }
            //все прочие тэги
            if( !sr.text().toString().isEmpty() )
            {
                qDebug() << thisToken.back() <<  "исключение" ;
                m_book.append("<span> " + sr.text().toString() + "</span>");
            }
            break;
        }
    }
    f.close();

    removeLineBreaksFromContent();

    qDebug() << m_book.size();

    QMap<unsigned int, QString>::const_iterator i = m_content.cbegin();
    while (i != m_content.cend()) {
        qDebug() << i.key() << " " << i.value();
        ++i;
    }
}
