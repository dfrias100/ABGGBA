#include "ARM7TDMI.h"

inline bool ARM7TDMI::TestCondition(ConditionField armCondField) {
    switch (armCondField) {
    case ConditionField::EQ:

	return m_CPSR.Z == 1;

    case ConditionField::NE:

	return m_CPSR.Z == 0;

    case ConditionField::CS:

	return m_CPSR.C == 1;

    case ConditionField::CC:

	return m_CPSR.C == 0;

    case ConditionField::MI:

	return m_CPSR.N == 1;

    case ConditionField::PL:

	return m_CPSR.N == 0;

    case ConditionField::VS:

	return m_CPSR.V == 1;

    case ConditionField::VC:

	return m_CPSR.V == 0;

    case ConditionField::HI:

	return (m_CPSR.C == 1) && (m_CPSR.Z == 0);

    case ConditionField::LS:

	return (m_CPSR.C == 0) || (m_CPSR.Z == 1);

    case ConditionField::GE:

	return (m_CPSR.N == m_CPSR.V);

    case ConditionField::LT:

	return (m_CPSR.N != m_CPSR.V);

    case ConditionField::GT:

	return (m_CPSR.Z == 0) && (m_CPSR.N == m_CPSR.V);

    case ConditionField::LE:

	return (m_CPSR.Z == 1) || (m_CPSR.N != m_CPSR.V);

    case ConditionField::AL:
    default:
	return true;
    };
}
