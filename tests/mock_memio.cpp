#include "mock_memio.hpp"

namespace mock {

namespace {

mock::Memory g_memory;

extern "C" void raw_write32(uint32_t addr, uint32_t value) {
    g_memory.write32(addr, value);
}

extern "C" uint32_t raw_read32(uint32_t addr) {
    return g_memory.read32(addr);
}

}  // namespace


void Memory::write32(uint32_t addr, uint32_t value) {
    journal_.push_back(std::make_tuple(Memory::Op::WRITE32, addr, value));
    mem_map_[addr] = value;
}

void Memory::write16(uint32_t addr, uint16_t value) {
    journal_.push_back(std::make_tuple(Memory::Op::WRITE16, addr, value));
    const auto write_addr = addr & (~3);
    uint32_t old_value = 0;
    const auto it = mem_map_.find(write_addr);
    if (it != mem_map_.end()) {
        old_value = it->second;
    }

    mem_map_[write_addr] = (old_value & (~(0xffff << (8 * (addr & 2)))))
        | (value << (8 * (addr & 2)));
}

uint32_t Memory::read32(uint32_t addr) const {
    uint32_t res = 0;
    const auto it = mem_map_.find(addr);
    if (it != mem_map_.end()) {
        res = it->second;
    }

    journal_.push_back(std::make_tuple(Memory::Op::READ32, addr, res));

    return res;
}

// NOTE: This does not allow unaligned reads
uint16_t Memory::read16(uint32_t addr) const {
    const auto lookup_addr = addr & (~3);
    const auto it = mem_map_.find(lookup_addr);
    uint16_t res = 0;
    if (it != mem_map_.end()) {
        res = it->second >> ((addr & 2) ? 16 : 0);
    }

    journal_.push_back(std::make_tuple(Memory::Op::READ16, addr, res));

    return res;
}

uint8_t Memory::read8(uint32_t addr) const {
    const auto lookup_addr = addr & (~3);
    const auto it = mem_map_.find(lookup_addr);
    uint8_t res = 0;
    if (it != mem_map_.end()) {
        res = it->second >> (8 * (addr & 3));
    }

    journal_.push_back(std::make_tuple(Memory::Op::READ8, addr, res));

    return res;
}

void Memory::set_value_at(uint32_t addr, uint32_t value) {
    mem_map_[addr] = value;
}

uint32_t Memory::get_value_at(uint32_t addr) {
    return mem_map_[addr];
}

const Memory::JournalT& Memory::get_journal() const {
    return journal_;
}

void Memory::reset() {
    mem_map_.clear();
    journal_.clear();
}

Memory& get_global_memory() {
    return g_memory;
}

}  // namespace mock