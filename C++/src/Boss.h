#pragma once
#include "Tank.h"

class Boss : public Tank {
public:
    Boss(float x, float y, int hp, double sizeMult, double speedMult,
         int bulletDamage, double bulletSpeedMult);

    sf::Vector2f getFirePoint() const override;
    Bullet* shoot() override;
    void draw(sf::RenderWindow& window) override;

    bool isBoss = true;

private:
    int maxHp;
    double sizeMult;
    double speedMult;
    int bulletDamage;
    double bulletSpeedMult;
};