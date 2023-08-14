/**
 ******************************************************************************
 * @file			: TC74.cpp
 * @brief			: Example Source for TC74.hpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "TC74_I2C.hpp"

using std::nullopt;

using Register		= TC74::I2C::Register;
using MemoryAddress = TC74::I2C::MemoryAddress;

optional<Register> TC74_I2C::read(MemoryAddress memoryAddress) noexcept {
	static_assert(sizeof(Register) == sizeof(uint8_t));
	uint8_t data = 0;

	auto status = HAL_I2C_Mem_Read(
		&this->hi2c,									// HAL I2C Handle
		static_cast<uint8_t>(this->deviceAddress) << 1, // TC74 I2C bus address
		static_cast<uint8_t>(memoryAddress),			// TC74 memory register address
		sizeof(uint8_t),								// 1 byte I2C address
		&data,											// Data array allocation. Return parameter.
		sizeof(data),									// Bytes
		100												// Timeout (milliseconds). Can optionally be reduced.
	);
	if (status != HAL_OK) return nullopt;
	else return Register(data);
}

optional<Register> TC74_I2C::write(MemoryAddress memoryAddress, Register data) noexcept {
	static_assert(sizeof(Register) == sizeof(uint8_t));
	auto status = HAL_I2C_Mem_Write(
		&this->hi2c,									// HAL I2C Handle
		static_cast<uint8_t>(this->deviceAddress) << 1, // TC74 I2C bus address
		static_cast<uint8_t>(memoryAddress),			// TC74 memory register address
		sizeof(uint8_t),								// 1 byte I2C address
		&data,											// Data array allocation. Input parameter.
		sizeof(data),									// Bytes
		100												// Timeout (milliseconds). Can optionally be reduced.
	);
	if (status != HAL_OK) return nullopt;
	else return data;
}

TC74_I2C::TC74_I2C(I2C_HandleTypeDef &hi2c, DeviceAddress deviceAddress) noexcept
	: hi2c(hi2c), deviceAddress(deviceAddress) {}
