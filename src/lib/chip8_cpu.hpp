//
//  chip8_cpu.hpp
//  chip-8
//
//  Created by PAVEL PEREKHOZHIKH on 4/13/19.
//

#ifndef chip8_cpu_hpp
#define chip8_cpu_hpp

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace chip {
   struct cpu_delegate {
      virtual void log(const std::string&) = 0;
      virtual std::vector<uint8_t> open_file(const std::string& name) = 0;
   };
   class cpu {
   public:
      enum Mode {
         CHIP8,
         SCHIP
      };
   public:
      cpu();
      void init();
      void draw(uint8_t X, uint8_t Y, uint8_t N);
      void next();
      virtual int32_t random(){
         return std::rand();
      }
      void decrease_timers(){
         if(m_delay_timer > 0)
            --m_delay_timer;
         
         if(m_sound_timer > 0)
            --m_sound_timer;
      }
      void set_delegate(cpu_delegate* delegate) {
         m_delegate = delegate;
      }
      bool open(const std::string& name);
      bool is_visible_point(int32_t x, int32_t y) {
         return m_screen[x][y] == 1;
      }
      Mode mode() const {
         return m_mode;
      }
      void set_key(int index, bool press) {
         m_key[index] = press ? 1 : 0;
      }
   private:
      static void opcode_x0(cpu*);
      static void opcode_x1(cpu*);
      static void opcode_x2(cpu*);
      static void opcode_x3(cpu*);
      static void opcode_x4(cpu*);
      static void opcode_x5(cpu*);
      static void opcode_x6(cpu*);
      static void opcode_x7(cpu*);
      static void opcode_x8(cpu*);
      static void opcode_x9(cpu*);
      static void opcode_x10(cpu*);
      static void opcode_x11(cpu*);
      static void opcode_x12(cpu*);
      static void opcode_x13(cpu*);
      static void opcode_x14(cpu*);
      static void opcode_x15(cpu*);
      static void opcode_x16(cpu*);
      static void opcode_nop(cpu*);
   private:
      
      typedef void(*exec_opt)(cpu*);
      exec_opt       m_opcode_exec[16];
      uint8_t        m_hp48_flags[8];
      uint8_t        m_screen[128][64];
      uint8_t        m_key[16];
      uint16_t       m_opcode;
      uint8_t        m_V[16];
      uint16_t       m_I;
      uint16_t       m_PC;
      uint8_t        m_SP;
      uint16_t       m_stack[16];
      uint8_t        m_memory[4096];
      uint8_t        m_delay_timer;
      uint8_t        m_sound_timer;
      cpu_delegate*  m_delegate = nullptr;
      Mode           m_mode;
      bool           m_stop;
   };
}

#endif /* chip8_cpu_hpp */
