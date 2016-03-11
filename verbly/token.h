#ifndef TOKEN_H_AD62C505
#define TOKEN_H_AD62C505

#include <string>
#include <list>
#include <sstream>
#include "verb.h"

namespace verbly {
  
  enum class type {
    verb,
    fillin,
    string,
    utterance
  };
  
  class selrestr {
  };
  
  class synrestr {
  };
  
  enum class fillin_type {
    noun_phrase,
    participle_phrase,
    adjective,
    adverb
  };
  
  class token {
    protected:
      // General
      type type;
      
      token(enum type type) : type(type)
      {
        
      }
      
    public:
      enum type token_type() const
      {
        return type;
      }
      
      virtual bool complete() const = 0;
      virtual std::string compile() const = 0;
      virtual token* copy() const = 0;
  };
  
  class verb_token : public token {
    private:
      // Verb
      const verb* m_verb;
      conjugation verb_infl = conjugation::infinitive;
    
    public:
      verb_token(const class verb& verb) : token(type::verb), m_verb(&verb)
      {

      }
      
      const class verb& verb() const
      {
        return *m_verb;
      }
      
      verb_token& conjugate(conjugation infl)
      {
        verb_infl = infl;
        return *this;
      }
      
      bool complete() const
      {
        return true;
      }
      
      std::string compile() const
      {
        return m_verb->conjugate(verb_infl);
      }
      
      token* copy() const
      {
        return new verb_token(*this);
      }
  };
  
  class utterance_token : public token {
    private:
      // Utterance
      std::list<std::unique_ptr<token>> utterance;

    public:
      typedef std::list<std::unique_ptr<token>>::iterator iterator;
      /*class iterator {
        private:
          friend class utterance_token;
          
          std::list<std::unique_ptr<token>>::iterator it;
          
        public:
          iterator(std::list<std::unique_ptr<token>>::iterator it) : it(it)
          {
            
          }
          
          iterator& operator++()
          {
            ++it;
            return *this;
          }
          
          iterator& operator--()
          {
            --it;
            return *this;
          }
          
          bool operator==(const iterator& other) const
          {
            return it == other.it;
          }
          
          bool operator!=(const iterator& other) const
          {
            return it != other.it;
          }
          
          token& operator*()
          {
            return **it;
          }
          
          token& operator->()
          {
            return **it;
          }
      };*/
      
      utterance_token(std::initializer_list<token*> tkns) : token(type::utterance)
      {
        for (auto tkn : tkns)
        {
          utterance.push_back(std::unique_ptr<token>(tkn));
        }
      }
      
      utterance_token(const utterance_token& other) : token(type::utterance)
      {
        for (auto& tkn : other.utterance)
        {
          utterance.push_back(std::unique_ptr<token>(tkn->copy()));
        }
      }
      
      utterance_token(utterance_token&& other) : token(type::utterance), utterance(std::move(other.utterance))
      {
        
      }
      
      utterance_token& operator=(const utterance_token& other)
      {
        utterance.clear();
        
        for (auto& tkn : other.utterance)
        {
          utterance.push_back(std::unique_ptr<token>(tkn->copy()));
        }
        
        return *this;
      }
      
      utterance_token& operator=(utterance_token&& other)
      {
        utterance = std::move(other.utterance);
        
        return *this;
      }
      
      iterator begin()
      {
        return std::begin(utterance);
      }
      
      iterator end()
      {
        return std::end(utterance);
      }
      
      void erase(iterator it)
      {
        utterance.erase(it);
      }
      
      bool complete() const
      {
        return std::all_of(std::begin(utterance), std::end(utterance), [] (const std::unique_ptr<token>& tkn) {
          return tkn->complete();
        });
      }
      
      std::string compile() const
      {
        std::stringstream result;
        for (auto& t : utterance)
        {
          if (t->complete())
          {
            result << t->compile() << " ";
          } else {
            return "Could not compile!";
          }
        }
    
        std::string output = result.str();
        if (output != "")
        {
          output.pop_back();
        }
    
        return output;
      }
      
      token* copy() const
      {
        return new utterance_token(*this);
      }
  };
  
  class fillin_token : public token {
    private:    
      // Fillin
      std::string m_theme;
      fillin_type m_fillin_type;
      
    public:
      fillin_token(fillin_type ft) : token(type::fillin), m_fillin_type(ft)
      {

      }
      
/*      void synrestrs(std::initializer_list<synrestr> ins)
      {
        m_synrestrs = std::set<synrestr>(ins);
      }
      
      std::set<synrestr>& synrestrs()
      {
        return m_synrestrs;
      }
      
      void selrestrs(std::initializer_list<selrestr> ins)
      {
        m_selrestrs = std::set<selrestr>(ins);
      }
      
      std::set<selrestr>& selrestrs()
      {
        return m_selrestrs;
      }*/
      
      fillin_token theme(std::string theme)
      {
        m_theme = theme;
        
        return *this;
      }
      
      std::string& theme()
      {
        return m_theme;
      }
      
      fillin_type fillin_type() const
      {
        return m_fillin_type;
      }
      
      bool complete() const
      {
        return false;
      }
      
      std::string compile() const
      {
        return "";
      }
      
      token* copy() const
      {
        return new fillin_token(*this);
      }
  };
  
  class string_token : public token {
    private:
      // String
      std::string str;
      
    public:
      string_token(std::string str) : token(type::string), str(str)
      {
        
      }
      
      bool complete() const
      {
        return true;
      }
      
      std::string compile() const
      {
        return str;
      }
      
      token* copy() const
      {
        return new string_token(*this);
      }
  };
  
};

#endif /* end of include guard: TOKEN_H_AD62C505 */