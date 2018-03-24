#include "catch.hpp"

#include <cstdint>

#include "mock_memio.hpp"

#include "syscontrol.h"

constexpr uint32_t vtor_addr = 0xe000ed08;

TEST_CASE("Test Vector Table relocation") {
    constexpr uint32_t old_location = 0;
    constexpr uint32_t tblbase = (1 << 29);

    auto& mem = mock::get_global_memory();

    mem.set_value_at(vtor_addr, old_location);
    mem.set_value_at(old_location, 0xaa55aa55);
    mem.set_value_at(old_location + 4, 0x55aa55aa);
    mem.set_value_at(old_location + 8, 0x00aa00aa);

    SECTION("Test with new location in RAM region") {
        constexpr uint32_t new_location = 0x2001 << 16;
        syscontrol_relocate_vt(new_location, 2);
        auto vt_location = mem.get_value_at(vtor_addr);
        /* Check TBLBASE bit */
        CHECK((vt_location & tblbase) == tblbase);
        CHECK(mem.get_value_at(new_location) == 0xaa55aa55);
        CHECK(mem.get_value_at(new_location + 4) == 0x55aa55aa);
        CHECK(mem.get_value_at(new_location + 8) == 0x00aa00aa);
    }

    SECTION("Test with new location in CODE region") {
        constexpr uint32_t new_location = 0x0081 << 16;
        syscontrol_relocate_vt(new_location, 2);
        auto vt_location = mem.get_value_at(vtor_addr);
        CHECK((vt_location & tblbase) == 0);
        CHECK(mem.get_value_at(new_location) == 0xaa55aa55);
        CHECK(mem.get_value_at(new_location + 4) == 0x55aa55aa);
        CHECK(mem.get_value_at(new_location + 8) == 0x00aa00aa);
    }
}