#ifndef ANIMAL_H
#define ANIMAL_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QSet>
#include <QMap>
#include <QtQml>

class Animal {
public:
    Animal(QString id, QString name, QString power, QString upgrade = "", int moveScore = 10, int matchScore = 20, int upgradedScore = 40);
    virtual ~Animal() = default;

    // Getters
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString powerDescription() const { return m_powerDescription; }
    QString upgradeDescription() const { return m_upgradeDescription; }
    QStringList friends() const { return m_friends; }
    int score() const {return m_matchScore;}
    int upgradedScore() const {return m_upgradedScore;}

    // Setters
    void setDescriptions(const QString& power, const QString& upgrade);
    void setFriends(const QStringList& friendsList);
    void setUpgraded(bool isUpgraded);


    // Virtual methods that can be override by child
    /// @brief Compute score
    virtual int calculateScore(int matchCount, bool isMovedByUser) const;

    /// @brief Detect match (3 aligned H or V)
    /// @returns indexes to destroy
    virtual QSet<int> getMatchIndices(int startIndex, const QList<QString>& grid) const;

    /// @returns The complete path from the selected cell to the selected destination
    virtual QList<int> getMovementPath(int from, int to, const QList<QString>& grid) const;

protected:
    QString m_id; // ex: "panda"
    QString m_name; // ex: "Panda Zen"
    QString m_powerDescription;
    QString m_upgradeDescription;
    QStringList m_friends;

    bool m_isUpgraded;   //< True if animal has been upgraded. False by default.

    int m_moveScore;     //< Score when moving the animal
    int m_matchScore;    //< Score when matching
    int m_upgradedScore; //< Score when matching (after upgrade)
};

#endif // ANIMAL_H