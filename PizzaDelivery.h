#pragma once

class PizzaDeliveryGame : public ArcadeGame {
private:
    float py;
    int score;
    int state;
    
    struct Mailbox { float y; bool rightSide; bool hit; };
    Mailbox boxes[4];

    struct Pizza { float x, y, vx, vy; bool active; };
    Pizza pizzas[2];

    bool lastL, lastR;

public:
    const char* getName() override { return "Pizza Delivery"; }
    int getScore() override { return score; }

    void init() override {
        score = 0; state = 0;
        py = 50; lastL = true; lastR = true;
        for(int i=0; i<4; i++) {
            boxes[i].y = -i * 30;
            boxes[i].rightSide = random(2);
            boxes[i].hit = false;
        }
        for(int i=0; i<2; i++) pizzas[i].active = false;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            // Throw pizza
            if (cL && !lastL) {
                for(int i=0; i<2; i++) if(!pizzas[i].active) {
                    pizzas[i].active = true; pizzas[i].x = 64; pizzas[i].y = py;
                    pizzas[i].vx = -3.0; pizzas[i].vy = -1.0; break;
                }
            }
            if (cR && !lastR) {
                for(int i=0; i<2; i++) if(!pizzas[i].active) {
                    pizzas[i].active = true; pizzas[i].x = 64; pizzas[i].y = py;
                    pizzas[i].vx = 3.0; pizzas[i].vy = -1.0; break;
                }
            }

            lastL = cL; lastR = cR;

            // Scroll world
            for(int i=0; i<4; i++) {
                boxes[i].y += 1.5;
                if (boxes[i].y > 64) {
                    boxes[i].y = -30;
                    boxes[i].rightSide = random(2);
                    if (!boxes[i].hit) { state = 1; delay(500); } // missed delivery
                    boxes[i].hit = false;
                }
            }

            // Update Pizzas
            for(int i=0; i<2; i++) {
                if (pizzas[i].active) {
                    pizzas[i].x += pizzas[i].vx;
                    pizzas[i].y += pizzas[i].vy;
                    if (pizzas[i].x < 0 || pizzas[i].x > 128 || pizzas[i].y < 0) pizzas[i].active = false;

                    // Collision with box
                    for(int j=0; j<4; j++) {
                        if (!boxes[j].hit) {
                            float bx = boxes[j].rightSide ? 108 : 20;
                            if (abs(pizzas[i].x - bx) < 8 && abs(pizzas[i].y - boxes[j].y) < 8) {
                                boxes[j].hit = true;
                                pizzas[i].active = false;
                                score++;
                            }
                        }
                    }
                }
            }

            display.clearDisplay();
            
            // Road
            display.drawLine(40, 0, 40, 64, WHITE);
            display.drawLine(88, 0, 88, 64, WHITE);

            // Mailboxes
            for(int i=0; i<4; i++) {
                int bx = boxes[i].rightSide ? 104 : 16;
                if (!boxes[i].hit) display.drawRect(bx, (int)boxes[i].y, 8, 8, WHITE);
                else display.fillRect(bx, (int)boxes[i].y, 8, 8, WHITE);
            }

            // Player car
            display.fillRect(60, (int)py, 8, 12, WHITE);

            // Pizzas
            for(int i=0; i<2; i++) {
                if (pizzas[i].active) display.fillCircle((int)pizzas[i].x, (int)pizzas[i].y, 3, WHITE);
            }
            
            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("MISSED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(PizzaDeliveryGame)
