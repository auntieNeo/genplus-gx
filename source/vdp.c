/***************************************************************************************
 *  Genesis Plus
 *  Video Display Processor (memory handlers)
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
#include "hvc.h"

/* Pack and unpack CRAM data */
#define PACK_CRAM(d)    ((((d)&0xE00)>>9)|(((d)&0x0E0)>>2)|(((d)&0x00E)<<5))
#define UNPACK_CRAM(d)  ((((d)&0x1C0)>>5)|((d)&0x038)<<2|(((d)&0x007)<<9))

/* Mark a pattern as dirty */
#define MARK_BG_DIRTY(addr)                                           \
{                                                                     \
  name = (addr >> 5) & 0x7FF;                                         \
  if(bg_name_dirty[name] == 0) bg_name_list[bg_list_index++] = name;  \
  bg_name_dirty[name] |= (1 << ((addr >> 2) & 0x07));                 \
}

/* VDP context */
uint8 sat[0x400];     /* Internal copy of sprite attribute table */
uint8 vram[0x10000];  /* Video RAM (64Kx8) */
uint8 cram[0x80];     /* On-chip color RAM (64x9) */
uint8 vsram[0x80];    /* On-chip vertical scroll RAM (40x11) */
uint8 reg[0x20];      /* Internal VDP registers (23x8) */
uint8 hint_pending;   /* 0= Line interrupt is pending */
uint8 vint_pending;   /* 1= Frame interrupt is pending */
uint8 irq_status;     /* 68K IRQ status */
uint16 status;        /* VDP status flags */
uint32 dma_length;    /* DMA remaining length */
 
/* Global variables */
uint16 ntab;                      /* Name table A base address */
uint16 ntbb;                      /* Name table B base address */
uint16 ntwb;                      /* Name table W base address */
uint16 satb;                      /* Sprite attribute table base address */
uint16 hscb;                      /* Horizontal scroll table base address */

uint8 bg_name_dirty[0x800];       /* 1= This pattern is dirty */
uint16 bg_name_list[0x800];       /* List of modified pattern indices */
uint16 bg_list_index;             /* # of modified patterns in list */
uint8 bg_pattern_cache[0x80000];  /* Cached and flipped patterns */
uint8 playfield_shift;            /* Width of planes A, B (in bits) */
uint8 playfield_col_mask;         /* Vertical scroll mask */
uint16 playfield_row_mask;        /* Horizontal scroll mask */
uint8 odd_frame;                  /* 1: odd field, 0: even field */
uint8 im2_flag;                   /* 1= Interlace mode 2 is being used */
uint8 interlaced;                 /* 1: Interlaced mode 1 or 2 */
uint8 vdp_pal;                    /* 1: PAL , 0: NTSC (default) */
uint16 v_counter;                 /* VDP scanline counter */
uint16 lines_per_frame;           /* PAL: 313 lines, NTSC: 262 lines */
int32 fifo_write_cnt;             /* VDP writes fifo count */
uint32 fifo_lastwrite;            /* last VDP write cycle */
uint32 hvc_latch;                 /* latched HVCounter (INT2) */
uint32 vc_max;                    /* Vertical Counter max value */


/* Tables that define the playfield layout */
static const uint8 shift_table[]      = { 6, 7, 0, 8 }; /* fixes Window Bug test program */
static const uint8 col_mask_table[]   = { 0x0F, 0x1F, 0x0F, 0x3F };
static const uint16 row_mask_table[]  = { 0x0FF, 0x1FF, 0x2FF, 0x3FF };

static uint8 border;          /* Border color index */
static uint8 dma_type;        /* DMA mode */
static uint8 dmafill;         /* next VDP Write is DMA Fill */
static uint8 pending;         /* Pending write flag */
static uint8 code;            /* Code register */
static uint16 addr;           /* Address register */
static uint16 addr_latch;     /* Latched A15, A14 of address */
static uint16 fill_data;      /* VRAM Fill data */
static uint16 sat_base_mask;  /* Base bits of SAT */
static uint16 sat_addr_mask;  /* Index bits of SAT */
static uint32 dma_endCycles;  /* 68k cycles to DMA end */
static uint32 fifo_latency;   /* CPU access latency */
static int cached_write;      /* 2nd part of 32-bit CTRL port write */

/* DMA Timings

 According to the manual, here's a table that describes the transfer
 rates of each of the three DMA types:

    DMA Mode      Width       Display      Transfer Count
    -----------------------------------------------------
    68K > VDP     32-cell     Active       16
                              Blanking     167
                  40-cell     Active       18
                              Blanking     205
    VRAM Fill     32-cell     Active       15
                              Blanking     166
                  40-cell     Active       17
                              Blanking     204
    VRAM Copy     32-cell     Active       8
                              Blanking     83
                  40-cell     Active       9
                              Blanking     102

 'Active' is the active display period, 'Blanking' is either the vertical
 blanking period or when the display is forcibly blanked via register #1.

 The above transfer counts are all in bytes, unless the destination is
 CRAM or VSRAM for a 68K > VDP transfer, in which case it is in words.

*/
static const uint8 dma_rates[16] = {
  8,   83,  9, 102, /* 68K to VRAM (1 word = 2 bytes) */
  16, 167, 18, 205, /* 68K to CRAM or VSRAM */
  16, 167, 18, 205, /* DMA fill (same as above, dma length is adjusted to take first write in account)*/
  8,   83,  9, 102, /* DMA Copy */
};

/*--------------------------------------------------------------------------*/
/* Functions prototype                                                      */
/*--------------------------------------------------------------------------*/
static void fifo_update(unsigned int cycles);
static void data_w(unsigned int data);
static void reg_w(unsigned int r, unsigned int d);
static void dma_copy(int length);
static void dma_vbus (int length);
static void dma_fill(int length, unsigned int data);

/*--------------------------------------------------------------------------*/
/* Init, reset, shutdown functions                                          */
/*--------------------------------------------------------------------------*/
void vdp_init(void)
{
  /* PAL/NTSC timings */
  lines_per_frame = vdp_pal ? 313: 262;
  status = (status & ~1) | vdp_pal;
}

void vdp_reset(void)
{
  memset ((char *) sat, 0, sizeof (sat));
  memset ((char *) vram, 0, sizeof (vram));
  memset ((char *) cram, 0, sizeof (cram));
  memset ((char *) vsram, 0, sizeof (vsram));
  memset ((char *) reg, 0, sizeof (reg));

  addr            = 0;
  addr_latch      = 0;
  code            = 0;
  pending         = 0;
  hint_pending    = 0;
  vint_pending    = 0;
  irq_status      = 0;
  hvc_latch       = 0;
  dmafill         = 0;
  fill_data       = 0;
  dma_length      = 0;
  dma_endCycles   = 0;
  dma_type        = 0;
  odd_frame       = 0;
  im2_flag        = 0;
  interlaced      = 0;
  fifo_write_cnt  = 0;
  fifo_lastwrite  = 0;
  fifo_latency    = 190;
  cached_write    = -1;

  vc_max = 0xEA + 24*vdp_pal;
  v_counter = lines_per_frame - 1;

  status = vdp_pal | 0x200;  /* FIFO empty flag */

  ntab = 0;
  ntbb = 0;
  ntwb = 0;
  satb = 0;
  hscb = 0;

  sat_base_mask       = 0xFE00;
  sat_addr_mask       = 0x01FF;
  playfield_shift     = 6;
  playfield_col_mask  = 0x0F;
  playfield_row_mask  = 0x0FF;

  bg_list_index = 0;
  memset ((char *) bg_name_dirty, 0, sizeof (bg_name_dirty));
  memset ((char *) bg_name_list, 0, sizeof (bg_name_list));
  memset ((char *) bg_pattern_cache, 0, sizeof (bg_pattern_cache));

  /* default HVC tables */
  hctab = cycle2hc32;

  /* default display area */
  bitmap.viewport.w   = 256;
  bitmap.viewport.h   = 224;
  bitmap.viewport.ow  = 256;
  bitmap.viewport.oh  = 224;

  /* default overscan area */
  bitmap.viewport.x = (config.overscan & 2) * 7;
  bitmap.viewport.y = (config.overscan & 1) * (8 + 24*vdp_pal);

  /* initialize registers if OS ROM is not used */
  if (config.tmss == 1)
  {
    reg_w(0 , 0x04);  /* Palette bit set */
    reg_w(1 , 0x04);  /* Mode 5 enabled */
    reg_w(10, 0xff);  /* HINT disabled */
    reg_w(12, 0x81);  /* H40 mode */
    reg_w(15, 0x02);  /* auto increment */
  }
}

void vdp_shutdown(void)
{}

int vdp_context_save(uint8 *state)
{
  int bufferptr = 0;

  save_param(sat, sizeof(sat));
  save_param(vram, sizeof(vram));
  save_param(cram, sizeof(cram));
  save_param(vsram, sizeof(vsram));
  save_param(reg, sizeof(reg));
  save_param(&addr, sizeof(addr));
  save_param(&addr_latch, sizeof(addr_latch));
  save_param(&code, sizeof(code));
  save_param(&pending, sizeof(pending));
  save_param(&status, sizeof(status));
  save_param(&dmafill, sizeof(dmafill));
  save_param(&hint_pending, sizeof(hint_pending));
  save_param(&vint_pending, sizeof(vint_pending));
  save_param(&irq_status, sizeof(irq_status));

  /* extended state */
  save_param(&dma_length, sizeof(dma_length));
  save_param(&dma_type, sizeof(dma_type));
  save_param(&fill_data, sizeof(fill_data));
  save_param(&cached_write, sizeof(cached_write));

  return bufferptr;
}

int vdp_context_load(uint8 *state, char *version)
{
  int i, bufferptr = 0;
  uint8 temp_reg[0x20];

  load_param(sat, sizeof(sat));
  load_param(vram, sizeof(vram));
  load_param(cram, sizeof(cram));
  load_param(vsram, sizeof(vsram));
  load_param(temp_reg, sizeof(temp_reg));
  load_param(&addr, sizeof(addr));
  load_param(&addr_latch, sizeof(addr_latch));
  load_param(&code, sizeof(code));
  load_param(&pending, sizeof(pending));
  load_param(&status, sizeof(status));
  load_param(&dmafill, sizeof(dmafill));
  load_param(&hint_pending, sizeof(hint_pending));
  load_param(&vint_pending, sizeof(vint_pending));
  load_param(&irq_status, sizeof(irq_status));

  /* extended state */
  if (version[15] > 0x30)
  {
    load_param(&dma_length, sizeof(dma_length));
    load_param(&dma_type, sizeof(dma_type));
    load_param(&fill_data, sizeof(fill_data));
    load_param(&cached_write, sizeof(cached_write));
  }

  /* restore VDP registers */
  for (i=0;i<0x20;i++) 
  {
    reg_w(i, temp_reg[i]);
  }

  /* reinitialize HVC tables */
  hctab = (reg[12] & 1) ? cycle2hc40 : cycle2hc32;
  vc_max = 0xEA + 24*vdp_pal;
  if (reg[1] & 8)
  {
    vc_max += (28 - 20*vdp_pal);
  }

  /* keep current region mode */
  status = (status & ~1) | vdp_pal;

  /* restore FIFO timings */
  fifo_latency = (reg[12] & 1) ? 190 : 214;
  if ((code & 0x0F) == 0x01) 
    fifo_latency = fifo_latency * 2;

  /* remake cache */
  for (i=0;i<0x800;i++) 
  {
    bg_name_list[i]=i;
    bg_name_dirty[i]=0xFF;
  }
  bg_list_index=0x800;

  /* reinitialize palette */
  color_update(0x00, *(uint16 *)&cram[border << 1]);
  for(i = 1; i < 0x40; i += 1)
    color_update(i, *(uint16 *)&cram[i << 1]);

  return bufferptr;
}


/*--------------------------------------------------------------------------*/
/* DMA update                                                               */
/*--------------------------------------------------------------------------*/

void vdp_update_dma(unsigned int cycles)
{
  int dma_cycles;

  /* DMA transfer rate (bytes per line) */
  unsigned int rate = dma_type;
  if ((status & 8) || !(reg[1] & 0x40)) rate++;
  if (reg[12] & 1) rate += 2;
  rate = dma_rates[rate];

  /* Remaining DMA cycles */
  if (status & 8)
  {
    /* Process DMA until the end of VBLANK */
    dma_cycles = (lines_per_frame * MCYCLES_PER_LINE) - cycles;
  }
  else
  {
    /* Process DMA until the end of current line */
    dma_cycles = (mcycles_vdp + MCYCLES_PER_LINE) - cycles;
  }

  /* Remaining DMA bytes */
  int dma_bytes = (dma_cycles * rate) / MCYCLES_PER_LINE;

#ifdef LOGVDP
  error("[%d(%d)][%d(%d)] DMA type %d (%d access/line)(%d cycles left)-> %d access (%d remaining) (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE,dma_type/4, rate, dma_cycles, dma_bytes, dma_length, m68k_get_reg (NULL, M68K_REG_PC));
#endif

  /* Check if DMA will be finished during current line */
  if (dma_length < dma_bytes)
  {
    /* Adjust remaining DMA */
    dma_bytes = dma_length;
    dma_cycles = (dma_bytes * MCYCLES_PER_LINE) / rate;
  }

  /* Update DMA timings */
  if (dma_type < 8)
  {
    /* 68K is frozen during DMA operation */
    mcycles_68k = cycles + dma_cycles;
  #ifdef LOGVDP
    error("-->CPU frozen for %d cycles\n", dma_cycles);
  #endif
  }
  else
  {
    /* Set DMA Busy flag */
    status |= 0x0002;

    /* 68K is still running, set DMA end cycle */
    dma_endCycles = cycles + dma_cycles;
#ifdef LOGVDP
    error("-->DMA ends in %d cycles\n", dma_cycles);
#endif
  }

  /* Process DMA */
  if (dma_bytes > 0)
  {
    /* Update DMA length */
    dma_length -= dma_bytes;

    /* Select DMA operation */
    switch (dma_type >> 2)
    {
      case 0:
      case 1:
      {
        /* 68K to VRAM, CRAM or VSRAM */
        dma_vbus(dma_bytes);
        break;
      }

      case 2:
      {
        /* VRAM Fill */
        if (dmafill)
        {
          /* process intial write */
          dmafill = 0;
          data_w(fill_data);

          /* adjust remaining DMA length */
          dma_bytes--;

          /* fill MSB */
          fill_data = (fill_data >> 8) & 0xff;

          /* check remaining DMA length */
          if (!dma_bytes) break;
        }

        dma_fill(dma_bytes, fill_data);
        break;
      }

      case 3:
      {
        /* VRAM Copy */
        dma_copy(dma_bytes);
        break;
      }
    }

    /* Check if DMA is finished */
    if (!dma_length)
    {
      /* Reset DMA length registers */
      reg[19] = reg[20] = 0;

      /* Perform cached write, if any */
      if (cached_write >= 0)
      {
        vdp_ctrl_w(cached_write);
        cached_write = -1;
      }
    }
  }
}

/*--------------------------------------------------------------------------*/
/* VDP Ports handler                                                        */
/*--------------------------------------------------------------------------*/
void vdp_ctrl_w(unsigned int data)
{
  if (pending == 0)
  {
    /* check that DMA operation was not first triggered with the same instruction */
    if (dma_length)
    {
      /* 2nd part of 32-bit write should be done after DMA completion (Formula One, Kawasaki Superbike Challenge) */
      cached_write = data;
      return;
    }

    if ((data & 0xC000) == 0x8000)
    {
      /* VDP register write */
      reg_w((data >> 8) & 0x1F,data & 0xFF);
    }
    else
    {
      /* Set pending flag */
      pending = 1;
    }

    addr = addr_latch | (data & 0x3FFF);
    code = ((code & 0x3C) | ((data >> 14) & 0x03));
  }
  else
  {
    /* Clear pending flag */
    pending = 0;

    /* Save address bits A15 and A14 */
    addr_latch = (data & 3) << 14;

    /* Update address and code registers */
    addr = addr_latch | (addr & 0x3FFF);
    code = ((code & 0x03) | ((data >> 2) & 0x3C));

    /* DMA operation */
    if ((code & 0x20) && (reg[1] & 0x10))
    {
      switch (reg[23] >> 6)
      {
        case 2:
        {
          /* VRAM write operation only (Williams Greatest Hits after soft reset) */
          if ((code & 0x0F) == 1)
          {
            /* VRAM fill is triggered next time DATA port is written */
            dmafill = 1;
          }
          break;
        }

        case 3:
        {
          /* VRAM read/write operation only */
          if ((code & 0x1F) == 0x10)
          {
            /* retrieve DMA length */
            dma_length = (reg[20] << 8 | reg[19]) & 0xFFFF;
            if (!dma_length) dma_length = 0x10000;

            /* VRAM copy */
            dma_type = 12;
            vdp_update_dma(mcycles_68k);
          }
          break;
        }

        default:
        {
          /* retrieve DMA length */
          dma_length = (reg[20] << 8 | reg[19]) & 0xFFFF;
          if (!dma_length) dma_length = 0x10000;

          /* SVP transfer latency */
          if (svp && !(reg[23] & 0x60)) reg[21]--;

          /* 68k to VDP DMA */
          dma_type = (code & 0x06) ? 4 : 0;
          vdp_update_dma(mcycles_68k);
          break;
        }
      }
    }
  }

  /* 
     FIFO emulation (Chaos Engine/Soldier of Fortune, Double Clutch, Sol Deace) 
     --------------------------------------------------------------------------

      CPU access per line is limited during active display:
         H32: 16 access --> 3420/16 = ~214 Mcycles between access
         H40: 18 access --> 3420/18 = ~190 Mcycles between access

      This is an approximation, on real hardware, the delay between access is
	  more likely 16 pixels (128 or 160 Mcycles) with no access allowed during
	  HBLANK (~860 Mcycles), H40 mode being probably a little more restricted.

      Each VRAM access is byte wide, so one VRAM write (word) need twice cycles.

  */
  fifo_latency = (reg[12] & 1) ? 190 : 214;
  if ((code & 0x0F) == 0x01)
    fifo_latency = fifo_latency * 2;
}

/*
 * Return VDP status
 *
 * Bits are
 * 0  0:1 ntsc:pal
 * 1  DMA Busy
 * 2  During HBlank
 * 3  During VBlank
 * 4  0:1 even:odd field (interlaced modes)
 * 5  Sprite collision
 * 6  Too many sprites per line
 * 7  v interrupt occurred
 * 8  Write FIFO full
 * 9  Write FIFO empty
 * 10 - 15  Open Bus
 */
unsigned int vdp_ctrl_r(unsigned int cycles)
{
  /* update FIFO flags */
  fifo_update(cycles);

  /* update DMA Busy flag */
  if ((status & 2) && !dma_length && (cycles >= dma_endCycles))
    status &= 0xFFFD;

  unsigned int temp = status;

  /* display OFF: VBLANK flag is set */
  if (!(reg[1] & 0x40))
    temp |= 0x08; 

  /* HBLANK flag (Sonic 3 and Sonic 2 "VS Modes", Lemmings 2, Mega Turrican, V.R Troopers, Gouketsuji Ichizoku, ...) */
  if ((cycles % MCYCLES_PER_LINE) < 588) temp |= 0x04;

  /* clear pending flag */
  pending = 0;

  /* clear SPR/SCOL flags */
  status &= 0xFF9F;

#ifdef LOGVDP
  error("[%d(%d)][%d(%d)] VDP status read -> 0x%x (%x)\n", v_counter, cycles/MCYCLES_PER_LINE, cycles, cycles%MCYCLES_PER_LINE, temp, m68k_get_reg (NULL, M68K_REG_PC));
#endif
  return (temp);
}

unsigned int vdp_hvc_r(unsigned int cycles)
{
  /* HVC is frozen (Lightgun games, Sunset Riders) */
  if (hvc_latch) return (hvc_latch & 0xffff);

  /* Horizontal Counter (Striker, Mickey Mania, Skitchin, Road Rash I,II,III, Sonic 3D Blast...) */
  uint8 hc = hctab[cycles%MCYCLES_PER_LINE];

  /* Vertical Counter */
  int16 vc = v_counter;

  /* Check counter overflow */
  if (vc > vc_max) vc -= lines_per_frame;

  /* Interlaced Modes */
  if (interlaced)
  {
    /* interlace mode 2 (Sonic the Hedgehog 2, Combat Cars) */  
    if (im2_flag) vc = vc << 1;

    /* replace bit 0 with bit 8 */
    vc = (vc & ~1) | ((vc >> 8) & 1);
  }

#ifdef LOGVDP
  error("[%d(%d)][%d(%d)] VDP HVC Read -> 0x%04x (%x)\n", v_counter, cycles/MCYCLES_PER_LINE, cycles, cycles%MCYCLES_PER_LINE,(vc << 8) | hc, m68k_get_reg (NULL, M68K_REG_PC));
#endif
  return (((vc & 0xff) << 8) | hc);
}

void vdp_test_w(unsigned int data)
{
#ifdef LOGERROR
  error("Unused VDP Write 0x%x (%08x)\n", data, m68k_get_reg (NULL, M68K_REG_PC));
#endif
}

void vdp_data_w(unsigned int data)
{
  /* Clear pending flag */
  pending = 0;

  /* DMA Fill */
  if (dmafill)
  {
    /* save fill data */
    fill_data = data;

    /* retrieve DMA length */
    dma_length = (reg[20] << 8 | reg[19]) & 0xFFFF;
    if (!dma_length) dma_length = 0x10000;

    /* adjust DMA length (first write counts for timings) */
    dma_length++;

    /* perform DMA */
    dma_type = 8;
    vdp_update_dma(mcycles_68k);
    return;
  }

  /* restricted VDP writes during active display */
  if (!(status & 8) && (reg[1] & 0x40))
  {
    /* update VDP FIFO */
    fifo_update(mcycles_68k);

    /* increase FIFO word count */
    fifo_write_cnt ++;

    /* FIFO is not empty anymore */
    status &= 0xFDFF;

    /* FIFO full ? */
    if (fifo_write_cnt >= 4)
    {
      /* update VDP status flag */
      status |= 0x100; 

      /* CPU is delayed (Chaos Engine, Soldiers of Fortune, Double Clutch) */
      if (fifo_write_cnt > 4)
        mcycles_68k = fifo_lastwrite + fifo_latency;
    }
  }

  /* write data */
  data_w(data);
}

unsigned int vdp_data_r(void)
{
  uint16 temp = 0;

  /* Clear pending flag */
  pending = 0;

  switch (code & 0x0F)
  {
    case 0x00:  /* VRAM */
      temp = *(uint16 *) &vram[(addr & 0xFFFE)];
#ifdef LOGVDP
      error("[%d(%d)][%d(%d)] VRAM 0x%x read -> 0x%x (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, addr, temp, m68k_get_reg (NULL, M68K_REG_PC));
#endif
      break;

    case 0x08:  /* CRAM */
      temp = *(uint16 *) &cram[(addr & 0x7E)];
      temp = UNPACK_CRAM (temp);
#ifdef LOGVDP
      error("[%d(%d)][%d(%d)] CRAM 0x%x read -> 0x%x (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, addr, temp, m68k_get_reg (NULL, M68K_REG_PC));
#endif
      break;

    case 0x04:  /* VSRAM */
      temp = *(uint16 *) &vsram[(addr & 0x7E)];
#ifdef LOGVDP
      error("[%d(%d)][%d(%d)] VSRAM 0x%x read -> 0x%x (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, addr, temp, m68k_get_reg (NULL, M68K_REG_PC));
#endif
      break;
	  
#ifdef LOGVDP
    default:
	  error("[%d(%d)][%d(%d)] Unknown (%d) 0x%x read (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, code, addr, m68k_get_reg (NULL, M68K_REG_PC));
      break;
#endif
  }

  /* Increment address register */
  addr += reg[15];

  /* return data */
  return (temp);
}

/*--------------------------------------------------------------------------*/
/* VDP Interrupts callback                                                  */
/*--------------------------------------------------------------------------*/

int vdp_int_ack_callback(int int_level)
{
#ifdef LOGVDP
  error("[%d(%d)][%d(%d)] INT Level %d ack (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE,int_level, m68k_get_reg (NULL, M68K_REG_PC));
#endif

  /* VINT triggered ? */
  if ((irq_status & 6) == 6)
  {
    vint_pending = 0;
    status &= ~0x80;  /* clear VINT flag */
#ifdef LOGVDP
    error("---> VINT cleared\n");
#endif
    /* update IRQ status */
    if (hint_pending & reg[0])
    {
      irq_status = 0x14;
    }
    else
    {
      irq_status = 0x00;
      m68k_set_irq(0);
    }
  }
  else
  {
    hint_pending = 0;
#ifdef LOGVDP
    error("---> HINT cleared\n");
#endif
    /* update IRQ status */
    if (vint_pending & reg[1])
    {
      irq_status = 0x16;
    }
    else
    {
      irq_status = 0x00;
      m68k_set_irq(0);
    }
  }

  return M68K_INT_ACK_AUTOVECTOR;
}

/*--------------------------------------------------------------------------*/
/* FIFO emulation                                                  */
/*--------------------------------------------------------------------------*/
static void fifo_update(unsigned int cycles)
{
  if (fifo_write_cnt > 0)
  {
    /* update FIFO reads */
    int fifo_read = ((cycles - fifo_lastwrite) / fifo_latency);
    if (fifo_read > 0)
    {
      fifo_write_cnt -= fifo_read;

      /* update VDP status flags */
      if (fifo_write_cnt < 4)
      {
        /* FIFO is not full anymore */
        status &= 0xFEFF;

        if (fifo_write_cnt <= 0)
        {
          /* FIFO is empty */
          status |= 0x200; 
          fifo_write_cnt = 0;
        }
      }

      /* update cycle count */
      fifo_lastwrite += (fifo_read * fifo_latency);
    }
  }
}

/*--------------------------------------------------------------------------*/
/* Memory access functions                                                  */
/*--------------------------------------------------------------------------*/
static void data_w(unsigned int data)
{
  switch (code & 0x0F)
  {
    case 0x01:  /* VRAM */
#ifdef LOGVDP
      error("[%d(%d)][%d(%d)] VRAM 0x%x write -> 0x%x (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, addr, data, m68k_get_reg (NULL, M68K_REG_PC));
#endif
      /* Byte-swap data if A0 is set */
      if (addr & 1)
        data = ((data >> 8) | (data << 8)) & 0xFFFF;

      /* Copy SAT data to the internal SAT */
      if ((addr & sat_base_mask) == satb)
      {
        *(uint16 *) &sat[addr & sat_addr_mask & 0xFFFE] = data;
      }

      /* Only write unique data to VRAM */
      if (data != *(uint16 *) &vram[addr & 0xFFFE])
      {
        /* Write data to VRAM */
        *(uint16 *) &vram[addr & 0xFFFE] = data;

        /* Update the pattern cache */
        int name;
        MARK_BG_DIRTY (addr);
      }
      break;

    case 0x03:  /* CRAM */
    {
#ifdef LOGVDP
      error("[%d(%d)][%d(%d)] CRAM 0x%x write -> 0x%x (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, addr, data, m68k_get_reg (NULL, M68K_REG_PC));
#endif
      uint16 *p = (uint16 *) &cram[(addr & 0x7E)];
      data = PACK_CRAM (data);
      if (data != *p)
      {
        int index = (addr >> 1) & 0x3F;
        *p = data;

        /* update color palette */
        /* color entry 0 of each palette is never displayed (transparent pixel) */
        if (index & 0x0F)
          color_update(index, data);

        /* update background color */
        if (border == index)
          color_update (0x00, data);

        /* CRAM modified during HBLANK (Striker, Zero the Kamikaze, etc) */
        if ((v_counter < bitmap.viewport.h) && (reg[1]& 0x40) && (mcycles_68k <= (mcycles_vdp + 860)))
        {
          /* remap current line */
          remap_buffer(v_counter);
#ifdef LOGVDP
          error("Line remapped\n");
#endif
        }
#ifdef LOGVDP
        else error("Line NOT remapped\n");
#endif
      }
      break;
    }

    case 0x05:  /* VSRAM */
#ifdef LOGVDP
      error("[%d(%d)][%d(%d)] VSRAM 0x%x write -> 0x%x (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, addr, data, m68k_get_reg (NULL, M68K_REG_PC));
#endif
      *(uint16 *) &vsram[(addr & 0x7E)] = data;
      break;
	  
#ifdef LOGVDP
    default:
      error("[%d(%d)][%d(%d)] Unknown (%d) 0x%x write -> 0x%x (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, code, addr, data, m68k_get_reg (NULL, M68K_REG_PC));
      break;
#endif
  }

  /* Increment address register */
  addr += reg[15];
}

static void reg_w(unsigned int r, unsigned int d)
{
#ifdef LOGVDP
  error("[%d(%d)][%d(%d)] VDP register %d write -> 0x%x (%x)\n", v_counter, mcycles_68k/MCYCLES_PER_LINE, mcycles_68k, mcycles_68k%MCYCLES_PER_LINE, r, d, m68k_get_reg (NULL, M68K_REG_PC));
#endif

  /* Mode 4 (SMS mode) is enabled 
     According to official doc, VDP registers #11 to #23 can not be written unless bit2 in register #1 is set
     Fix Captain Planet & Avengers (Alt version), Bass Master Classic Pro Edition (they incidentally activate Mode 4) 
  */
  if (!(reg[1] & 4) && (r > 10))
    return;

  switch(r)
  {
    case 0: /* CTRL #1 */

      /* look for changed bits */
      r = d ^ reg[r];
      reg[0] = d;

      /* Line Interrupt */
      if ((r & 0x10) && hint_pending)
      {
        /* update IRQ status */
        irq_status = 0x30;
        if (vint_pending & reg[1])
          irq_status |= 6;
        else if (d & 0x10)
          irq_status |= 4;
      }

      /* Palette bit  */
      if (r & 0x04)
      {
        /* Update colors */
        int i;
        color_update (0x00, *(uint16 *) & cram[border << 1]);
        for (i = 1; i < 0x40; i += 1)
          color_update (i, *(uint16 *) & cram[i << 1]);
      }

      /* HVC latch bit */
      if (r & 0x02)
      {
        if (reg[0] & 2) /* latch current HVC */
          hvc_latch = 0x10000 | vdp_hvc_r(mcycles_68k);
        else            /* free-running HVC */
          hvc_latch = 0;
      }

      break;

    case 1: /* CTRL #2 */

      /* look for changed bits */
      r = d ^ reg[r];
      reg[1] = d;

      /* Frame Interrupt */
      if ((r & 0x20) && vint_pending)
      {
        /* update IRQ status */
        irq_status = 0x30;
        if (d & 0x20) 
          irq_status |= 6;
        else if (hint_pending & reg[0])
          irq_status |= 4;
      }

      /* See if the viewport height has actually been changed */
      if (r & 0x08)
      {
        if (v_counter < bitmap.viewport.h)
        {
          /* Update active display */
          bitmap.viewport.h = 224 + ((d & 8) << 1);
          bitmap.viewport.y = (config.overscan & 1) * ( 8 - (d & 8) + vdp_pal*24);
        }
        else
        {
          /* changes should be applied on next frame */
          bitmap.viewport.changed |= 2;
        }

        /* update vertical counter max value (see hvc.h) */
        vc_max = 0xEA + 24*vdp_pal;
        if (d & 8)
        {
          vc_max += (28 - 20*vdp_pal);
        }
      }

      /* Display status modified during active display (Legend of Galahad, Lemmings 2,  */
      /* Formula One Championship,  Nigel Mansell's World Championship Racing, ...)     */
      if ((r & 0x40) && (v_counter < bitmap.viewport.h))
      {
        int offset = mcycles_68k - mcycles_vdp - 860;
        if (offset <= 0)
        {
          /* redraw entire line */
          render_line(v_counter);

#ifdef LOGVDP
          error("Line redrawn (%d sprites) \n",object_count[object_which^1]);
#endif

          /* If display is was disabled during HBLANK (Mickey Mania 3D level), sprite processing is limited  */
          /* Below values have been deducted from testing on this game, accurate emulation would require  */
          /* to know exact sprite (pre)processing timings. Hopefully, they don't seem to break any other  */
          /* games, so they might not be so much inaccurate.                                              */
          if (d & 0x40)
          {
            parse_satb(0x81 + v_counter);
            if ((object_count[object_which^1] > 5) && (mcycles_68k % MCYCLES_PER_LINE >= 360))
              object_count[object_which^1] = 5;
          }
        }
        else
        {
          /* pixel offset */
          if (reg[12] & 1) offset = offset / 8;
          else offset = (offset / 10) + 16;

#ifdef LOGVDP
          error("Line %d redrawn from pixel %d\n",v_counter,offset);
#endif

          /* line is partially blanked */
          if (offset < bitmap.viewport.w)
          {
            if (d & 0x40)
            {
              render_line(v_counter);
              blank_line(v_counter, 0, offset);
            }
            else
            {
              blank_line(v_counter, offset, bitmap.viewport.w - offset);
            }
          }
        }
      } 
      break;

    case 2: /* NTAB */
      reg[2] = d;
      ntab = (d << 10) & 0xE000;
      break;

    case 3: /* NTWB */
      reg[3] = d;
      if (reg[12] & 1)
        ntwb = (d << 10) & 0xF000;
      else
        ntwb = (d << 10) & 0xF800;
      break;

    case 4: /* NTBB */
      reg[4] = d;
      ntbb = (d << 13) & 0xE000;
      break;

    case 5: /* SATB */
      reg[5] = d;
      satb = (d << 9) & sat_base_mask;
      break;

    case 7:
      reg[7] = d;
      /* See if the background color has actually changed */
      d &= 0x3F;
      if (d != border)
      {
        /* update background color */
        border = d;
        color_update(0x00, *(uint16 *)&cram[(d << 1)]);

        /* background color modified during Horizontal Blanking (Road Rash 1,2,3)*/
        if ((v_counter < bitmap.viewport.h) && (mcycles_68k <= (mcycles_vdp + 860)))
        {
          /* remap entire line */
          remap_buffer(v_counter);
#ifdef LOGVDP
          error("Line remapped\n");
#endif
        }
#ifdef LOGVDP
        else error("Line NOT remapped\n");
#endif
      }
      break;

    case 12:

      /* look for changed bits */
      r = d ^ reg[r];
      reg[12] = d;

      /* See if the viewport width has actually been changed */
      if (r & 0x01)
      {
        if (d & 1)
        {
          /* Update display-dependant registers */
          ntwb = (reg[3] << 10) & 0xF000;
          satb = (reg[5] << 9) & 0xFC00;
          sat_base_mask = 0xFC00;
          sat_addr_mask = 0x03FF;

          /* Update HC table */
          hctab = cycle2hc40;
            
          /* Update fifo timings */
          fifo_latency = 190;
        }
        else
        {
          /* Update display-dependant registers */
          ntwb = (reg[3] << 10) & 0xF800;
          satb = (reg[5] << 9) & 0xFE00;
          sat_base_mask = 0xFE00;
          sat_addr_mask = 0x01FF;

          /* Update HC table */
          hctab = cycle2hc32;

          /* Update FIFO timings */
          fifo_latency = 214;
        }

        /* Adjust VRAM timings */
        if ((code & 0x0F) == 0x01)
        {
          fifo_latency = fifo_latency * 2;
        }

        /* Update clipping */
        window_clip();
        
        /* Display width switched during HBLANK (Bugs Bunny Double Trouble) */
        if ((v_counter < bitmap.viewport.h) && (mcycles_68k <= (mcycles_vdp + 860)))
        {
          /* Update active display */
          bitmap.viewport.w = 256 + ((d & 1) << 6);

          /* Redraw entire line */
          render_line(v_counter);
        }
        else
        {
          /* Changes should only be applied on next frame (Golden Axe III intro) */
          bitmap.viewport.changed |= 2;
        }
      }

      /* Interlaced modes */
      if (r & 0x06)
      {
        /* changes should be applied on next frame */
        bitmap.viewport.changed |= 2;
      }

      /* See if the S/TE mode bit has changed */
      if (r & 0x08)
      {
        /* Update colors */
        int i;
        color_update (0x00, *(uint16 *) & cram[border << 1]);
        for (i = 1; i < 0x40; i += 1)
          color_update (i, *(uint16 *) & cram[i << 1]);
      }
      break;

    case 13: /* HScroll Base Address */
      reg[13] = d;
      hscb = (d << 10) & 0xFC00;
      break;

    case 16: /* Playfield size */
      reg[16] = d;
      playfield_shift = shift_table[(d & 3)];
      playfield_col_mask = col_mask_table[(d & 3)];
      playfield_row_mask = row_mask_table[(d >> 4) & 3];
      break;

    case 17: /* Window/Plane A clipping */
      reg[17] = d;
      window_clip();
      break;

    default:
      reg[r] = d;
      break;
  }
}

/*--------------------------------------------------------------------------*/
/* DMA Operations                                                           */
/*--------------------------------------------------------------------------*/

/*  DMA Copy
    Read byte from VRAM (source), write to VRAM (addr),
    bump source and add r15 to addr.

    - see how source addr is affected
      (can it make high source byte inc?)
*/
static void dma_copy(int length)
{
  int name;
  unsigned int source = (reg[22] << 8 | reg[21]) & 0xFFFF;

  do
  {
    vram[addr] = vram[source];
    MARK_BG_DIRTY(addr);
    source = (source + 1) & 0xFFFF;
    addr += reg[15];
  } while (--length);

  /* update source address registers */
  reg[21] = source & 0xFF; /* not sure */
  reg[22] = (source >> 8) & 0xFF; 
}

/* 68K Copy to VRAM, VSRAM or CRAM */
static void dma_vbus (int length)
{
  unsigned int source = ((reg[23] & 0x7F) << 17 | reg[22] << 9 | reg[21] << 1) & 0xFFFFFE;
  unsigned int base   = source;
  uint16 temp;
  
  /* DMA source */
  if ((source >> 17) == 0x50)
  {
    /* Z80 & I/O area */
    do
    {
      /* Return $FFFF only when the Z80 isn't hogging the Z-bus.
        (e.g. Z80 isn't reset and 68000 has the bus) */
      if (source <= 0xa0ffff)
        temp = ((zstate ^ 3) ? *(uint16 *)(work_ram + (source & 0xffff)) : 0xffff);

      /* The I/O chip and work RAM try to drive the data bus which results 
          in both values being combined in random ways when read.
          We return the I/O chip values which seem to have precedence, */
      else if (source <= 0xa1001f)
      {
        temp = io_read((source >> 1) & 0x0f);
        temp = (temp << 8 | temp);
      }

      /* All remaining locations access work RAM */
      else
      {
        temp = *(uint16 *)(work_ram + (source & 0xffff));
      }

      source += 2;
      source = ((base & 0xFE0000) | (source & 0x1FFFF));
      data_w(temp);
    }
    while (--length);
  }
  else
  {
    /* ROM & RAM */
    do
    {
      temp = *(uint16 *)(m68k_memory_map[source>>16].base + (source & 0xffff));
      source += 2;
      source = ((base & 0xFE0000) | (source & 0x1FFFF));
      data_w(temp);
    }
    while (--length);
  }

  /* update source address registers */
  reg[21] = (source >> 1) & 0xFF;
  reg[22] = (source >> 9) & 0xFF;
  reg[23] = (reg[23] & 0x80) | ((source >> 17) & 0x7F);
}

/* VRAM FILL */
static void dma_fill(int length, unsigned int data)
{
  int name;

  /* intercept SAT writes */
  if ((addr & sat_base_mask) == satb)
  {
    do
    {
      /* update internal SAT copy */
      WRITE_BYTE(sat, (addr & sat_addr_mask)^1, data);
      WRITE_BYTE(vram, addr^1, data);
      MARK_BG_DIRTY (addr);
      addr += reg[15];
    }
    while (--length);
  }
  else
  {
    do
    {
      WRITE_BYTE(vram, addr^1, data);
      MARK_BG_DIRTY (addr);
      addr += reg[15];
    }
    while (--length);
  }
}
