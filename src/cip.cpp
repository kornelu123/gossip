#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <array>
#include <iostream>

#define MAX_PACK_LEN 256

namespace cip_pack{
  class
  recved_pack{
    private:
      uint8_t                                   header;
      std::array<uint8_t, MAX_PACK_LEN - 1>     content;

    public:
      recved_pack(int size, uint8_t *content)
      {
        header = *content;
        
        this->content = std::array<uint8_t, MAX_PACK_LEN - 1>();
        std::memcpy(this->content.begin(), content + 1, MAX_PACK_LEN - 1);
      }

    static void
      print_pack(cip_pack::recved_pack pack)
    {
      std::cout << "Header:" << pack.header << std::endl;

      for (const auto& e : pack.content){
        std::cout << std::hex;
      }
    }
  };
}
