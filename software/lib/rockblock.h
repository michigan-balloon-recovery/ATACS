#ifndef ROCKBLOCK_H_
#define ROCKBLOCK_H_

#include <utility>
#include <pads.h>

class RockBLOCK {
    public:
        RockBLOCK(padpin &rxd, padpin &txd, padpin &netav, padpin &ri, padpin &onoff);
        bool network_available();
    private:
        const padpin rxd, txd, netav, ri, onoff;
};

#endif // ROCKBLOCK_H_
