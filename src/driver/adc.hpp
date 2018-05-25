#pragma once

#include "driver/peripheral.hpp"


namespace driver {

class ADC : virtual public Peripheral {
    public:
        enum class ID {
            ADC0,
            ADC1,
            ADC2,
            ADC3,
            ADC4,
            ADC5,
        };

        ADC() {}
        ADC(uint32_t base, unsigned int irq_n) : Peripheral(base, irq_n) {}

        virtual int request() { return -1; }
        virtual int start(int max_samples) { (void)max_samples; return 0; }
        virtual void stop() {}
        virtual unsigned get_num_channels() const { return 0; }
        virtual uint32_t get_result(unsigned channel, unsigned sample) {
            (void)channel;
            (void)sample;
            return 0;
        }

        static ADC* request_by_id(ID id);
};

}  // namespace
