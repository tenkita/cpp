
#include "BitcoinExchange.hpp"


BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) {
  this->_rateMap = other._rateMap;
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other) {
  if (this != &other) 
    this->_rateMap = other._rateMap;
  return *this;
}

BitcoinExchange::~BitcoinExchange() {}


void BitcoinExchange::loadRateDatabase(const std::string& filename) {
  std::ifstream file(filename.c_str());
  if (!file.is_open()) {
    std::cerr << "Error: could not open database file." << std::endl;
    return;
  }

  std::string line;
  std::getline(file, line);

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string date, rateStr;
    if (!std::getline(ss, date, ',') || !std::getline(ss, rateStr)) {
      continue;
    }
    double rate = std::atof(rateStr.c_str());
    // double rate = std::stod(rateStr);
    _rateMap[date] = rate;
  }
}

double BitcoinExchange::getRateBydata(const std::string& date) const {
  std::map<std::string, double>::const_iterator it = _rateMap.lower_bound(date);
  if (it != _rateMap.begin() && (it == _rateMap.end() || it->first != date))
    --it;
  if (it == _rateMap.end()) {
    std::cerr << "Error: date not found in DB." << std::endl;
    return 0.0;
  }
  return it->second;
}

bool BitcoinExchange::isValidDate(const std::string& date) const {
  if (date.size() != 10 || date[4] != '-' || date[7] != '-')
    return false;
  int y, m, d;
  std::stringstream ss(date);//
  char dash;
  ss >> y >> dash >> m >> dash >> d;

  if (y < 2009 || m < 1 || m > 12 || d < 1 || d > 31)
    return false;
  return true;
}

bool BitcoinExchange::isValidValue(const std::string& valueStr, double& value) const {
  std::stringstream ss(valueStr);
  ss >> value;
  if (!(ss >> value)|| !ss.eof() || value < 0 || value > 1000)
    return false;
  return true;
}
