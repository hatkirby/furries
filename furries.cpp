#include <yaml-cpp/yaml.h>
#include <twitter.h>
#include <verbly.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include "sentence.h"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "usage: furries [configfile]" << std::endl;
    return -1;
  }

  std::string configfile(argv[1]);
  YAML::Node config = YAML::LoadFile(configfile);

  std::random_device random_device;
  std::mt19937 random_engine{random_device()};

  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());

  twitter::client client(auth);
  verbly::database database(config["verbly_datafile"].as<std::string>());
  sentence generator(database, random_engine);

  for (;;)
  {
    std::cout << "Generating tweet..." << std::endl;

    std::string result = generator.generate();
    result.resize(140);

    std::cout << result << std::endl;

    try
    {
      client.updateStatus(result);

      std::cout << "Tweeted!" << std::endl;
      std::cout << "Waiting..." << std::endl;

      std::this_thread::sleep_for(std::chrono::hours(1));

      std::cout << std::endl;
    } catch (const twitter::twitter_error& e)
    {
      std::cout << "Twitter error: " << e.what() << std::endl;
    }
  }
}
