#ifndef TIER_H
#define TIER_H

#include <QtCore>
#include <QtGui>
#include "sql.h"
#include "memoryholder.h"
#include "tiernode.h"

class TierMachine;
class TierCategory;
struct TeamBattle;
struct PokeBattle;
class WaitingObject;
class LoadThread;
class QDomElement;

/* Banned sets have been removed because i was lazy to do the GUI to edit them. */
/* If you uncomment them, they should work, but there'll be no GUI to edit them */

struct MemberRating
{
    QString name;
    int matches;
    int rating;
    int displayed_rating;
    int last_check_time;
    int bonus_time;

    MemberRating(const QString &name="", int matches=0, int rating=1000, int displayed_rating = 1000,
                 int last_check_time = -1, int bonus_time = 0) : name(name.toLower()), matches(matches), rating(rating),
                   displayed_rating(displayed_rating), bonus_time(bonus_time) {
        if (last_check_time == -1) {
            this->last_check_time = time(NULL);
        } else {
            this->last_check_time = last_check_time;
        }
    }

    QString toString() const;
    void changeRating(int other, bool win);
    void calculateDisplayedRating();
    QPair<int, int> pointChangeEstimate(int otherRating);
};


//struct BannedPoke {
//    int poke;
//    int item;
//    QSet<int> moves;

//    BannedPoke(int poke=0, int item=0):poke(poke),item(item) {}

//    bool isBanned(const PokeBattle &poke) const;
//    /* This time checks if the pokemon is 'contained' within the BannedPoke,
//       for when instead of banning pokemons you want to restrict to some pokemons,
//       or some pokemons / movesets */
//    bool isForcedMatch(const PokeBattle &poke) const;

//    void loadFromXml(const QDomElement &elem);
//    QDomElement & toXml(QDomElement &dest) const;
//};

//inline uint qHash(const BannedPoke &p) {
//    return qHash(p.poke + (p.item << 16));
//}

class Tier : public TierNode
{
    friend class TierMachine;
    friend class ScriptEngine;
    friend class TierWindow;
public:
    void changeName(const QString &name);
    void changeId(int newid);

    Tier(TierMachine *boss = NULL, TierCategory *cat = NULL);

    QDomElement & toXml(QDomElement &dest) const;

    int rating(const QString &name);
    int inner_rating(const QString &name);
    int ratedBattles(const QString &name);

    void changeRating(const QString &winner, const QString &loser);
    void changeRating(const QString &player, int newRating);
    QPair<int, int> pointChangeEstimate(const QString &player, const QString &foe);

    void addBanParent(Tier *t);

    bool isBanned(const PokeBattle &p) const;
    bool isRestricted(const PokeBattle &p) const;
    bool isValid(const TeamBattle &t) const;
    bool exists(const QString &name);
    int ranking(const QString &name);
    int count();
    void updateMember(const MemberRating &m, bool add=false);
    void updateMemberInDatabase(const MemberRating &m, bool add=false);
    void loadMemberInMemory(const QString &name, QObject *o=NULL, const char *slot=NULL);
    void fetchRankings(const QVariant &data, QObject *o, const char *slot);
    void processQuery(QSqlQuery *q, const QVariant &name, int type, WaitingObject *w);
    void insertMember(QSqlQuery *q, void *data, int type);
    bool allowMode(int mode) const;
    bool allowGen(int gen) const;
    int getClauses() const;
    int getMaxLevel() const;
    void fixTeam(TeamBattle &t) const;

    QString getBannedPokes() const;
    QString getRestrictedPokes() const;
    QString getBannedMoves() const;
    QString getBannedItems() const;
    void importBannedPokes(const QString &);
    void importRestrictedPokes(const QString &);
    void importBannedMoves(const QString &);
    void importBannedItems(const QString &);

    void exportDatabase() const;
    void processDailyRun();
    /* Removes all ranking */
    void resetLadder();
    /* Clears the cache, forces synchronization with SQL database */
    void clearCache();
    /* Load tier configuration */
    void loadFromXml(const QDomElement &elem);
    /* Load tier ladders */
    void loadFromFile();
    /* Kills and deletes itself, as well as from the category parent. Beware to not use any member functions after */
    void kill();

    /* Gives a dummy tier with the same data, just used as a representation or something to work on */
    Tier *dataClone() const;
    bool isTier() const { return true; }
protected:
    enum QueryType {
        GetInfoOnUser,
        GetRankings,
    };

    int make_query_number(int type);
    int id() const {
        return m_id;
    }
private:
    TierMachine *boss;
    TierCategory *node;

    bool banPokes;
//    QMultiHash<int, BannedPoke> bannedSets; // The set is there to keep good perfs
//    QMultiHash<int, BannedPoke> restrictedSets;
    int maxRestrictedPokes;
    int numberOfPokemons;
    int maxLevel;
    int gen;
    QString banParentS;
    Tier *parent;
    QSet<int> bannedItems;
    QSet<int> bannedMoves;
    QSet<Pokemon::uniqueId> bannedPokes;
    QSet<Pokemon::uniqueId> restrictedPokes;
    int mode; /* < 0 : any, otherwise specific mode */
    quint32 clauses;

    /* Used for table name in SQL database */
    QString sql_table;
    int m_id;
    int m_count;
    int last_count_time;

    mutable MemoryHolder<MemberRating> holder;

    MemberRating member(const QString &name);

    LoadThread *getThread();
};

#endif // TIER_H