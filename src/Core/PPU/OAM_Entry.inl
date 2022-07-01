struct OAM_Entry {
    union {
	struct {
	    uint8_t uyCoordinate;
	    uint8_t objRotationScaling : 1;
	    union {
		uint8_t objDblSize     : 1;
		uint8_t objDisable     : 1;
	    };
	    uint8_t objMode            : 2;
	    uint8_t objMosaic          : 1;
	    uint8_t objColors          : 1;
	    uint8_t objShape           : 2;
	};
	uint16_t objAttribute0Packed;
    } objAttribute0;

    union {
	struct {
	    uint16_t uxCoordinate : 9;
	    union {
		struct {
		    uint8_t objScalingParamSelect : 5;
		};
		struct {
		    uint8_t objUnused    : 3;
		    uint8_t objHorizFlip : 1;
		    uint8_t objVertFlip  : 1;
		};
	    };
	    uint8_t objSize : 2;
	};
	uint16_t objAttribute1Packed;
    } objAttribute1;

    union {
	struct {
	    uint16_t objChrName    : 10;
	    uint8_t  objBgPriority :  2;
	    uint8_t  objPaletteNum :  4;
	};
	uint16_t objAttribute2Packed;
    } objAttribute2;
};
