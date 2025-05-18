
#include "BitcoinExchange.hpp"

int main(int ac, char **av) {
  if (ac != 2){
    std::cerr << "Error: could not open file." << std::endl;
    return 1;
  }

  BitcoinExchange btc;
  btc.loadRateDatabase("data.csv");

  std::ifstream infile(av[1]);
  if (!infile.is_open()) {
    std::cerr << "Error: could not open file." << std::endl;
    return 1;
  }

  std::string line;
  std::getline(infile, line);

  while (std::getline(infile, line)) {
    std::stringstream ss(line);
    std::string date, valueStr;

    if (!std::getline(ss, date, ',') || !std::getline(ss, valueStr)) {
      std::cerr << "Error: bad input => " << line << std::endl;
      continue;
    }

    date.erase(0, date.find_first_not_of(" \t"));
    date.erase(date.find_last_not_of(" \t") + 1);
    valueStr.erase(0, valueStr.find_first_not_of(" \t"));
    valueStr.erase(valueStr.find_last_not_of(" \t") + 1);

    double value;
    if (!btc.isValidDate(date)) {
      std::cerr << "Error: bad input => " << date << std::endl;
      continue;
    } else if (!btc.isValidValue(valueStr, value)) {
      if (value < 0)
        std::cerr << "Error: not a positive number." << std::endl;
      else 
        std::cerr << "Error: too large a number." << std::endl;
      continue;
    }
    double rate = btc.getRateBydata(date);
    std::cout << date << "=>" << value << " = " << (value * rate) << std::endl;
  }
  return 0;
}