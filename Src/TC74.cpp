/**
 ******************************************************************************
 * @file			: TC74.cpp
 * @brief			: Source for TC74.hpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "TC74.hpp"

using std::nullopt;

TC74::TC74(I2C &i2c) : i2c(i2c) {}

std::optional<TC74::Config> TC74::readConfig(void) const noexcept {
	auto transmission = this->i2c.read(MemoryAddress::CONFIG);

	if (!transmission) return nullopt;

	return Config(*transmission);
}

optional<int8_t> TC74::readTemperature(void) const noexcept {
	auto transmission = this->i2c.read(MemoryAddress::TEMP);

	if (!transmission) return nullopt;

	auto temperature = static_cast<int8_t>(*transmission);
	return temperature;
}

TC74::Config::Config(StandbySwitch standbySwitch) noexcept : standbySwitch(standbySwitch) {}

TC74::Config::Config(StandbySwitch standbySwitch, DataReady dataReady) noexcept
	: standbySwitch(standbySwitch), //
	  dataReady(dataReady) {}

TC74::Config::Config(Register rwcr) noexcept
	: standbySwitch(static_cast<StandbySwitch>(rwcr & 0x80u)), //
	  dataReady(static_cast<DataReady>(rwcr & 0x40u)) {}

TC74::Config::operator Register() const noexcept {
	return static_cast<Register>(standbySwitch) | static_cast<Register>(dataReady);
}
