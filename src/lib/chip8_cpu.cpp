//
//  chip8_cpu.cpp
//  chip-8
//
//  Created by PAVEL PEREKHOZHIKH on 4/13/19.
//

#include "chip8_cpu.hpp"
#include <sstream>
#include <iomanip>

namespace chip {
   cpu::cpu() {
      init();
   }

   void cpu::init() {
      m_PC = 0x200;
      m_SP = 0;
      m_I = 0;
      m_delay_timer = 0;
      m_sound_timer = 0;
      
      for (int i = 0; i < 4096; i++)
         m_memory[i] = 0;
      
      for (int y = 0; y < 64; y++)
         for (int x = 0; x < 128; x++)
            m_screen[x][y] = 0;
      
      for (int i = 0; i < 16; i++)
         m_V[i] = 0;
      
      for (int i = 0; i < 16; i++)
         m_key[i] = 0;
      
      for (int i = 0; i < 16; i++)
         m_stack[i] = 0;
      
      unsigned char font[16*5] = {
         0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
         0x20, 0x60, 0x20, 0x20, 0x70,   // 1
         0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
         0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
         0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
         0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
         0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
         0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
         0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
         0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
         0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
         0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
         0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
         0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
         0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
         0xF0, 0x80, 0xF0, 0x80, 0x80    // F
      };
      // Load font
      for (int i = 0; i < 16*5; i++)
         m_memory[i] = font[i];
      
      unsigned char bigfont[16*10] = {
         0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF,     // 0
         0x18, 0x78, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF,     // 1
         0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF,     // 2
         0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF,     // 3
         0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03, // 4
         0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF,     // 5
         0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF,     // 6
         0xFF, 0xFF, 0x03, 0x03, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x18, // 7
         0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF,     // 8
         0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF,     // 9
         0x7E, 0xFF, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xC3, // A
         0xFC, 0xFC, 0xC3, 0xC3, 0xFC, 0xFC, 0xC3, 0xC3, 0xFC, 0xFC, // B
         0x3C, 0xFF, 0xC3, 0xC0, 0xC0, 0xC0, 0xC0, 0xC3, 0xFF, 0x3C, // C
         0xFC, 0xFE, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFE, 0xFC, // D
         0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, // E
         0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0  // F
      };
      
      // Load big font
      for (int i=0; i < 16*10; i++)
         m_memory[i+80] = bigfont[i];
      
      for (int i=0; i < 8; i++)
         m_hp48_flags[i] = 0;
      
      m_stop = true;
      
      m_mode = Mode::CHIP8;
      m_opcode_exec[0x0] = cpu::opcode_x0;
      m_opcode_exec[0x1] = cpu::opcode_x1;
      m_opcode_exec[0x2] = cpu::opcode_x2;
      m_opcode_exec[0x3] = cpu::opcode_x3;
      m_opcode_exec[0x4] = cpu::opcode_x4;
      m_opcode_exec[0x5] = cpu::opcode_x5;
      m_opcode_exec[0x6] = cpu::opcode_x6;
      m_opcode_exec[0x7] = cpu::opcode_x7;
      m_opcode_exec[0x8] = cpu::opcode_x8;
      m_opcode_exec[0x9] = cpu::opcode_x9;
      m_opcode_exec[0xA] = cpu::opcode_x10;
      m_opcode_exec[0xB] = cpu::opcode_x11;
      m_opcode_exec[0xC] = cpu::opcode_x12;
      m_opcode_exec[0xD] = cpu::opcode_x13;
      m_opcode_exec[0xE] = cpu::opcode_x14;
      m_opcode_exec[0xF] = cpu::opcode_x15;
   }
   void cpu::draw(uint8_t X, uint8_t Y, uint8_t N) {
      m_V[0xF] = 0;
      switch (m_mode) {
         case Mode::CHIP8: { // CHIP-8 mode
            if (N == 0) N = 16;
            for (int yline = 0; yline < N; yline++)
            {
               uint8_t data = m_memory[m_I + yline];
               for (int xpix = 0; xpix < 8; xpix++)
               {
                  if((data & (0x80 >> xpix)) != 0)
                  {
                     if ((m_V[X] + xpix) < 64 && (m_V[Y] + yline) < 32 && (m_V[X] + xpix) >= 0 && (m_V[Y] + yline) >= 0)
                     {
                        if (m_screen[(m_V[X] + xpix)*2][(m_V[Y] + yline)*2] == 1) m_V[0xF] = 1;
                        m_screen[(m_V[X] + xpix)*2][(m_V[Y] + yline)*2] ^= 1;
                        m_screen[(m_V[X] + xpix)*2 + 1][(m_V[Y] + yline)*2] ^= 1;
                        m_screen[(m_V[X] + xpix)*2][(m_V[Y] + yline)*2 + 1] ^= 1;
                        m_screen[(m_V[X] + xpix)*2 + 1][(m_V[Y] + yline)*2 + 1] ^= 1;
                     }
                  }
               }
            }
            break;
         }
            
         case Mode::SCHIP: { // SCHIP mode
            if (N == 0) {
               for (int yline = 0; yline < 16; yline++)
               {
                  uint8_t data = m_memory[m_I + yline*2];
                  for (int xpix = 0; xpix < 8; xpix++)
                  {
                     if((data & (0x80 >> xpix)) != 0)
                     {
                        if ((m_V[X] + xpix) < 128 && (m_V[Y] + yline) < 64 && (m_V[X] + xpix) >= 0 && (m_V[Y] + yline) >= 0)
                        {
                           if (m_screen[m_V[X] + xpix][m_V[Y] + yline] == 1) m_V[0xF] = 1;
                           m_screen[m_V[X] + xpix][m_V[Y] + yline] ^= 1;
                        }
                     }
                  }
                  data = m_memory[m_I + 1 + yline*2];
                  for (int xpix = 0; xpix < 8; xpix++)
                  {
                     if((data & (0x80 >> xpix)) != 0)
                     {
                        if ((m_V[X] + xpix + 8) < 128 && (m_V[Y] + yline) < 64 && (m_V[X] + xpix + 8) >= 0 && (m_V[Y] + yline) >= 0)
                        {
                           if (m_screen[m_V[X] + xpix + 8][m_V[Y] + yline] == 1) m_V[0xF] = 1;
                           m_screen[m_V[X] + xpix + 8][m_V[Y] + yline] ^= 1;
                        }
                     }
                  }
               }
            } else {
               for (int yline = 0; yline < N; yline++)
               {
                  unsigned char data = m_memory[m_I + yline];
                  for (int xpix = 0; xpix < 8; xpix++)
                  {
                     if((data & (0x80 >> xpix)) != 0)
                     {
                        if ((m_V[X] + xpix) < 128 && (m_V[Y] + yline) < 64 && (m_V[X] + xpix) >= 0 && (m_V[Y] + yline) >= 0)
                        {
                           if (m_screen[m_V[X] + xpix][m_V[Y] + yline] == 1) m_V[0xF] = 1;
                           m_screen[m_V[X] + xpix][m_V[Y] + yline] ^= 1;
                        }
                     }
                  }
               }
            }
            break;
         }
      }
   }
   void cpu::next() {
      if (m_stop) return;
      m_opcode = (m_memory[m_PC]<<8) + m_memory[m_PC+1];
      m_PC += 2;
      uint16_t op = ((m_opcode & 0xF000)>>12);
      m_opcode_exec[op](this);
   }
   void cpu::opcode_nop(cpu*) {}
   void cpu::opcode_x1(cpu* cp) {
      cp->m_PC = cp->m_opcode & 0x0FFF;
   }
   void cpu::opcode_x2(cpu* cp) {
      cp->m_stack[cp->m_SP++] = cp->m_PC;
      cp->m_PC = cp->m_opcode & 0x0FFF;
   }
   void cpu::opcode_x3(cpu* cp) {
      if (cp->m_V[((cp->m_opcode & 0x0F00)>>8)] == (cp->m_opcode & 0x00FF)) cp->m_PC += 2;
   }
   void cpu::opcode_x4(cpu* cp) {
      if (cp->m_V[((cp->m_opcode & 0x0F00)>>8)] != (cp->m_opcode & 0x00FF)) cp->m_PC += 2;
   }
   void cpu::opcode_x5(cpu* cp) {
      if (cp->m_V[((cp->m_opcode & 0x0F00)>>8)] == cp->m_V[((cp->m_opcode & 0x00F0)>>4)]) cp->m_PC += 2;
   }
   void cpu::opcode_x6(cpu* cp) {
      cp->m_V[((cp->m_opcode & 0x0F00)>>8)] = cp->m_opcode & 0x00FF;
   }
   void cpu::opcode_x7(cpu* cp) {
      cp->m_V[((cp->m_opcode & 0x0F00)>>8)] += cp->m_opcode & 0x00FF;
   }
   void cpu::opcode_x8(cpu* cp) {
      switch (cp->m_opcode & 0x000F)
      {
         case 0x0:               // 8XY0 - set VX = VY
            cp->m_V[((cp->m_opcode & 0x0F00)>>8)] = cp->m_V[((cp->m_opcode & 0x00F0)>>4)];
            
            break;
            
         case 0x1:               // 8XY1 - set VX = VX | VY
            cp->m_V[((cp->m_opcode & 0x0F00)>>8)] |= cp->m_V[((cp->m_opcode & 0x00F0)>>4)];
            break;
            
         case 0x2:               // 8XY2 - set VX = VX & VY
            cp->m_V[((cp->m_opcode & 0x0F00)>>8)] &= cp->m_V[((cp->m_opcode & 0x00F0)>>4)];
            break;
            
         case 0x3:               // 8XY3 - set VX = VX ^ VY
            cp->m_V[((cp->m_opcode & 0x0F00)>>8)] ^= cp->m_V[((cp->m_opcode & 0x00F0)>>4)];
            break;
            
         case 0x4:               // 8XY4 - set VX = VX + VY, VF = carry
            int i;
            i = static_cast<int>(cp->m_V[((cp->m_opcode & 0x0F00)>>8)]) + static_cast<int>(cp->m_V[((cp->m_opcode & 0x00F0)>>4)]);
            
            if (i > 255)
               cp->m_V[0xF] = 1;
               else
                  cp->m_V[0xF] = 0;
                  
                  cp->m_V[((cp->m_opcode & 0x0F00)>>8)] = i;
                  break;
            
         case 0x5:               // 8XY5 - set VX = VX - VY, VF = !borrow
            if (cp->m_V[((cp->m_opcode & 0x0F00)>>8)] >= cp->m_V[((cp->m_opcode & 0x00F0)>>4)])
               cp->m_V[0xF] = 1;
               else
                  cp->m_V[0xF] = 0;
                  
                  cp->m_V[((cp->m_opcode & 0x0F00)>>8)] -= cp->m_V[((cp->m_opcode & 0x00F0)>>4)];
                  break;
            
         case 0x6:               // 8XY6 - set VX = VX >> 1, VF = carry
            cp->m_V[0xF] = cp->m_V[((cp->m_opcode & 0x0F00)>>8)] & 0x1;
            cp->m_V[((cp->m_opcode & 0x0F00)>>8)] >>= 1;
            break;
            
         case 0x7:               // 8XY7 - set VX = VY - VX, VF = !borrow
            if (cp->m_V[((cp->m_opcode & 0x00F0)>>4)] >= cp->m_V[((cp->m_opcode & 0x0F00)>>8)])
               cp->m_V[0xF] = 1;
               else
                  cp->m_V[0xF] = 0;
                  
                  cp->m_V[((cp->m_opcode & 0x0F00)>>8)] = cp->m_V[((cp->m_opcode & 0x00F0)>>4)] - cp->m_V[((cp->m_opcode & 0x0F00)>>8)];
                  break;
            
         case 0xE:               // 8XYE - set VX = VX << 1, VF = carry
            cp->m_V[0xF] = (cp->m_V[((cp->m_opcode & 0x0F00)>>8)] >> 7) & 0x01;
            cp->m_V[((cp->m_opcode & 0x0F00)>>8)] <<= 1;
            break;
            
         default: {
            std::stringstream str;
            str << "Unknown opcode: 0x"
            << std::setfill('0')
            << std::setw(4)
            << std::hex
            << cp->m_opcode;
            cp->m_delegate->log(str.str());
         }
      }
   }
   void cpu::opcode_x9(cpu* cp) {
      if (cp->m_V[((cp->m_opcode & 0x0F00)>>8)] != cp->m_V[((cp->m_opcode & 0x00F0)>>4)]) cp->m_PC += 2;
   }
   void cpu::opcode_x10(cpu* cp) {
      cp->m_I = cp->m_opcode & 0x0FFF;
   }
   void cpu::opcode_x11(cpu* cp) {
      cp->m_PC = (cp->m_opcode & 0x0FFF) + cp->m_V[0];
   }
   void cpu::opcode_x12(cpu* cp) {
      cp->m_V[((cp->m_opcode & 0x0F00)>>8)] = (cp->random() % 255) & (cp->m_opcode & 0x00FF);
      
   }
   void cpu::opcode_x13(cpu* cp) {
      cp->draw(((cp->m_opcode & 0x0F00)>>8), ((cp->m_opcode & 0x00F0)>>4), (cp->m_opcode & 0x000F));
   }
   void cpu::opcode_x14(cpu* cp) {
      switch (cp->m_opcode & 0x00FF)
      {
         case 0x9E:              // EX9E - skip next instruction if key VX down
            if (cp->m_key[cp->m_V[((cp->m_opcode & 0x0F00)>>8)]] == 1)
               cp->m_PC += 2;
            break;
            
         case 0xA1:              // EXA1 - skip next instruction if key VX up
            if (cp->m_key[cp->m_V[((cp->m_opcode & 0x0F00)>>8)]] == 0)
               cp->m_PC += 2;
            break;
            
         default:{
            std::stringstream str;
            str << "Unknown opcode: 0x"
            << std::setfill('0')
            << std::setw(4)
            << std::hex
            << cp->m_opcode;
            cp->m_delegate->log(str.str());
         }
      }
   }
   void cpu::opcode_x15(cpu* cp) {
      switch (cp->m_opcode & 0x00FF)
      {
         case 0x07:              // FX07 - set VX = delaytimer
            cp->m_V[((cp->m_opcode & 0x0F00)>>8)] = cp->m_delay_timer;
            break;
            
         case 0x0A:              // FX0A - set VX = key, wait for keypress
            cp->m_PC -= 2;
            for (uint8_t n=0; n < 16; n++)
            {
               if (cp->m_key[n] == 1)
               {
                  cp->m_V[((cp->m_opcode & 0x0F00)>>8)] = n;
                  cp->m_PC += 2;
                  break;
               }
            }
            break;
            
         case 0x15:              // FX15 - set delaytimer = VX
            cp->m_delay_timer = cp->m_V[((cp->m_opcode & 0x0F00)>>8)];
            break;
            
         case 0x18:              // FX18 - set soundtimer = VX
            cp->m_sound_timer = cp->m_V[((cp->m_opcode & 0x0F00)>>8)];
            break;
            
         case 0x1E:              // FX1E - set I = I + VX; set VF if buffer overflow
            if ((cp->m_I += cp->m_V[((cp->m_opcode & 0x0F00)>>8)]) > 0xfff)
               cp->m_V[0xF] = 1;
            else
               cp->m_V[0xF] = 0;
            break;
            
         case 0x29:              // FX29 - point I to 5 byte numeric sprite for value in VX
            cp->m_I = cp->m_V[((cp->m_opcode & 0x0F00)>>8)] * 5;
            break;
            
         case 0x30:              // FX30 - point I to 10 byte numeric sprite for value in VX *SCHIP*
            cp->m_I = cp->m_V[((cp->m_opcode & 0x0F00)>>8)] * 10 + 80;
            break;
            
         case 0x33:              // FX33 - store BCD of VX in [I], [I+1], [I+2]
            int n;
            n = cp->m_V[((cp->m_opcode & 0x0F00)>>8)];
            cp->m_memory[cp->m_I] = (n - (n % 100)) / 100;
            n -= cp->m_memory[cp->m_I] * 100;
            cp->m_memory[cp->m_I+1] = (n - (n % 10)) / 10;
            n -= cp->m_memory[cp->m_I+1] * 10;
            cp->m_memory[cp->m_I+2] = n;
            break;
            
         case 0x55:              // FX55 - store V0 .. VX in [I] .. [I+X]
            for (int n=0; n <= ((cp->m_opcode & 0x0F00)>>8); n++)
               cp->m_memory[cp->m_I+n] = cp->m_V[n];
            break;
            
         case 0x65:              // FX65 - read V0 .. VX from [I] .. [I+X]
            for (int n=0; n <= ((cp->m_opcode & 0x0F00)>>8); n++)
               cp->m_V[n] = cp->m_memory[cp->m_I+n];
            break;
            
         case 0x75:              // FX75 - save V0...VX (X<8) in the HP48 flags *SCHIP*
            //for (int i=0; i < 8; i++)
            //hp48_flags[i] = 0;
            for (int i=0; i <= ((cp->m_opcode & 0x0F00)>>8); i++)
               cp->m_hp48_flags[i] = cp->m_V[i];
            break;
            
         case 0x85:              // FX85 - load V0...VX (X<8) from the HP48 flags *SCHIP*
            for (int i=0; i <= ((cp->m_opcode & 0x0F00)>>8); i++)
               cp->m_V[i] = cp->m_hp48_flags[i];
            //for (int i=0; i < 8; i++)
            //hp48_flags[i] = 0;
            break;
            
         default: {
            std::stringstream str;
            str << "Unknown opcode: 0x"
            << std::setfill('0')
            << std::setw(4)
            << std::hex
            << cp->m_opcode;
            cp->m_delegate->log(str.str());
         }
      }
   }
   
   void cpu::opcode_x0(cpu* cp) {
      uint16_t opcode = cp->m_opcode;
      if ((opcode & 0x00F0)>>4 == 0xC) // 00CN - scroll display N lines down *SCHIP*
      {
         int N = opcode & 0x000F;
         for (int y = 63; y > N; y--)
            for (int x = 0; x < 128; x++)
               cp->m_screen[x][y] = cp->m_screen[x][y-N];
         for (int y = 0; y < N; y++)
            for (int x = 0; x < 128; x++)
               cp->m_screen[x][y] = 0;
         return;
      }
      
      switch  (opcode & 0x00FF)
      {
         case 0xE0:              // 00E0 - clear the screen
            for (int y = 0; y < 64; y++)
               for (int x = 0; x < 128; x++)
                  cp->m_screen[x][y] = 0;
            break;
            
         case 0xEE:              // 00EE - return from subroutine
            cp->m_PC = cp->m_stack[--cp->m_SP];
            break;
            
         case 0xFB:              // 00FB - scroll display 4 pixels right *SCHIP*
            for (int y = 0; y < 64; y++)
            {
               for (int x = 127; x > 3; x--)
                  cp->m_screen[x][y] = cp->m_screen[x-4][y];
               cp->m_screen[0][y] = 0;
               cp->m_screen[1][y] = 0;
               cp->m_screen[2][y] = 0;
               cp->m_screen[3][y] = 0;
            }
            break;
            
         case 0xFC:              // 00FB - scroll display 4 pixels left *SCHIP*
            for (int y = 0; y < 64; y++)
            {
               for (int x = 0; x < 124; x++)
                  cp->m_screen[x][y] = cp->m_screen[x+4][y];
               cp->m_screen[124][y] = 0;
               cp->m_screen[125][y] = 0;
               cp->m_screen[126][y] = 0;
               cp->m_screen[127][y] = 0;
            }
            break;
            
         case 0xFD:              // 00FD - Quit the emulator
            cp->m_stop = true;
            break;
            
         case 0xFE:              // 00FE - disable extended screen mode *SCHIP*
            cp->m_mode = Mode::CHIP8;
            break;
            
         case 0xFF:              // 00FF - enable extended screen mode *SCHIP*
            cp->m_mode = Mode::SCHIP;
            break;
            
         default:{
            std::stringstream str;
            str << "Unknown opcode: 0x"
            << std::setfill('0')
            << std::setw(4)
            << std::hex
            << cp->m_opcode;
            cp->m_delegate->log(str.str());
         }
      }
   }
   bool cpu::open(const std::string& name){
      std::vector<uint8_t> buff = m_delegate->open_file(name);
      if (buff.empty()){
         m_delegate->log("Error: unable to open file '" + name + "'");
         return false;
      }
      size_t size = buff.size();
      if (size > 0x0FFF - 0x200) {
         m_delegate->log("Error: file '" + name + "' is too large.");
         return false;
      }
      init();
      memcpy(&m_memory[0x200], buff.data(), size);
      m_stop = false;
      return true;
   }
}
