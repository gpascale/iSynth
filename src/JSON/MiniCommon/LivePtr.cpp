#include "LivePtr.h"

LiveObj::~LiveObj() {
    while (head) {
        head->ptr = 0;
        head = head->next;
    }
}
