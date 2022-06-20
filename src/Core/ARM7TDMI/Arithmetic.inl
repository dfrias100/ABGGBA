inline uint32_t ARM7TDMI::SUB(uint32_t unLeftOperand, uint32_t unRightOperand, bool bAffectFlags) {
    uint64_t ulResult = (uint64_t)unLeftOperand - unRightOperand;

    if (bAffectFlags) {
	if (unRightOperand <= unLeftOperand)
	    m_CPSR.C = 1;
	else
	    m_CPSR.C = 0;

	ulResult &= 0xFFFF'FFFF;

	m_CPSR.Z = ulResult == 0;
	m_CPSR.N = (ulResult & 0x8000'0000) != 0;
	m_CPSR.V = ((unLeftOperand ^ unRightOperand) & (~unRightOperand ^ ulResult) & 0x8000'0000) != 0;
    }

    return ulResult;
}

inline uint32_t ARM7TDMI::ADD(uint32_t unLeftOperand, uint32_t unRightOperand, bool bAffectFlags) {
    uint64_t ulResult = (uint64_t)unLeftOperand + unRightOperand;

    if (bAffectFlags) {
	if (ulResult > 0xFFFF'FFFF)
	    m_CPSR.C = 1;
	else
	    m_CPSR.C = 0;

	ulResult &= 0xFFFF'FFFF;

	m_CPSR.Z = ulResult == 0;
	m_CPSR.N = (ulResult & 0x8000'0000) != 0;
	m_CPSR.V = ((unLeftOperand ^ ulResult) & (ulResult ^ unRightOperand) & 0x8000'0000) != 0;
    }

    return ulResult;
}

inline uint32_t ARM7TDMI::ADC(uint32_t unLeftOperand, uint32_t unRightOperand, bool bAffectFlags) {
    uint64_t ulResult = (uint64_t)unLeftOperand + unRightOperand + m_CPSR.C;

    if (bAffectFlags) {
	if (ulResult > 0xFFFF'FFFF)
	    m_CPSR.C = 1;
	else
	    m_CPSR.C = 0;

	ulResult &= 0xFFFF'FFFF;

	m_CPSR.Z = ulResult == 0;
	m_CPSR.N = (ulResult & 0x8000'0000) != 0;
	m_CPSR.V = ((unLeftOperand ^ ulResult) & (ulResult ^ unRightOperand) & 0x8000'0000) != 0;
    }

    return ulResult;
}

inline uint32_t ARM7TDMI::SBC(uint32_t unLeftOperand, uint32_t unRightOperand, bool bAffectFlags) {
    // Special case of ADC
    uint64_t ulResult = (uint64_t)unLeftOperand + ~unRightOperand + m_CPSR.C;

    if (bAffectFlags) {
	if (ulResult >= 0xFFFF'FFFF)
	    m_CPSR.C = 1;
	else
	    m_CPSR.C = 0;

	ulResult &= 0xFFFF'FFFF;

	m_CPSR.Z = ulResult == 0;
	m_CPSR.N = (ulResult & 0x8000'0000) != 0;
	m_CPSR.V = ((unLeftOperand ^ ulResult) & (ulResult ^ ~unRightOperand) & 0x8000'0000) != 0;
    }

    return ulResult;
}

inline uint32_t ARM7TDMI::MSC(uint32_t unExpression, bool bAffectFlags) {
    if (bAffectFlags) {
	m_CPSR.Z = unExpression == 0;
	m_CPSR.N = (unExpression & 0x8000'0000) != 0;
    }
    return unExpression;
}