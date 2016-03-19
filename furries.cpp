#include <yaml-cpp/yaml.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <twitcurl.h>
#include <verbly.h>

class fill_blanks {
  private:
    verbly::data& database;
    
  public:
    fill_blanks(verbly::data& database) : database(database)
    {
      
    }
    
    void visit(std::unique_ptr<verbly::token>& it)
    {
      switch (it->token_type())
      {
        case verbly::token::type::utterance:
        {
          auto& action = *dynamic_cast<verbly::utterance_token*>(it.get());
          for (auto& tkn : action)
          {
            if (!tkn->complete())
            {
              visit(tkn);
        
              break;
            }
          }
          
          break;
        }
        
        case verbly::token::type::fillin:
        {
          auto& tkn = *dynamic_cast<verbly::fillin_token*>(it.get());
          switch (tkn.fillin_type())
          {
            case verbly::fillin_type::participle_phrase:
            {
              verbly::verb v = database.verbs().random(true).limit(1).run().front();
              /*verbly::utterance_token phrase = verbly::random(v.frames).make_utterance();
              while (std::begin(phrase)->token_type() != verbly::type::verb)
              {
                phrase.erase(std::begin(phrase));
              }

              *std::begin(phrase) = verbly::verb_token(v).conjugate(verbly::conjugation::present_participle);
              *it = phrase;*/
              auto avt = std::make_unique<verbly::verb_token>(v);
              avt->inflect(verbly::verb_token::inflection::ing_form);
              it = std::move(avt);
  
              break;
            }
            
            case verbly::fillin_type::adjective:
            {
              verbly::adjective adj = database.adjectives().random(true).limit(1).run().front();
              it = std::make_unique<verbly::string_token>(adj.base_form());
              
              break;
            }
            
            case verbly::fillin_type::adverb:
            {
              verbly::adverb adv = database.adverbs().random(true).limit(1).run().front();
              it = std::make_unique<verbly::string_token>(adv.base_form());
              
              break;
            }

            default:
            {
              it = std::make_unique<verbly::string_token>("*the reality of the situation*");
  
              break;
            }
          }
          
          break;
        }
      }
    }
};

int main(int argc, char** argv)
{
  srand(time(NULL));
  
  YAML::Node config = YAML::LoadFile("config.yml");
    
  twitCurl twitter;
  twitter.getOAuth().setConsumerKey(config["consumer_key"].as<std::string>());
  twitter.getOAuth().setConsumerSecret(config["consumer_secret"].as<std::string>());
  twitter.getOAuth().setOAuthTokenKey(config["access_key"].as<std::string>());
  twitter.getOAuth().setOAuthTokenSecret(config["access_secret"].as<std::string>());
  
  for (;;)
  {
    std::cout << "Generating tweet" << std::endl;
    
    std::vector<verbly::utterance_token> forms;
    forms.push_back({
      new verbly::string_token("the furries are"),
      new verbly::fillin_token(verbly::fillin_type::participle_phrase)
    });
    forms.push_back({
      new verbly::string_token("the furries are"),
      new verbly::fillin_token(verbly::fillin_type::adjective)
    });
    forms.push_back({
      new verbly::string_token("the furries are"),
      new verbly::fillin_token(verbly::fillin_type::adverb),
      new verbly::fillin_token(verbly::fillin_type::adjective)
    });
    
    verbly::data database {"data.sqlite3"};
    fill_blanks yeah {database};
    std::unique_ptr<verbly::token> action = std::make_unique<verbly::utterance_token>(forms[rand() % forms.size()]);
    while (!action->complete())
    {
      yeah.visit(action);
    }
    
    std::string result = action->compile();
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
}
