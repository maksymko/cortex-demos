#include "catch.hpp"

#include <cstring>
#include <tuple>

#include "mock_memio.hpp"

#include "driver/uarte.hpp"
#include "pinctrl.hpp"
#include "nrf52/pinctrl.hpp"

constexpr uint32_t uarte0_base = 0x40002000;
constexpr uint32_t uarte1_base = 0x40028000;

// Offsets for some registers used in tests
constexpr auto psel_rts = 0x508;
constexpr auto psel_txd = 0x50c;
constexpr auto psel_cts = 0x510;
constexpr auto psel_rxd = 0x514;

constexpr auto rxd_ptr = 0x534;
constexpr auto rxd_maxcnt = 0x538;
constexpr auto rxd_amount = 0x53c;

constexpr auto txd_ptr = 0x544;
constexpr auto txd_maxcnt = 0x548;
constexpr auto txd_amount = 0x54c;

constexpr auto config = 0x56c;


TEST_CASE("TEST UARTE API") {
    auto& mem = mock::get_global_memory();
    mem.reset();

    SECTION("UARTE0") {
        auto* uarte0 = driver::UART::request_by_id(driver::UART::ID::UARTE0);
        REQUIRE(uarte0 != nullptr);

        // Check that we are only configuring once
        uarte0 = driver::UART::request_by_id(driver::UART::ID::UARTE0);
        REQUIRE(uarte0 != nullptr);

        auto& journal = mem.get_journal();
        auto conf_count = 0;
        for (auto& entry : journal) {
            if (std::get<1>(entry) == uarte0_base + rxd_maxcnt) {
                ++conf_count;
            }
        }

        CHECK(conf_count == 1);

        // Check that the peripheral was enabled
        CHECK(mem.get_value_at(uarte0_base + 0x500) == 8);

        // Check that the pins were configured
        auto cts = pinctrl::get_pin(pinctrl::function::UARTE0_CTS);
        CHECK(cts >= 0);
        CHECK((mem.get_value_at(uarte0_base + psel_cts) & 0x1f) == cts);

        auto rts = pinctrl::get_pin(pinctrl::function::UARTE0_RTS);
        CHECK(rts >= 0);
        CHECK((mem.get_value_at(uarte0_base + psel_rts) & 0x1f) == rts);

        auto txd = pinctrl::get_pin(pinctrl::function::UARTE0_TXD);
        CHECK(txd >= 0);
        CHECK((mem.get_value_at(uarte0_base + psel_txd) & 0x1f) == txd);

        auto rxd = pinctrl::get_pin(pinctrl::function::UARTE0_RXD);
        CHECK(rxd >= 0);
        CHECK((mem.get_value_at(uarte0_base + psel_rxd) & 0x1f) == rxd);

        auto* txd_buffer = mem.get_ptr_at(uarte0_base + txd_ptr);
        auto* rxd_buffer = mem.get_ptr_at(uarte0_base + rxd_ptr);

        REQUIRE(txd_buffer != nullptr);
        REQUIRE(rxd_buffer != nullptr);

        REQUIRE(mem.get_value_at(uarte0_base + txd_maxcnt) != 0);
        REQUIRE(mem.get_value_at(uarte0_base + rxd_maxcnt) != 0);

        // Here we are just checking that we are not segfaulting
        std::memset(txd_buffer, 0, mem.get_value_at(uarte0_base + txd_maxcnt));
        std::memset(rxd_buffer, 0, mem.get_value_at(uarte0_base + rxd_maxcnt));
    }

    SECTION("UARTE1") {
        auto* uarte1 = driver::UART::request_by_id(driver::UART::ID::UARTE1);
        REQUIRE(uarte1 != nullptr);
    }
}