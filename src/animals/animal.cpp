#include "animal.h"

Animal::Animal(QString id, QString name, QString power, QString upgrade, int moveScore, int matchScore)
    : m_id(id), m_name(name), m_powerDescription(power), m_upgradeDescription(upgrade), m_moveScore(moveScore), m_matchScore(matchScore)
{
}

void Animal::setDescriptions(const QString& power, const QString& upgrade) {
    m_powerDescription = power;
    m_upgradeDescription = upgrade;
}

void Animal::setFriends(const QStringList& friendsList) {
    m_friends = friendsList;
}


int Animal::calculateScore(int matchCount, bool isMovedByUser) const {
    // Match without moving (ex: spawn)
    if (matchCount < 3) {
        return isMovedByUser ? m_moveScore : 0;
    }

    int score = isMovedByUser ? m_moveScore : 0; // Add moving score

    score += 3 * m_matchScore; // Add matching score

    // Double score for matching over 3
    if (matchCount > 3) {
        int surplus = matchCount - 3;
        score += surplus * (2 * m_matchScore);
    }

    return score;
}

QSet<int> Animal::getMatchIndices(int startIndex, const QList<QString>& grid) const {
    QSet<int> matchedIndices;
    QString type = grid.at(startIndex);
    if (type.isEmpty()) return matchedIndices;

    int startX = startIndex % 7;
    int startY = startIndex / 7;

    /// --- Horizontal search
    QSet<int> horizontalMatch;
    horizontalMatch.insert(startIndex);

    // Check left side
    for (int x = startX - 1; x >= 0; --x) {
        int idx = startY * 7 + x;
        if (grid.at(idx) == type) horizontalMatch.insert(idx);
        else break;
    }

    // Check right side
    for (int x = startX + 1; x < 7; ++x) {
        int idx = startY * 7 + x;
        if (grid.at(idx) == type) horizontalMatch.insert(idx);
        else break;
    }

    /// --- Vertical search
    QSet<int> verticalMatch;
    verticalMatch.insert(startIndex);

    // Check top
    for (int y = startY - 1; y >= 0; --y) {
        int idx = y * 7 + startX;
        if (grid.at(idx) == type) verticalMatch.insert(idx);
        else break;
    }
    // Check bottom
    for (int y = startY + 1; y < 5; ++y) {
        int idx = y * 7 + startX;
        if (grid.at(idx) == type) verticalMatch.insert(idx);
        else break;
    }

    /// --- Validate
    // If 3 or more, we validate the match
    if (horizontalMatch.size() >= 3) matchedIndices.unite(horizontalMatch);
    if (verticalMatch.size() >= 3) matchedIndices.unite(verticalMatch);

    return matchedIndices;
}