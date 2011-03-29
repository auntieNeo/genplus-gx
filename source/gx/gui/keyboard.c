#include "shared.h"
#include "keyboard.h"
#include "font.h"
#include "gui.h"

#ifdef HW_RVL
#include <wiiuse/wpad.h>
#endif

#define BG_COLOR_1 {0x49,0x49,0x49,0xff}
#define BG_COLOR_2 {0x66,0x66,0x66,0xff}

#define NUM_KEYBOARD_KEYS 46
#define KEYBOARD_X_POS 50
#define KEYBOARD_Y_POS 150
#define KEYBOARD_ROW_1 0
#define KEYBOARD_ROW_2 KEYBOARD_ROW_1 + 11
#define KEYBOARD_ROW_3 KEYBOARD_ROW_2 + 11
#define KEYBOARD_ROW_4 KEYBOARD_ROW_3 + 12
#define KEYBOARD_ROW_5 KEYBOARD_ROW_4 + 11

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
extern const u8 Button_spacebar_png[];
extern const u8 Button_spacebar_over_png[];
extern const u8 Button_mod_key_over_png[];
extern const u8 Frame_keyboard_png[];
extern const u8 Frame_keyboard_title_png[];

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

static butn_data button_spacebar_data =
{
  {NULL,NULL},
  {Button_spacebar_png,Button_spacebar_over_png}
};

/*****************************************************************************/
/*  GUI helpers                                                              */
/*****************************************************************************/
static gui_item action_done =
{
  NULL,Key_B_png,"","Done",10,422,28,28
};

static gui_item action_select =
{
  NULL,Key_A_png,"","",602,422,28,28
};

/*****************************************************************************/
/*  Menu Items description                                                   */
/*****************************************************************************/
static gui_item items_keyboard[NUM_KEYBOARD_KEYS] =
{
  {NULL,NULL,"1",      "Add Character",     60, 60, 40, 40},  // 0
  {NULL,NULL,"2",      "Add Character",    106, 60, 40, 40},
  {NULL,NULL,"3",      "Add Character",    152, 60, 40, 40},
  {NULL,NULL,"4",      "Add Character",    198, 60, 40, 40},
  {NULL,NULL,"5",      "Add Character",    244, 60, 40, 40},
  {NULL,NULL,"6",      "Add Character",    290, 60, 40, 40},
  {NULL,NULL,"7",      "Add Character",    336, 60, 40, 40},
  {NULL,NULL,"8",      "Add Character",    382, 60, 40, 40},
  {NULL,NULL,"9",      "Add Character",    428, 60, 40, 40},
  {NULL,NULL,"0",      "Add Character",    474, 60, 40, 40},
  {NULL,NULL,"Back",   "Remove Character", 520, 60, 60, 40},  // 10
  {NULL,NULL,"q",      "Add Character",     72,106, 40, 40},
  {NULL,NULL,"w",      "Add Character",    118,106, 40, 40},
  {NULL,NULL,"e",      "Add Character",    164,106, 40, 40},
  {NULL,NULL,"r",      "Add Character",    210,106, 40, 40},
  {NULL,NULL,"t",      "Add Character",    256,106, 40, 40},
  {NULL,NULL,"y",      "Add Character",    302,106, 40, 40},
  {NULL,NULL,"u",      "Add Character",    348,106, 40, 40},
  {NULL,NULL,"i",      "Add Character",    394,106, 40, 40},
  {NULL,NULL,"o",      "Add Character",    440,106, 40, 40},
  {NULL,NULL,"p",      "Add Character",    486,106, 40, 40},  // 20
  {NULL,NULL,"-",      "Add Character",    532,106, 40, 40},
  {NULL,NULL,"Caps",   "Caps Lock",         18,152, 60, 40},
  {NULL,NULL,"a",      "Add Character",     84,152, 40, 40},
  {NULL,NULL,"s",      "Add Character",    130,152, 40, 40},
  {NULL,NULL,"d",      "Add Character",    176,152, 40, 40},
  {NULL,NULL,"f",      "Add Character",    222,152, 40, 40},
  {NULL,NULL,"g",      "Add Character",    268,152, 40, 40},
  {NULL,NULL,"h",      "Add Character",    314,152, 40, 40},
  {NULL,NULL,"j",      "Add Character",    360,152, 40, 40},
  {NULL,NULL,"k",      "Add Character",    406,152, 40, 40},  // 30
  {NULL,NULL,"l",      "Add Character",    452,152, 40, 40},
  {NULL,NULL,";",      "Add Character",    498,152, 40, 40},
  {NULL,NULL,"'",      "Add Character",    544,152, 40, 40},
  {NULL,NULL,"Shift",  "Shift",             30,198, 60, 40},
  {NULL,NULL,"z",      "Add Character",     96,198, 40, 40},
  {NULL,NULL,"x",      "Add Character",    142,198, 40, 40},
  {NULL,NULL,"c",      "Add Character",    188,198, 40, 40},
  {NULL,NULL,"v",      "Add Character",    234,198, 40, 40},
  {NULL,NULL,"b",      "Add Character",    280,198, 40, 40},
  {NULL,NULL,"n",      "Add Character",    326,198, 40, 40},  // 40
  {NULL,NULL,"m",      "Add Character",    372,198, 40, 40},
  {NULL,NULL,",",      "Add Character",    418,198, 40, 40},
  {NULL,NULL,".",      "Add Character",    464,198, 40, 40},
  {NULL,NULL,"/",      "Add Character",    510,198, 40, 40},
  {NULL,NULL," ",      "Add Space",        200,244,200, 40},
};

/*****************************************************************************/
/*  Menu Buttons description                                                 */
/*****************************************************************************/
static gui_butn buttons_keyboard[NUM_KEYBOARD_KEYS] =
{
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 60, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},106, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},152, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},198, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},244, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},290, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},336, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},382, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},428, 60, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},474, 60, 40, 40},
  {&button_mod_key_data,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},520, 60, 60, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 72,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},118,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},164,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},210,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},256,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},302,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},348,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},394,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},440,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},486,106, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},532,106, 40, 40},
  {&button_mod_key_data,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 18,152, 60, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 84,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},130,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},176,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},222,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},268,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},314,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},360,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},406,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},452,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},498,152, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},544,152, 40, 40},
  {&button_mod_key_data,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 30,198, 60, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1}, 96,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},142,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},188,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},234,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},280,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},326,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},372,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},418,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},464,198, 40, 40},
  {&button_digit_data  ,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},510,198, 40, 40},
  {&button_spacebar_data,BUTTON_VISIBLE|BUTTON_ACTIVE|BUTTON_OVER_SFX,{4,4,1,1},200,244,200, 40},
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
          FONT_writeCenter(item->text,18,item->x-4 + xOffset,item->x+item->w+4 + xOffset,button->y+(button->h-18)/2+18 + yOffset,(GXColor)DARK_GREY);
        }
      }
      else
      {
        if (button->data)
          gxDrawTexture(button->data->texture[0],button->x + xOffset,button->y + yOffset,button->w, button->h,255);

        if (item)
        {
          FONT_writeCenter(item->text,16,item->x + xOffset,item->x+item->w + xOffset,button->y+(button->h - 16)/2+16 + yOffset,(GXColor)DARK_GREY);
        }
      }
    }
  }
}

void shift_keyboard(void)
{
  // TODO
}

void KeyboardMenu(gui_menu *parent, const char *name, char *string, size_t size)
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
  const u8 *oldHelper0 = NULL, *oldHelper1 = NULL;
#ifdef HW_RVL
  int x,y;
#endif

  /* window position */
  int xwindow = 20;
  int ywindow = 90;

  /* disable helper comments */
  if (parent->helpers[0])
  {
    oldHelper0 = parent->helpers[0]->data;
    parent->helpers[0]->data = NULL;
  }
  if (parent->helpers[1])
  {
    oldHelper1 = parent->helpers[1]->data;
    parent->helpers[1]->data = NULL;
  }

  /* initilize images */
  window = gxTextureOpenPNG(Frame_keyboard_png,0);
  top = gxTextureOpenPNG(Frame_keyboard_title_png,0);
  button_digit_data.texture[0] = gxTextureOpenPNG(button_digit_data.image[0],0);
  button_digit_data.texture[1] = gxTextureOpenPNG(button_digit_data.image[1],0);
  button_mod_key_data.texture[0] = gxTextureOpenPNG(button_mod_key_data.image[0],0);
  button_mod_key_data.texture[1] = gxTextureOpenPNG(button_mod_key_data.image[1],0);
  button_spacebar_data.texture[0] = gxTextureOpenPNG(button_spacebar_data.image[0],0);
  button_spacebar_data.texture[1] = gxTextureOpenPNG(button_spacebar_data.image[1],0);
  action_done.texture = gxTextureOpenPNG(action_done.data,0);
  action_select.texture = gxTextureOpenPNG(action_select.data,0);

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

    drawKeyboard(selected, xwindow, ywindow + yoffset);

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

    /* update title */
    snprintf(title, MAXPATHLEN, "%d %s: %s", selected, name, string);

    /* display title */
    FONT_writeCenter(title,20,xwindow,xwindow+window->width,ywindow+(top->height-20)/2+20,(GXColor)WHITE);

    /* draw keyboard */
    drawKeyboard(selected, xwindow, ywindow);

    /* draw left comment */
    gxDrawTexture(action_done.texture,action_done.x,action_done.y,action_done.w,action_done.h,255);
    FONT_write(action_done.comment,16,action_done.x+action_done.w+6,action_done.y+(action_done.h-16)/2 + 16,640,(GXColor)WHITE);

    /* draw right comment */
    if(selected != -1)
    {
      strcpy(action_select.comment, items_keyboard[selected].comment);
      gxDrawTexture(action_select.texture,action_select.x,action_select.y,action_select.w,action_select.h,255);
      FONT_alignRight(action_select.comment,16,action_select.x-6,action_select.y+(action_select.h-16)/2+16,(GXColor)WHITE);
    }

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
      gxTextureClose(&button_spacebar_data.texture[0]);
      gxTextureClose(&button_spacebar_data.texture[1]);
      gxTextureClose(&action_done.texture);
      gxTextureClose(&action_select.texture);
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
          if((x>=button->x+xwindow)&&(x<=(button->x+xwindow+button->w))&&(y>=button->y+ywindow)&&(y<=(button->y+ywindow+button->h)))
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
      else if (selected < KEYBOARD_ROW_5)
        currentRow = 4;
      else
        currentRow = 5;
      if (p & PAD_BUTTON_UP)
      {
        switch(currentRow)
        {
          case 1:
            // do nothing
            break;
          case 2:
            selected -= 11;
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
          case 5:  // spacebar
            selected = 39;  // b
            break;
        }
      }
      else if (p & PAD_BUTTON_DOWN)
      {
        switch(currentRow)
        {
          case 1:
            selected += 11;
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
            selected = 45;  // spacebar
          case 5:
            // do nothing
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
          i = strlen(string);
          if(i != 0)
          {
            string[i - 1] = '\0';
          }
        }
        else if(selected == 22)  // Caps Lock
        {
          /* toggle capslock */
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
          /* toggle shiftlock */
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
          snprintf(string, size, "%s%c", string, items_keyboard[selected].text[0]);
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

    drawKeyboard(selected, xwindow, ywindow + yoffset);

    /* update display */
    gxSetScreen();

    /* slide speed */
    yoffset -= 60;
  }

  /* restore helper comment */
  if (parent->helpers[0])
  {
    parent->helpers[0]->data = oldHelper0;
  }
  if (parent->helpers[1])
  {
    parent->helpers[1]->data = oldHelper1;
  }

  /* close textures */
  gxTextureClose(&window);
  gxTextureClose(&top);
  gxTextureClose(&button_digit_data.texture[0]);
  gxTextureClose(&button_digit_data.texture[1]);
  gxTextureClose(&button_mod_key_data.texture[0]);
  gxTextureClose(&button_mod_key_data.texture[1]);
  gxTextureClose(&button_spacebar_data.texture[0]);
  gxTextureClose(&button_spacebar_data.texture[1]);
}
