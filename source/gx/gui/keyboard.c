#include "shared.h"
#include "keyboard.h"
#include "font.h"
#include "gui.h"

#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif

#define BG_COLOR_1 {0x49,0x49,0x49,0xff}
#define BG_COLOR_2 {0x66,0x66,0x66,0xff}

#define NUM_KEYBOARD_KEYS 45
#define KEYBOARD_X_POS 50
#define KEYBOARD_Y_POS 150
#define KEYBOARD_ROW_1 0
#define KEYBOARD_ROW_2 KEYBOARD_ROW_1 + 11
#define KEYBOARD_ROW_3 KEYBOARD_ROW_2 + 11
#define KEYBOARD_ROW_4 KEYBOARD_ROW_3 + 12

#ifdef HW_RVL
extern const u8 Key_Minus_wii_png[];
extern const u8 Key_Plus_wii_png[];
#else
extern const u8 Key_L_gcn_png[];
extern const u8 Key_R_gcn_png[];
#endif
extern const u8 Key_DPAD_png[];

extern const u8 Button_mod_key_png[];
extern const u8 Button_mod_key_over_png[];

/*****************************************************************************/
/*  GUI Buttons data                                                         */
/*****************************************************************************/
static butn_data button_digit_data =
{
  {NULL,NULL},
  {Button_digit_png,Button_digit_over_png}
};

static butn_data button_mod_key_data =
{
  {NULL,NULL},
  {Button_mod_key_png,Button_mod_key_over_png}
};

/*****************************************************************************/
/*  GUI helpers                                                              */
/*****************************************************************************/
static gui_item action_cancel =
{
  NULL,Key_B_png,"","Exit",10,422,28,28
};

static gui_item action_select =
{
  NULL,Key_A_png,"","Edit Entry",602,422,28,28
};

/*****************************************************************************/
/*  Menu Items description                                                   */
/*****************************************************************************/
static gui_item items_keyboard[NUM_KEYBOARD_KEYS] =
{
  {NULL,NULL,"1",      "Add Character",     62,  0, 40, 40},  // 0
  {NULL,NULL,"2",      "Add Character",    108,  0, 40, 40},
  {NULL,NULL,"3",      "Add Character",    154,  0, 40, 40},
  {NULL,NULL,"4",      "Add Character",    200,  0, 40, 40},
  {NULL,NULL,"5",      "Add Character",    246,  0, 40, 40},
  {NULL,NULL,"6",      "Add Character",    292,  0, 40, 40},
  {NULL,NULL,"7",      "Add Character",    338,  0, 40, 40},
  {NULL,NULL,"8",      "Add Character",    384,  0, 40, 40},
  {NULL,NULL,"9",      "Add Character",    430,  0, 40, 40},
  {NULL,NULL,"0",      "Add Character",    476,  0, 40, 40},
  {NULL,NULL,"Back",   "Remove Character", 522,  0, 80, 40},  // 10
  {NULL,NULL,"q",      "Add Character",     74, 46, 40, 40},
  {NULL,NULL,"w",      "Add Character",    120, 46, 40, 40},
  {NULL,NULL,"e",      "Add Character",    166, 46, 40, 40},
  {NULL,NULL,"r",      "Add Character",    212, 46, 40, 40},
  {NULL,NULL,"t",      "Add Character",    258, 46, 40, 40},
  {NULL,NULL,"y",      "Add Character",    304, 46, 40, 40},
  {NULL,NULL,"u",      "Add Character",    350, 46, 40, 40},
  {NULL,NULL,"i",      "Add Character",    396, 46, 40, 40},
  {NULL,NULL,"o",      "Add Character",    442, 46, 40, 40},
  {NULL,NULL,"p",      "Add Character",    488, 46, 40, 40},  // 20
  {NULL,NULL,"-",      "Add Character",    534, 46, 40, 40},
  {NULL,NULL,"Caps",   "Caps Lock",          0, 92, 80, 40},
  {NULL,NULL,"a",      "Add Character",     86, 92, 40, 40},
  {NULL,NULL,"s",      "Add Character",    132, 92, 40, 40},
  {NULL,NULL,"d",      "Add Character",    178, 92, 40, 40},
  {NULL,NULL,"f",      "Add Character",    224, 92, 40, 40},
  {NULL,NULL,"g",      "Add Character",    270, 92, 40, 40},
  {NULL,NULL,"h",      "Add Character",    316, 92, 40, 40},
  {NULL,NULL,"j",      "Add Character",    362, 92, 40, 40},
  {NULL,NULL,"k",      "Add Character",    408, 92, 40, 40},  // 30
  {NULL,NULL,"l",      "Add Character",    454, 92, 40, 40},
  {NULL,NULL,";",      "Add Character",    500, 92, 40, 40},
  {NULL,NULL,"'",      "Add Character",    546, 92, 40, 40},
  {NULL,NULL,"Shift",  "Shift",             12,138, 80, 40},
  {NULL,NULL,"z",      "Add Character",     98,138, 40, 40},
  {NULL,NULL,"x",      "Add Character",    144,138, 40, 40},
  {NULL,NULL,"c",      "Add Character",    190,138, 40, 40},
  {NULL,NULL,"v",      "Add Character",    236,138, 40, 40},
  {NULL,NULL,"b",      "Add Character",    282,138, 40, 40},
  {NULL,NULL,"n",      "Add Character",    328,138, 40, 40},  // 40
  {NULL,NULL,"m",      "Add Character",    374,138, 40, 40},
  {NULL,NULL,",",      "Add Character",    420,138, 40, 40},
  {NULL,NULL,".",      "Add Character",    466,138, 40, 40},
  {NULL,NULL,"/",      "Add Character",    512,138, 40, 40},
};

/*****************************************************************************/
/*  Menu Buttons description                                                 */
/*****************************************************************************/
static gui_butn buttons_keyboard[NUM_KEYBOARD_KEYS] =
{
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 62,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},108,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},154,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},200,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},246,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},292,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},338,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},384,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},430,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},476,  0, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},522,  0, 80, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 74, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},120, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},166, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},212, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},258, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},304, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},350, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},396, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},442, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},488, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},534, 46, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},  0, 92, 80, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 86, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},132, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},178, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},224, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},270, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},316, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},362, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},408, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},454, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},500, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},546, 92, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 12,138, 80, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 98,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},144,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},190,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},236,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},282,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},328,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},374,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},420,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},466,138, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},512,138, 40, 40},
};

void drawKeyboard(int selected, int xOffset, int yOffset)
{
  int i;
  gui_butn *button;
  gui_item *item;

  /* draw key buttons and items */
  for (i = 0; i < NUM_KEYBOARD_KEYS; i++)
  {
    button = &buttons_keyboard[i];

    if (button->state & BUTTON_VISIBLE)
    {
      item = &items_keyboard[i];

      /* draw button + items */ 
      if (i == selected)
      {
        if (button->data)
          gxDrawTexture(button->data->texture[1],button->x-4 + xOffset,button->y-4 + yOffset,button->w+8,button->h+8,255);

        if (item)
        {
          FONT_writeCenter(item->text,18,item->x-4 + xOffset,item->x+item->w+4 + yOffset,button->y+(button->h-18)/2+18,(GXColor)DARK_GREY);
        }
      }
      else
      {
        if (button->data)
          gxDrawTexture(button->data->texture[0],button->x + xOffset,button->y + yOffset,button->w, button->h,255);

        if (item)
        {
          FONT_writeCenter(item->text,16,item->x + xOffset,item->x+item->w + yOffset,button->y+(button->h - 16)/2+16,(GXColor)DARK_GREY);
        }
      }
    }
  }
}

void KeyboardMenu(gui_menu *parent, const char *name, char *string, size_t size, int (*isValid)(const char *new, const char *old))
{
  int i;
  int old, selected = 0;
  int currentRow;
  int done = 0;
  int shiftlock = 0;
  int capslock = 0;
  char title[MAXPATHLEN];
  s16 p;
  gui_butn *button;
  gx_texture *window, *top;
#ifdef HW_RVL
  int x,y;
#endif

  /* window position */
  int xwindow = 166;
  int ywindow = 160;

  /* initilize images */
  window = gxTextureOpenPNG(Frame_s2_png,0);
  top = gxTextureOpenPNG(Frame_s2_title_png,0);
  button_digit_data.texture[0] = gxTextureOpenPNG(button_digit_data.image[0],0);
  button_digit_data.texture[1] = gxTextureOpenPNG(button_digit_data.image[1],0);
  button_mod_key_data.texture[0] = gxTextureOpenPNG(button_mod_key_data.image[0],0);
  button_mod_key_data.texture[1] = gxTextureOpenPNG(button_mod_key_data.image[1],0);

  /* initilize title*/
  snprintf(title, MAXPATHLEN, "%s: %s", name, string);

  /* slide in */
  int yoffset = -(ywindow + window->height);
  while (yoffset < 0)
  {
    /* draw parent menu */
    GUI_DrawMenu(parent);

    /* draw window */
    gxDrawTexture(window,xwindow,ywindow+yoffset,window->width,window->height,225);
    gxDrawTexture(top,xwindow,ywindow+yoffset,top->width,top->height,255);

    /* display title */
    FONT_writeCenter(title,20,xwindow,xwindow+window->width,ywindow+(top->height-20)/2+20+yoffset,(GXColor)WHITE);

//    drawKeyboard(selected, xwindow + xoffset, xwindow + yoffset);

    /* update display */
    gxSetScreen();

    /* slide speed */
    yoffset += 60;
  }

  while (!done)
  {
    /* draw parent menu */
    GUI_DrawMenu(parent);

    /* draw window */
    gxDrawTexture(window,xwindow,ywindow,window->width,window->height,225);
    gxDrawTexture(top,xwindow,ywindow,top->width,top->height,255);

    /* display title */
    FONT_writeCenter(title,20,xwindow,xwindow+window->width,ywindow+(top->height-20)/2+20,(GXColor)WHITE);

    /* draw keyboard */
    drawKeyboard(selected, xwindow, ywindow);

    old = selected;
    p = m_input.keys;

#ifdef HW_RVL
    if (Shutdown)
    {
      gxTextureClose(&window);
      gxTextureClose(&top);
      gxTextureClose(&button_digit_data.texture[0]);
      gxTextureClose(&button_digit_data.texture[1]);
      gxTextureClose(&button_mod_key_data.texture[0]);
      gxTextureClose(&button_mod_key_data.texture[1]);
      gxTextureClose(&w_pointer);
      GUI_DeleteMenu(parent);
      GUI_FadeOut();
      shutdown();
      SYS_ResetSystem(SYS_POWEROFF, 0, 0);
    }
    else if (m_input.ir.valid)
    {
      /* get cursor position */
      x = m_input.ir.x;
      y = m_input.ir.y;

      /* draw wiimote pointer */
      gxDrawTextureRotate(w_pointer, x-w_pointer->width/2, y-w_pointer->height/2, w_pointer->width, w_pointer->height,m_input.ir.angle,255);

      /* check for valid buttons */
      selected = -1;
      for (i = 0; i < NUM_KEYBOARD_KEYS; i++)
      {
        button = &buttons_keyboard[i];
        if ((button->state & BUTTON_ACTIVE) && (button->state & BUTTON_VISIBLE))
        {
          if((x>=button->x)&&(x<=(button->x+button->w))&&(y>=button->y)&&(y<=(button->y+button->h)))
          {
            selected = i;
            break;
          }
        }
      }
    }
    else
    {
      /* reinitialize selection */
      if (selected == -1)
        selected = 0;
    }
#endif

    /* update screen */
    gxSetScreen();

    /* allow user to navigate with directional pad */
    if (p & (PAD_BUTTON_UP | PAD_BUTTON_DOWN))
    {
      if (selected < KEYBOARD_ROW_2)
        currentRow = 1;
      else if (selected < KEYBOARD_ROW_3)
        currentRow = 2;
      else if (selected < KEYBOARD_ROW_4)
        currentRow = 3;
      else
        currentRow = 4;
      if (p & PAD_BUTTON_UP)
      {
        switch(currentRow)
        {
          case 1:
            // do nothing
            break;
          case 2:
            if(selected == 21)  // -
            {
              selected = 9;  // 0
            }
            else
            {
              selected -= 10;
            }
            break;
          case 3:
            if(selected == 22)  // Caps Lock
            {
              selected = 11;  // Q
            }
            else
            {
              selected -= 12;
            }
            break;
          case 4:
            selected -= 12;
            break;
        }
      }
      else if (p & PAD_BUTTON_DOWN)
      {
        switch(currentRow)
        {
          case 1:
            selected += 10;
            break;
          case 2:
            selected += 12;
            break;
          case 3:
            if(selected == 33)  // '
            {
              selected = 44;  // /
            }
            else
            {
              selected += 12;
            }
            break;
          case 4:
            // TODO: move to spacebar
            break;
        }
      }
    }
    else if (p & PAD_BUTTON_LEFT)
    {
      switch(selected)
      {
        case KEYBOARD_ROW_1:
        case KEYBOARD_ROW_2:
        case KEYBOARD_ROW_3:
        case KEYBOARD_ROW_4:
          break;
        default:
          selected -= 1;
      }
    }
    else if (p & PAD_BUTTON_RIGHT)
    {
      switch(selected)
      {
        case KEYBOARD_ROW_2 - 1:
        case KEYBOARD_ROW_3 - 1:
        case KEYBOARD_ROW_4 - 1:
        case NUM_KEYBOARD_KEYS - 1:
          break;
        default:
          selected += 1;
      }
    }

    /* sound fx */
    if (selected != old)
    {
      if (selected >= 0)
      {
        ASND_SetVoice(ASND_GetFirstUnusedVoice(),VOICE_MONO_16BIT,22050,0,(u8 *)button_over_pcm,button_over_pcm_size,
            ((int)config.sfx_volume * 255) / 100,((int)config.sfx_volume * 255) / 100,NULL);
      }
    }

    if (p & PAD_BUTTON_A)
    {
      if (selected >= 0)
      {
        if(selected == 10)  // Backspace
        {
          /* remove a character */
          i = strlen(string) - 1;
          if(i != 0)
          {
            string[i - 1] = '\0';
          }
        }
        else if(selected == 22)  // Caps Lock
        {
          if(shiftlock)
          {
            shift_keyboard();
            shiftlock = 0;
          }
          shift_keyboard();
          capslock = 1;

        }
        else if(selected == 34)  // Shift
        {
          if(!capslock && !shiftlock)  // shift doesn't work when capslock is on
          {
            shift_keyboard();
            shiftlock = 1;
          }
          else if(shiftlock)
          {
            shift_keyboard();
            shiftlock = 0;
          }
        }
        else
        {
          /* add a character */
          snprintf(string, size, "%s%c", name, items_keyboard[selected].text[0]);
          if(shiftlock)
          {
            shift_keyboard();
            shiftlock = 0;
          }
        }
      }
    }
    else if (p & PAD_BUTTON_B)
    {
      done = 1;
    }
  }

  /* slide out */
  yoffset = 0;
  while (yoffset > -(ywindow + window->height))
  {
    /* draw parent menu */
    GUI_DrawMenu(parent);

    /* draw window */
    gxDrawTexture(window,xwindow,ywindow+yoffset,window->width,window->height,225);
    gxDrawTexture(top,xwindow,ywindow+yoffset,top->width,top->height,255);

    /* display title */
    FONT_writeCenter(title,20,xwindow,xwindow+window->width,ywindow+(top->height-20)/2+20+yoffset,(GXColor)WHITE);

//    drawKeyboard(selected, xwindow + xoffset, xwindow + yoffset);

    /* update display */
    gxSetScreen();

    /* slide speed */
    yoffset -= 60;
  }

  /* close textures */
  gxTextureClose(&window);
  gxTextureClose(&top);
  gxTextureClose(&button_digit_data.texture[0]);
  gxTextureClose(&button_digit_data.texture[1]);
  gxTextureClose(&button_mod_key_data.texture[0]);
  gxTextureClose(&button_mod_key_data.texture[1]);
}

void keyboardmenu_cb(void)
{
}

void shift_keyboard(void)
{
}
