#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "lib/chip8_cpu.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

struct test : chip::cpu_delegate {
   virtual void log(const std::string& str) override {
      std::cout << str << std::endl;
   }
   virtual std::vector<uint8_t> open_file(const std::string& name) override {
      std::vector<uint8_t> res;
      std::ifstream file(name, std::ios::in | std::ios::binary | std::ios::ate);
      if (file.is_open())
      {
         std::ifstream::pos_type size;
         size = file.tellg();
         res.resize(size);
         file.seekg(0, std::ios::beg);
         file.read((char*)res.data(), size);
         file.close();
      }
      return res;
   }
};

int main() {
   sf::RenderWindow window(sf::VideoMode(640, 480), "");
   window.setVerticalSyncEnabled(true);
   ImGui::SFML::Init(window);
   sf::Color bgColor;
   char windowTitle[255] = "ImGui CHIP-8";
   window.setTitle(windowTitle);
   
   sf::Clock deltaClock;
   sf::Texture texture;
   sf::Image img;
   img.create(128, 64);
   chip::cpu chip;
   test t;
   chip.set_delegate(&t);
   chip.open("12");
   sf::Clock clock;
   sf::Time lastTime = clock.getElapsedTime();
   texture.update(img);
   texture.create(128, 64);
   int opcode_count = 0;
   int cycles_per_second;
   std::string current_dir;
   current_dir.resize(1024 * 256);
   getcwd(&current_dir[0], current_dir.capacity());
   struct FileInfo {
      std::string name;
      bool is_dir;
      size_t size;
   };
   std::vector<FileInfo> infos;
   while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
         ImGui::SFML::ProcessEvent(event);
         
         if (event.type == sf::Event::Closed) {
            window.close();
         }
      }
      {
         if (chip.mode() != chip.SCHIP)
            cycles_per_second = 10; // execute 600 opcodes per second
         else
            cycles_per_second = 30; // 1800 opcodes per second
         
         while (opcode_count < cycles_per_second)
         {
            chip.next();
            opcode_count++;
         }
         if (sf::Joystick::isConnected(0)) {
            chip.set_key(1, sf::Joystick::isButtonPressed(0, 1));
            chip.set_key(2, sf::Joystick::isButtonPressed(0, 2));
            chip.set_key(3, sf::Joystick::isButtonPressed(0, 3));
            chip.set_key(4, sf::Joystick::isButtonPressed(0, 4));
            chip.set_key(5, sf::Joystick::isButtonPressed(0, 5));
            chip.set_key(6, sf::Joystick::isButtonPressed(0, 6));
            chip.set_key(7, sf::Joystick::isButtonPressed(0, 7));
            chip.set_key(8, sf::Joystick::isButtonPressed(0, 8));
            chip.set_key(9, sf::Joystick::isButtonPressed(0, 9));
            chip.set_key(10, sf::Joystick::isButtonPressed(0, 10));
            chip.set_key(11, sf::Joystick::isButtonPressed(0, 11));
            chip.set_key(12, sf::Joystick::isButtonPressed(0, 12));
            chip.set_key(13, sf::Joystick::isButtonPressed(0, 13));
            chip.set_key(14, sf::Joystick::isButtonPressed(0, 14));
            chip.set_key(15, sf::Joystick::isButtonPressed(0, 15));
            chip.set_key(16, sf::Joystick::isButtonPressed(0, 16));
         } else {
            chip.set_key(1, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1));
            chip.set_key(2, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2));
            chip.set_key(3, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3));
            chip.set_key(4, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4));
            chip.set_key(5, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q));
            chip.set_key(6, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W));
            chip.set_key(7, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R));
            chip.set_key(8, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T));
            chip.set_key(9, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A));
            chip.set_key(10, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S));
            chip.set_key(11, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D));
            chip.set_key(12, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F));
            chip.set_key(13, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z));
            chip.set_key(14, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X));
            chip.set_key(15, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C));
            chip.set_key(16, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V));
         }
      }
      {
         sf::Time current = clock.getElapsedTime();
         if (current.asMilliseconds() - lastTime.asMilliseconds() >= 1000/60) {
//            texture.da
            for (int y = 0; y < 64; y++) {
               for (int x = 0; x < 128; x++) {
                  if (chip.is_visible_point(x, y)) {
                     img.setPixel(x, y, sf::Color::White);
                  } else {
                     img.setPixel(x, y, sf::Color::Black);
                  }
               }
            }
            texture.update(img);
            lastTime = clock.getElapsedTime();
            chip.decrease_timers();
            opcode_count = 0;
         }
      }
      
      ImGui::SFML::Update(window, deltaClock.restart());
      
      ImGui::Begin(windowTitle);
      if (infos.empty() == false){
         for (auto& info : infos) {
            if (info.name == "..") {
               if (ImGui::Selectable("..")) {
                  auto pos = current_dir.find_last_of('/');
                  if (pos != current_dir.npos) {
                     current_dir = current_dir.substr(0, pos);
                     infos.clear();
                     break;
                  }
               }
            } else {
               if (ImGui::Selectable(info.name.c_str())) {
                  if (info.is_dir) {
                     current_dir += ('/' + std::string(info.name));
                     infos.clear();
                     break;
                  } else {
                     chip.open(current_dir + ('/' + std::string(info.name)));
                  }
               }
               if (info.is_dir == false) {
                  ImGui::SameLine();
                  ImGui::Text("%zu", info.size);
               }
            }
         }
      } else {
         DIR *dir;
         struct dirent *ent;
         if ((dir = opendir (current_dir.c_str())) != nullptr) {
            infos.emplace_back();
            infos.back().is_dir = true;
            infos.back().name = "..";
            while ((ent = readdir (dir)) != NULL) {
               if (ent->d_name[0] == '.')
                  continue;
               if (ent->d_type & DT_DIR) {
                  infos.emplace_back();
                  infos.back().name = ent->d_name;
                  infos.back().is_dir = true;
               } else if (ent->d_type & DT_REG) {
                  struct stat st;
                  infos.emplace_back();
                  infos.back().name = ent->d_name;
                  infos.back().is_dir = false;
                  if (stat((current_dir + '/' + infos.back().name).c_str(), &st) == 0)
                     infos.back().size = st.st_size;
               }
            }
            closedir(dir);
         }
      }
      ImGui::End();
      ImGui::Begin("DISPLAY");
      ImGui::Image(texture, sf::Vector2f(128 * 3, 64 * 3));
      ImGui::End();
      
      window.clear(bgColor);
      ImGui::SFML::Render(window);
      window.display();
   }
   
   ImGui::SFML::Shutdown();
}
