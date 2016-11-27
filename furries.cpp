#include <yaml-cpp/yaml.h>
#include <twitter.h>
#include <verbly.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <random>

class fill_blanks {
  private:
    verbly::data& database;
    
  public:
    fill_blanks(verbly::data& database) : database(database)
    {
      
    }
    
    verbly::filter<verbly::noun> parse_selrestrs(verbly::frame::selrestr selrestr)
    {
      switch (selrestr.get_type())
      {
        case verbly::frame::selrestr::type::empty:
        {
          return verbly::filter<verbly::noun>{};
        }
        
        case verbly::frame::selrestr::type::singleton:
        {
          verbly::noun n;
          
          if (selrestr.get_restriction() == "concrete")
          {
            n = database.nouns().with_singular_form("physical entity").limit(1).run().front();
          } else if (selrestr.get_restriction() == "time")
          {
            n = database.nouns().with_singular_form("time").limit(1).run().front();
          } else if (selrestr.get_restriction() == "state")
          {
            n = database.nouns().with_singular_form("state").limit(1).run().front();
          } else if (selrestr.get_restriction() == "abstract")
          {
            n = database.nouns().with_singular_form("abstract entity").limit(1).run().front();
          } else if (selrestr.get_restriction() == "time")
          {
            n = database.nouns().with_singular_form("time").limit(1).run().front();
          } else if (selrestr.get_restriction() == "scalar")
          {
            n = database.nouns().with_singular_form("number").limit(1).run().front();
          } else if (selrestr.get_restriction() == "currency")
          {
            auto nn2 = database.nouns().with_singular_form("currency").limit(2).run();
            std::vector<verbly::noun> nn(std::begin(nn2), std::end(nn2));
            n = nn[1];
          } else if (selrestr.get_restriction() == "location")
          {
            n = database.nouns().with_singular_form("location").limit(1).run().front();
          } else if (selrestr.get_restriction() == "organization")
          {
            n = database.nouns().with_singular_form("organization").limit(1).run().front();
          } else if (selrestr.get_restriction() == "int_control")
          {
            n = database.nouns().with_singular_form("causal agent").limit(1).run().front();
          } else if (selrestr.get_restriction() == "natural")
          {
            n = database.nouns().with_singular_form("natural object").limit(1).run().front();
          } else if (selrestr.get_restriction() == "phys_obj")
          {
            n = database.nouns().with_singular_form("physical object").limit(1).run().front();
          } else if (selrestr.get_restriction() == "solid")
          {
            n = database.nouns().with_singular_form("solid").limit(1).run().front();
          } else if (selrestr.get_restriction() == "shape")
          {
            n = database.nouns().with_singular_form("shape").limit(1).run().front();
          } else if (selrestr.get_restriction() == "substance")
          {
            n = database.nouns().with_singular_form("substance").limit(1).run().front();
          } else if (selrestr.get_restriction() == "idea")
          {
            n = database.nouns().with_singular_form("idea").limit(1).run().front();
          } else if (selrestr.get_restriction() == "sound")
          {
            auto nn2 = database.nouns().with_singular_form("sound").limit(4).run();
            std::vector<verbly::noun> nn(std::begin(nn2), std::end(nn2));
            n = nn[3];
          } else if (selrestr.get_restriction() == "communication")
          {
            n = database.nouns().with_singular_form("communication").limit(1).run().front();
          } else if (selrestr.get_restriction() == "region")
          {
            n = database.nouns().with_singular_form("region").limit(1).run().front();
          } else if (selrestr.get_restriction() == "place")
          {
            n = database.nouns().with_singular_form("place").limit(1).run().front();
          } else if (selrestr.get_restriction() == "machine")
          {
            n = database.nouns().with_singular_form("machine").limit(1).run().front();
          } else if (selrestr.get_restriction() == "animate")
          {
            n = database.nouns().with_singular_form("animate being").limit(1).run().front();
          } else if (selrestr.get_restriction() == "plant")
          {
            auto nn2 = database.nouns().with_singular_form("plant").limit(2).run();
            std::vector<verbly::noun> nn(std::begin(nn2), std::end(nn2));
            n = nn[1];
          } else if (selrestr.get_restriction() == "comestible")
          {
            n = database.nouns().with_singular_form("food").limit(1).run().front();
          } else if (selrestr.get_restriction() == "artifact")
          {
            n = database.nouns().with_singular_form("artifact").limit(1).run().front();
          } else if (selrestr.get_restriction() == "vehicle")
          {
            n = database.nouns().with_singular_form("vehicle").limit(1).run().front();
          } else if (selrestr.get_restriction() == "human")
          {
            n = database.nouns().with_singular_form("person").limit(1).run().front();
          } else if (selrestr.get_restriction() == "animal")
          {
            n = database.nouns().with_singular_form("animal").limit(1).run().front();
          } else if (selrestr.get_restriction() == "body_part")
          {
            n = database.nouns().with_singular_form("body part").limit(1).run().front();
          } else if (selrestr.get_restriction() == "garment")
          {
            n = database.nouns().with_singular_form("clothing").limit(1).run().front();
          } else if (selrestr.get_restriction() == "tool")
          {
            n = database.nouns().with_singular_form("tool").limit(1).run().front();
          } else {
            return verbly::filter<verbly::noun>{};
          }
          
          return verbly::filter<verbly::noun>{n, !selrestr.get_pos()};
        }
        
        case verbly::frame::selrestr::type::group:
        {
          verbly::filter<verbly::noun> ret;
          ret.set_orlogic(selrestr.get_orlogic());

          std::transform(std::begin(selrestr), std::end(selrestr), std::back_inserter(ret), [&] (verbly::frame::selrestr sr) {
            return parse_selrestrs(sr);
          });
          
          return ret;
        }
      }
    }
    
    template <typename RNG>
    void visit(verbly::token& it, RNG&& rng)
    {
      switch (it.get_type())
      {
        case verbly::token::type::utterance:
        {
          for (auto& tkn : it)
          {
            if (!tkn.is_complete())
            {
              visit(tkn, rng);
              
              break;
            }
          }
          
          break;
        }
        
        case verbly::token::type::fillin:
        {
          switch (it.get_fillin_type())
          {
            case verbly::token::fillin_type::participle_phrase:
            { 
              for (;;)
              {
                verbly::verb v = database.verbs().has_frames().random().limit(1).run().front();
                auto frames = v.frames().run();
                std::vector<verbly::frame> filtered;
                std::remove_copy_if(std::begin(frames), std::end(frames), std::back_inserter(filtered), [] (verbly::frame& f) {
                  if (f.parts().size() < 2)
                  {
                    return true;
                  }
                
                  if (f.parts()[0].get_type() != verbly::frame::part::type::noun_phrase)
                  {
                    return true;
                  }
                
                  if (f.parts()[0].get_role() != "Agent")
                  {
                    return true;
                  }
                
                  if (f.parts()[1].get_type() != verbly::frame::part::type::verb)
                  {
                    return true;
                  }
                
                  return false;
                });
                
                if (filtered.empty())
                {
                  continue;
                }
                
                int fr_i = std::uniform_int_distribution<int>(0, filtered.size()-1)(rng);
                verbly::frame fr = filtered[fr_i];
                verbly::token utter;
                for (auto part : fr.parts())
                {
                  switch (part.get_type())
                  {
                    case verbly::frame::part::type::noun_phrase:
                    {
                      if (part.get_role() == "Agent")
                      {
                        continue;
                      }
                      
                      if (part.get_synrestrs().count("adjp") == 1)
                      {
                        utter << verbly::token{verbly::token::fillin_type::adjective_phrase};
                        
                        continue;
                      } else if ((part.get_synrestrs().count("be_sc_ing") == 1)
                        || (part.get_synrestrs().count("ac_ing") == 1)
                        || (part.get_synrestrs().count("sc_ing") == 1)
                        || (part.get_synrestrs().count("np_omit_ing") == 1)
                        || (part.get_synrestrs().count("oc_ing") == 1))
                      {
                        utter << verbly::token{verbly::token::fillin_type::participle_phrase};
                        
                        continue;
                      } else if ((part.get_synrestrs().count("poss_ing") == 1)
                        || (part.get_synrestrs().count("possing") == 1)
                        || (part.get_synrestrs().count("pos_ing") == 1))
                      {
                        utter << verbly::token{"their"};
                        utter << verbly::token{verbly::token::fillin_type::participle_phrase};
                        
                        continue;
                      } else if (part.get_synrestrs().count("genitive") == 1)
                      {
                        utter << verbly::token{"their"};
                        
                        continue;
                      } else if (part.get_synrestrs().count("adv_loc") == 1)
                      {
                        if (std::bernoulli_distribution(1.0/2.0)(rng))
                        {
                          utter << verbly::token{"here"};
                        } else {
                          utter << verbly::token{"there"};
                        }
                        
                        continue;
                      } else if (part.get_synrestrs().count("refl") == 1)
                      {
                        utter << verbly::token{"themselves"};
                        
                        continue;
                      } else if ((part.get_synrestrs().count("sc_to_inf") == 1)
                        || (part.get_synrestrs().count("ac_to_inf") == 1)
                        || (part.get_synrestrs().count("vc_to_inf") == 1)
                        || (part.get_synrestrs().count("rs_to_inf") == 1)
                        || (part.get_synrestrs().count("oc_to_inf") == 1))
                      {
                        utter << verbly::token{verbly::token::fillin_type::infinitive_phrase};
                        
                        continue;
                      } else if (part.get_synrestrs().count("oc_bare_inf") == 1)
                      {
                        verbly::token tkn{verbly::token::fillin_type::infinitive_phrase};
                        tkn.set_extra(1);
                        
                        utter << tkn;
                        
                        continue;
                      }
                      
                      auto selrestrs = fr.roles()[part.get_role()];
                      auto query = database.nouns().limit(1).random().is_not_proper().full_hyponym_of(parse_selrestrs(selrestrs));
                      verbly::noun n = query.run().front();
                      if ((std::bernoulli_distribution(1.0/2.0)(rng)) && (part.get_synrestrs().count("definite") == 0))
                      {
                        utter << verbly::token{"the"};
                      } else {
                        if (n.starts_with_vowel_sound())
                        {
                          utter << verbly::token{"an"};
                        } else {
                          utter << verbly::token{"a"};
                        }
                      }
                      
                      if (part.get_synrestrs().count("plural") == 1)
                      {
                        utter << verbly::token{n, verbly::token::noun_inflection::plural};
                      } else {
                        utter << verbly::token{n};
                      }
                      
                      if (part.get_synrestrs().count("acc_ing") == 1)
                      {
                        utter << verbly::token{verbly::token::fillin_type::participle_phrase};
                      }
                    
                      break;
                    }
                  
                    case verbly::frame::part::type::verb:
                    {
                      utter << verbly::token{v, verbly::token::verb_inflection::ing_form};
                    
                      break;
                    }
                  
                    case verbly::frame::part::type::literal_preposition:
                    {
                      int ch_i = std::uniform_int_distribution<int>(0, part.get_choices().size()-1)(rng);
                      utter << verbly::token{part.get_choices()[ch_i]};
                    
                      break;
                    }
                    
                    case verbly::frame::part::type::selection_preposition:
                    {
                      auto query = database.prepositions();
                      for (auto preprestr : part.get_preprestrs())
                      {
                        query.in_group(preprestr);
                      }
                      utter << verbly::token{query.random().limit(1).run().front()};
                      
                      break;
                    }
                  
                    case verbly::frame::part::type::adjective:
                    {
                      utter << verbly::token{verbly::token::fillin_type::adjective_phrase};
                    
                      break;
                    }
                  
                    case verbly::frame::part::type::adverb:
                    {
                      utter << verbly::token{verbly::token::fillin_type::adverb_phrase};
                    
                      break;
                    }
                  
                    case verbly::frame::part::type::literal:
                    {
                      utter << verbly::token{part.get_literal()};
                    
                      break;
                    }
                  }
                }
              
                it = utter;
  
                break;
              }
              
              break;
            }
            
            case verbly::token::fillin_type::adjective_phrase:
            {
              verbly::token phrase;
              
              if (std::bernoulli_distribution(1.0/4.0)(rng))
              {
                phrase << verbly::token{verbly::token::fillin_type::adverb_phrase};
              }
              
              if (std::bernoulli_distribution(1.0/2.0)(rng))
              {
                phrase << verbly::token{verbly::token::fillin_type::participle_phrase};
              } else {
                phrase << verbly::token{database.adjectives().random().limit(1).run().front()};
              }
              
              it = phrase;
              
              break;
            }
            
            case verbly::token::fillin_type::adverb_phrase:
            {
              it = verbly::token{database.adverbs().random().limit(1).run().front()};
              
              break;
            }
            
            case verbly::token::fillin_type::infinitive_phrase:
            {
              verbly::token utter;
              if (it.get_extra() != 1)
              {
                utter << verbly::token{"to"};
              }
              
              utter << verbly::token{database.verbs().random().limit(1).run().front()};
              
              it = utter;
              
              break;
            }

            default:
            {
              it = verbly::token{"*the reality of the situation*"};
  
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
  std::random_device random_device;
  std::mt19937 random_engine{random_device()};
  
  YAML::Node config = YAML::LoadFile("config.yml");
  
  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());
  
  twitter::client client(auth);
  verbly::data database {"data.sqlite3"};
  
  for (;;)
  {
    std::cout << "Generating tweet..." << std::endl;

    fill_blanks yeah {database};
    verbly::token action{
      {"the furries are"},
      {verbly::token::fillin_type::adjective_phrase}
    };
    while (!action.is_complete())
    {
      yeah.visit(action, random_engine);
    }
    
    std::string result = action.compile();
    result.resize(140);

    try
    {
      client.updateStatus(result);
      
      std::cout << "Tweeted!" << std::endl;
    } catch (const twitter::twitter_error& e)
    {
      std::cout << "Twitter error: " << e.what() << std::endl;
    }
    
    std::cout << "Waiting..." << std::endl;
    
    std::this_thread::sleep_for(std::chrono::hours(1));
  }
}
