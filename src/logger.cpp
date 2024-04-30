#include <iostream>

#include <cstdint>

#define MAX_LOG       4

#define LOG_LOG     0
#define LOG_INFO    1
#define LOG_DEBUG   2
#define LOG_WARN    3
#define LOG_ERR     4
#define LOG_NONE    5

#define COLOR_PREAMBLE      "\033[1;"
#define COLOR_POSTAMBLE     "m"

#define COLOR_EPILOGUE      "\033[0m"

const char *log_pre[] = {
  "[LOG]:",
  "[INFO]:",
  "[DEBUG]:",
  "[WARNING]:",
  "[ERROR]:"
};

const char *log_color[] = {
  "36",                   //cyan      -> log
  "32",                   //green     -> info
  "34",                   //blue      -> debug
  "33",                   //yellow    -> warning
  "31"                    //red       -> error
};

class
logger
{
  private:
    uint8_t log_opt;

  public:
    logger()
    {
      this->log_opt = CONFIG_LOG;
    }

    void
      log(char *msg, uint8_t debug_lvl)
    {
      if(debug_lvl >= this->log_opt){
        logger::print_log_pre(debug_lvl);
        std::cout << " " << msg << std::endl;
      }
    }

  private:
    inline void
      print_log_pre(uint8_t logger_opt)
    {
      std::cout << COLOR_PREAMBLE << log_color[logger_opt] << COLOR_POSTAMBLE;
      std::cout << log_pre[logger_opt] << COLOR_EPILOGUE;
    }
};
