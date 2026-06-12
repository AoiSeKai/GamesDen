#include "animalsgame.h"
#include <QRandomGenerator>

AnimalsGame::AnimalsGame(QObject *parent) : QObject(parent)
{
    // @TODO
    m_animalRegistry.insert("gorilla", new Animal("gorilla", "gorilla",
                                                  "Can be matched with monkeys but only get half points",
                                                  "Can be matched with monkeys <s>but only get half points</s>"));

    m_animalRegistry.insert("monkey", new Animal("monkey", "monkey",
                                                 "Can be matched with gorillas but only get half points",
                                                 "Can be matched with gorillas <s>but only get half points</s>"));

    m_animalRegistry.insert("chicken", new Animal("chicken", "chicken",
                                                  "Can match as long as they are next to each others. But you need at least 5 of them to have a match."
                                                  "Can match as long as they are next to each others. <s>But you need at least 5 of them to have a match.</s>"));


    m_animalRegistry.insert("rabbit", new Animal("rabbit", "rabbit", "Can jump over animals when moving"));
    m_animalRegistry.insert("snake", new Animal("snake", "snake", "Does not finish turn as long as the movement makes a match"));

    m_animalRegistry.insert("dog", new Animal("dog", "dog", "Call the nearest dog and make it move next to him if possible"));

    m_animalRegistry.insert("cow", new Animal("cow", "cow", "Can swap place with another animal"));



    m_animalRegistry.insert("panda", new Animal("panda", "panda",
                                                "Can only move on a cell next to him",
                                                "Can also move on a cell diagonally next to him",
                                                50, 100));

    m_animalRegistry.insert("crocodile", new Animal("crocodile", "crocodile", "Can be removed from the grid. End of the turn."));

    m_animalRegistry.insert("whale", new Animal("whale", "whale", "Can be fed another animal"));

    m_animalRegistry.insert("hippo", new Animal("hippo", "hippo", "Crush other animals on his path when moving"));

    initGame();
}

AnimalsGame::~AnimalsGame()
{
    qDeleteAll(m_animalRegistry);
    m_animalRegistry.clear();
}

void AnimalsGame::initGame()
{
    m_score = 0;
    m_currentLevel = 1;
    m_isLevelingUp = false;
    m_selectedIndex = -1;

    // Choose 2 animals from the pool
    m_currentBestiary.clear();
    while(m_currentBestiary.size() < 2) {
        QString animal = m_allPossibleAnimals.at(QRandomGenerator::global()->bounded(m_allPossibleAnimals.size()));
        if(!m_currentBestiary.contains(animal)) {
            m_currentBestiary.append(animal);
        }
    }


    // Clear the grid
    m_grid.clear();
    for (int i = 0; i < 35; ++i) {
        m_grid.append("");
    }

    // Place 3 animals randomly
    int placed = 0;
    while (placed < 3) {
        int targetIndex = QRandomGenerator::global()->bounded(35);
        if (m_grid.at(targetIndex).isEmpty()) {
            m_grid[targetIndex] = getRandomAnimal();
            placed++;
        }
    }

    m_animalQueue.clear();
    for (int i = 0; i < 3; ++i)
        m_animalQueue.append(getRandomAnimal());

    // Signals Qml that we changed everything
    emit scoreChanged();
    emit currentLevelChanged();
    emit bestiaryChanged();
    emit animalQueueChanged();
    emit gridChanged();
    emit stateChanged();
    emit selectedIndexChanged();
}

QString AnimalsGame::getRandomAnimal() const
{
    int index = QRandomGenerator::global()->bounded(m_currentBestiary.size());
    return m_currentBestiary.at(index);
}

void AnimalsGame::placeAnimal(int index)
{
    // Blocked if we are in level up mode
    if (m_isLevelingUp) return;

    // Check if the index is valid et not occupied already
    if (index < 0 || index >= m_grid.size() || !m_grid.at(index).isEmpty()) {
        return;
    }

    // Place the first animal from the queue
    m_grid[index] = m_animalQueue.first();

    m_score += 400; // @TODO Change with correct rules

    // move the queue forward
    advanceQueue();

    checkLevelUp();

    emit scoreChanged();
    emit gridChanged();
}

void AnimalsGame::advanceQueue()
{
    if (!m_animalQueue.isEmpty()) {
        m_animalQueue.removeFirst();
    }
    m_animalQueue.append(getRandomAnimal());
    emit animalQueueChanged();
}

void AnimalsGame::checkLevelUp() {

    // @TODO Change the condition for level up
    // Level up every 1000 points
    if (m_score >= m_currentLevel * 1000) {
        m_isLevelingUp = true;
        m_upgradeOptions.clear();


        if (m_currentLevel == 1) {
            // LEVEL 2 =  Choose 2 animals from the whole pool that are not already selected in the current pool
            while(m_upgradeOptions.size() < 2) {
                QString cand = m_allPossibleAnimals.at(QRandomGenerator::global()->bounded(m_allPossibleAnimals.size()));
                if(!m_currentBestiary.contains(cand) && !m_upgradeOptions.contains(cand)) {
                    m_upgradeOptions.append(cand);
                }
            }
        }
        else if (m_currentLevel == 2) {
            // LEVEL 3 : Upgrade an animal from the bestiary
            // Pick 2 animals from the bestiary to choose from
            if (m_currentBestiary.size() >= 2) {
                while(m_upgradeOptions.size() < 2) {
                    QString cand = m_currentBestiary.at(QRandomGenerator::global()->bounded(m_currentBestiary.size()));
                    if(!m_upgradeOptions.contains(cand)) {
                        m_upgradeOptions.append(cand);
                    }
                }
            }
        }
        emit stateChanged();
        emit upgradeChanged();
    }
}

void AnimalsGame::selectUpgrade(int choiceIndex)
{
    if (choiceIndex < 0 || choiceIndex >= m_upgradeOptions.size()) return;

    // Add animal to bestiary
    m_currentBestiary.append(m_upgradeOptions.at(choiceIndex));

    // Level up
    m_currentLevel++;
    m_isLevelingUp = false;

    emit bestiaryChanged();
    emit currentLevelChanged();
    emit stateChanged();
}

void AnimalsGame::handleCellClick(int index)
{
    if (m_isLevelingUp) return;

    // If nothing is selected, select an animal
    if (m_selectedIndex == -1) {
        if (!m_grid.at(index).isEmpty()) {
            m_selectedIndex = index;
            emit selectedIndexChanged();
        }
        return;
    }

    // Cancel selection if we re-click on the same cell
    if (m_selectedIndex == index) {
        m_selectedIndex = -1;
        emit selectedIndexChanged();
        return;
    }


    // If click on another animal, change selection
    if (!m_grid.at(index).isEmpty()) {
        m_selectedIndex = index;
        emit selectedIndexChanged();
        return;
    }
}

QList<int> AnimalsGame::getMovementPath(int from, int to)
{
    QList<int> path;

    // Check bounds
    if (from < 0 || from >= 35 || to < 0 || to >= 35) return path;
    if (from == to) return path;
    if (!m_grid.at(to).isEmpty()) return path;  // Destination is occupied

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
                if (!parentMap.contains(nextIndex) && (m_grid.at(nextIndex).isEmpty() || nextIndex == to)) {
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


void AnimalsGame::finalizeMovement(int from, int to) {
    if (from < 0 || from >= 35 || to < 0 || to >= 35) return;

    // Move the value in the C++ grid
    QString animalType = m_grid.at(from);
    m_grid[to] = animalType;
    m_grid[from] = "";
    m_selectedIndex = -1; // Reset selection

    // Matching after move
    Animal* currentAnimal = m_animalRegistry.value(animalType);
    if (currentAnimal) {
        const QSet<int> matchIndices = currentAnimal->getMatchIndices(to, m_grid);

        m_score += currentAnimal->calculateScore(matchIndices.size(), true);

        // if match > 3, remove them from the grid
        if (matchIndices.size() >= 3) {
            emit matchOccurred(matchIndices.values());
            for (int idx : matchIndices) {
                m_grid[idx] = "";
            }
        }
    }

    QList<int> spawnedIndices = spawnNewAnimals(); // Spawn animals after moving

    // Check if match happens after spawning
    for (int spawnIdx : std::as_const(spawnedIndices)) {
        if (m_grid[spawnIdx].isEmpty()) continue; // If animal was already destroyed by another matching

        Animal* spawnedAnimal = m_animalRegistry.value(m_grid[spawnIdx]);
        if (spawnedAnimal) {
            const QSet<int> spawnMatches = spawnedAnimal->getMatchIndices(spawnIdx, m_grid);

            if (spawnMatches.size() >= 3) {
                emit matchOccurred(spawnMatches.values());
                m_score += spawnedAnimal->calculateScore(spawnMatches.size(), false);

                // Destruction
                for (int idx : spawnMatches) {
                    m_grid[idx] = "";
                }
            }
        }
    }

    checkLevelUp();

    emit gridChanged();
    emit scoreChanged();
    emit selectedIndexChanged();
}

QList<int> AnimalsGame::spawnNewAnimals()
{
    QList<int> spawnedIndices;

    // Number of animal to spawn
    // @TODO Change depending on level
    int animalsToSpawn = 1;

    for (int i = 0; i < animalsToSpawn; ++i) {
        // find all empty cells
        QList<int> emptyCells;
        for (int j = 0; j < m_grid.size(); ++j) {
            if (m_grid.at(j).isEmpty()) emptyCells.append(j);
        }

        if (emptyCells.isEmpty()) break; // No more spaces =  @TODO Game Over

        // Take the first animal in the queue and move the queue
        QString nextAnimal = m_animalQueue.first();
        m_animalQueue.removeFirst();
        m_animalQueue.append(getRandomAnimal());

        // Place the animal on an empty cell
        int randomCell = emptyCells.at(QRandomGenerator::global()->bounded(emptyCells.size()));
        m_grid[randomCell] = nextAnimal;
        spawnedIndices.append(randomCell);
    }
    emit animalQueueChanged();
    return spawnedIndices;
}