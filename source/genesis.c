/***************************************************************************************
 *  Genesis Plus
 *  Genesis internals & Bus controller
 *
 *  Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003  Charles Mac Donald (original code)
 *  Eke-Eke (2007,2008,2009), additional code & fixes for the GCN/Wii port
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************************/

#include "shared.h"

uint8 bios_rom[0x10000];  /* OS ROM   */
uint8 work_ram[0x10000];  /* 68K RAM  */
uint8 zram[0x2000];       /* Z80 RAM  */
uint32 zirq;              /* /IRQ to Z80 */
uint32 zstate;            /* Z80 bus state (d0 = BUSACK, d1 = /RESET) */
uint32 zbank;             /* Z80 bank window address */
uint32 gen_running;       /* 0: cpu are in locked state */
int32 resetline;          /* soft reset is triggered on a random line (X-Men 2, Eternal Champions) */

/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown functions                                          */
/*--------------------------------------------------------------------------*/
void set_softreset(void)
{
  resetline = (int) ((double) (lines_per_frame - 1) * rand() / (RAND_MAX + 1.0));
}

void gen_init(void)
{
  int i;

  /* initialize CPUs */
  m68k_set_cpu_type(M68K_CPU_TYPE_68000);
  m68k_init();
  z80_init(0,0,0,z80_irq_callback);

  /* initialize 68k mapped memory */
  /* $000000-$7fffff is affected to cartridge area (see cart_hw.c) */
  /* $800000-$ffffff is affected to WRAM (see VDP DMA) */
  for (i=0x80; i<0x100; i++)
  {
    m68k_memory_map[i].base     = work_ram;
    m68k_memory_map[i].read8    = NULL;
    m68k_memory_map[i].read16   = NULL;
    m68k_memory_map[i].write8   = NULL;
    m68k_memory_map[i].write16  = NULL;
    zbank_memory_map[i].read    = NULL;
    zbank_memory_map[i].write   = NULL;
  }

  /* initialize 68k memory handlers */
  for (i=0x80; i<0xe0; i++)
  {
    /* illegal area */
    m68k_memory_map[i].read8    = m68k_lockup_r_8;
    m68k_memory_map[i].read16   = m68k_lockup_r_16;
    m68k_memory_map[i].write8   = m68k_lockup_w_8;
    m68k_memory_map[i].write16  = m68k_lockup_w_16;
    zbank_memory_map[i].read    = zbank_lockup_r;
    zbank_memory_map[i].write   = zbank_lockup_w;
  }

  /* Z80 bus */
  m68k_memory_map[0xa0].read8   = z80_read_byte;
  m68k_memory_map[0xa0].read16  = z80_read_word;
  m68k_memory_map[0xa0].write8  = z80_write_byte;
  m68k_memory_map[0xa0].write16 = z80_write_word;
  zbank_memory_map[0xa0].read   = zbank_lockup_r;
  zbank_memory_map[0xa0].write  = zbank_lockup_w;

  /* I/O & Control registers */
  m68k_memory_map[0xa1].read8   = ctrl_io_read_byte;
  m68k_memory_map[0xa1].read16  = ctrl_io_read_word;
  m68k_memory_map[0xa1].write8  = ctrl_io_write_byte;
  m68k_memory_map[0xa1].write16 = ctrl_io_write_word;
  zbank_memory_map[0xa1].read   = zbank_read_ctrl_io;
  zbank_memory_map[0xa1].write  = zbank_write_ctrl_io;

  /* SEGA PICO */
  if (system_hw == SYSTEM_PICO)
  {
    m68k_memory_map[0x80].read8    = pico_read_byte;
    m68k_memory_map[0x80].read16   = pico_read_word;
    m68k_memory_map[0x80].write8   = m68k_unused_8_w;
    m68k_memory_map[0x80].write16  = m68k_unused_16_w;

    /* there is no I/O area (Notaz) */
    m68k_memory_map[0xa0].read8   = m68k_read_bus_8;
    m68k_memory_map[0xa0].read16  = m68k_read_bus_16;
    m68k_memory_map[0xa0].write8  = m68k_unused_8_w;
    m68k_memory_map[0xa0].write16 = m68k_unused_16_w;
    m68k_memory_map[0xa1].read8   = m68k_read_bus_8;
    m68k_memory_map[0xa1].read16  = m68k_read_bus_16;
    m68k_memory_map[0xa1].write8  = m68k_unused_8_w;
    m68k_memory_map[0xa1].write16 = m68k_unused_16_w;
  }

  /* VDP */
  for (i=0xc0; i<0xe0; i+=8)
  {
    m68k_memory_map[i].read8   = vdp_read_byte;
    m68k_memory_map[i].read16  = vdp_read_word;
    m68k_memory_map[i].write8  = vdp_write_byte;
    m68k_memory_map[i].write16 = vdp_write_word;
    zbank_memory_map[i].read   = zbank_read_vdp;
    zbank_memory_map[i].write  = zbank_write_vdp;
  }
}

void gen_reset(uint32 hard_reset)
{
  if (hard_reset)
  {
    /* Clear RAM */
    memset (work_ram, 0x00, sizeof (work_ram));
    memset (zram, 0x00, sizeof (zram));

    /* TMSS BIOS support */
    if (config.bios_enabled == 3)
      m68k_memory_map[0].base = bios_rom;

    /* Reset CPU cycle counts */
    mcycles_68k = 0;
    mcycles_z80 = 0;
  }

  zstate  = 0;  /* Z80 is reset & has control of the bus */
  zirq    = 0;  /* No interrupts occuring */
  zbank   = 0;  /* Assume default bank is $000000-$007FFF */

  /* Reset CPUs */
  resetline = -1;
  gen_running = 1; 
  m68k_pulse_reset();
  z80_reset();
}

void gen_shutdown(void)
{
  z80_exit();
}

/*-----------------------------------------------------------------------
  Bus controller chip functions                                            
  -----------------------------------------------------------------------*/
void gen_busreq_w(uint32 state)
{
  if (state)
  {
    /* Bus requested */
    if (zstate == 1)
    {
      /* Z80 is stopped */
      /* Z80 was ON during the last 68k cycles */
      z80_run(mcycles_68k);
    }

    /* update Z80 bus status */
    zstate |= 2;
  }
  else
  {
    /* Bus released */
    if (zstate == 3)
    {
      /* Z80 is restarted */
      /* Z80 was OFF during the last 68k cycles */
      mcycles_z80 = mcycles_68k;
    }

    /* update Z80 bus status */
    zstate &= 1;
  }
}

void gen_reset_w(uint32 state)
{
  if (state)
  {
    /* stop RESET process */
    if (!zstate)
    {
      /* Z80 is restarted */
      /* Z80 was OFF during the last cycles */
      mcycles_z80 = mcycles_68k;
    }

    /* update Z80 bus status */
    zstate |= 1;
  }
  else
  {
    /* start RESET process */
    if (zstate == 1)
    {
      /* Z80 stopped */
      /* z80 was ON during the last 68k cycles */
      z80_run(mcycles_68k);
    }

    /* Reset Z80 & YM2612 */
    z80_reset();
    fm_reset();

    /* update Z80 bus status */
    zstate &= 2;
  }
}

void gen_bank_w (uint32 state)
{
  zbank = ((zbank >> 1) | ((state & 1) << 23)) & 0xFF8000;
}

int z80_irq_callback (int param)
{
  zirq = 0;
  z80_set_irq_line (0, CLEAR_LINE);
  return 0xFF;
}
