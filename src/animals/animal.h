#ifndef ANIMAL_H
#define ANIMAL_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QSet>

class Animal {
public:
    Animal(QString id, QString name, QString power, QString upgrade = "", int moveScore = 10, int matchScore = 20);
    virtual ~Animal() = default;

    // Getters
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString powerDescription() const { return m_powerDescription; }
    QString upgradeDescription() const { return m_upgradeDescription; }
    QStringList friends() const { return m_friends; }

    // Setters
    void setDescriptions(const QString& power, const QString& upgrade);
    void setFriends(const QStringList& friendsList);


    // Virtual methods that can be override by child
    /// @brief Compute score
    virtual int calculateScore(int matchCount, bool isMovedByUser) const;

    /// @brief Detect match (3 aligned H or V)
    /// @returns indexes to destroy
    virtual QSet<int> getMatchIndices(int startIndex, const QList<QString>& grid) const;

protected:
    QString m_id; // ex: "panda"
    QString m_name; // ex: "Panda Zen"
    QString m_powerDescription;
    QString m_upgradeDescription;
    QStringList m_friends;

    int m_moveScore;  // Score when moving the animal
    int m_matchScore; // Score when matching
};

#endif // ANIMAL_H