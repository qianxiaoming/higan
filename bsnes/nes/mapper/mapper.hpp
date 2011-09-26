namespace Mapper {
  struct Mapper {
    virtual void main();
    virtual void tick();

    unsigned mirror(unsigned addr, unsigned size) const;
    uint8& prg_data(unsigned addr);
    uint8& chr_data(unsigned addr);

    virtual uint8 prg_read(uint16 addr) = 0;
    virtual void prg_write(uint16 addr, uint8 data) = 0;

    virtual uint8 chr_read(uint16 addr) = 0;
    virtual void chr_write(uint16 addr, uint8 data) = 0;

    virtual uint8 ciram_read(uint13 addr) = 0;
    virtual void ciram_write(uint13 addr, uint8 data) = 0;

    virtual unsigned ram_size();
    virtual uint8* ram_data();

    virtual void power() = 0;
    virtual void reset() = 0;

    virtual void serialize(serializer&) = 0;
  };

  #include "none/none.hpp"
  #include "aorom/aorom.hpp"
  #include "bandai-fcg/bandai-fcg.hpp"
  #include "cnrom/cnrom.hpp"
  #include "mmc1/mmc1.hpp"
  #include "mmc3/mmc3.hpp"
  #include "uorom/uorom.hpp"
  #include "vrc6/vrc6.hpp"
}