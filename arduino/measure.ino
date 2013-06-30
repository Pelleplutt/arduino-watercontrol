#include "greenhouse.h"


unsigned char measuring;
unsigned long measuring_triggers;

void
_measurementhandler() {
    measuring_triggers++;
}

unsigned int
measure(unsigned char monitor) {
    unsigned char a0, a1, enable, sense, interrupt;
    unsigned long measure_dur;
    unsigned long measuring_start;

    if(monitor < 4) {
        interrupt = PORT_SENSE0_INT;
        sense = PORT_SENSE0_S;
        enable = PORT_SENSE0_E;
        a0 = PORT_SENSE0_A0;
        a1 = PORT_SENSE0_A1;
    } else {
        interrupt = PORT_SENSE1_INT;
        sense = PORT_SENSE1_S;
        enable = PORT_SENSE1_E;
        a0 = PORT_SENSE1_A0;
        a1 = PORT_SENSE1_A1;
    }

    digitalWrite(a0, monitor & 1 ?HIGH:LOW);
    digitalWrite(a1, monitor & 2 ?HIGH:LOW);
    digitalWrite(enable, HIGH);

    measuring_triggers = 0;
    attachInterrupt(interrupt, _measurementhandler, FALLING);
    measuring_start = millis();
    delay(5000);
    detachInterrupt(interrupt);
    measure_dur = millis() - measuring_start;

    digitalWrite(a0, LOW);
    digitalWrite(a1, LOW);
    digitalWrite(enable, LOW);

    measuring_triggers = measuring_triggers * 1000 / measure_dur;

    monitors[monitor].current_value = measuring_triggers;
    return measuring_triggers;
}

