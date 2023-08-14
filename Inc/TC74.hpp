/**
 ******************************************************************************
 * @file			: TC74.hpp
 * @brief			: TC74 I2C Temperature Sensor Driver
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once
#include <cstdint>
#include <optional>

using std::optional;

class TC74 {
public:
	class I2C {
	public:
		typedef uint8_t Register;

		enum class DeviceAddress : Register {
			A0 = 0b1001'000u,
			A1 = 0b1001'001u,
			A2 = 0b1001'010u,
			A3 = 0b1001'011u,
			A4 = 0b1001'100u,
			A5 = 0b1001'101u,
			A6 = 0b1001'110u,
			A7 = 0b1001'111u,
		};

		enum class MemoryAddress : Register {
			TEMP   = 0x00u,
			CONFIG = 0x01u,
		};

		virtual optional<Register> read(MemoryAddress address) noexcept					= 0;
		virtual optional<Register> write(MemoryAddress address, Register data) noexcept = 0;

		virtual ~I2C() noexcept = default;
	};
	using Register		= I2C::Register;
	using DeviceAddress = I2C::DeviceAddress;
	using MemoryAddress = I2C::MemoryAddress;

	struct Config {
		enum class StandbySwitch : Register {
			STANDBY = 0x80u,
			NORMAL	= 0x00u,
		};
		enum class DataReady : Register {
			READY	  = 0x40u,
			NOT_READY = 0x00u,
		};

		StandbySwitch	standbySwitch = StandbySwitch::NORMAL;
		const DataReady dataReady	  = DataReady::NOT_READY;

		explicit Config(StandbySwitch standbySwitch) noexcept;
		operator Register() const noexcept;

	private:
		friend class TC74;
		explicit Config(Register rwcr) noexcept;
		explicit Config(StandbySwitch standbySwitch, DataReady dataReady) noexcept;

#ifdef TC74_GTEST_TESTING
		friend class TC74_Test;
		FRIEND_TEST(TC74_Test, readConfigNormallyReturnsConfig);
		FRIEND_TEST(TC74_ConfigTest, registerOperatorReturnsRegisterValue);
#endif
	};

protected:
	I2C &i2c;

public:
	TC74(I2C &i2c);

	optional<Config> readConfig(void) const noexcept;

	optional<int8_t> readTemperature(void) const noexcept;

#ifdef TC74_GTEST_TESTING
	FRIEND_TEST(TC74_Test, constructorAssignsI2cReference);
#endif
};
