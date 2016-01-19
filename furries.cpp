#include <yaml-cpp/yaml.h>
#include <iostream>
#include <mysql.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <twitcurl.h>

int db_error(MYSQL* driver, const char* error)
{
  std::cout << error << ": " << mysql_error(driver) << std::endl;
  return 1;
}

int main(int argc, char** argv)
{
  srand(time(NULL));
  
  YAML::Node config = YAML::LoadFile("config.yml");
  const char* host = config["host"].as<std::string>().c_str();
  const char* user = config["user"].as<std::string>().c_str();
  const char* pass = config["pass"].as<std::string>().c_str();
  const char* db = config["db"].as<std::string>().c_str();
  
  MYSQL* driver = mysql_init(NULL);
  if (!mysql_real_connect(driver, host, user, pass, db, 0, NULL, 0))
  {
    return db_error(driver, "Error connecting to database");
  }
  
  twitCurl twitter;
  twitter.getOAuth().setConsumerKey(config["consumer_key"].as<std::string>());
  twitter.getOAuth().setConsumerSecret(config["consumer_secret"].as<std::string>());
  twitter.getOAuth().setOAuthTokenKey(config["access_key"].as<std::string>());
  twitter.getOAuth().setOAuthTokenSecret(config["access_secret"].as<std::string>());
  
  for (;;)
  {
    std::cout << "Generating tweet" << std::endl;
    
    std::stringstream output;
    output << "the furries are ";
  
    //if (rand() % 2 == 0)
    {
      // Adverb(s) + Adjective
      while (rand() % 4 == 0)
      {
        const char* getadverb = "SELECT ss1.word FROM wn_pertainym INNER JOIN wn_synset AS ss1 ON wn_pertainym.synset_id_1 = ss1.synset_id AND wn_pertainym.wnum_1 = ss1.w_num INNER JOIN wn_synset AS ss2 ON wn_pertainym.synset_id_2 = ss2.synset_id AND wn_pertainym.wnum_2 = ss2.w_num WHERE ss1.ss_type = 'r' ORDER BY RAND() LIMIT 1";
        if (mysql_query(driver, getadverb)) return db_error(driver, "Query failed");
        MYSQL_RES* getadverb2 = mysql_use_result(driver); if (getadverb2 == NULL) return db_error(driver, "Query failed");
        MYSQL_ROW getadverb3 = mysql_fetch_row(getadverb2); if (getadverb3 == NULL) return db_error(driver, "Query failed");
        output << getadverb3[0] << " ";
        mysql_free_result(getadverb2);
      }
  
      const char* getword = "SELECT word FROM wn_synset WHERE ss_type = 'a' OR ss_type = 's' ORDER BY RAND() LIMIT 1";
      if (mysql_query(driver, getword)) return db_error(driver, "Query failed");
      MYSQL_RES* getword2 = mysql_use_result(driver); if (getword2 == NULL) return db_error(driver, "Query failed");
      MYSQL_ROW getword3 = mysql_fetch_row(getword2); if (getword3 == NULL) return db_error(driver, "Query failed");
      output << getword3[0];
      mysql_free_result(getword2);
    } /*else {
      // Verb phrase
      const char* getword = "SELECT word FROM wn_synset WHERE ss_type = 'a ' OR ss_type = 's' ORDER BY RAND() LIMIT 1";
      if (mysql_query(driver, getword)) return db_error(driver, "Query failed");
      MYSQL_RES* getword2 = mysql_use_result(driver); if (getword2 == NULL) return db_error(driver, "Query failed");
      MYSQL_ROW getword3 = mysql_fetch_row(getword2); if (getword3 == NULL) return db_error(driver, "Query failed");
    }
    
  
  
  
    if (rand() % 2 == 0)
    {
      std::stringstream ispart;
      ispart << "SELECT wn_verb_frame.f_num FROM wn_participle INNER JOIN wn_verb_frame ON wn_verb_frame.synset_id_1 = synset_id_2 AND (wn_verb_frame.w_num = wnum_2 OR wn_verb_frame.w_num = 0) WHERE wn_participle.synset_id_1 = " << wordssid << " AND wn_participle.wnum_1 = " << wordnum;
      if (mysql_query(driver, ispart.str().c_str())) return db_error(driver, "Query failed");
      MYSQL_RES* ispart2 = mysql_use_result(driver); if (ispart2 == NULL) return db_error(driver, "Query failed");
      MYSQL_ROW ispart3 = mysql_fetch_row(ispart2);
      mysql_free_result(ispart2);
  
      if (ispart3 != NULL)
      {
        int frame = atoi(ispart3[0]);
        std::cout << "frame " << frame << std::endl;
      
        if (frame == 4 || frame == 22)
        {
          // the furries are ----ing *prepositional phrase*
          // ex: the furries are laughing over there
        } else if (frame == 5)
        {
          // the furries are ----ing something *adj/n*
          // ex: the furries are regarding life inconsequential
        } else if (frame == 6 || frame == 7)
        {
          // the furries are ----ing *adj/n*
          // ex: the furries are turning gay
        } else if (frame == 8 || frame == 9 || frame == 10 || frame == 11)
        {
          // the furries are ----ing something/somebody
          // ex: the furries are holding hostages
        } else if (frame == 12 || frame == 27)
        {
          // the furries are ----ing to somebody
          // ex: the furries are appealing to God
        } else if (frame == 13)
        {
          // the furries are ----ing on something
          // ex: the furries are lecturing on heterosexuality
        } else if (frame == 14)
        {
          // the furries are ----ing somebody something
          // ex: the furries are reading your mom the menu
        } else if (frame == 15)
        {
          // the furries are ----ing something to somebody
          // ex: the furries are pitching a product to Apple
        } else if (frame == 16)
        {
          // the furries are ----ing something from somebody
          // ex: the furries are separating your money from you
        } else if (frame == 17)
        {
          // the furries are ----ing somebody with something
          // ex: the furries are injecting me with solemnity
        } else if (frame == 18)
        {
          // the furries are ----ing somebody of something
          // ex: the furries are depriving me of hope
        } else if (frame == 19)
        {
          // the furries are ----ing something on somebody
          // ex: the furries are forcing pervision on us
        } else if (frame == 20 || frame == 21)
        {
          // the furries are ----ing somebody/something *prepositional phrase*
          // ex: the furries are leaving us behind
        } else if (frame == 24)
        {
          // the furries are ----ing somebody to INF
          // ex: the furries are getting us to eat
        } else if (frame == 25)
        {
          // the furries are ----ing somebody INF
          // ex: the furries are making us procreate
        } else if (frame == 26)
        {
          // the furries are ----ing that CLAUSE
          // ex: the furries are understaing that life is precious
        } else if (frame == 28)
        {
          // the furries are ----ing to INF
          // ex: the furries are beginning to understand
        } else if (frame == 29)
        {
          // the furries are ----ing whether to INF
          // ex: the furries are deciding whether to hallucinate
        } else if (frame == 30)
        {
          // the furries are ----ing somebody into V-ing something
          // ex: the furries are tempting me into consecrating a magnet
        } else if (frame == 31)
        {
          // the furries are ----ing something with something
          // ex: the furries are replacing existence with solidarity
        }
      }
    }
    */
    
    std::string result = output.str();
    result.resize(140);
    
    std::string replyMsg;
    if (twitter.statusUpdate(result))
    {
      twitter.getLastWebResponse(replyMsg);
      std::cout << "Twitter message: " << replyMsg << std::endl;
    } else {
      twitter.getLastCurlError(replyMsg);
      std::cout << "Curl error: " << replyMsg << std::endl;
    }
    
    std::cout << "Waiting" << std::endl;
    sleep(60 * 60 * 3);
  }
  
  mysql_close(driver);
}
