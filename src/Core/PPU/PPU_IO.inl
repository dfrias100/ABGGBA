union DISPCNT {
    struct {
	uint8_t ppuBgMode        : 3;
	uint8_t ppuReserved      : 1;
	uint8_t ppuFrameSelect   : 1;
	uint8_t ppuHblankFree    : 1;
	uint8_t ppuObjMapping    : 1;
	uint8_t ppuForcedBlank   : 1;
	uint8_t ppuEnabledBg0    : 1;
	uint8_t ppuEnabledBg1    : 1;
	uint8_t ppuEnabledBg2    : 1;
	uint8_t ppuEnabledBg3    : 1;
	uint8_t ppuEnabledObj    : 1;
	uint8_t ppuEnabledWin0   : 1;
	uint8_t ppuEnabledWin1   : 1;
	uint8_t ppuEnabledWinObj : 1;
    };
    uint16_t usnDispcntPacked;
};

union DISPSTAT {
    struct {
	uint8_t ppuVblank      : 1;
	uint8_t ppuHblank      : 1;
	uint8_t ppuVcountMatch : 1;
	uint8_t ppuVblankIrq   : 1;
	uint8_t ppuHblankIrq   : 1;
	uint8_t ppuVcountIrq   : 1;
	uint8_t ppuUnused      : 2;
	uint8_t ppuLyc;

    };
    uint16_t usnDispstatPacked;
};

union VCOUNT {
    struct {
	uint8_t ppuScanline;
	uint8_t ppuUnused;
    };
    uint16_t usnVcountPacked;
};

union BGxCNT {
    struct {
	uint8_t bgPriority        : 2;
	uint8_t bgCharBase        : 2;
	uint8_t bgUnused          : 2;
	uint8_t bgMosaic          : 1;
	uint8_t bgColors          : 1;
	uint8_t bgScreenBase      : 5;
	uint8_t bgDisplayOverflow : 1;
	uint8_t bgScreenSize      : 2;
    };
    uint16_t usnBgcntPacked;
};

union BGxHOFS {
    struct {
	uint16_t bgHorizontalOffset : 9;
	uint8_t  bgUnused : 7;
    };
    uint16_t usnBghofsPacked;
};

union BGxVOFS {
    struct {
	uint16_t bgVerticalOffset : 9;
	uint8_t  bgUnused : 7;
    };
    uint16_t usnBgvofsPacked;
};

union BGxX {
    struct {
	uint32_t bgFixedPointY : 28;
	uint32_t bgUnused : 4;
    };
    uint32_t unBgxPacked;
};

union BGxY {
    struct {
	uint32_t bgFixedPointY : 28;
	uint32_t bgUnused : 4;
    };
    uint32_t unBgyPacked;
};