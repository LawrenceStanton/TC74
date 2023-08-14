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
	/**
	 * @brief I2C Communication Interface
	 *
	 * @details
	 * This interface is used to communicate with the TC74 temperature sensor.
	 * It is a pure virtual class and must be implemented by a concrete class inheriting the interface functions.
	 * The concrete class is then aggregated by the TC74 class via dependency injection in the constructor.
	 *
	 * This interface also allows a mocked I2C interface to be injected into the TC74 class for unit testing.
	 *
	 * @warning
	 * The TC74 class does not own the I2C interface and therefore does not manage its lifetime.
	 * The I2C interface must be managed by the user of the TC74 class to ensure the interface outlives the TC74 class.
	 * It is recommended to make both objects static to ensure neither are not destroyed before the program exits.
	 *
	 * @note The TC74 does not supply the I2C address of the device. It is expected that this parameter is supplied by
	 * the user.
	 */
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

	protected:
		DeviceAddress deviceAddress;

	public:
		/**
		 * @brief Read a register from the TC74 peripheral.
		 *
		 * @param address The TC74 internal memory address to read from.
		 * @return The value of the register read from the TC74 peripheral, if successful. Otherwise, nullopt.
		 */
		virtual optional<Register> read(MemoryAddress address) noexcept = 0;

		/**
		 * @brief Write a register value to the TC74 peripheral.
		 *
		 * @param address The TC74 internal memory address to write to.
		 * @param data The value to write to the register.
		 * @return The value of the register written to the TC74 peripheral, if successful. Otherwise, nullopt.
		 */
		virtual optional<Register> write(MemoryAddress address, Register data) noexcept = 0;

		I2C(DeviceAddress deviceAddress) noexcept;
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
	/**
	 * @brief Construct a new TC74 client.
	 *
	 * @param i2c Reference to the I2C interface to use for communication with the TC74 peripheral.
	 * @note Refer to `TC74::I2C` for more information.
	 */
	TC74(I2C &i2c);

	/**
	 * @brief Read the TC74 configuration register.
	 *
	 * @return The TC74 Config, if successful. Otherwise, nullopt.
	 */
	optional<Config> readConfig(void) const noexcept;

	/**
	 * @brief Read the TC74 temperature.
	 *
	 * @return The temperature in degrees Celsius, if successful. Otherwise, nullopt.
	 */
	optional<int8_t> readTemperature(void) const noexcept;

#ifdef TC74_GTEST_TESTING
	FRIEND_TEST(TC74_Test, constructorAssignsI2cReference);
#endif
};
