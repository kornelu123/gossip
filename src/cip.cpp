#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <array>
#include <iostream>

#define MAX_PACK_LEN 256

namespace cip{
  enum header{
    log,
    reg,
    act_user,
  };

  class
    pack{
      public:
      uint8_t                                   header;
      std::array<uint8_t, MAX_PACK_LEN - 1>     content;

        void
          print_pack()
        {
          printf("%2X ", this->header);
          for (int i=0; i<this->content.size(); i++){
            printf("%2X ", this->content[i]);
            if((i + 1) % 16 == 15)
              printf("\n");
          }
        }
    };

  class
  intern_pack : public pack{
    public:
      int                       user_fd;

      intern_pack(uint8_t *content, int fd)
      {
        this->header = *content;
        
        this->content = std::array<uint8_t, MAX_PACK_LEN - 1>();
        std::memcpy(this->content.begin(), content + 1, MAX_PACK_LEN - 1);
      }
  };
}
