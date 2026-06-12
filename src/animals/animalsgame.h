#ifndef ANIMALSGAME_H
#define ANIMALSGAME_H

#include <QObject>
#include <QStringList>
#include <QtQml>
#include <QMap>
#include "animal.h"

/// @brief This class will implement the logic for the "Animals" game
/// It will update stuff like the score, levels etc
/// but it will also implements the gameplay
class AnimalsGame : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(int currentLevel READ currentLevel NOTIFY currentLevelChanged)
    Q_PROPERTY(QStringList animalQueue READ animalQueue NOTIFY animalQueueChanged)
    Q_PROPERTY(QStringList grid READ grid NOTIFY gridChanged)
    Q_PROPERTY(QStringList currentBestiary READ currentBestiary NOTIFY bestiaryChanged)
    Q_PROPERTY(QStringList upgradeOptions READ upgradeOptions NOTIFY upgradeChanged)
    Q_PROPERTY(bool isLevelingUp READ isLevelingUp NOTIFY stateChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY selectedIndexChanged) // Which cell is currently selected

    QML_ELEMENT

public:
    explicit AnimalsGame(QObject *parent = nullptr);
    ~AnimalsGame();

    // Getters
    int score() const { return m_score; }
    int currentLevel() const { return m_currentLevel; }
    QStringList animalQueue() const { return m_animalQueue; }
    QStringList grid() const { return m_grid; }
    QStringList currentBestiary() const { return m_currentBestiary; }
    QStringList upgradeOptions() const { return m_upgradeOptions; }
    bool isLevelingUp() const { return m_isLevelingUp; }
    int selectedIndex() const { return m_selectedIndex; }

    // Méthodes appelables depuis le QML
    /// @brief Initialize the game with the score and level
    /// Load the first animals in the queue
    Q_INVOKABLE void initGame();

    /// @brief Place an animal on the grid
    /// @param index Index on the grid where to place the animal
    Q_INVOKABLE void placeAnimal(int index);

    /// @brief Select the upgrade after leveling up
    /// @param choiceIndex Index of the upgrade
    Q_INVOKABLE void selectUpgrade(int choiceIndex);

    /// @brief Handles a click on a cell
    /// First click must target a cell containing an animal
    /// Second click must target a valid destination cell. It will be handled by QML and call getMovementPath to trigger the animation
    Q_INVOKABLE void handleCellClick(int index);

    /// @returns The complete path from the selected cell to the selected destination
    Q_INVOKABLE QList<int> getMovementPath(int from, int to);

    Q_INVOKABLE void finalizeMovement(int from, int to);

signals:
    void scoreChanged();
    void currentLevelChanged();
    void animalQueueChanged();
    void gridChanged();
    void bestiaryChanged();
    void upgradeChanged();
    void stateChanged();
    void selectedIndexChanged();
    /// @param indices List of destroyed cells
    void matchOccurred(const QList<int>& indices);

private:
    /// @brief Get a random animal from the m_currentBestiary to add to m_animalQueue
    /// Returns the random animal to add to m_animalQueue
    QString getRandomAnimal() const;

    /// @brief Remove the first item from m_animalQueue and add a new one
    void advanceQueue();

    // @brief Checks if we are leveling up
    void checkLevelUp();

    /// @brief Check if there is a valid path to the destination
    /// By default, a path is a serie of horizontal or vertical movement which are not obstructed by an obstacle or an animal
    /// However some animals have special movements and might not follow those rules
    bool hasValidPath(int from, int to) const;

    /// @brief Spawn an animal from m_animalQueue on a random non-occupied cell
    /// @returns the list of cells where animals spawned
    QList<int> spawnNewAnimals();

    /// @brief Process matches (by default 3 or more same animals forming an horizontal or vertical line)
    void processMatches(int targetIndex, bool isMovedByUser);

    int m_score = 0;            ///< Current score
    int m_currentLevel = 1;     ///< Current level
    bool m_isLevelingUp = false;
    int m_selectedIndex = -1;   ///< Index of the selected cell. None = -1

    QStringList m_animalQueue;  ///< A queue indicating which animals will be placed on the grid next
    QStringList m_grid;         ///< The grid in which the animals are placed

    //@brief Current actif bestiary (taken from the pool in animalQueue).
    // Starts with 3 animals
    QStringList m_currentBestiary;
    QStringList m_upgradeOptions;    // Choices after leveling up

    /// @brief Map to retrieve Animal from their name
    /// K = animal name, V = animal class
    QMap<QString, Animal*> m_animalRegistry;

    /// @brief contains the list of every possible animals
    const QStringList m_allPossibleAnimals = {
        "cow", "chicken", "dog", "rabbit", "snake",
        "panda", "gorilla", "monkey", "crocodile", "whale", "hippo"
    };
};

#endif // ANIMALSGAME_H