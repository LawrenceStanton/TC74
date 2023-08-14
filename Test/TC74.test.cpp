/**
 ******************************************************************************
 * @file			: TC74.test.cpp
 * @brief			: Unit Testing for TC74
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define TC74_GTEST_TESTING

#include "TC74.hpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnArg;

using std::nullopt;

using MemoryAddress = TC74::I2C::MemoryAddress;
using Register		= TC74::I2C::Register;

class MockI2C : public TC74::I2C {
public:
	MOCK_METHOD(optional<Register>, read, (MemoryAddress), (noexcept, override, final));
	MOCK_METHOD(optional<Register>, write, (MemoryAddress, Register), (noexcept, override, final));
};

class TC74_Test : public ::testing::Test {
public:
	MockI2C i2c{};
	TC74	tc74{i2c};

	inline void disableI2C(void) const noexcept {
		ON_CALL(i2c, read).WillByDefault(Return(nullopt));
		ON_CALL(i2c, write).WillByDefault(Return(nullopt));
	}
};

TEST_F(TC74_Test, constructorAssignsI2cReference) {
	ASSERT_EQ(&tc74.i2c, &i2c);
}

TEST_F(TC74_Test, readConfigNormallyReturnsConfig) {
	using Config		= TC74::Config;
	using StandbySwitch = Config::StandbySwitch;
	using DataReady		= Config::DataReady;

	const std::map<Register, Config> configExpectedReturnValues = {
		{0x00u, Config{StandbySwitch::NORMAL, DataReady::NOT_READY}},
		{0x40u, Config{StandbySwitch::NORMAL, DataReady::READY}},
		{0x80u, Config{StandbySwitch::STANDBY, DataReady::NOT_READY}},
		{0xC0u, Config{StandbySwitch::STANDBY, DataReady::READY}}};

	for (auto &expectedConfig : configExpectedReturnValues) {
		EXPECT_CALL(i2c, read(Eq(MemoryAddress::CONFIG))).WillOnce(Return(Register(expectedConfig.first)));
		auto transmission = tc74.readConfig();

		EXPECT_TRUE(transmission.has_value());

		auto config = *transmission;
		EXPECT_EQ(config.standbySwitch, expectedConfig.second.standbySwitch);
		EXPECT_EQ(config.dataReady, expectedConfig.second.dataReady);
	}
}

TEST_F(TC74_Test, readConfigReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(tc74.readConfig(), nullopt);
}

TEST_F(TC74_Test, readTemperatureNormallyReturnsTemperature) {
	// https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/21462D.pdf
	// Page 8
	const std::map<Register, int8_t> temperatureExpectedReturnValues = {
		{0b0111'1111u, 127},
		{0b0111'1110u, 126},
		{0b0001'1001u, 25},
		{0b0000'0000u, 0},
		{0b1111'1111u, -1},
		{0b1110'0111u, -25},
		{0b1110'0110u, -26},
		{0b1100'1001u, -55},
		{0b1011'1111u, -65},
	};

	for (auto &expectedTemperature : temperatureExpectedReturnValues) {
		EXPECT_CALL(i2c, read(Eq(MemoryAddress::TEMP))).WillOnce(Return(expectedTemperature.first));
		auto transmission = tc74.readTemperature();

		EXPECT_TRUE(transmission.has_value());

		auto temperature = *transmission;
		EXPECT_EQ(temperature, expectedTemperature.second);
	}
}

TEST_F(TC74_Test, readTemperatureReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(tc74.readTemperature(), nullopt);
}

TEST(TC74_ConfigTest, registerOperatorReturnsRegisterValue) {
	using Config		= TC74::Config;
	using StandbySwitch = Config::StandbySwitch;
	using DataReady		= Config::DataReady;

	const std::map<Config, Register> configExpectedReturnValues = {
		{Config{StandbySwitch::NORMAL, DataReady::NOT_READY}, Register(0x00u)},
		{Config{StandbySwitch::NORMAL, DataReady::READY}, Register(0x40u)},
		{Config{StandbySwitch::STANDBY, DataReady::NOT_READY}, Register(0x80u)},
		{Config{StandbySwitch::STANDBY, DataReady::READY}, Register(0xC0u)}};

	for (auto &expectedConfig : configExpectedReturnValues) {
		EXPECT_EQ(Register(expectedConfig.first), expectedConfig.second);
	}
}
