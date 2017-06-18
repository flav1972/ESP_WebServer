#include <Arduino.h>
#include <Ticker.h>
#include "globals.h"
#include "flip.h"

Ticker flipper;

int count = 0;
int gstate = 0;

void flip()
{
  int state = digitalRead(PIN);

  digitalWrite(PIN, !state);

  ++count;
  if(count == 20) {
    flipper.attach(0.05, flip);
    gstate = 1;
  }
  else if (count == 120) {
    flipper.attach(0.3, flip);
    count = 0;
    gstate = 0;
  }
}
