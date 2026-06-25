#include "animal.h"

Animal::Animal(QString id, QString name, QString power, QString upgrade, int moveScore, int matchScore, int upgradedScore)
    : m_id(id), m_name(name), m_powerDescription(power), m_upgradeDescription(upgrade), m_moveScore(moveScore), m_matchScore(matchScore), m_upgradedScore(upgradedScore), m_isUpgraded(false)
{
}

void Animal::setDescriptions(const QString& power, const QString& upgrade) {
    m_powerDescription = power;
    m_upgradeDescription = upgrade;
}

void Animal::setFriends(const QStringList& friendsList) {
    m_friends = friendsList;
}

void Animal::setUpgraded(bool isUpgraded) {
    m_isUpgraded = isUpgraded;
}


int Animal::calculateScore(int matchCount, bool isMovedByUser) const {
    // Match without moving (ex: spawn)
    if (matchCount < 3) {
        return isMovedByUser ? m_moveScore : 0;
    }

    int score = isMovedByUser ? m_moveScore : 0; // Add moving score

    int matchScore = m_isUpgraded ? m_upgradedScore : m_matchScore;

    score += 3 * matchScore; // Add matching score

    // Double score for matching over 3
    if (matchCount > 3) {
        int surplus = matchCount - 3;
        score += surplus * (2 * matchScore);
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



QList<int> Animal::getMovementPath(int from, int to, const QList<QString>& grid) const
{
    QList<int> path;

    // Check bounds
    if (from < 0 || from >= 35 || to < 0 || to >= 35) return path;
    if (from == to) return path;
    if (!grid.at(to).isEmpty()) return path;  // Destination is occupied

    // grid dimension
    const int COLS = 7;
    const int ROWS = 5;

    /// NOTE : Using Breadth-First Search (BFS) to validate the path

    // Queue to mark visited cells
    QQueue<int> queue;
    QMap<int, int> parentMap; // visiting cell -> from parent cell

    queue.enqueue(from);
    parentMap.insert(from, -1); // First cell has no parent

    // The allowed 1-cell movement (UP, DOWN, LEFT, RIGHT)
    const int dirX[] = {0, 0, -1, 1};
    const int dirY[] = {-1, 1, 0, 0};
    bool destinationReached = false;

    while (!queue.isEmpty()) {
        int current = queue.dequeue();

        // Reached destination so path is valid.
        if (current == to) {
            destinationReached = true;
            break;
        }

        int currX = current % COLS;
        int currY = current / COLS;

        // Try every neighbour cells
        for (int i = 0; i < 4; ++i) {
            int nextX = currX + dirX[i];
            int nextY = currY + dirY[i];
            int nextIndex = nextY * COLS + nextX;

            // Checks grid's bounds
            if (nextX >= 0 && nextX < COLS && nextY >= 0 && nextY < ROWS) {
                // If cell was not visited yet and is not empty
                if (!parentMap.contains(nextIndex) && (grid.at(nextIndex).isEmpty() || nextIndex == to)) {
                    parentMap.insert(nextIndex, current); // Mark where we are coming from
                    queue.enqueue(nextIndex);
                }
            }
        }
    }

    // Reconstruct the path
    if (destinationReached) {
        int current = to;
        // We trace back from parent to parent until we reach the starting point
        while (current != -1) {
            path.prepend(current); // Order = from -> ... -> to
            current = parentMap.value(current);
        }

        // Remove "from" since it is not needed
        if (!path.isEmpty()) {
            path.removeFirst();
        }
    }

    return path;
}