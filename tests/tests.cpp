#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "../src/files.cpp"

#define TEST_FNAME  "test"

constexpr std::string unames[3] = {
  "j00r",
  "uname",
  "name"
};

constexpr std::string passwds[3] = {
  "test",
  "testing",
  "tester"
};

std::string
  create_record(std::string uname, std::string passwd)
{
  std::stringstream fmt;
  fmt << uname << ';' << passwd << std::endl;;
  return fmt.str();
}

std::string line_0 = create_record(unames[0].c_str(), passwds[0].c_str());
std::string line_1 = create_record(unames[1].c_str(), passwds[1].c_str());

class tests{
  public:
      tests(void)
    {
    }

    int
      run(void)
    {
      int ret = 0;
      ret = ret | file_test();
      return ret;
    }

  private:
    int
      file_test(void)
    {
      std::fstream file_t(TEST_FNAME, std::ios::out);

      if(!file_t.is_open()){
        printf("File didn't open\n");
        return -1;
      }

      file_t << line_0 << line_1;

      file_t.flush();
      file_t.close();
      
      int ret = 0;

      db_file file(TEST_FNAME);
      if(file.usr_log(const_cast<char *>(unames[0].c_str()),const_cast<char *>(passwds[0].c_str())) != USER_FOUND){
          printf("valid uname and passwd marked as invalid, [FAILED]\n");
          ret = -1;
      }

      if(file.usr_log(const_cast<char *>(unames[0].c_str()),const_cast<char *>(passwds[1].c_str())) != USER_NOT_FOUND){
          printf("Invalid uname and passwd marked as valid, [FAILED]\n");
          ret = -1;
      }

      if(file.usr_reg(const_cast<char *>(unames[1].c_str()),const_cast<char *>(passwds[2].c_str())) != REG_FAILED){
          printf("Registered uname that has been taken , [FAILED]\n");
          ret = -1;
      }

      if(file.usr_reg(const_cast<char *>(unames[2].c_str()),const_cast<char *>(passwds[2].c_str())) != REG_SUCC){
          printf("Not registered free uname , [FAILED]\n");
          ret = -1;
      }

      if(file.usr_log(const_cast<char *>(unames[2].c_str()),const_cast<char *>(passwds[2].c_str())) == USER_NOT_FOUND){
          printf("Username parsed, but not saved , [FAILED]\n");
          ret = -1;
      }

      if(file.usr_del(const_cast<char *>(unames[2].c_str()),const_cast<char *>(passwds[2].c_str())) != DEL_SUCC){
          printf("Deleting user not working , [FAILED]\n");
          ret = -1;
      }

      if(file.usr_log(const_cast<char *>(unames[2].c_str()),const_cast<char *>(passwds[2].c_str())) != USER_NOT_FOUND){
          printf("Deleting user not working , [FAILED]\n");
          ret = -1;
      }

      if(file.usr_log(const_cast<char *>(unames[2].c_str()),const_cast<char *>(passwds[2].c_str())) != USER_NOT_FOUND){
          printf("Deleted already deleted user , [FAILED]\n");
          ret = -1;
      }


      return ret;
    }

};


int
main(void)
{
  tests test;
  return test.run();
}
