#include "rabbit.h"


QList<int> Rabbit::getMovementPath(int start, int end, const QList<QString>& grid) const  {
    QList<int> path;

    int startX = start % 7;
    int startY = start / 7;
    int endX = end % 7;
    int endY = end / 7;

    // Column
    int currentX = startX;
    while (currentX != endX) {
        currentX += (endX > startX) ? 1 : -1;
        path.append(startY * 7 + currentX);
    }

    // Then line
    int currentY = startY;
    while (currentY != endY) {
        currentY += (endY > startY) ? 1 : -1;
        path.append(currentY * 7 + endX);
    }

    return path;
}