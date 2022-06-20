inline uint32_t ARM7TDMI::LSL(uint32_t unOperand, uint32_t unShiftAmount, bool bAffectFlags) {
    if (unShiftAmount != 0) {
	uint32_t unCarryOut; 
	if (unShiftAmount < 32) {
	    unCarryOut = (unOperand & (1 << (32 - unShiftAmount))) >> (32 - unShiftAmount);
	    unOperand <<= unShiftAmount;
	} else {
	    if (unShiftAmount == 32)
		unCarryOut = unOperand & 0x1;
	    else
		unCarryOut = 0;
	    unOperand = 0;
	}
	if (bAffectFlags)
	    m_CPSR.C = unCarryOut;
    }
    return unOperand;
}

inline uint32_t ARM7TDMI::LSR(uint32_t unOperand, uint32_t unShiftAmount, bool bAffectFlags, bool bImmediate) {
    uint32_t unCarryOut = m_CPSR.C;

    if (unShiftAmount != 0) {
	if (unShiftAmount < 32) {
	    unCarryOut = (unOperand & (1 << (unShiftAmount - 1))) >> (unShiftAmount - 1);
	    unOperand >>= unShiftAmount;
	} else {
	    if (unShiftAmount == 32)
		unCarryOut = unOperand >> 31;
	    else
		unCarryOut = 0;
	    unOperand = 0;
	}
    } else if (bImmediate) {
	// If the shift amount is 0 and an immediate,
	// this is LSR #32
	unCarryOut = (unOperand & 0x10000000) >> 31;
	unOperand = 0;
    }

    if (bAffectFlags)
	m_CPSR.C = unCarryOut;

    return unOperand;
}

inline uint32_t ARM7TDMI::ASR(uint32_t unOperand, uint32_t unShiftAmount, bool bAffectFlags, bool bImmediate) {
    uint32_t unCarryOut = m_CPSR.C;
    
    if (unShiftAmount < 32 && unShiftAmount > 0) {
	unCarryOut = (unOperand & (1 << (unShiftAmount - 1))) >> (unShiftAmount - 1);
	unOperand = ((int32_t)unOperand >> unShiftAmount);
    } else if (bImmediate || unShiftAmount >= 32) {
	unCarryOut = (unOperand & 0x80000000) >> 31;
	unOperand = unCarryOut ? 0xFFFFFFFF : 0x0;
    }

    if (bAffectFlags)
	m_CPSR.C = unCarryOut;

    return unOperand;
}

inline uint32_t ARM7TDMI::ROR(uint32_t unOperand, uint32_t unShiftAmount, bool bAffectFlags, bool bImmediate) {
    uint32_t unCarryOut = m_CPSR.C;

    if (unShiftAmount != 0) {
	while (unShiftAmount > 32) {
	    unShiftAmount -= 32;
	}
	unCarryOut = (unOperand & (1 << (unShiftAmount - 1))) >> (unShiftAmount - 1);
	uint32_t unOvershotBits = (int32_t)0x80000000L >> (unShiftAmount - 1);
	unOvershotBits >>= (32 - unShiftAmount);
	unOvershotBits = unOperand & unOvershotBits;
	unOvershotBits <<= (32 - unShiftAmount);
	unOperand >>= unShiftAmount;
	unOperand |= unOvershotBits;
    } else if (bImmediate) {
	// RRX
	unCarryOut = unOperand & 0x1;
	unOperand >>= 1;
	unOperand |= (m_CPSR.C << 31);
    }

    if (bAffectFlags)
	m_CPSR.C = unCarryOut;
    return unOperand;
}