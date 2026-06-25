#ifndef RABBIT_H
#define RABBIT_H

#include "animal.h"

class Rabbit : public Animal {
public:
    Rabbit() : Animal("rabbit", "Rabbit", "Can jump over animals when moving", "", 10, 30, 60) {
    }

    // Override de la méthode de déplacement
    QList<int> getMovementPath(int start, int end, const QList<QString>& grid) const override;
};

#endif