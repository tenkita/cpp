/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hshimizu <hshimizu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 06:12:44 by hshimizu          #+#    #+#             */
/*   Updated: 2024/10/07 21:44:55 by hshimizu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

#include <cerrno>
#include <cstdlib>
#include <limits>
#include <stdexcept>

int ft_stoi(char *str) {
  char *endptr;
  errno = 0;
  long i = std::strtol(str, &endptr, 10);
  if (errno || endptr == str || *endptr ||
      std::numeric_limits<int>::max() < i ||
      std::numeric_limits<int>::min() > i)
    throw std::runtime_error("strtol");
  return i;
}

int main(int argc, char **argv) {
  try {
    std::size_t size = argc - 1;
    int data[size];
    for (std::size_t i = 0; i < size; i++)
      if ((data[i] = ft_stoi(argv[i + 1])) < 0)
        throw std::runtime_error("Invalid argument");
    PmergeMe(data, size);
    return EXIT_SUCCESS;
  } catch (const std::exception &e) {
    std::cerr << "Error" << std::endl;
    return EXIT_FAILURE;
  }
}

/*
	Again, “Better test...” I think I hear a voice saying,
	If we conform to SUBJECT, can't we test on the second container?
	Huh?
	Anyway, I've written the test code in python.
	I'll test around with it, that should do it, right?
	You can ask me anything you want, gauge my understanding!
*/
/*
#!/usr/bin/env python3
import random
import subprocess
import sys

def test():
	for i in range(1, 1000):
		for j in range(1, 5):
			numbers = random.sample(range(0, (1 << 31) -1), i)
			sys.stdout.write(f"{i} item {j} time: ")
			sys.stdout.flush()
			sorted_numbers = sorted(numbers)
			result = subprocess.run(
				['./PmergeMe'] + [str(i) for i in numbers] ,
				stdout=subprocess.PIPE,
				stderr=subprocess.PIPE,
			)
			after_strs = [i[7:] for i in result.stdout.decode("ascii").split("\n") if i.startswith("After:")]
			for after_str in after_strs:
				after = [int(i) for i in after_str.split(" ")]
				if after == sorted_numbers:
					sys.stdout.write("OK")
				else:
					sys.stdout.write("NG")
					return False
				sys.stdout.write(" ")
			sys.stdout.write("\n")
	return True

if __name__ == '__main__':
	test()
*/