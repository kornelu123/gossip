#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define USER_NOT_FOUND -1
#define USER_FOUND      0

#define REG_FAILED     -1
#define REG_SUCC        0

#define DEL_FAILED     -1
#define DEL_SUCC        0


class
  db_file{
  private:
    std::fstream    inout;
    const char     *fname;

  public:
    db_file(const char* fname):
      fname(fname)
    {
    }

    int
      usr_log(char *uname, char *passwd)
    {
      const int ret = chk_credent(uname, passwd);
      if(ret == USER_NOT_FOUND)
        return ret;

      return USER_FOUND;
    }

    int
      usr_reg(char *uname, char *passwd)
    {
      const int ret = chk_uname(uname);

      if(ret != USER_NOT_FOUND) 
        return REG_FAILED;

      inout.open(fname, std::ios::out | std::ios::app);

      inout << uname << ';' << passwd << std::endl;

      inout.flush();
      inout.close();
      return REG_SUCC;
    }

    int
      usr_del(char *uname, char *passwd)
    {
      const char index = chk_credent(uname, passwd);
      if(index == USER_NOT_FOUND)
        return DEL_FAILED;

      std::fstream temp;
      temp.open("temp", std::ios::out);

      std::string cur_line;
      inout.open(fname);
      int count = 0;

      while(getline(inout, cur_line)){
        if(count == index)
          continue;
        
        temp << cur_line << std::endl;
        count++;
      }

      inout.flush();
      inout.close();
      temp.close();

      remove(fname);
      rename("temp", fname);

      return DEL_SUCC;
    }

  private:
    int
      chk_credent(char *uname, char *passwd)
    {
      inout.open(fname);

      std::stringstream fmt;
      fmt << uname << ";" << passwd;
      std::string search = fmt.str();

      std::string line;

      int count = 0;
      while(getline(inout, line)){
        if(search == line){
          inout.close();
          return count;
        }

        count++;
      }

      inout.close();
      return USER_NOT_FOUND;
    }

    int
      chk_uname(char *uname)
    {
      inout.open(fname, std::ios::in);

      std::string search(uname);
      std::string line;

      while(getline(inout, line)){
        line  = line.substr(0,line.rfind(';'));
        if(search == line){
          inout.close();
          return USER_FOUND;
        }
      }

      inout.close();
      return USER_NOT_FOUND;
    }
};
