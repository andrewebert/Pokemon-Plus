#include "theme.h"
#include "../PokemonInfo/pokemoninfo.h"
#include "../Utilities/otherwidgets.h"
#include <QtCore>
#include <QFontDatabase>
#include <QPixmapCache>

static void fill_container_with_file(QList<QColor> &container, const QString &filename)
{
    QFile file(filename);

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream filestream(&file);
    container.clear();

    /* discarding all the uninteresting lines, should find a more effective way */
    while (!filestream.atEnd() && filestream.status() != QTextStream::ReadCorruptData)
    {
        container << filestream.readLine();
    }
}

template <class T>
static void fill_container_with_file(T &container, const QString & filename)
{
    QFile file(filename);

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream filestream(&file);
    container.clear();

    /* discarding all the uninteresting lines, should find a more effective way */
    while (!filestream.atEnd() && filestream.status() != QTextStream::ReadCorruptData)
    {
        typename T::value_type var;
        filestream >> var;
        container << var;
    }
}

QString Theme::m_Directory;
QList<QColor> Theme::m_TColors;
QList<QColor> Theme::m_CColors;
QList<QColor> Theme::m_ChatColors;
QList<QPixmap> Theme::m_TPics;
QHash<int, QPixmap> Theme::m_statusIcons;
QHash<int, QPixmap> Theme::m_battleIcons;
QHash<QString, QColor> Theme::m_Colors;

QString Theme::path(const QString& file, bool def)
{
    if (!def) {
        QString test = m_Directory+file;
        if (QFile::exists(test))
            return test;
    }

    return ("Themes/" DEFAULT_PO_THEME "/") + file;
}

void Theme::loadColors()
{
    fill_container_with_file(m_TColors, path("types/type_colors.txt"));
    fill_container_with_file(m_CColors, path("categories/category_colors.txt"));
    fill_container_with_file(m_ChatColors, path("client/chat_colors.txt"));

    /* Loads first the default file, in case the custom file misses
      some keys */
    QSettings ini(path("colors.ini", true), QSettings::IniFormat);
    foreach(QString key, ini.allKeys()) {
        m_Colors[key] = ini.value(key).value<QColor>();
    }
    QSettings ini2(path("colors.ini"), QSettings::IniFormat);
    foreach(QString key, ini2.allKeys()) {
        m_Colors[key] = ini2.value(key).value<QColor>();
    }
}

void Theme::init(const QString &dir)
{
    m_Directory = dir;

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    if (QFile::exists(dir+"Fonts")) {
        QDir d(dir + "Fonts");

        foreach (QString s, d.entryList(QDir::NoDotAndDotDot | QDir::Files)) {
            QFontDatabase::addApplicationFont(d.absoluteFilePath(s));
        }
    }

    loadColors();
    loadPixmaps();
}

void Theme::Reload(const QString &dir)
{
    m_Directory = dir;
    QPixmapCache::clear();
    loadColors();
    loadPixmaps();
}

QColor Theme::TypeColor(int typenum)
{
    return m_TColors[typenum];
}

QColor Theme::Color(const QString &code)
{
    return m_Colors.value(code);
}

QColor Theme::CategoryColor(int typenum)
{
    return m_CColors[typenum];
}

QColor Theme::ChatColor(int num)
{
    return m_ChatColors[num % m_ChatColors.size()];
}

QPixmap Theme::TypePicture(int type)
{
    return (type >= 0 && type < TypeInfo::NumberOfTypes()) ? m_TPics[type] : QPixmap();
}

void Theme::loadPixmaps()
{
    m_TPics.clear();
    for (int i = 0; i < TypeInfo::NumberOfTypes();i++) {
        m_TPics.push_back(Pic((QString("types/type%1.png").arg(i))));
    }
    m_statusIcons[Pokemon::Koed] = Pic(QString("status/status%1.png").arg(Pokemon::Koed));
    m_battleIcons[Pokemon::Koed] = Pic(QString("status/battle_status%1.png").arg(Pokemon::Koed));

    for (int i = 0; i < 6; i++) {
        m_statusIcons[i] = Pic(QString("status/status%1.png").arg(i));
        m_battleIcons[i] = Pic(QString("status/battle_status%1.png").arg(i));
    }
}

QPixmap Theme::BattleStatusIcon(int status)
{
    return m_battleIcons[status];
}

QPixmap Theme::StatusIcon(int status)
{
    return m_statusIcons[status];
}

QPixmap Theme::BlueBall()
{
    return Sprite("blueball");
}

QPixmap Theme::WhiteBall()
{
    return Sprite("whiteball");
}

QPixmap Theme::GreyBall()
{
    return Sprite("greyball");
}

QPixmap Theme::BlackBall()
{
    return Sprite("blackball");
}

QPixmap Theme::OrangeBall()
{
    return Sprite("orangeball");
}

QPixmap Theme::FrameBall()
{
    return Sprite("frameball");
}

QColor Theme::StatusColor(int status)
{
    switch (status) {
    case Pokemon::Koed: return "#171b1a";
    case Pokemon::Fine: return TypeColor(Pokemon::Normal);
    case Pokemon::Paralysed: return TypeColor(Pokemon::Electric);
    case Pokemon::Burnt: return TypeColor(Pokemon::Fire);
    case Pokemon::Frozen: return TypeColor(Pokemon::Ice);
    case Pokemon::Asleep: return TypeColor(Pokemon::Psychic);
    case Pokemon::Poisoned: return TypeColor(Pokemon::Poison);
    case Pokemon::Confused: return TypeColor(Pokemon::Ghost);
    default: return QColor();
    }
}


QPixmap Theme::GenderPicture(int gender, GenderMode mode)
{
    return Sprite(QString("gender%1%2").arg(char('a'+int(mode))).arg(gender));
}

QImageButton *Theme::Button(const QString &code)
{
    bool def;
    QStringList s = value("buttons/"+code, &def).toStringList();

    while (s.size() < 3)
        s.append("");

    return new QImageButton(path(s[0], def), path(s[1], def), path(s[2], def));
}


QImageButtonP *Theme::PressedButton(const QString &code)
{
    bool def;
    QStringList s = value("buttons/"+code, &def).toStringList();

    while (s.size() < 3)
        s.append("");

    return new QImageButtonP(path(s[0], def), path(s[1], def), path(s[2], def));
}


QImageButtonLR *Theme::LRButton(const QString &code)
{
    bool def;
    QStringList s = value("buttons/"+code, &def).toStringList();

    while (s.size() < 2)
        s.append("");

    return new QImageButtonLR(path(s[0], def), path(s[1], def));
}

void Theme::ChangePics(QImageButton *b, const QString &code)
{
    bool def;
    QStringList s = value("buttons/"+code, &def).toStringList();

    while (s.size() < 3)
        s.append("");

    b->changePics(path(s[0], def), path(s[1], def), path(s[2], def));
}

QVariant Theme::value(const QString &key, bool *def)
{
    QSettings ini (path("pictures.ini"), QSettings::IniFormat);
    QVariant ret = ini.value(key);

    if (ret.isNull()) {
        QSettings ini2 (path("pictures.ini", true), QSettings::IniFormat);
        ret = ini2.value(key);
        *def = true;
    } else {
        *def = false;
    }

    return ret;
}

QPixmap Theme::Pic(const QString &way)
{
    return QPixmap(path(way));
}

QPixmap Theme::Sprite(const QString &code)
{
    QPixmap pm;

    if (!QPixmapCache::find(code, &pm)) {
        bool def;
        QString way = value("pictures/"+code, &def).toString();
        pm = QPixmap(path(way, def));

        QPixmapCache::insert(code, pm);
    }

    return pm;
}

QPixmap Theme::TrainerSprite(int num)
{
    return Pic(QString("Trainer Sprites/%1.png").arg(num));
}

QIcon Theme::Icon(const QString &code)
{
    QPixmap pm = Sprite(code);

    return QIcon(pm);
}
