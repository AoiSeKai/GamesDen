#include "animalsgame.h"
#include <QRandomGenerator>
#include "rabbit.h"

AnimalsGame::AnimalsGame(QObject *parent) : QObject(parent)
{
    m_animalRegistry.insert("rabbit", new Rabbit());


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
    int nextLevel = m_currentLevel + 1;

    // Max level reached already, nothing to add.
    if (!m_levelThresholds.contains(nextLevel)) {
        return;
    }

    // Check if score reached a threshold
    if (m_score >= m_levelThresholds.value(nextLevel)) {
        m_currentLevel = nextLevel;
        m_animalOptions.clear();

        // Get the reward type
        RewardType reward = m_levelRewards.value(m_currentLevel, RewardType::None);

        // Selecting a new animal
        if (reward == RewardType::NewAnimal) {
            m_isLevelingUp = true;
            m_isUpgrading = false;

            // Choose between 2 new animals to add to current bestiary
            while (m_animalOptions.size() < 2) {
                QString cand = m_allPossibleAnimals.at(QRandomGenerator::global()->bounded(m_allPossibleAnimals.size()));
                if (!m_currentBestiary.contains(cand) && !m_animalOptions.contains(cand)) {
                    m_animalOptions.append(cand);
                }
            }
        }
        // Upgrade an animal
        else if (reward == RewardType::Upgrade) {
            m_isLevelingUp = false;
            m_isUpgrading = true;

            // Choose between 2 animals from current bestiary to upgrade
            if (m_currentBestiary.size() >= 2) {
                while (m_animalOptions.size() < 2) {
                    QString cand = m_currentBestiary.at(QRandomGenerator::global()->bounded(m_currentBestiary.size()));
                    if (!m_animalOptions.contains(cand)) {
                        m_animalOptions.append(cand);
                    }
                }
            }
        }
        // Nothing
        else {
            m_isLevelingUp = false;
            m_isUpgrading = false;
        }

        emit currentLevelChanged();
        emit stateChanged();
        emit upgradeChanged();
    }
}

void AnimalsGame::selectNewAnimal(int choiceIndex)
{
    if (choiceIndex < 0 || choiceIndex >= m_animalOptions.size()) return;

    // Add animal to bestiary
    m_currentBestiary.append(m_animalOptions.at(choiceIndex));

    // Level up
    m_currentLevel++;
    m_isLevelingUp = false;

    emit bestiaryChanged();
    emit currentLevelChanged();
    emit stateChanged();
}

void AnimalsGame::selectUpgrade(int choiceIndex) {

    if (choiceIndex < 0 || choiceIndex >= m_animalOptions.size()) {
        qWarning() << "⚠ selectUpgrade: Invalid index :" << choiceIndex;
        return;
    }

    // Get the animal id
    QString selectedAnimalType = m_animalOptions.at(choiceIndex);

    // Upgrading animal
    if (m_animalRegistry.contains(selectedAnimalType)) {
        Animal* animal = m_animalRegistry.value(selectedAnimalType);
        if (animal) {
            animal->setUpgraded(true);
        }
    } else {
        qWarning() << "Error: unknown animal " << selectedAnimalType;
    }

    m_animalOptions.clear();
    m_isUpgrading = false;

    emit stateChanged();
    emit upgradeChanged();
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

    // An animal is selected and we are moving it to an empty cell.
    if (m_grid.at(index).isEmpty()) {
        prepareMovement(index);
        return;
    }


    // If click on another animal, change selection
    if (!m_grid.at(index).isEmpty()) {
        m_selectedIndex = index;
        emit selectedIndexChanged();
        return;
    }
}

void AnimalsGame::prepareMovement(int targetIndex)
{
    // On récupère l'instance de l'animal dans le registre via son type (string)
    QString animalType = m_grid.at(m_selectedIndex);
    Animal* animal = m_animalRegistry.value(animalType);

    if (animal) {
        // On demande à l'animal de calculer son chemin (Rabbit override, les autres utilisent le défaut)
        QList<int> path = animal->getMovementPath(m_selectedIndex, targetIndex, m_grid);

        if (!path.isEmpty()) {
            emit requestMovementAnimation(m_selectedIndex, animalType, path);
        }
    }
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


QString AnimalsGame::getAnimalPower(const QString& animalType) const {
    Animal* animal = m_animalRegistry.value(animalType);
    if (animal) {
        return animal->powerDescription();
    }

    return "Error: Unknown animal";
}

int AnimalsGame::getAnimalScore(const QString& animalType) const {
    Animal* animal = m_animalRegistry.value(animalType);
    if (animal) {
        return animal->score();
    }

    return -1;
}

QString AnimalsGame::getAnimalUpgrade(const QString& animalType) const {
    Animal* animal = m_animalRegistry.value(animalType);
    if (animal) {
        return animal->upgradeDescription();
    }

    return "Error: Unknown animal";
}

int AnimalsGame::getAnimalUpgradedScore(const QString& animalType) const {
    Animal* animal = m_animalRegistry.value(animalType);
    if (animal) {
        return animal->upgradedScore();
    }

    return -1;
}

